#pragma once
#ifndef _AlgorithmCommon_H_
#define _AlgorithmCommon_H_

#define INF CPoint(-1,-1)

// 叉積運算，回傳純量（除去方向）
__inline int cross(CPoint& v1, CPoint& v2)
{
	// 沒有除法，儘量避免誤差。
	return v1.x * v2.y - v1.y * v2.x;
}

__inline float dot(CPoint& o, CPoint& a, CPoint& b)
{
	return (a.x - o.x) * (b.x - o.x) + (a.y - o.y) * (b.y - o.y);
}

__inline float cross(CPoint& o, CPoint& a, CPoint& b)
{
	return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

__inline float distance(CPoint& p1, CPoint& p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

__inline CPoint intersection(CPoint& a1, CPoint& a2, CPoint& b1, CPoint& b2)
{
	CPoint a = a2 - a1, b = b2 - b1, s = b1 - a1;

	float c1 = cross(a1, a2, b1);
	float c2 = cross(a1, a2, b2);
	float c3 = cross(b1, b2, a1);
	float c4 = cross(b1, b2, a2);

	if (c1 * c2 < 0 && c3 * c4 < 0)
	{
		float c5 = cross(a, b), c6 = cross(s, b);
		float c7 = c6 / c5;

		CPoint d;
		d.x = a.x * c7;
		d.y = a.y * c7;
		// 計算交點
		return a1 + d;
	}
	else
		return INF;
}

__inline bool intersect_collinear(CPoint p, CPoint p1, CPoint p2)
{
	// 點與線段共線，而且點要在線段之間。
	return cross(p, p1, p2) == 0
		&& dot(p, p1, p2) <= 0;
}

__inline bool intersect_collinear(CPoint& a1, CPoint& a2, CPoint& b1, CPoint& b2)
{
	return intersect_collinear(a1, a2, b1)
		|| intersect_collinear(a1, a2, b2)
		|| intersect_collinear(b1, b2, a1)
		|| intersect_collinear(b1, b2, a2);
}

#endif
