//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cmath>
#include <cstdint>
#include <vector>

namespace rdr
{
	class Math
	{
	public:
		static constexpr int MaxBlurRadius = 5;

		static std::vector<float> CalcGaussWeights(float sigma)
		{
			float twoSigma2 = 2.0f * sigma * sigma;

			int blurRadius = static_cast<int>(std::ceil(2.0f * sigma));

			assert(blurRadius <= MaxBlurRadius);

			std::vector<float> weights;
			weights.resize(2 * blurRadius + 1);

			float weightSum = 0.0f;

			for (int i = -blurRadius; i <= blurRadius; ++i)
			{
				float x = (float)i;

				weights[i + blurRadius] = expf(-x * x / twoSigma2);

				weightSum += weights[i + blurRadius];
			}

			for (int i = 0; i < weights.size(); ++i)
			{
				weights[i] /= weightSum;
			}

			return weights;
		}

		// Returns random float in [0, 1).
		static float RandF()
		{
			return (float)(rand()) / (float)RAND_MAX;
		}

		// Returns random float in [a, b).
		static float RandF(float a, float b)
		{
			return a + RandF() * (b - a);
		}

		static int Rand(int a, int b)
		{
			return a + rand() % ((b - a) + 1);
		}

		template<typename T>
		static T Min(const T& a, const T& b)
		{
			return a < b ? a : b;
		}

		template<typename T>
		static T Max(const T& a, const T& b)
		{
			return a > b ? a : b;
		}

		template<typename T>
		static T Lerp(const T& a, const T& b, float t)
		{
			return a + (b - a) * t;
		}

		template<typename T>
		static T Clamp(const T& x, const T& low, const T& high)
		{
			return x < low ? low : (x > high ? high : x);
		}

		// Returns the polar angle of the point (x,y) in [0, 2*PI).
		static float AngleFromXY(float x, float y);

		static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
		{
			return DirectX::XMVectorSet(
				radius * sinf(phi) * cosf(theta),
				radius * cosf(phi),
				radius * sinf(phi) * sinf(theta),
				1.0f);
		}

		static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
		{
			// Inverse-transpose is just applied to normals.  So zero out 
			// translation row so that it doesn't get into our inverse-transpose
			// calculation--we don't want the inverse-transpose of the translation.
			DirectX::XMMATRIX A = M;
			A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
			return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
		}

		const static DirectX::XMFLOAT4X4& Identity4x4()
		{
			static DirectX::XMFLOAT4X4 I(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			return I;
		}

		const static DirectX::XMFLOAT4X4& TextureMatrix()
		{
			static DirectX::XMFLOAT4X4 T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);
			return T;
		}

		static DirectX::XMVECTOR RandUnitVec3();
		static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

		static const float Infinity;
		static const float Pi;
	};
}

