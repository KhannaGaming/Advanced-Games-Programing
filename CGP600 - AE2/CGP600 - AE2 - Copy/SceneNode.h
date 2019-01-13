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

class SceneNode
{
public:
	SceneNode();
	~SceneNode();
	void SetModel(Model*model);
	void SetCamera(camera*Camera);
	void SetPos(XMVECTOR pos);
	XMVECTOR GetPos();
	void SetRotation(float xAngle, float yAngle, float zAngle);
	XMVECTOR GetRotation();
	void SetScale(float scale);
	float GetScale();
	void addChildNode(SceneNode* n);
	bool detachNode(SceneNode* n);
	void execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection);
	bool IncPos(float xAmount, float yAmount, float zAmount, SceneNode* root_node, vector<SceneNode*> nodesAlreadymoved);
	bool IncRotation(float xAmount, float yAmount, float zAmount, SceneNode* root_node, vector<SceneNode*> nodesAlreadymoved);
	bool IncScale(float scaleAmount, SceneNode* root_node, vector<SceneNode*> nodesAlreadymoved);
	bool LookAt_XZ(float xWorld, float zWorld, SceneNode* root_node);
	bool LookAt_XYZ(float xWorld, float yWorld, float zWorld, SceneNode* root_node);
	bool MoveForward(float distance, SceneNode* root_node);
	bool MoveForwardIncY(float distance, SceneNode* root_node);
	vector<SceneNode*> GetChildren();
	XMVECTOR get_world_centre_position();
	void update_collision_tree(XMMATRIX* world, float scale);
	SceneNode* check_collision(SceneNode* compare_tree, vector<SceneNode*> nodesAlreadyMoved);
	SceneNode* check_collision(SceneNode* compare_tree, SceneNode* object_tree_root, vector<SceneNode*> nodesAlreadyMoved);//15
	SceneNode* check_collision_ray(XMVECTOR ray_position, XMVECTOR direction_ray);
	bool isMoveable();
	void MoveForwardIncYNoCollisions(float distance, SceneNode* root_node);

private:
	Model* m_p_model;
	camera* m_p_camera;
	vector<SceneNode*> m_children;
	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	float m_world_centre_x, m_world_centre_y, m_world_centre_z;
	float m_world_scale;
	XMMATRIX m_local_world_matrix;
	bool moveable;
};

