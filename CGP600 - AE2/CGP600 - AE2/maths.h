#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <Windows.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT
#include <xnamath.h>
#include<Xinput.h>
#include<WinUser.h>
#include<windowsx.h>


struct Plane
{
	XMVECTOR normal; /* the normal to the plane*/
	float d; /* the 'd' contstant in the equation for this plane */
};

class maths
{
public:
	maths();
	~maths();

	float dot(XMVECTOR v1, XMVECTOR v2);
	XMVECTOR cross(XMVECTOR v1, XMVECTOR v2 );
	XMVECTOR normal(XMVECTOR v1, XMVECTOR v2, XMVECTOR v3);
	Plane CalculatePlane(XMVECTOR v1, XMVECTOR v2, XMVECTOR v3);
	float valueOfPlaneEquation(Plane plane, XMVECTOR v);
	XMVECTOR planeIntersection(Plane* p, XMVECTOR v1, XMVECTOR v2);
	bool in_triangle(XMVECTOR triangle0, XMVECTOR triangle1, XMVECTOR triangle2, XMVECTOR point);
	int sign(float number);

};

