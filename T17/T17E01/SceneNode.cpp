#include "SceneNode.h"



SceneNode::SceneNode()
{
	m_p_model = nullptr;
	m_p_camera = nullptr;
	m_x = m_y = m_z = m_xAngle = m_yAngle = m_zAngle = 0.0f;
	m_scale = 1.0f;
}


SceneNode::~SceneNode()
{
	int maxChildSize = m_children.size();
	for (int i = 0; i < maxChildSize; i++)
	{
		delete m_children[i];
		m_children[i] = nullptr;
		//m_children.pop_back();//erase(m_children.begin() + i);
	}
	
	//	delete m_p_model;
		m_p_model = nullptr;
	
	for (int i = 0; i < maxChildSize; i++)
	{
		 m_children.pop_back();
	}
}

void SceneNode::SetModel(Model * model)
{
	m_p_model = model;
}

void SceneNode::SetCamera(camera * Camera)
{
	m_p_camera = Camera;
}

void SceneNode::SetPos(float xpos, float ypos, float zpos)
{
	m_x = xpos;
	m_y = ypos;
	m_z = zpos;
}

XMVECTOR SceneNode::GetPos()
{
	XMVECTOR Pos = XMVectorSet(m_x, m_y, m_z, 0);
	return Pos;
}

void SceneNode::SetRotation(float xAngle, float yAngle, float zAngle)
{
	m_xAngle = xAngle;
	m_yAngle = yAngle;
	m_zAngle = zAngle;
}

XMVECTOR SceneNode::GetRotation()
{
	XMVECTOR Rot = XMVectorSet(m_xAngle, m_yAngle, m_zAngle, 0);
	return Rot;
}

void SceneNode::SetScale(float scale)
{
	m_scale = scale;
}

float SceneNode::GetScale()
{
	return m_scale;
}

void SceneNode::addChildNode(SceneNode * n)
{
	m_children.push_back(n);
}

bool SceneNode::detachNode(SceneNode * n)
{
	//traverse tree to find node to detach
	for (int i = 0; i < m_children.size(); i++)
	{
		if (n == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			return true;
		}
		if (m_children[i]->detachNode(n) == true) return true;
	}
	return false; // node not in this tree
}

void SceneNode::execute(XMMATRIX * world, XMMATRIX * view, XMMATRIX * projection)
{
	// the local_world matrix will be used to calc the local transformation for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixScaling(m_scale, m_scale, m_scale);
	local_world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	local_world *= XMMatrixTranslation(m_x, m_y, m_z);
	// the local matrix is multiplied by the passed in world matrix that contains the concatentated
	//transformations of all parent nodes so that this nodes transformations are relative to those 
	local_world *= *world;

	//only draw if there is a model attached
	if (m_p_model) m_p_model->Draw(&local_world, view, projection);

	//traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->execute(&local_world, view, projection);
	}

}

void SceneNode::IncPos(float xAmount, float yAmount, float zAmount)
{
	m_x += xAmount;
	m_y += yAmount;
	m_z += zAmount;
}

void SceneNode::IncRotation(float xAmount, float yAmount, float zAmount)
{
	m_xAngle += xAmount;
	m_yAngle += yAmount;
	m_zAngle += zAmount;
}

void SceneNode::IncScale(float scaleAmount)
{
	m_scale += scaleAmount;
}

void SceneNode::LookAt_XZ(float xWorld, float zWorld)
{
	float dx, dz;
	dx = xWorld - m_x;
	dz = zWorld - m_z;
	m_yAngle = atan2(dx, dz)*(180.0 / XM_PI);
}

void SceneNode::LookAt_XYZ(float xWorld, float yWorld, float zWorld)
{
	float dx, dy, dz;
	dx = xWorld - m_x;
	dy = yWorld - m_y;
	dz = zWorld - m_z;
	//m_xAngle = atan2(dy, dx)*(180.0 / XM_PI);// *180.0 / XM_PI;
	
	
	float pyth = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

	m_xAngle = -atan2(dy, pyth)*180.0 / XM_PI;
	m_yAngle = atan2(dx, dz)*(180.0 / XM_PI);
}

void SceneNode::MoveForward(float distance)
{
	m_x += sin(m_yAngle* (XM_PI / 180.0))*distance;
	m_z += cos(m_yAngle* (XM_PI / 180.0))*distance;
}

void SceneNode::MoveForwardIncY(float distance)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0));
	m_y += -sin(m_xAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0));
}

vector<SceneNode*> SceneNode::GetChildren()
{
	return m_children;
}
