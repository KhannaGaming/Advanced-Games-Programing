#include "SceneNode.h"



SceneNode::SceneNode(DeltaTime* deltaTime, bool collidable, string tag, SceneNode* world_root_node, AudioManager* audioManager, float moveSpeed)
{
	m_p_model = nullptr;
	m_p_camera = nullptr;
	m_x = m_y = m_z = m_xAngle = m_yAngle = m_zAngle = 0.0f;
	m_scale = 1.0f;
	m_velocity = XMVectorZero();
	m_pDeltaTime = deltaTime;
	m_collidable = collidable;
	m_tag = tag;
	m_max_health = 10.0f;
	m_health = 10.0f;
	m_isActive = true;
	m_world_root_node = world_root_node;
	m_laser_life = 1.0f;
	m_cur_laser_life = 0.0f;
	m_audio_manager = audioManager;
	m_move_speed = moveSpeed;
}


SceneNode::~SceneNode()
{
	m_world_root_node = nullptr;

	m_original_parent_node = nullptr;

	m_audio_manager = nullptr;

	m_pDeltaTime = nullptr;

	int maxChildSize = m_children.size();
	for (int i = 0; i < maxChildSize; i++)
	{
		delete m_children[i];
		m_children[i] = nullptr;
	}
	
	m_p_model = nullptr;
	
	for (int i = 0; i < maxChildSize; i++)
	{
		 m_children.pop_back();
	}
}

void SceneNode::SetModel(Model * model)
{
	m_p_model = model;
	m_moveable = m_p_model->isModelMoveable();
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

void SceneNode::AddChildNode(SceneNode * n)
{
	m_children.push_back(n);
}

bool SceneNode::DetachNode(SceneNode * n)
{
	//traverse tree to find node to detach
	for (size_t i = 0; i < m_children.size(); i++)
	{
		if (n == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			n->SetPos(this->GetPos() + n->GetOffset());
			return true;
		}
		if (m_children[i]->DetachNode(n) == true) return true;
	}
	return false; // node not in this tree
}

void SceneNode::Execute(XMMATRIX * world, XMMATRIX * view, XMMATRIX * projection, SceneNode* root_node)
{
	if (m_isActive)
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
		for (size_t i = 0; i < m_children.size(); i++)
		{
			m_children[i]->Execute(&local_world, view, projection, root_node);
		}
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
	root_node->UpdateCollisionTree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (CheckCollision(root_node) != nullptr)
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
	root_node->UpdateCollisionTree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (CheckCollision(root_node) != nullptr)
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
	m_yAngle = atan2(dx, dz)*(180.0f / XM_PI);


	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (CheckCollision(root_node) != nullptr)
	{
		// if collision restore state
		m_yAngle = old_y;
		return true;
	}

	return false;
}

bool SceneNode::LookAt_XYZ(XMVECTOR world, SceneNode* root_node)
{
	float dx, dy, dz;
	dx = world.x - m_x;
	dy = world.y - m_y;
	dz = world.z - m_z;
	m_lookAt = GetLookAt(XMVectorSet(dx, dy, dz, 0));
	
	//m_xAngle = atan2(dy, dx)*(180.0 / XM_PI);// *180.0 / XM_PI;
	
	
	float pyth = Pythagoras(XMVectorSet(dx, dy, dz, 0));

	float old_x = m_xAngle;
	float old_y = m_yAngle;
	m_xAngle = -atan2(dy, pyth)*180.0f / XM_PI;
	m_yAngle = atan2(dx, dz)*(180.0f / XM_PI);

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0f);

	//check for collision of this node (and children) against all other nodes
	if (CheckCollision(root_node) != nullptr)
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

	m_x += sin(m_yAngle* (XM_PI / 180.0f))*distance;
	m_z += cos(m_yAngle* (XM_PI / 180.0f))*distance;

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0f);
	SceneNode* nodeToCheck = CheckCollision(root_node);
	//check for collision of this node (and children) against all other nodes
	if (nodeToCheck != nullptr)
	{
		if (nodeToCheck->m_moveable)
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

bool SceneNode::MoveForwardIncY(SceneNode* root_node)
{
	SetVelocity(XMVectorSet(m_move_speed * m_lookAt.x, m_move_speed * m_lookAt.y, m_move_speed * m_lookAt.z, 0));
	return false;
}

vector<SceneNode*> SceneNode::GetChildren()
{
	return m_children;
}

void SceneNode::CreateLasers(Model * model)
{		
	m_shoot_cooldown = 1.0f;
	m_cur_shoot_cooldown = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		SceneNode* tempLaser = new SceneNode(m_pDeltaTime, false, Tags::EnemyLaser,m_world_root_node, m_audio_manager, 4.0f);
		tempLaser->SetModel(model);
		m_vLasers.push_back(tempLaser);
		AddChildNode(tempLaser);
		tempLaser->SetScale(0.3f);
		tempLaser->Activate(false);
		tempLaser->SetOriginalParentNode(this);
	}
}

XMVECTOR SceneNode::GetWorldCentrePosition()
{
	return XMVectorSet(m_world_centre_x, m_world_centre_y, m_world_centre_z, 0.0f);
}

void SceneNode::UpdateCollisionTree(XMMATRIX * world, float scale)
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
	for (size_t i = 0; i < m_children.size(); i++)
	{
		m_children[i]->UpdateCollisionTree(&m_local_world_matrix, m_world_scale);
	}
}

SceneNode* SceneNode::CheckCollision(SceneNode * compare_tree)
{
	return CheckCollision(compare_tree, this);
}

SceneNode* SceneNode::CheckCollision(SceneNode * compare_tree, SceneNode * object_tree_root)
{
	//check to see if root of tree being compared is same as root node of object tree being checked
	//i.e. stop object node and children being checked against each other
	if (object_tree_root == compare_tree) return nullptr;
	
	//only check for collisions if both nodes contain a model
	if (m_p_model && compare_tree->m_p_model)
	{
		XMVECTOR v1 = GetWorldCentrePosition();
		XMVECTOR v2 = compare_tree->GetWorldCentrePosition();
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
		if (Pythagoras(XMVectorSet(dx, dy, dz, 0)) <
			(compare_tree->m_p_model->GetBoundingSphereRaius()*compare_tree->m_world_scale) +
			(this->m_p_model->GetBoundingSphereRaius() *m_world_scale))
		{
			if (CheckForLaser(compare_tree, object_tree_root))
			{
				return compare_tree;
			}
			else
			{
				return nullptr;
			}
		}
	}

	//iterate through compared trees child nodes
	for (size_t i = 0; i < compare_tree->m_children.size(); i++)
	{
		//check for collision agaisnt all compared tree child nodes
		if (CheckCollision(compare_tree->m_children[i], object_tree_root) != nullptr)
		{
			if (CheckForLaser(compare_tree, object_tree_root))
			{
				return compare_tree->m_children[i];
			}
			else
			{
				return nullptr;
			}
		}
	}

	//iterate through composite object child nodes
	for (size_t i = 0; i < m_children.size(); i++)
	{
		//check all the children node of the composite object against compared tree
		if (m_children[i]->CheckCollision(compare_tree, object_tree_root) != nullptr)
		{
			if (CheckForLaser(compare_tree, object_tree_root))
			{
				return m_children[i];
			}
			else
			{
				return nullptr;
			}
		}
	}
	return nullptr;
}

SceneNode* SceneNode::CheckCollisionRay(XMVECTOR ray_position, XMVECTOR direction_ray)
{
	if (m_p_model)
	{
		float distance = Pythagoras(GetWorldCentrePosition(),ray_position);
		float rayLength = Pythagoras(ray_position, ray_position + direction_ray);
		 
		float sumOfRadiAndRayLength = (this->m_p_model->GetBoundingSphereRaius()*m_world_scale) + rayLength;

		if (distance < sumOfRadiAndRayLength)
		{
			for (unsigned int i = 0; i < m_p_model->GetObjectA()->numverts; i+=3)
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

bool SceneNode::IsMoveable()
{
	return m_moveable;
}

void SceneNode::MoveForwardIncYNoCollisions(float distance, SceneNode* root_node)
{
	float old_x = m_x;
	float old_y = m_y;
	float old_z = m_z;

	m_x += (sin(m_yAngle * (XM_PI / 180.0f)) * distance * cos(m_xAngle * (XM_PI / 180.0f)))*m_pDeltaTime->GetDeltaTime();
	m_y += (-sin(m_xAngle * (XM_PI / 180.0f)) * distance)*m_pDeltaTime->GetDeltaTime();
	m_z += (cos(m_yAngle * (XM_PI / 180.0f)) * distance * cos(m_xAngle * (XM_PI / 180.0f)))*m_pDeltaTime->GetDeltaTime();

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0f);
	//check for collision of this node (and children) against all other nodes

}

void SceneNode::AddVelocity(SceneNode* root_node)
{
	SetPos(GetPos() + (m_velocity * m_pDeltaTime->GetDeltaTime()));

	XMMATRIX identity = XMMatrixIdentity();

	// update tree to reflect new camera postiion
	UpdateCollisionTree(&identity, 1.0f);

	SceneNode* nodeToCheck = CheckCollision(root_node);
	//check for collision of this node (and children) against all other nodes
	if (nodeToCheck != nullptr)
	{
		if (nodeToCheck->IsMoveable())
		{
			XMVECTOR dir = nodeToCheck->GetPos() - GetPos();
			float normalMagnitude = Pythagoras(dir);
			dir = dir / normalMagnitude;
			nodeToCheck->SetVelocity(nodeToCheck->GetVelocity() + (GetVelocity() * m_scale));
			//if there is a collision, restore camera and camera node positions
			SetVelocity(GetVelocity()/(VELOCITY_IMPACT_FACTOR/m_scale));
		}
		else
		{			
			SetVelocity(XMVectorZero());
		}
	}

}

XMVECTOR SceneNode::GetLookAt(XMVECTOR direction)
{
	// Normalise the look at
	float normalMagnitude = Pythagoras(direction);
	direction = direction / normalMagnitude;
	return direction;
}

void SceneNode::Update(SkyBox* skybox, int& score, SceneNode* rootNode)
{
	if (m_tag == Tags::Asteroid)
	{
		if (m_health <= 0)
		{
			bool positionSet = false;
			do
			{
				SetPos(XMVectorSet((float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().y + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), 0.0f));
				if (CheckChildrenPosition(this, rootNode))
				{
					positionSet = true;
					if (Pythagoras(skybox->GetPos(), GetPos()) > 40.0f)
					{
						positionSet = true;
					}
					else
					{
						positionSet = false;
					}
				}
			} while (!positionSet);
			SetVelocity(XMVectorSet((float)RandomNumberGenerator(MAX_VELOCITY) / 10.0f, (float)RandomNumberGenerator(MAX_VELOCITY) / 10.0f, (float)RandomNumberGenerator(MAX_VELOCITY) / 10.0f, 0));
			m_health = m_max_health;
			score += (int)m_max_health;
		}

		if (Pythagoras(skybox->GetPos(), GetPos()) > 300.0f)
		{
			bool positionSet = false;
			do
			{
				SetPos(XMVectorSet((float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().y + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), 0.0f));
				if (CheckChildrenPosition(this, rootNode))
				{
					positionSet = true;
				}
			} while (!positionSet);

			SetVelocity(XMVectorSet((float)RandomNumberGenerator(MAX_VELOCITY) / 10.0f, (float)RandomNumberGenerator(MAX_VELOCITY) / 10.0f, (float)RandomNumberGenerator(MAX_VELOCITY) / 10.0f, 0));
			m_health = m_max_health;
		}

	}

	if (m_tag == Tags::SpaceShip)
	{
		CheckVelocities();

		if (m_health <= 0)
		{
			bool positionSet = false;
			do
			{
				SetPos(XMVectorSet((float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().y + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), 0.0f));
				if (CheckChildrenPosition(this, rootNode))
				{
					positionSet = true;
					if (Pythagoras(skybox->GetPos(), GetPos()) > 40.0f)
					{
						positionSet = true;
					}
					else
					{
						positionSet = false;
					}
				}
			} while (!positionSet);

			m_health = m_max_health;
			score += 100;
		}

		if (Pythagoras(skybox->GetPos(), GetPos()) > 200.0f)
		{
			bool positionSet = false;
			do
			{
				SetPos(XMVectorSet((float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().y + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), 0.0f));
				if (CheckChildrenPosition(this, rootNode))
				{
					positionSet = true;
				}
			} while (!positionSet);
			SetVelocity(XMVectorZero());
			m_health = m_max_health;
		}
	}

	if (m_tag == Tags::Laser)
	{
		if (m_isActive)
		{
			if (m_cur_laser_life > m_laser_life)
			{
				ResetNode(this);
				m_cur_laser_life = 0;
				m_isActive = false;
			}
			m_cur_laser_life += m_pDeltaTime->GetDeltaTime();
		}
	}

	if (m_tag == Tags::EnemyLaser)
	{
		if (m_isActive)
		{
			if (m_cur_laser_life > m_laser_life)
			{
				ResetNode(this);
				m_cur_laser_life = 0;
				m_isActive = false;
			}
			m_cur_laser_life += m_pDeltaTime->GetDeltaTime();
		}
	}

	if (m_tag == Tags::Camera)
	{
		CheckVelocities();

		if (m_health <= 0)
		{
			bool positionSet = false;
			do
			{
				SetPos(XMVectorSet((float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().y + skybox->GetScale())), (float)RandomNumberGenerator((int)(skybox->GetPos().x + skybox->GetScale())), 0.0f));
				if (CheckChildrenPosition(this, rootNode))
				{
					positionSet = true;					
				}
			} while (!positionSet);
			m_p_camera->SetPos(this->GetPos().x, this->GetPos().y, this->GetPos().z);
			m_health = m_max_health;
			SetVelocity(XMVectorZero());
		}
	}
}

void SceneNode::Activate(bool isEnabled)
{
	m_isActive = isEnabled;
}

void SceneNode::SetOffset(XMVECTOR childOffset)
{
	m_child_offset = childOffset;
	SetPos(m_child_offset);
}

void SceneNode::FireLaser(SceneNode * detachingNode, SceneNode * new_parent_node)
{
	m_cur_laser_life = 0;
	DetachNode(detachingNode);
	new_parent_node->AddChildNode(detachingNode);
	if (detachingNode->GetTag() == Tags::Laser)
	{
		detachingNode->SetRotation(-m_p_camera->GetRot().x, m_p_camera->GetRot().y, m_p_camera->GetRot().z);
	}
	else if (detachingNode->GetTag() == Tags::EnemyLaser)
	{
		detachingNode->SetRotation(detachingNode->m_original_parent_node->GetRotation().x, detachingNode->m_original_parent_node->GetRotation().y, detachingNode->m_original_parent_node->GetRotation().z);
	}
	if (detachingNode->GetTag() == Tags::Laser)
	{
		detachingNode->SetVelocity(m_p_camera->GetLookAt() * LASER_SPEED);
	}
	else if (detachingNode->GetTag() == Tags::EnemyLaser)
	{
		detachingNode->SetVelocity(detachingNode->m_original_parent_node->GetLookAt(m_p_camera->GetPos()- detachingNode->m_original_parent_node->GetPos()) * LASER_SPEED);
	}
	m_audio_manager->PlaySoundEffect("Laser");	
}

void SceneNode::SetOriginalParentNode(SceneNode * original_parent_node)
{
	m_original_parent_node = original_parent_node;
}

void SceneNode::ResetNode(SceneNode* node)
{
	m_world_root_node->DetachNode(node);
	node->GetOriginalParentNode()->AddChildNode(node);
	node->SetPos(node->GetOffset());
	node->SetVelocity(XMVectorZero());
	node->Activate(false);
}

void SceneNode::SetMaxHealth()
{
	m_max_health = m_scale;
	m_health = m_max_health;
}

void SceneNode::AI(SkyBox* skybox, int& score, SceneNode * player, SceneNode * root_node)
{
	MoveForwardIncY(root_node);
	if (m_health < 3)
	{
		LookAt_XYZ(this->GetPos() - (player->GetPos() - this->GetPos()), root_node);
	}
	else
	{
		LookAt_XYZ(player->GetPos(), root_node);
		if (m_cur_shoot_cooldown > m_shoot_cooldown)
		{
			for (size_t i = 0; i < GetChildren().size(); i++)
			{
				if (GetChildren()[i]->GetTag() == Tags::EnemyLaser)
				{
					m_cur_shoot_cooldown = 0.0f;
					GetChildren()[i]->Activate(true);
					FireLaser(GetChildren()[i], root_node);
					break;
				}
			}
		}
		m_cur_shoot_cooldown += m_pDeltaTime->GetDeltaTime();
		XMVECTOR dir = player->GetPos() - this->GetPos();
		if (Pythagoras(dir) < 20.0f)
		{
			SetVelocity(XMVectorZero());
		}
	}

	for (size_t i = 0; i < m_vLasers.size(); i++)
	{
		m_vLasers[i]->Update(skybox, score, root_node);
	}
}

int SceneNode::RandomNumberGenerator(int maxDistance)
{
	int randNum = 0;

	do
	{
		randNum = rand() % maxDistance;
		randNum -= maxDistance / 2;
		randNum *= 2;
	} while (randNum < (maxDistance) && randNum > maxDistance);
	randNum += maxDistance;
	return randNum;
}

float SceneNode::Pythagoras(XMVECTOR v)
{	
	return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

float SceneNode::Pythagoras(XMVECTOR v1, XMVECTOR v2)
{
	return sqrt(pow(v2.x- v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

bool SceneNode::CheckChildrenPosition(SceneNode * nodeToCheck, SceneNode * rootNode)
{
	if (Pythagoras(nodeToCheck->GetPos(), rootNode->GetPos()) < 30.0f)
	{
		return false;
	}

	for (size_t i = 0; i < rootNode->GetChildren().size(); i++)
	{
		if (!CheckChildrenPosition(nodeToCheck, rootNode->GetChildren()[i]))
		{
			if (nodeToCheck != rootNode->GetChildren()[i])
			{
				return false;
			}
		}
	}
	return true;
}

void SceneNode::CheckVelocities()
{
	if (m_velocity.x > MAX_VELOCITY)
	{
		m_velocity = XMVectorSetX(m_velocity, MAX_VELOCITY);
	}
	else if (m_velocity.x < -MAX_VELOCITY)
	{
		m_velocity = XMVectorSetX(m_velocity, -MAX_VELOCITY);
	}

	if (m_velocity.y > MAX_VELOCITY)
	{
		m_velocity = XMVectorSetY(m_velocity, MAX_VELOCITY);
	}
	else if (m_velocity.y < -MAX_VELOCITY)
	{
		m_velocity = XMVectorSetY(m_velocity, -MAX_VELOCITY);
	}

	if (m_velocity.z > MAX_VELOCITY)
	{
		m_velocity = XMVectorSetZ(m_velocity, MAX_VELOCITY);
	}
	else if (m_velocity.z < -MAX_VELOCITY)
	{
		m_velocity = XMVectorSetZ(m_velocity, -MAX_VELOCITY);
	}
}

bool SceneNode::CheckForLaser(SceneNode* compare_tree, SceneNode* object_tree_root)
{
	if (!compare_tree->m_collidable)
	{
		if (compare_tree->m_tag == Tags::Laser)
		{
			if ((object_tree_root->m_tag != Tags::Laser && object_tree_root->m_tag !=Tags::EnemyLaser)&& object_tree_root->m_tag != Tags::Camera)
			{
				if (compare_tree->m_isActive)
				{
					object_tree_root->m_health--;
					if (object_tree_root->m_tag == Tags::SpaceShip)
					{
						m_audio_manager->PlaySoundEffect("SpaceshipExplosion");
					}
					else
					{
						m_audio_manager->PlaySoundEffect("Explosion");
					}
					compare_tree->ResetNode(compare_tree);
				}
			}
		}
		else if (compare_tree->m_tag == Tags::EnemyLaser)
		{
			if ((object_tree_root->m_tag != Tags::EnemyLaser && object_tree_root->m_tag != Tags::Laser) && object_tree_root->m_tag != Tags::SpaceShip)
			{
				if (compare_tree->m_isActive)
				{
					object_tree_root->m_health--;
					if (object_tree_root->m_tag == Tags::Camera)
					{
						m_audio_manager->PlaySoundEffect("SpaceshipExplosion");
					}
					else
					{
						m_audio_manager->PlaySoundEffect("Explosion");
					}
					compare_tree->ResetNode(compare_tree);
				}
			}
		}
		return false;
	}
	else if (!object_tree_root->m_collidable)
	{
		if (object_tree_root->m_tag == Tags::Laser)
		{
			if ((compare_tree->m_tag != Tags::EnemyLaser && compare_tree->m_tag != Tags::Laser) && compare_tree->m_tag != Tags::Camera)
			{
				if (object_tree_root->m_isActive)
				{
					compare_tree->m_health--;
					if (compare_tree->m_tag == Tags::SpaceShip)
					{
						m_audio_manager->PlaySoundEffect("SpaceshipExplosion");
					}
					else
					{
						m_audio_manager->PlaySoundEffect("Explosion");
					}
					object_tree_root->ResetNode(object_tree_root);
				}
			}
		}
		else if (object_tree_root->m_tag == Tags::EnemyLaser)
		{
			if ((compare_tree->m_tag != Tags::EnemyLaser && compare_tree->m_tag != Tags::Laser) && compare_tree->m_tag != Tags::SpaceShip)
			{
				if (object_tree_root->m_isActive)
				{
					compare_tree->m_health--;
					if (compare_tree->m_tag == Tags::Camera)
					{
						m_audio_manager->PlaySoundEffect("SpaceshipExplosion");
					}
					else
					{
						m_audio_manager->PlaySoundEffect("Explosion");
					}
					object_tree_root->ResetNode(object_tree_root);
				}
			}
		}
		return false;
	}
	return true;
}
