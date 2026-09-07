#pragma once
#include "DxLib.h"
#include "Precompiled.h"

/// <summary>
/// カプセル(当たり判定兼仮表示)
/// </summary>
struct Capsule
{
	VECTOR p0 = VGet(0.0f, 0.0f, 0.0f); // 線分の始点
	VECTOR p1 = VGet(0.0f, 0.0f, 0.0f); // 線分の終点
	float  radius = 0.0f;               // 半径
};

/// <summary>
/// カプセルの計算
/// </summary>
namespace CapsuleMath
{
	/// <summary>
	/// 2線分間の最短距離の2乗を求める
	/// </summary>
	inline float ClosestSqDistSegmentSegment(const VECTOR& p1, const VECTOR& q1,const VECTOR& p2, const VECTOR& q2)
	{
		const VECTOR d1 = VSub(q1, p1); // 線分1の方向
		const VECTOR d2 = VSub(q2, p2); // 線分2の方向
		const VECTOR r	= VSub(p1, p2);

		const float a = VDot(d1, d1);
		const float e = VDot(d2, d2);
		const float f = VDot(d2, r);

		constexpr float EPS = 1e-6f;
		float s				= 0.0f;
		float t				= 0.0f;

		if (a <= EPS && e <= EPS)
		{
			// 両方とも点
			const VECTOR diff = VSub(p1, p2);
			return VDot(diff, diff);
		}

		if (a <= EPS)
		{
			// 線分1が点
			t = std::fmin(std::fmax(f / e, 0.0f), 1.0f);
		}
		else
		{
			const float c = VDot(d1, r);
			if (e <= EPS)
			{
				// 線分2が点
				s = std::fmin(std::fmax(-c / a, 0.0f), 1.0f);
			}
			else
			{
				const float b		= VDot(d1, d2);
				const float denom	= a * e - b * b;

				if (denom > EPS)
				{
					s = std::fmin(std::fmax((b * f - c * e) / denom, 0.0f), 1.0f);
				}
				else
				{
					s = 0.0f; // 平行
				}

				t = (b * s + f) / e;

				if (t < 0.0f)
				{
					t = 0.0f;
					s = std::fmin(std::fmax(-c / a, 0.0f), 1.0f);
				}
				else if (t > 1.0f)
				{
					t = 1.0f;
					s = std::fmin(std::fmax((b - c) / a, 0.0f), 1.0f);
				}
			}
		}

		const VECTOR c1		= VAdd(p1, VScale(d1, s));
		const VECTOR c2		= VAdd(p2, VScale(d2, t));
		const VECTOR diff	= VSub(c1, c2);
		return VDot(diff, diff);
	}

	/// <summary>
	/// カプセル同士が交差しているか。
	/// </summary>
	inline bool Intersect(const Capsule& a, const Capsule& b)
	{
		const float sq	= ClosestSqDistSegmentSegment(a.p0, a.p1, b.p0, b.p1);
		const float r	= a.radius + b.radius;
		return sq <= r * r;
	}
}