#pragma once
#include "objfilemodel.h"
#include "LightManager.h"

class Model
{
public:
	//***************************************************
	//METHODS
	//***************************************************
	Model(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext, bool shiney, bool m_moveable);
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
	void LookAt_XYZ(float xWorld, float yWorld, float zWorld);
	void MoveForward(float distance);
	void MoveForwardIncY(float distance);
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRaius();
	ObjFileModel* GetObject();
	bool isModelMoveable();

private:
	//***************************************************
	//METHODS
	//***************************************************
	HRESULT SetSamplerState();
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();
	float Pythagoras(XMVECTOR v1, XMVECTOR v2);

private:
	//***************************************************
	//POINTERS
	//***************************************************
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

	//***************************************************
	//VARIABLES
	//***************************************************
	float		m_x, m_y, m_z;
	float		m_xAngle, m_yAngle, m_zAngle;
	float		m_scale;
	float		m_bounding_sphere_radius;
	bool		m_isShiney;
	bool		m_isMoveable;
	char*		m_textureName;
	XMVECTOR	m_directional_light_shines_from;
	XMVECTOR	m_directional_light_colour;
	XMVECTOR	m_ambient_light_color;
	XMVECTOR	m_bounding_sphere_centre;
};

