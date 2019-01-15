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
#define VELOCITY_IMPACT_FACTOR 5
#include "DeltaTime.h"
#include "Tags.h"
#include "SkyBox.h"
#include "AudioManager.h"

class SceneNode
{
public:
	SceneNode(DeltaTime* deltaTime, bool collidable, string tag, SceneNode* world_root_node, AudioManager* AudioManager);
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
	void execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, SceneNode* root_node);
	bool IncPos(float xAmount, float yAmount, float zAmount, SceneNode* root_node);
	bool IncRotation(float xAmount, float yAmount, float zAmount, SceneNode* root_node);
	bool IncScale(float scaleAmount, SceneNode* root_node);
	bool LookAt_XZ(float xWorld, float zWorld, SceneNode* root_node);
	bool LookAt_XYZ(XMVECTOR world, SceneNode* root_node);
	bool MoveForward(float distance, SceneNode* root_node);
	bool MoveForwardIncY(float distance, SceneNode* root_node);
	vector<SceneNode*> GetChildren();
	XMVECTOR get_world_centre_position();
	void update_collision_tree(XMMATRIX* world, float scale);
	SceneNode* check_collision(SceneNode* compare_tree);
	SceneNode* check_collision(SceneNode* compare_tree, SceneNode* object_tree_root);//15
	SceneNode* check_collision_ray(XMVECTOR ray_position, XMVECTOR direction_ray);
	bool isMoveable();
	void MoveForwardIncYNoCollisions(float distance, SceneNode* root_node);
	XMVECTOR GetVelocity() { return velocity; };
	void SetVelocity(XMVECTOR amount) { velocity = amount; };
	void AddVelocity(SceneNode* root_node);
	XMVECTOR GetLookAt(XMVECTOR direction);
	void Update(SkyBox* skybox, int& score);
	void Activate(bool isEnabled);
	void SetOffset(XMVECTOR childOffset);
	XMVECTOR GetOffset() { return m_child_offset; };
	void FireLaser(SceneNode* detachingNode, SceneNode* root_node);
	string GetTag() { return m_tag; };
	void SetOriginalParentNode(SceneNode* original_parent_node);
	SceneNode* GetOriginalParentNode() { return m_original_parent_node; };
	void ResetNode(SceneNode* node);
	void SetMaxHealth();

private:
	float RandomNumberGenerator(int maxDistance);

private:
	Model* m_p_model;
	camera* m_p_camera;
	vector<SceneNode*> m_children;
	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	float m_world_centre_x, m_world_centre_y, m_world_centre_z;
	float m_world_scale;
	XMVECTOR m_lookAt;
	XMMATRIX m_local_world_matrix;
	bool moveable;
	XMVECTOR velocity;
	DeltaTime* m_pDeltaTime;
	bool m_collidable;
	string m_tag;
	float m_health;
	float m_max_health;
	bool m_isActive;
	XMVECTOR m_child_offset;
	SceneNode* m_world_root_node;
	SceneNode* m_original_parent_node;
	float m_laser_life;
	float m_cur_laser_life;
	AudioManager* m_audio_manager;
};

