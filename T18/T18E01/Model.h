#pragma once
#include "objfilemodel.h"
#include "LightManager.h"

class Model
{
public:
	Model(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext,bool shiney);
	~Model();
	HRESULT LoadObjModel(char* filename, char* textureName);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	XMVECTOR GetPos();
	void IncPos(float xAmount, float yAmount, float zAmount);
	void IncRotation(float xAmount, float yAmount, float zAmount);
	void IncScale(float scaleAmount);
	HRESULT AddTexture();
	void LookAt_XZ(float xWorld, float zWorld);
	void LookAt_XYZ(float xWorld,float yWorld, float zWorld);

	void MoveForward(float distance);
	void MoveForwardIncY(float distance);
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRaius();
	bool CheckCollision(Model* modelToCompare);


private:
	HRESULT SetSamplerState();
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

private:
	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;
	ObjFileModel*				m_pObject;
	LightManager*				m_pLightManager;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	XMVECTOR					g_directional_light_shines_from;
	XMVECTOR					g_directional_light_colour;
	XMVECTOR					g_ambient_light_color;
	
	//Point Light
	//XMVECTOR g_point_light_position;
	//XMVECTOR g_point_light_colour;

	float			m_x, m_y, m_z;
	float			m_xAngle, m_yAngle, m_zAngle;
	float			m_scale;
	float			m_pTexture;
	char*			m_textureName;
	XMVECTOR m_bounding_sphere_centre;
	float m_bounding_sphere_radius;
	bool isShiney;
};

