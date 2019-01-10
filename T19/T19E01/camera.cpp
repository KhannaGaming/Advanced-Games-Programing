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
	if (m_camera_rotation_pitch >= 60.0f)
	{
		m_camera_rotation_pitch = 60.0f;
	}
	if (m_camera_rotation_pitch <= -60.0f)
	{
		m_camera_rotation_pitch = -60.0f;
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
	m_y += m_dy * distance_amount;
	m_z += m_dz * distance_amount;
}

void camera::Strafe(float distance_amount)
{
	XMVECTOR forward = m_position - m_lookat;
	XMVECTOR right = XMVector3Normalize(forward);
	right = XMVector3Cross(m_up, right);
	m_x -= right.x * distance_amount;
	m_z -= right.z * distance_amount;
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

void camera::SetPos(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void camera::SetRot(float x, float y, float z)
{
	m_dx = sin(x *(XM_PI / 180.0f));
	m_dy = sin(y *(XM_PI / 180.0f));
	m_dz = cos(z *(XM_PI / 180.0f));
}

XMVECTOR camera::GetRot()
{

	XMVECTOR Rotation = XMVectorSet(m_camera_rotation_pitch, m_camera_rotation_yaw, 0, 0);
	return Rotation;
}

XMVECTOR camera::GetLookAt()
{
	return XMVectorSet(m_dx, m_dy, m_dz, 0.0f);
}
