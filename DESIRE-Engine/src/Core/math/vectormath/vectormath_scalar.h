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

static DESIRE_FORCE_INLINE vec_float3_t newtonrapson_rsqrt4(const vec_float3_t& v)
{
	const float result = 1.0f / std::sqrt(v.x);
	return vec_float3_t(result, result, result);
}

static DESIRE_FORCE_INLINE vec_float4_t newtonrapson_rsqrt4(const vec_float4_t& v)
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
	static DESIRE_FORCE_INLINE void Construct(vec_float3_t& vec, float x, float y, float z)
	{
		vec.x = x;
		vec.y = y;
		vec.z = z;
	}

	// Construct from x, y, z, and w elements
	static DESIRE_FORCE_INLINE void Construct(vec_float4_t& vec, float x, float y, float z, float w)
	{
		vec.x = x;
		vec.y = y;
		vec.z = z;
		vec.w = w;
	}

	// Construct by setting all elements to the same scalar value
	static DESIRE_FORCE_INLINE void Construct(vec_float3_t& vec, float scalar)
	{
		vec.x = scalar;
		vec.y = scalar;
		vec.z = scalar;
	}
	static DESIRE_FORCE_INLINE void Construct(vec_float4_t& vec, float scalar)
	{
		vec.x = scalar;
		vec.y = scalar;
		vec.z = scalar;
		vec.w = scalar;
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static DESIRE_FORCE_INLINE void LoadXYZW(vec_float4_t& vec, const float *fptr)
	{
		memcpy(&vec, fptr, 4 * sizeof(float));
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static DESIRE_FORCE_INLINE void StoreXYZW(const vec_float4_t& vec, float *fptr)
	{
		memcpy(fptr, &vec, 4 * sizeof(float));
	}

	// Set element
	static DESIRE_FORCE_INLINE void SetX(vec_float3_t& vec, float x)		{ vec.x = x; }
	static DESIRE_FORCE_INLINE void SetX(vec_float4_t& vec, float x)		{ vec.x = x; }
	static DESIRE_FORCE_INLINE void SetY(vec_float3_t& vec, float y)		{ vec.y = y; }
	static DESIRE_FORCE_INLINE void SetY(vec_float4_t& vec, float y)		{ vec.y = y; }
	static DESIRE_FORCE_INLINE void SetZ(vec_float3_t& vec, float z)		{ vec.z = z; }
	static DESIRE_FORCE_INLINE void SetZ(vec_float4_t& vec, float z)		{ vec.z = z; }
	static DESIRE_FORCE_INLINE void SetW(vec_float4_t& vec, float w)		{ vec.w = w; }

	// Get element
	static DESIRE_FORCE_INLINE float GetX(const vec_float3_t& vec)			{ return vec.x; }
	static DESIRE_FORCE_INLINE float GetX(const vec_float4_t& vec)			{ return vec.x; }
	static DESIRE_FORCE_INLINE float GetY(const vec_float3_t& vec)			{ return vec.y; }
	static DESIRE_FORCE_INLINE float GetY(const vec_float4_t& vec)			{ return vec.y; }
	static DESIRE_FORCE_INLINE float GetZ(const vec_float3_t& vec)			{ return vec.z; }
	static DESIRE_FORCE_INLINE float GetZ(const vec_float4_t& vec)			{ return vec.z; }
	static DESIRE_FORCE_INLINE float GetW(const vec_float4_t& vec)			{ return vec.w; }

	// Operator overloads
	static DESIRE_FORCE_INLINE vec_float3_t Negate(const vec_float3_t& vec)
	{
		return vec_float3_t(-vec.x, -vec.y, -vec.z);
	}
	static DESIRE_FORCE_INLINE vec_float4_t Negate(const vec_float4_t& vec)
	{
		return vec_float4_t(-vec.x, -vec.y, -vec.z, -vec.w);
	}

	static DESIRE_FORCE_INLINE vec_float3_t Add(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z
		);
	}
	static DESIRE_FORCE_INLINE vec_float4_t Add(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z,
			a.w + b.w
		);
	}

	static DESIRE_FORCE_INLINE vec_float3_t Sub(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x - b.x,
			a.y - b.y,
			a.z - b.z
		);
	}
	static DESIRE_FORCE_INLINE vec_float4_t Sub(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x - b.x,
			a.y - b.y,
			a.z - b.z,
			a.w - b.w
		);
	}

	static DESIRE_FORCE_INLINE vec_float3_t Mul(const vec_float3_t& vec, float scalar)
	{
		return vec_float3_t(
			vec.x * scalar,
			vec.y * scalar,
			vec.z * scalar
		);
	}
	static DESIRE_FORCE_INLINE vec_float4_t Mul(const vec_float4_t& vec, float scalar)
	{
		return vec_float4_t(
			vec.x * scalar,
			vec.y * scalar,
			vec.z * scalar,
			vec.w * scalar
		);
	}

	static DESIRE_FORCE_INLINE vec_float3_t Mul(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z
		);
	}
	static DESIRE_FORCE_INLINE vec_float4_t Mul(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z,
			a.w * b.w
		);
	}

	static DESIRE_FORCE_INLINE vec_float3_t MulAdd(const vec_float3_t& a, const vec_float3_t& b, const vec_float3_t& c)
	{
		return SIMD::Add(c, SIMD::Mul(a, b));
	}
	static DESIRE_FORCE_INLINE vec_float4_t MulAdd(const vec_float4_t& a, const vec_float4_t& b, const vec_float4_t& c)
	{
		return SIMD::Add(c, SIMD::Mul(a, b));
	}

	static DESIRE_FORCE_INLINE vec_float3_t Div(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z
		);
	}
	static DESIRE_FORCE_INLINE vec_float4_t Div(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z,
			a.w / b.w
		);
	}

	// Comparison operators
	static DESIRE_FORCE_INLINE bool OpCmpGE(const vec_float3_t& a, const vec_float3_t& b)
	{
		return (
			a.x >= b.x &&
			a.y >= b.y &&
			a.z >= b.z
		);
	}

	static DESIRE_FORCE_INLINE bool OpCmpLE(const vec_float3_t& a, const vec_float3_t& b)
	{
		return (
			a.x <= b.x &&
			a.y <= b.y &&
			a.z <= b.z
		);
	}

	// Compute the dot product of two 3-D vectors
	static DESIRE_FORCE_INLINE vec_float3_t Dot3(const vec_float3_t& a, const vec_float3_t& b)
	{
		float result =	a.x * b.x +
						a.y * b.y +
						a.z * b.z;
		return vec_float3_t(result, result, result);
	}

	// Compute the dot product of two 4-D vectors
	static DESIRE_FORCE_INLINE vec_float4_t Dot4(const vec_float4_t& a, const vec_float4_t& b)
	{
		float result =	a.x * b.x +
						a.y * b.y +
						a.z * b.z +
						a.w * b.w;
		return vec_float4_t(result, result, result, result);
	}

	// Compute cross product of two 3-D vectors
	static DESIRE_FORCE_INLINE vec_float3_t Cross(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	// Compute the absolute value per element
	static DESIRE_FORCE_INLINE vec_float3_t AbsPerElem(const vec_float3_t& vec)
	{
		return vec_float3_t(
			fabsf(vec.x),
			fabsf(vec.y),
			fabsf(vec.z)
		);
	}

	static DESIRE_FORCE_INLINE vec_float4_t AbsPerElem(const vec_float4_t& vec)
	{
		return vec_float4_t(
			fabsf(vec.x),
			fabsf(vec.y),
			fabsf(vec.z),
			fabsf(vec.w)
		);
	}

	// Maximum of two vectors per element
	static DESIRE_FORCE_INLINE vec_float3_t MaxPerElem(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			(a.x > b.x) ? a.x : b.x,
			(a.y > b.y) ? a.y : b.y,
			(a.z > b.z) ? a.z : b.z
		);
	}

	static DESIRE_FORCE_INLINE vec_float4_t MaxPerElem(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			(a.x > b.x) ? a.x : b.x,
			(a.y > b.y) ? a.y : b.y,
			(a.z > b.z) ? a.z : b.z,
			(a.w > b.w) ? a.w : b.w
		);
	}

	// Minimum of two vectors per element
	static DESIRE_FORCE_INLINE vec_float3_t MinPerElem(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			(a.x < b.x) ? a.x : b.x,
			(a.y < b.y) ? a.y : b.y,
			(a.z < b.z) ? a.z : b.z
		);
	}

	static DESIRE_FORCE_INLINE vec_float4_t MinPerElem(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			(a.x < b.x) ? a.x : b.x,
			(a.y < b.y) ? a.y : b.y,
			(a.z < b.z) ? a.z : b.z,
			(a.w < b.w) ? a.w : b.w
		);
	}

	// Get maximum element
	static DESIRE_FORCE_INLINE vec_float3_t MaxElem3(const vec_float3_t& vec)
	{
		float result;
		result = (vec.x > vec.y) ? vec.x : vec.y;
		result = (vec.z > result) ? vec.z : result;
		return vec_float3_t(result, result, result);
	}

	static DESIRE_FORCE_INLINE vec_float4_t MaxElem4(const vec_float4_t& vec)
	{
		float result;
		result = (vec.x > vec.y) ? vec.x : vec.y;
		result = (vec.z > result) ? vec.z : result;
		result = (vec.w > result) ? vec.w : result;
		return vec_float4_t(result, result, result, result);
	}

	// Get minimum element
	static DESIRE_FORCE_INLINE vec_float3_t MinElem3(const vec_float3_t& vec)
	{
		float result;
		result = (vec.x < vec.y) ? vec.x : vec.y;
		result = (vec.z < result) ? vec.z : result;
		return vec_float3_t(result, result, result);
	}

	static DESIRE_FORCE_INLINE vec_float4_t MinElem4(const vec_float4_t& vec)
	{
		float result;
		result = (vec.x < vec.y) ? vec.x : vec.y;
		result = (vec.z < result) ? vec.z : result;
		result = (vec.w < result) ? vec.w : result;
		return vec_float4_t(result, result, result, result);
	}

	// Select packed single precision floating-point values from a and b using the mask
	static DESIRE_FORCE_INLINE vec_float4_t Blend(const vec_float3_t& a, const vec_float4_t& b, uint8_t mask)
	{
		const vec_float4_t A(a.x, a.y, a.z, 0.0);
		return Blend(A, b, mask);
	}

	static DESIRE_FORCE_INLINE vec_float4_t Blend(const vec_float4_t& a, const vec_float4_t& b, uint8_t mask)
	{
		vec_float4_t result;
		result.x = (mask & 0b1000) ? b.x : a.x;
		result.y = (mask & 0b0100) ? b.y : a.y;
		result.z = (mask & 0b0010) ? b.z : a.z;
		result.w = (mask & 0b0001) ? b.w : a.w;
		return result;
	}

	// Select mask
	static DESIRE_FORCE_INLINE uint8_t MaskX()
	{
		return 0b1000;
	}
	static DESIRE_FORCE_INLINE uint8_t MaskY()
	{
		return 0b0100;
	}

	static DESIRE_FORCE_INLINE uint8_t MaskZ()
	{
		return 0b0010;
	}

	static DESIRE_FORCE_INLINE uint8_t MaskW()
	{
		return 0b0001;
	}

	// Swizzle
	static DESIRE_FORCE_INLINE vec_float4_t Swizzle_XXXX(const vec_float4_t& vec)	{ return vec_float4_t(vec.x, vec.x, vec.x, vec.x); }
	static DESIRE_FORCE_INLINE vec_float4_t Swizzle_YYYY(const vec_float4_t& vec)	{ return vec_float4_t(vec.y, vec.y, vec.y, vec.y); }
	static DESIRE_FORCE_INLINE vec_float4_t Swizzle_ZZZZ(const vec_float4_t& vec)	{ return vec_float4_t(vec.z, vec.z, vec.z, vec.z); }
	static DESIRE_FORCE_INLINE vec_float4_t Swizzle_WWWW(const vec_float4_t& vec)	{ return vec_float4_t(vec.w, vec.w, vec.w, vec.w); }

	static DESIRE_FORCE_INLINE vec_float4_t Swizzle_YZXW(const vec_float4_t& vec)	{ return vec_float4_t(vec.y, vec.z, vec.x, vec.w); }
	static DESIRE_FORCE_INLINE vec_float4_t Swizzle_ZXYW(const vec_float4_t& vec)	{ return vec_float4_t(vec.z, vec.x, vec.y, vec.w); }
};
