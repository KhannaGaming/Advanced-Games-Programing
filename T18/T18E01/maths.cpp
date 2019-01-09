#include "maths.h"



maths::maths()
{
}


maths::~maths()
{
}

float maths::dot(XMVECTOR  v1, XMVECTOR  v2)
{
	float dot = (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
	return dot;
}

XMVECTOR maths::cross(XMVECTOR  v1, XMVECTOR  v2)
{
	XMVECTOR cross = XMVectorSet((v1.y*v2.z) - (v1.z*v2.y), (v1.z*v2.x) - (v1.x*v2.z), (v1.x*v2.y) - (v1.y*v2.x), 0.0f);
	return cross;
}

XMVECTOR maths::normal(XMVECTOR  v1, XMVECTOR  v2, XMVECTOR  v3)
{
	XMVECTOR diffv2v1 = XMVectorSubtract(v2, v1);
	XMVECTOR diffv3v1 = XMVectorSubtract(v3, v1);
	XMVECTOR crossV = cross(diffv2v1, diffv3v1);

	float normal = sqrt(pow(crossV.x, 2.0f) + pow(crossV.y, 2.0f) + pow(crossV.z, 2.0f));

	crossV = XMVectorSet(crossV.x / normal, crossV.y / normal, crossV.z / normal, 0.0f);

	return crossV;
}

Plane maths::CalculatePlane(XMVECTOR v1, XMVECTOR v2, XMVECTOR v3)
{
	XMVECTOR planeNormal = normal(v1, v2, v3);

	float dOffset = -dot(planeNormal, v1);

	Plane plane = { planeNormal, dOffset };
	return plane;
}

float maths::valueOfPlaneEquation(Plane plane, XMVECTOR v)
{
	float planeEquation = (plane.normal.x * v.x) + (plane.normal.y * v.y) + (plane.normal.z * v.z) + plane.d;
	return planeEquation;
}
