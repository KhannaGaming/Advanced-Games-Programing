#include "SceneNode.h"



SceneNode::SceneNode(DeltaTime* deltaTime)
{
	m_p_model = nullptr;
	m_p_camera = nullptr;
	m_x = m_y = m_z = m_xAngle = m_yAngle = m_zAngle = 0.0f;
	m_scale = 1.0f;
	velocity = XMVectorZero();
	m_pDeltaTime = deltaTime;
}


SceneNode::~SceneNode()
{
	m_pDeltaTime = nullptr;
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
	moveable = m_p_model->isModelMoveable();
}

void SceneNode::SetCamera(camera * Camera)
{
	m_p_camera = Camera;
}

void SceneNode::SetPos(XMVECTOR pos)
{
	m_x = XMVectorGetX(pos);
	m_y = XMVectorGetY(pos);
	m_z = XMVectorGetZ(pos);
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

void SceneNode::execute(XMMATRIX * world, XMMATRIX * view, XMMATRIX * projection, SceneNode* root_node)
{
	AddVelocity(root_node);
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
		m_children[i]->execute(&local_world, view, projection, root_node);
	}

}

bool SceneNode::IncPos(float xAmount, float yAmount, float zAmount, SceneNode* root_node)
{
	m_x += xAmount;
	m_y += yAmount;
	m_z += zAmount;
	return true;
}

bool SceneNode::IncRotation(float xAmount, float yAmount, float zAmount, SceneNode* root_node)
{
	float old_x = m_xAngle;
	float old_y = m_yAngle;
	float old_z = m_zAngle;

	m_xAngle += xAmount;
	m_yAngle += yAmount;
	m_zAngle += zAmount;

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (check_collision(root_node) != nullptr)
	{
		// if collision restore state
		m_xAngle = old_x;
		m_yAngle = old_y;
		m_zAngle = old_z;
		return true;
	}

	return false;
}

bool SceneNode::IncScale(float scaleAmount, SceneNode* root_node)
{

	float old_scale = m_scale;

	m_scale += scaleAmount;
	

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (check_collision(root_node) != nullptr)
	{
		// if collision restore state
		m_scale = old_scale;
		return true;
	}

	return false;
}

bool SceneNode::LookAt_XZ(float xWorld, float zWorld, SceneNode* root_node)
{
	float dx, dz;
	dx = xWorld - m_x;
	dz = zWorld - m_z;

	float old_y = m_yAngle;
	m_yAngle = atan2(dx, dz)*(180.0 / XM_PI);


	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (check_collision(root_node) != nullptr)
	{
		// if collision restore state
		m_yAngle = old_y;
		return true;
	}

	return false;
}

bool SceneNode::LookAt_XYZ(float xWorld, float yWorld, float zWorld, SceneNode* root_node)
{
	float dx, dy, dz;
	dx = xWorld - m_x;
	dy = yWorld - m_y;
	dz = zWorld - m_z;
	m_lookAt = XMVectorSet(dx, dy, dz, 0);
	//m_xAngle = atan2(dy, dx)*(180.0 / XM_PI);// *180.0 / XM_PI;
	
	
	float pyth = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

	float old_x = m_xAngle;
	float old_y = m_yAngle;
	m_xAngle = -atan2(dy, pyth)*180.0 / XM_PI;
	m_yAngle = atan2(dx, dz)*(180.0 / XM_PI);

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (check_collision(root_node) != nullptr)
	{
		// if collision restore state
		m_xAngle = old_x;
		m_yAngle = old_y;
		return true;
	}

	return false;
}

bool SceneNode::MoveForward(float distance, SceneNode* root_node)
{
	float old_x = m_x;
	float old_z = m_z;

	m_x += sin(m_yAngle* (XM_PI / 180.0))*distance;
	m_z += cos(m_yAngle* (XM_PI / 180.0))*distance;

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);
	SceneNode* nodeToCheck = check_collision(root_node);
	//check for collision of this node (and children) against all other nodes
	if (nodeToCheck != nullptr)
	{
		if (nodeToCheck->moveable)
		{
			nodeToCheck->MoveForward(-distance, root_node);
		}
		else
		{
			// if collision restore state
			m_x = old_x;
			m_z = old_z;
		}
		return true;
	}

	return false;
}

bool SceneNode::MoveForwardIncY(float distance, SceneNode* root_node)
{
	SetVelocity(XMVectorSet(distance*m_lookAt.x, distance*m_lookAt.y, distance*m_lookAt.z, 0));
	float old_x = m_x;
	float old_y = m_y;
	float old_z = m_z;

	
	//m_x += (sin(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0)))*m_pDeltaTime->GetDeltaTime();
	//m_y += (-sin(m_xAngle * (XM_PI / 180.0)) * distance)*m_pDeltaTime->GetDeltaTime();
	//m_z += (cos(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0)))*m_pDeltaTime->GetDeltaTime();

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);
	SceneNode* nodeToCheck = check_collision(root_node);
	//check for collision of this node (and children) against all other nodes
	if (nodeToCheck != nullptr)
	{
		if (nodeToCheck->isMoveable())
		{
			if (distance > 0) distance *= -1.0f;
			XMVECTOR dir = nodeToCheck->GetPos() - GetPos();
			//nodeToCheck->IncPos(dir.x*distance, dir.y*distance, dir.z*distance, root_node);
			nodeToCheck->SetVelocity(nodeToCheck->GetVelocity() + (GetVelocity()*m_scale));
			m_x = old_x;
			m_y = old_y;
			m_z = old_z;
		}
		else
		{
			// if collision restore state
			m_x = old_x;
			m_y = old_y;
			m_z = old_z;
		}
		return true;
	}

	return false;
}

vector<SceneNode*> SceneNode::GetChildren()
{
	return m_children;
}

XMVECTOR SceneNode::get_world_centre_position()
{
	return XMVectorSet(m_world_centre_x, m_world_centre_y, m_world_centre_z, 0.0f);
}

void SceneNode::update_collision_tree(XMMATRIX * world, float scale)
{
	// the local_world matrix will be used to calc the local transformation for this node
	m_local_world_matrix = XMMatrixIdentity();

	m_local_world_matrix = XMMatrixScaling(m_scale, m_scale, m_scale);
	m_local_world_matrix *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	m_local_world_matrix *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	m_local_world_matrix *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	m_local_world_matrix *= XMMatrixTranslation(m_x, m_y, m_z);
	// the local matrix is multiplied by the passed in world matrix that contains the concatentated
	//transformations of all parent nodes so that this nodes transformations are relative to those 
	m_local_world_matrix *= *world;

	// calc the world space scale of this object, is needed to calculate the
	//correct bounding sphere radius of an object in a scaled hierarchy
	m_world_scale = scale * m_scale;

	XMVECTOR v;
	if (m_p_model)
	{
		v = XMVectorSet(m_p_model->GetBoundingSphereWorldSpacePosition().x, m_p_model->GetBoundingSphereWorldSpacePosition().y, m_p_model->GetBoundingSphereWorldSpacePosition().z, 0.0f);
	}
	else
	{
		v = XMVectorSet(0, 0, 0, 0); // no model, default to 0
	}

	// find and store world space bounding sphere centre 
	v = XMVector3Transform(v, m_local_world_matrix);
	m_world_centre_x = XMVectorGetX(v);
	m_world_centre_y = XMVectorGetY(v);
	m_world_centre_z = XMVectorGetZ(v);

	//traverse all child nodes, passing in the concatentated world matrix and scale
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->update_collision_tree(&m_local_world_matrix, m_world_scale);
	}
}

SceneNode* SceneNode::check_collision(SceneNode * compare_tree)
{
	return check_collision(compare_tree, this);
}

SceneNode* SceneNode::check_collision(SceneNode * compare_tree, SceneNode * object_tree_root)
{
	//check to see if root of tree being compared is same as root node of object tree being checked
	//i.e. stop object node and children being checked against each other
	if (object_tree_root == compare_tree)return nullptr;

	//only check for collisions if both nodes contain a model
	if (m_p_model && compare_tree->m_p_model)
	{
		XMVECTOR v1 = get_world_centre_position();
		XMVECTOR v2 = compare_tree->get_world_centre_position();
		XMVECTOR vdiff = v1 - v2;

		//XMVECTOR a = XMVector3Length(vdiff);
		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		//check bounding sphere collision
		if (sqrt(dx*dx + dy * dy + dz * dz) <
			(compare_tree->m_p_model->GetBoundingSphereRaius()*compare_tree->m_world_scale) +
			(this->m_p_model->GetBoundingSphereRaius() *m_world_scale))
		{

			return compare_tree;
		}
	}

	//iterate through compared trees child nodes
	for (int i = 0; i < compare_tree->m_children.size(); i++)
	{
		//check for collision agaisnt all compared tree child nodes
		if (check_collision(compare_tree->m_children[i], object_tree_root) != nullptr) return compare_tree->m_children[i];
	}

	//iterate through composite object child nodes
	for (int i = 0; i < m_children.size(); i++)
	{
		//check all the children node of the composite object against compared tree
		if (m_children[i]->check_collision(compare_tree, object_tree_root) != nullptr)	return m_children[i];
	}

	return nullptr;
}

SceneNode* SceneNode::check_collision_ray(XMVECTOR ray_position, XMVECTOR direction_ray)
{
	if (m_p_model)
	{
		float distance = sqrt(pow(ray_position.x - m_world_centre_x, 2.0f) + pow(ray_position.y - m_world_centre_y, 2.0f) + pow(ray_position.z - m_world_centre_z, 2.0f));
		float rayLength = sqrt(pow((ray_position.x+direction_ray.x) - ray_position.x, 2.0f) + pow((ray_position.y + direction_ray.y) - ray_position.y, 2.0f) + pow((ray_position.z + direction_ray.z) - ray_position.z, 2.0f));
		 
		float sumOfRadiAndRayLength = (this->m_p_model->GetBoundingSphereRaius()*m_world_scale) + rayLength;

		if (distance < sumOfRadiAndRayLength)
		{
			for (int i = 0; i < m_p_model->GetObjectA()->numverts; i+=3)
			{
				XMVECTOR p1 = XMVectorSet(m_p_model->GetObjectA()->vertices[i].Pos.x,
					m_p_model->GetObjectA()->vertices[i].Pos.y,
					m_p_model->GetObjectA()->vertices[i].Pos.z, 0.0f);
				XMVECTOR p2 = XMVectorSet(m_p_model->GetObjectA()->vertices[i+1].Pos.x,
					m_p_model->GetObjectA()->vertices[i+1].Pos.y,
					m_p_model->GetObjectA()->vertices[i+1].Pos.z, 0.0f);
				XMVECTOR p3 = XMVectorSet(m_p_model->GetObjectA()->vertices[i + 2].Pos.x,
					m_p_model->GetObjectA()->vertices[i + 2].Pos.y,
					m_p_model->GetObjectA()->vertices[i + 2].Pos.z, 0.0f);
			}
		}
		
	}
	return nullptr;
}

bool SceneNode::isMoveable()
{
	return moveable;
}

void SceneNode::MoveForwardIncYNoCollisions(float distance, SceneNode* root_node)
{
	float old_x = m_x;
	float old_y = m_y;
	float old_z = m_z;

	m_x += (sin(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0)))*m_pDeltaTime->GetDeltaTime();
	m_y += (-sin(m_xAngle * (XM_PI / 180.0)) * distance)*m_pDeltaTime->GetDeltaTime();
	m_z += (cos(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0)))*m_pDeltaTime->GetDeltaTime();

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);
	//check for collision of this node (and children) against all other nodes

}

void SceneNode::AddVelocity(SceneNode* root_node)
{
	SetPos(GetPos() + (velocity * m_pDeltaTime->GetDeltaTime()));

	XMMATRIX identity = XMMatrixIdentity();

	// update tree to reflect new camera postiion
	update_collision_tree(&identity, 1.0f);

	SceneNode* nodeToCheck = check_collision(root_node);
	//check for collision of this node (and children) against all other nodes
	if (nodeToCheck != nullptr)
	{
		if (nodeToCheck->isMoveable())
		{
			XMVECTOR dir = nodeToCheck->GetPos() - GetPos();
			//nodeToCheck->IncPos(dir.x*moveAmount, dir.y*moveAmount, dir.z*moveAmount, g_root_node);
			nodeToCheck->SetVelocity(nodeToCheck->GetVelocity() + (GetVelocity()*m_scale) );
			//if there is a collision, restore camera and camera node positions
			SetVelocity(GetVelocity()/(VELOCITY_IMPACT_FACTOR/m_scale));
		}
		else
		{			
			SetVelocity(XMVectorZero());
		}
	}

}

void SceneNode::GetLookAt()
{
	//normalise the look at==============================================================================================================
}
