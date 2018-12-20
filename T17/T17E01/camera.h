#pragma once
#include<d3d11.h>
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT
#include<xnamath.h>
#include<math.h>
class camera
{
public:
	camera(float x, float y, float z, float camera_rotation_pitch, float camera_rotation_yaw);
	~camera();
	void RotateAroundX(float rotation_amount);
	void RotateAroundY(float rotation_amount);
	void Forward(float distance_amount);
	void Strafe(float distance_amount);
	void Up(float distance_amount);
	XMMATRIX GetViewMatrix();
	XMVECTOR GetPos();
	void SetPos(float x, float y, float z);
	void SetRot(float x, float y, float z);
	XMVECTOR GetRot();
private:
	float m_x, m_y, m_z, m_dx, m_dy, m_dz, m_camera_rotation_yaw, m_camera_rotation_pitch;
	XMVECTOR m_position, m_lookat, m_up;
};

