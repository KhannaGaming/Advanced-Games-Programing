#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include "DeltaTime.h"

using namespace std;

struct Particle
{
	float gravity;
	XMFLOAT3 position;
	XMFLOAT3 m_velocity;
	XMFLOAT4 color;
	float age;
};

enum typeOfPartical { ENGINE_PARTICLE };

class ParticleGenerator
{
public:
	//***************************************************
	//METHODS
	//***************************************************
	ParticleGenerator(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext,bool shiney, DeltaTime* deltaTime);
	~ParticleGenerator();
	HRESULT LoadObjModel(char* textureName);
	void Draw(XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraposition);
	void SetPos(XMVECTOR position);
	void SetRot(XMVECTOR position);
	XMVECTOR GetPos();
	void IncPos(float xAmount, float yAmount, float zAmount);
	void IncRotation(float xAmount, float yAmount, float zAmount);
	void IncScale(float scaleAmount);
	HRESULT AddTexture(char* textureName);
	void LookAt_XZ(float xWorld, float zWorld);
	void LookAt_XYZ(float xWorld,float yWorld, float zWorld);
	void MoveForward(float distance);
	void MoveForwardIncY(float distance);
	int CreateParticle();


private:
	//***************************************************
	//METHODS
	//***************************************************
	HRESULT SetSamplerState();
	float	RandomZeroToOne();
	float	RandomNegOneToPosOne();

private:
	//***************************************************
	//POINTERS
	//***************************************************
	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11RasterizerState*		m_pRasterSolid = 0; 
	ID3D11RasterizerState*		m_pRasterParticle = 0;
	ID3D11Buffer*				m_pVertexBuffer;
	DeltaTime*					m_pDeltaTime;
	ID3D11BlendState*			m_pAlphaBlendEnable;
	ID3D11BlendState*			m_pAlphaBlendDisable;

	//***************************************************
	//VARIABLES
	//***************************************************
	typeOfPartical				m_particalType;
	list<Particle*>				m_free;
	list<Particle*>				m_active;
	list<Particle*>::iterator	it;//iteration list for pointing to the correct particle in the list
	float						m_x, m_y, m_z;
	float						m_xAngle, m_yAngle, m_zAngle;
	float						m_scale;
	float						m_pTexture;
	float						m_timePrevious;
	float						m_untilParticle;
	float						m_age;
	bool						m_isShiney;
	bool						m_isActive;
	char*						m_textureName;
};

