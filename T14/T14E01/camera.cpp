#include "camera.h"



camera::camera(float x, float y, float z, float camera_rotation_yaw, float camera_rotation_pitch)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation_yaw = camera_rotation_yaw;
	m_camera_rotation_pitch = camera_rotation_pitch;
	m_dx = sin(m_camera_rotation_yaw *(XM_PI / 180.0f));
	m_dy = sin(m_camera_rotation_pitch *(XM_PI / 180.0f));
	m_dz = cos(m_camera_rotation_yaw *(XM_PI / 180.0f));
}


camera::~camera()
{
}

void camera::RotateAroundX(float rotation_amount)
{
	m_camera_rotation_pitch += rotation_amount;
	if (m_camera_rotation_pitch >= 90.0f)
	{
		m_camera_rotation_pitch = 90.0f;
	}
	if (m_camera_rotation_pitch <= -90.0f)
	{
		m_camera_rotation_pitch = -90.0f;
	}
	m_dy = sin(m_camera_rotation_pitch *(XM_PI / 180.0f));
}

void camera::RotateAroundY(float rotation_amount)
{
	m_camera_rotation_yaw += rotation_amount;
	m_dx = sin(m_camera_rotation_yaw *(XM_PI / 180.0f));
	m_dz = cos(m_camera_rotation_yaw *(XM_PI / 180.0f));
}

void camera::Forward(float distance_amount)
{
	m_x += m_dx * distance_amount;
	m_z += m_dz * distance_amount;
}

void camera::Strafe(float distance_amount)
{
	m_lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0);
	m_x += XMVector3Cross(m_position, m_lookat).x * distance_amount;
	m_z += XMVector3Cross(m_position, m_lookat).z * distance_amount;
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0);
}

void camera::Up(float distance_amount)
{
	m_y += distance_amount;
}

XMMATRIX camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	XMMATRIX view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;
}

XMVECTOR camera::GetPos()
{
	XMVECTOR Position = XMVectorSet(m_x, m_y, m_z,0);
	return Position;
}
