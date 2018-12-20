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
	void SetPos(float xpos, float ypos, float zpos);
	XMVECTOR GetPos();
	void SetRotation(float xAngle, float yAngle, float zAngle);
	XMVECTOR GetRotation();
	void SetScale(float scale);
	float GetScale();
	void addChildNode(SceneNode* n);
	bool detachNode(SceneNode* n);
	void execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection);
	void IncPos(float xAmount, float yAmount, float zAmount);
	void IncRotation(float xAmount, float yAmount, float zAmount);
	void IncScale(float scaleAmount);
	void LookAt_XZ(float xWorld, float zWorld);
	void LookAt_XYZ(float xWorld, float yWorld, float zWorld);
	void MoveForward(float distance);
	void MoveForwardIncY(float distance);
	vector<SceneNode*> GetChildren();
	XMVECTOR get_world_centre_position();
private:
	Model* m_p_model;
	camera* m_p_camera;
	vector<SceneNode*> m_children;
	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	float m_world_centre_x, m_world_centre_y, m_world_centre_z;

};

