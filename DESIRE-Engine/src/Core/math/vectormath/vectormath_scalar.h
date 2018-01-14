#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	SCALAR vector types
// --------------------------------------------------------------------------------------------------------------------

struct vec_float3_t;
struct vec_float4_t;

struct vec_float3_t
{
	float x;
	float y;
	float z;

	vec_float3_t() {}
	vec_float3_t(float x, float y, float z) : x(x), y(y), z(z) {}
	vec_float3_t(const vec_float4_t& vec);
	vec_float3_t& operator =(const vec_float4_t& vec);
};

struct vec_float4_t
{
	float x;
	float y;
	float z;
	float w;

	vec_float4_t() {}
	vec_float4_t(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	vec_float4_t& operator =(const vec_float3_t& vec);
};

inline vec_float3_t::vec_float3_t(const vec_float4_t& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

inline vec_float3_t& vec_float3_t::operator =(const vec_float4_t& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	return *this;
}

inline vec_float4_t& vec_float4_t::operator =(const vec_float3_t& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	return *this;
}

// --------------------------------------------------------------------------------------------------------------------
//	SCALAR Helper functions
// --------------------------------------------------------------------------------------------------------------------

static inline vec_float3_t newtonrapson_rsqrt4(const vec_float3_t& v)
{
	const float result = 1.0f / std::sqrt(v.x);
	return vec_float3_t(result, result, result);
}

static inline vec_float4_t newtonrapson_rsqrt4(const vec_float4_t& v)
{
	const float result = 1.0f / std::sqrt(v.x);
	return vec_float4_t(result, result, result, result);
}

// --------------------------------------------------------------------------------------------------------------------
//	SCALAR implementation of the SIMD functions
// --------------------------------------------------------------------------------------------------------------------

class SIMD
{
public:
	// Construct from x, y, and z elements
	static inline void Construct(vec_float3_t& vec, float x, float y, float z)
	{
		vec.x = x;
		vec.y = y;
		vec.z = z;
	}

	// Construct from x, y, z, and w elements
	static inline void Construct(vec_float4_t& vec, float x, float y, float z, float w)
	{
		vec.x = x;
		vec.y = y;
		vec.z = z;
		vec.w = w;
	}

	// Construct by setting all elements to the same scalar value
	static inline void Construct(vec_float3_t& vec, float scalar)
	{
		vec.x = scalar;
		vec.y = scalar;
		vec.z = scalar;
	}
	static inline void Construct(vec_float4_t& vec, float scalar)
	{
		vec.x = scalar;
		vec.y = scalar;
		vec.z = scalar;
		vec.w = scalar;
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static inline void LoadXYZW(vec_float4_t& vec, const float *fptr)
	{
		memcpy(&vec, fptr, 4 * sizeof(float));
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static inline void StoreXYZW(const vec_float4_t& vec, float *fptr)
	{
		memcpy(fptr, &vec, 4 * sizeof(float));
	}

	// Set element
	static inline void SetX(vec_float3_t& vec, float x)		{ vec.x = x; }
	static inline void SetX(vec_float4_t& vec, float x)		{ vec.x = x; }
	static inline void SetY(vec_float3_t& vec, float y)		{ vec.y = y; }
	static inline void SetY(vec_float4_t& vec, float y)		{ vec.y = y; }
	static inline void SetZ(vec_float3_t& vec, float z)		{ vec.z = z; }
	static inline void SetZ(vec_float4_t& vec, float z)		{ vec.z = z; }
	static inline void SetW(vec_float4_t& vec, float w)		{ vec.w = w; }

	// Get element
	static inline float GetX(const vec_float3_t& vec)		{ return vec.x; }
	static inline float GetX(const vec_float4_t& vec)		{ return vec.x; }
	static inline float GetY(const vec_float3_t& vec)		{ return vec.y; }
	static inline float GetY(const vec_float4_t& vec)		{ return vec.y; }
	static inline float GetZ(const vec_float3_t& vec)		{ return vec.z; }
	static inline float GetZ(const vec_float4_t& vec)		{ return vec.z; }
	static inline float GetW(const vec_float4_t& vec)		{ return vec.w; }

	// Operator overloads
	static inline vec_float3_t Negate(const vec_float3_t& vec)
	{
		return vec_float3_t(-vec.x, -vec.y, -vec.z);
	}
	static inline vec_float4_t Negate(const vec_float4_t& vec)
	{
		return vec_float4_t(-vec.x, -vec.y, -vec.z, -vec.w);
	}

	static inline vec_float3_t Add(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z
		);
	}
	static inline vec_float4_t Add(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z,
			a.w + b.w
		);
	}

	static inline vec_float3_t Sub(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x - b.x,
			a.y - b.y,
			a.z - b.z
		);
	}
	static inline vec_float4_t Sub(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x - b.x,
			a.y - b.y,
			a.z - b.z,
			a.w - b.w
		);
	}

	static inline vec_float3_t Mul(const vec_float3_t& vec, float scalar)
	{
		return vec_float3_t(
			vec.x * scalar,
			vec.y * scalar,
			vec.z * scalar
		);
	}
	static inline vec_float4_t Mul(const vec_float4_t& vec, float scalar)
	{
		return vec_float4_t(
			vec.x * scalar,
			vec.y * scalar,
			vec.z * scalar,
			vec.w * scalar
		);
	}

	static inline vec_float3_t Mul(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z
		);
	}
	static inline vec_float4_t Mul(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z,
			a.w * b.w
		);
	}

	static inline vec_float3_t MulAdd(const vec_float3_t& a, const vec_float3_t& b, const vec_float3_t& c)
	{
		return SIMD::Add(c, SIMD::Mul(a, b));
	}
	static inline vec_float4_t MulAdd(const vec_float4_t& a, const vec_float4_t& b, const vec_float4_t& c)
	{
		return SIMD::Add(c, SIMD::Mul(a, b));
	}

	static inline vec_float3_t Div(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z
		);
	}
	static inline vec_float4_t Div(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z,
			a.w / b.w
		);
	}

	// Comparison operators
	static inline bool OpCmpGE(const vec_float3_t& a, const vec_float3_t& b)
	{
		return (
			a.x >= b.x &&
			a.y >= b.y &&
			a.z >= b.z
		);
	}

	static inline bool OpCmpLE(const vec_float3_t& a, const vec_float3_t& b)
	{
		return (
			a.x <= b.x &&
			a.y <= b.y &&
			a.z <= b.z
		);
	}

	// Compute the dot product of two 3-D vectors
	static inline vec_float3_t Dot3(const vec_float3_t& a, const vec_float3_t& b)
	{
		float result =	a.x * b.x +
						a.y * b.y +
						a.z * b.z;
		return vec_float3_t(result, result, result);
	}

	// Compute the dot product of two 4-D vectors
	static inline vec_float4_t Dot4(const vec_float4_t& a, const vec_float4_t& b)
	{
		float result =	a.x * b.x +
						a.y * b.y +
						a.z * b.z +
						a.w * b.w;
		return vec_float4_t(result, result, result, result);
	}

	// Compute cross product of two 3-D vectors
	static inline vec_float3_t Cross(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	// Compute the absolute value per element
	static inline vec_float3_t AbsPerElem(const vec_float3_t& vec)
	{
		return vec_float3_t(
			fabsf(vec.x),
			fabsf(vec.y),
			fabsf(vec.z)
		);
	}

	static inline vec_float4_t AbsPerElem(const vec_float4_t& vec)
	{
		return vec_float4_t(
			fabsf(vec.x),
			fabsf(vec.y),
			fabsf(vec.z),
			fabsf(vec.w)
		);
	}

	// Maximum of two vectors per element
	static inline vec_float3_t MaxPerElem(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			(a.x > b.x) ? a.x : b.x,
			(a.y > b.y) ? a.y : b.y,
			(a.z > b.z) ? a.z : b.z
		);
	}

	static inline vec_float4_t MaxPerElem(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			(a.x > b.x) ? a.x : b.x,
			(a.y > b.y) ? a.y : b.y,
			(a.z > b.z) ? a.z : b.z,
			(a.w > b.w) ? a.w : b.w
		);
	}

	// Minimum of two vectors per element
	static inline vec_float3_t MinPerElem(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			(a.x < b.x) ? a.x : b.x,
			(a.y < b.y) ? a.y : b.y,
			(a.z < b.z) ? a.z : b.z
		);
	}

	static inline vec_float4_t MinPerElem(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			(a.x < b.x) ? a.x : b.x,
			(a.y < b.y) ? a.y : b.y,
			(a.z < b.z) ? a.z : b.z,
			(a.w < b.w) ? a.w : b.w
		);
	}

	// Get maximum element
	static inline vec_float3_t MaxElem3(const vec_float3_t& vec)
	{
		float result;
		result = (vec.x > vec.y) ? vec.x : vec.y;
		result = (vec.z > result) ? vec.z : result;
		return vec_float3_t(result, result, result);
	}

	static inline vec_float4_t MaxElem4(const vec_float4_t& vec)
	{
		float result;
		result = (vec.x > vec.y) ? vec.x : vec.y;
		result = (vec.z > result) ? vec.z : result;
		result = (vec.w > result) ? vec.w : result;
		return vec_float4_t(result, result, result, result);
	}

	// Get minimum element
	static inline vec_float3_t MinElem3(const vec_float3_t& vec)
	{
		float result;
		result = (vec.x < vec.y) ? vec.x : vec.y;
		result = (vec.z < result) ? vec.z : result;
		return vec_float3_t(result, result, result);
	}

	static inline vec_float4_t MinElem4(const vec_float4_t& vec)
	{
		float result;
		result = (vec.x < vec.y) ? vec.x : vec.y;
		result = (vec.z < result) ? vec.z : result;
		result = (vec.w < result) ? vec.w : result;
		return vec_float4_t(result, result, result, result);
	}

	// Special blends for one single precision floating-point value
	static inline vec_float3_t Blend_X(const vec_float3_t& to, const vec_float3_t& from)	{ return { from.x, to.y, to.z }; }
	static inline vec_float3_t Blend_Y(const vec_float3_t& to, const vec_float3_t& from)	{ return { to.x, from.y, to.z }; }
	static inline vec_float3_t Blend_Z(const vec_float3_t& to, const vec_float3_t& from)	{ return { to.x, to.y, from.z }; }

	static inline vec_float4_t Blend_X(const vec_float4_t& to, const vec_float4_t& from)	{ return { from.x, to.y, to.z, to.w }; }
	static inline vec_float4_t Blend_Y(const vec_float4_t& to, const vec_float4_t& from)	{ return { to.x, from.y, to.z, to.w }; }
	static inline vec_float4_t Blend_Z(const vec_float4_t& to, const vec_float4_t& from)	{ return { to.x, to.y, from.z, to.w }; }
	static inline vec_float4_t Blend_W(const vec_float4_t& to, const vec_float4_t& from)	{ return { to.x, to.y, to.z, from.w }; }

	// Swizzle
	static inline vec_float4_t Swizzle_XXXX(const vec_float4_t& vec)		{ return { vec.x, vec.x, vec.x, vec.x }; }
	static inline vec_float4_t Swizzle_XXYY(const vec_float4_t& vec)		{ return { vec.x, vec.x, vec.y, vec.y }; }
	static inline vec_float4_t Swizzle_XXZZ(const vec_float4_t& vec)		{ return { vec.x, vec.x, vec.z, vec.z }; }
	static inline vec_float4_t Swizzle_XYXY(const vec_float4_t& vec)		{ return { vec.x, vec.y, vec.x, vec.y }; }

	static inline vec_float4_t Swizzle_YYYY(const vec_float4_t& vec)		{ return { vec.y, vec.y, vec.y, vec.y }; }
	static inline vec_float4_t Swizzle_YYWW(const vec_float4_t& vec)		{ return { vec.y, vec.y, vec.w, vec.w }; }
	static inline vec_float4_t Swizzle_YZXW(const vec_float4_t& vec)		{ return { vec.y, vec.z, vec.x, vec.w }; }

	static inline vec_float4_t Swizzle_ZXYW(const vec_float4_t& vec)		{ return { vec.z, vec.x, vec.y, vec.w }; }
	static inline vec_float4_t Swizzle_ZWZW(const vec_float4_t& vec)		{ return { vec.z, vec.w, vec.z, vec.w }; }
	static inline vec_float4_t Swizzle_ZZWW(const vec_float4_t& vec)		{ return { vec.z, vec.z, vec.w, vec.w }; }
	static inline vec_float4_t Swizzle_ZZZZ(const vec_float4_t& vec)		{ return { vec.z, vec.z, vec.z, vec.z }; }

	static inline vec_float4_t Swizzle_WWWW(const vec_float4_t& vec)		{ return { vec.w, vec.w, vec.w, vec.w }; }
};
