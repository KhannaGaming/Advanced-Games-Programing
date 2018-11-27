#pragma once
#include "objfilemodel.h"

class Model
{
public:
	Model(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext);
	~Model();
	HRESULT LoadObjModel(char* filename, float xpos, float ypos, float zpos);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void SetPos(float xpos, float ypos, float zpos);
	XMVECTOR GetPos();
	void IncPos(float xAmount, float yAmount, float zAmount);
	void IncRotation(float xAmount, float yAmount, float zAmount);
	void IncScale(float scaleAmount);
	HRESULT AddTexture(char* filename);

private:
	HRESULT SetSamplerState();

private:
	ID3D11Device*		m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;
	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;
	ObjFileModel*		m_pObject;
	ID3D11VertexShader*	m_pVShader;
	ID3D11PixelShader*	m_pPShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pConstantBuffer;
	XMVECTOR g_directional_light_shines_from;
	XMVECTOR g_directional_light_colour;
	XMVECTOR g_ambient_light_color;
	//Point Light
	//XMVECTOR g_point_light_position;
	//XMVECTOR g_point_light_colour;

	float			m_x, m_y, m_z;
	float			m_xAngle, m_yAngle, m_zAngle;
	float			m_scale;
	float			m_pTexture;
};

