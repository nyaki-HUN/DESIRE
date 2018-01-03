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
	const float result = 1.0f / sqrtf(v.x);
	return vec_float3_t(result, result, result);
}

static DESIRE_FORCE_INLINE vec_float4_t newtonrapson_rsqrt4(const vec_float4_t& v)
{
	const float result = 1.0f / sqrtf(v.x);
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

	// Set the x, y, and z elements (does not change the w element)
	static DESIRE_FORCE_INLINE void SetXYZ(vec_float4_t& vec, const vec_float3_t& xyz)
	{
		memcpy(&vec, &xyz, 3 * sizeof(float));
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

	// Compute the conjugate of a quaternion
	static DESIRE_FORCE_INLINE vec_float4_t Conjugate(const vec_float4_t& quat)
	{
		return vec_float4_t(-quat.x, -quat.y, -quat.z, quat.w);
	}

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

	// Multiply vectors per element
	static DESIRE_FORCE_INLINE vec_float3_t MulPerElem(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z
		);
	}

	static DESIRE_FORCE_INLINE vec_float4_t MulPerElem(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z,
			a.w * b.w
		);
	}

	// Divide vectors per element
	static DESIRE_FORCE_INLINE vec_float3_t DivPerElem(const vec_float3_t& a, const vec_float3_t& b)
	{
		return vec_float3_t(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z
		);
	}

	static DESIRE_FORCE_INLINE vec_float4_t DivPerElem(const vec_float4_t& a, const vec_float4_t& b)
	{
		return vec_float4_t(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z,
			a.w / b.w
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
};
