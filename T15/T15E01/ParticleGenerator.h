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
using namespace std;

struct Particle
{
	float gravity;
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT4 color;
};

class ParticleGenerator
{
public:
	ParticleGenerator(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext,bool shiney);
	~ParticleGenerator();
	HRESULT LoadObjModel(char* filename, float xpos, float ypos, float zpos, char* textureName);
	void Draw(XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraposition);
	void SetPos(float xpos, float ypos, float zpos);
	XMVECTOR GetPos();
	void IncPos(float xAmount, float yAmount, float zAmount);
	void IncRotation(float xAmount, float yAmount, float zAmount);
	void IncScale(float scaleAmount);
	HRESULT AddTexture();
	void LookAt_XZ(float xWorld, float zWorld);
	void LookAt_XYZ(float xWorld,float yWorld, float zWorld);

	void MoveForward(float distance);
	void MoveForwardIncY(float distance);
	int CreateParticle();


private:
	HRESULT SetSamplerState();
	HRESULT CompileShaders();
	void DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraposition);

private:
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
	//Point Light
	//XMVECTOR g_point_light_position;
	//XMVECTOR g_point_light_colour;

	float			m_x, m_y, m_z;
	float			m_xAngle, m_yAngle, m_zAngle;
	float			m_scale;
	float			m_pTexture;
	char*			m_textureName;
	bool isShiney;
	float m_timePrevious;
	float m_untilParticle;
	float RandomZeroToOne();
	float RandomNegOneToPosOne();

	list<Particle*> m_free;
	list<Particle*> m_active;
};

