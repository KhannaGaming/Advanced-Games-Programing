#pragma once
#include<d3d11.h>
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT
#include<xnamath.h>
#include<math.h>
class camera
{
public:
	camera(float x, float y, float z, float camera_rotation);
	~camera();
	void Rotate(float rotation_amount);
	void Forward(float distance_amount);
	void Strafe(float distance_amount);
	void Up(float distance_amount);
	XMMATRIX GetViewMatrix();
private:
	float m_x, m_y, m_z, m_dx, m_dz, m_camera_rotation;
	XMVECTOR m_position, m_lookat, m_up;
};

