static float PI = 3.14159265;

//------------------------------------------------------------------------------
// Specular BRDF
//------------------------------------------------------------------------------

float pow5(float x)
{
	float x2 = x * x;
	return x2 * x2 * x;
}

// Normal distribution function (specular D term)
float D_GGX(float NoH, float linearRoughness)
{
	float a = NoH * linearRoughness;
	float k = linearRoughness / (1 - NoH * NoH + a * a);
	return k * k * (1 / PI);
}

// Visibility function (specular V term)
float V_SmithGGXCorrelated(float NoV, float NoL, float linearRoughness)
{
	float a2 = linearRoughness * linearRoughness;
	float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
	float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
	return 0.5 / (lambdaV + lambdaL);
}

// Fresnel (specular F term)
float3 F_Schlick(float VoH, float3 f0)
{
	float f = pow5(1 - VoH);
	return f + f0 * (1 - f);
}

float F_Schlick(float VoH, float f0, float f90)
{
	return f0 + (f90 - f0) * pow5(1 - VoH);
}

//------------------------------------------------------------------------------
// Diffuse BRDF
//------------------------------------------------------------------------------

// Burley 2012, "Physically-Based Shading at Disney"
float Fd_Burley(float NoV, float NoL, float LoH, float linearRoughness)
{
	float f90 = 0.5 + 2 * linearRoughness * LoH * LoH;
	float lightScatter = F_Schlick(NoL, 1, f90);
	float viewScatter  = F_Schlick(NoV, 1, f90);
	return lightScatter * viewScatter * (1 / PI);
}

// Lambertian
float Fd_Lambert()
{
	return 1 / PI;
}

//------------------------------------------------------------------------------
// Indirect lighting
//------------------------------------------------------------------------------

// Irradiance from "Ditch River" IBL (http://www.hdrlabs.com/sibl/archive.html)
float3 Irradiance_SphericalHarmonics(float3 n)
{
	return	float3( 0.754554516862612,  0.748542953903366,  0.790921515418539) +
			float3(-0.083856548007422,  0.092533500963210,  0.322764661032516) * n.y +
			float3( 0.308152705331738,  0.366796330467391,  0.466698181299906) * n.z +
			float3(-0.188884931542396, -0.277402551592231, -0.377844212327557) * n.x;
}

// Karis 2014, "Physically Based Material on Mobile"
float2 PrefilteredDFG_Karis(float roughness, float NoV)
{
	float4 c0 = { -1.0, -0.0275, -0.572,  0.022 };
	float4 c1 = {  1.0,  0.0425,  1.040, -0.040 };

	float4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;

	return float2(-1.04, 1.04) * a004 + r.zw;
}

//------------------------------------------------------------------------------

float3 BRDF()
{
	float3 color = baseColor;
	float metallic = 0;
	float roughness = 1;
	n = CalcNormal();

	float4 lightColorAndIntensity;
	float3 l = GetLight(lightColorAndIntensity);
	float3 v = -rayDir;
	float3 h = normalize(v + l);
	float3 r = reflect(rayDir, n);

	float NoV = dot(n, v) + 1e-5;
	float NoL = saturate(dot(n, l));
	float NoH = saturate(dot(n, h));
	float LoH = saturate(dot(l, h));

	float indirectIntensity = 0.64;

	float3 diffuseColor = (1 - metallic) * color;
	float3 f0 = lerp(0.04, color, metallic);

	float attenuation = CalcSoftshadow(hitPos, l) * CalcAO(hitPos, n);
	float linearRoughness = roughness * roughness;

	// Specular BRDF
	float D = D_GGX(NoH, linearRoughness);
	float V = V_SmithGGXCorrelated(NoV, NoL, linearRoughness);
	float3 F = F_Schlick(LoH, f0);
	float3 Fr = (D * V) * F;

	// Diffuse BRDF
	float3 Fd = diffuseColor * Fd_Burley(NoV, NoL, LoH, linearRoughness);

	color = (Fd + Fr) * lightColorAndIntensity.rgb * (lightColorAndIntensity.w * attenuation * NoL);

	// Specular indirect
	float2 indirectHit = DoTrace(hitPos, r);
	float3 indirectSpecular = GetSkyColor(r);
	if(indirectHit.y > 0.0)
	{
		if(indirectHit.y <= 1.0)
		{
			float3 indirectHitPos = hitPos + indirectHit.x * r;
			// Checkerboard floor
			indirectSpecular = 0.4 + 0.6 * mod(floor(5 * indirectHitPos.z) + floor(5 * indirectHitPos.x), 2);
		}
		else
		{
			float tmp;
			indirectSpecular = GetMaterial(indirectHit.y, tmp, tmp);
		}
	}
	indirectSpecular = lerp(indirectSpecular, 1, roughness);
	float2 dfg = PrefilteredDFG_Karis(roughness, NoV);
	float3 specularColor = f0 * dfg.x + dfg.y;

	// Diffuse indirect
	float3 indirectDiffuse = max(Irradiance_SphericalHarmonics(n), 0) * Fd_Lambert();

	// Indirect contribution
	color += (diffuseColor * indirectDiffuse + indirectSpecular * specularColor) * indirectIntensity;
}
