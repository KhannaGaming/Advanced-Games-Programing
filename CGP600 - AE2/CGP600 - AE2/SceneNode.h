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
#include "Model.h"
#include"camera.h"
#include "DeltaTime.h"
#include "SkyBox.h"
#include "AudioManager.h"
#include "Tags.h"

#define VELOCITY_IMPACT_FACTOR 5
#define LASER_SPEED 80.0f
#define MAX_VELOCITY 10

class SceneNode
{
public:
	//*************************
	//METHODS
	//*************************
	SceneNode(DeltaTime* deltaTime, bool collidable, string tag, SceneNode* world_root_node, AudioManager* AudioManager, float moveSpeed);
	~SceneNode();
	XMVECTOR			GetPos();
	XMVECTOR			GetWorldCentrePosition();
	XMVECTOR			GetVelocity() { return m_velocity; };
	XMVECTOR			GetLookAt(XMVECTOR direction);
	XMVECTOR			GetOffset() { return m_child_offset; };
	XMVECTOR			GetRotation();
	SceneNode*			CheckCollision(SceneNode* compare_tree);
	SceneNode*			CheckCollision(SceneNode* compare_tree, SceneNode* object_tree_root);
	SceneNode*			CheckCollisionRay(XMVECTOR ray_position, XMVECTOR direction_ray);
	SceneNode*			GetOriginalParentNode() { return m_original_parent_node; };
	string				GetTag() { return m_tag; };
	vector<SceneNode*>	GetChildren();
	void				CreateLasers(Model* model);
	void				SetRotation(float xAngle, float yAngle, float zAngle);
	void				SetScale(float scale);
	void				SetModel(Model*model);
	void				SetCamera(camera*Camera);
	void				SetPos(XMVECTOR pos);
	void				AddChildNode(SceneNode* n);
	void				Execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, SceneNode* root_node);
	void				UpdateCollisionTree(XMMATRIX* world, float scale);
	void				MoveForwardIncYNoCollisions(float distance, SceneNode* root_node);
	void				SetVelocity(XMVECTOR amount) { m_velocity = amount; };
	void				AddVelocity(SceneNode* root_node);
	void				Update(SkyBox* skybox, int& score, SceneNode* rootNode);
	void				Activate(bool isEnabled);
	void				SetOffset(XMVECTOR childOffset);
	void				FireLaser(SceneNode* detachingNode, SceneNode* root_node);
	void				SetOriginalParentNode(SceneNode* original_parent_node);
	void				ResetNode(SceneNode* node);
	void				SetMaxHealth();
	void				AI(SkyBox* skybox, int& score, SceneNode* player, SceneNode* root_node);
	float				GetScale();
	float				GetHealth() { return m_health; };
	bool				IsMoveable();
	bool				IncPos(float xAmount, float yAmount, float zAmount, SceneNode* root_node);
	bool				IncRotation(float xAmount, float yAmount, float zAmount, SceneNode* root_node);
	bool				IncScale(float scaleAmount, SceneNode* root_node);
	bool				LookAt_XZ(float xWorld, float zWorld, SceneNode* root_node);
	bool				LookAt_XYZ(XMVECTOR world, SceneNode* root_node);
	bool				MoveForward(float distance, SceneNode* root_node);
	bool				MoveForwardIncY(SceneNode* root_node);
	bool				DetachNode(SceneNode* n);	

private:
	//**************************
	//METHODS
	//**************************
	int			RandomNumberGenerator(int maxDistance);
	float		Pythagoras(XMVECTOR v);
	float		Pythagoras(XMVECTOR v1, XMVECTOR v2);
	bool		CheckChildrenPosition(SceneNode* nodeToCheck, SceneNode* rootNode);
	void		CheckVelocities();
	bool		CheckForLaser(SceneNode* compare_tree, SceneNode* object_tree_root);

private:
	//**************************
	//POINTERS
	//**************************
	Model*				m_p_model;
	camera*				m_p_camera;
	DeltaTime*			m_pDeltaTime;
	vector<SceneNode*>	m_children;
	AudioManager*		m_audio_manager;
	SceneNode*			m_world_root_node;
	SceneNode*			m_original_parent_node;

	//**************************
	//VECTORS
	//**************************
	vector<SceneNode*>	m_vLasers;

	//**************************
	//VARIABLES
	//**************************
	float		m_x, m_y, m_z;
	float		m_xAngle, m_yAngle, m_zAngle;
	float		m_scale;
	float		m_world_centre_x, m_world_centre_y, m_world_centre_z;
	float		m_world_scale;
	float		m_health;
	float		m_max_health;
	float		m_laser_life;
	float		m_cur_laser_life;	
	float		m_shoot_cooldown;
	float		m_cur_shoot_cooldown;
	float		m_move_speed;
	bool		m_isActive;
	bool		m_moveable;
	bool		m_collidable;
	XMVECTOR	m_lookAt;
	XMMATRIX	m_local_world_matrix;
	XMVECTOR	m_velocity;
	XMVECTOR	m_child_offset;
	string		m_tag;
};

