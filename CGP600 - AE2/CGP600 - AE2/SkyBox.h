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
#include"camera.h"
#include<WinUser.h>
#include<windowsx.h>



class SkyBox
{
public:
	SkyBox(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext, ID3D11Buffer*	pVertexBuffer, camera* camera);
	~SkyBox();
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	HRESULT Init(float xpos, float ypos, float zpos, const char* textureName);
	HRESULT AddTexture();
	float GetScale() { return m_scale; };
	XMVECTOR GetPos() { return XMVectorSet(m_x,m_y,m_z,0); };

private:
	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11Buffer*				m_pVertexBuffer;
	camera* m_pCamera;
	ID3D11RasterizerState*		m_pRasterSolid = 0;
	ID3D11RasterizerState*		m_pRasterSkybox = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSkybox = 0;
	
	float			m_x, m_y, m_z;
	float			m_xAngle, m_yAngle, m_zAngle;
	float			m_scale;
	const char*			m_textureName;
};

