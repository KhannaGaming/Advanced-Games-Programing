#include "SkyBox.h"
struct SKYBOX_CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;		// 64 bytes (4x4=16 floats x 4 floats)
}; //64 bytes

	//Define vertex structure
struct POS_COL_TEX_NORM_VERTEX//This will be added to and renamed in future tutorials
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};


SkyBox::SkyBox(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext,ID3D11Buffer*	VertexBuffer, camera* camera)
{
	m_pVertexBuffer = nullptr;
	m_pD3DDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pCamera = nullptr;
	m_pCamera = camera;
	m_pVertexBuffer = VertexBuffer;
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
	m_x, m_y, m_z = 0;
	m_xAngle, m_yAngle, m_zAngle = 0;
	m_scale = 80.0f;
}


SkyBox::~SkyBox()
{
	if (m_pRasterSolid) m_pRasterSolid->Release();
	
	if (m_pRasterSkybox)m_pRasterSkybox->Release();
	
	if (m_pDepthWriteSkybox)m_pDepthWriteSkybox->Release();
	
	if (m_pDepthWriteSolid)m_pDepthWriteSolid->Release();
	
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	else
	{
		m_pCamera = nullptr;
	}
	if(m_pTexture0)m_pTexture0->Release();
	if (m_pSampler0)m_pSampler0->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
}

void SkyBox::Draw(XMMATRIX * view, XMMATRIX * projection)
{


	//m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);

	UINT stride = 48;
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SKYBOX_CONSTANT_BUFFER0 skybox_cb_values;
	XMMATRIX  world;
	//skybox_cb_values.colour = {0.5f,1.0f,1.0f,1.0f};
	m_x = m_pCamera->GetPos().x;
	m_y = m_pCamera->GetPos().y;
	m_z = m_pCamera->GetPos().z;

	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	skybox_cb_values.WorldViewProjection = world * (*view)*(*projection);


	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
	m_pImmediateContext->VSSetShaderResources(0, 1, &m_pTexture0);

	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &skybox_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->RSSetState(m_pRasterSkybox);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSkybox, 0);
	m_pImmediateContext->Draw(36, 0);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSolid, 0);
	m_pImmediateContext->RSSetState(m_pRasterSolid);


}

HRESULT SkyBox::Init(float xpos, float ypos, float zpos, char* textureName)
{
	POS_COL_TEX_NORM_VERTEX vertices[] =
	{
		//Front face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },

		//Back Face
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,0.0f,1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,0.0f,1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f,0.0f,1.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f,0.0f,1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,0.0f,1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,0.0f,1.0f) },

		//Right Face
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(1.0f,0.0f,0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(1.0f,0.0f,0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(1.0f,0.0f,0.0f) },

		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(1.0f,0.0f,0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(1.0f,0.0f,0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(1.0f,0.0f,0.0f) },

		//Left Face
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(-1.0f,0.0f,0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(-1.0f,0.0f,0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),    XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(-1.0f,0.0f,0.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(-1.0f,0.0f,0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(-1.0f,0.0f,0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(-1.0f,0.0f,0.0f) },

		//Top Face
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f,1.0f,0.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,1.0f,0.0f) },

		//Bottom Face
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,-1.0f,0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,-1.0f,0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f,-1.0f,0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f,-1.0f,0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f,-1.0f,0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f,-1.0f,0.0f) },
	};
	HRESULT hr;
	m_x = xpos;
	m_y = ypos;
	m_z = zpos;



	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	hr = m_pD3DDevice->CreateRasterizerState(&rasterizer_desc, &m_pRasterSolid);

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	hr = m_pD3DDevice->CreateRasterizerState(&rasterizer_desc, &m_pRasterSkybox);

	D3D11_DEPTH_STENCIL_DESC DSDecsc;
	ZeroMemory(&DSDecsc, sizeof(DSDecsc));
	DSDecsc.DepthEnable = true;
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDecsc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = m_pD3DDevice->CreateDepthStencilState(&DSDecsc, &m_pDepthWriteSolid);
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_pD3DDevice->CreateDepthStencilState(&DSDecsc, &m_pDepthWriteSkybox);
	
	m_textureName = textureName;
	AddTexture();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = /*sizeof(POS_COL_VERTEX) **/ sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}
	
	D3D11_MAPPED_SUBRESOURCE ms;
	//Lock the buffer to allow writing
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock the buffer
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	//Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 64; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);
	//Load and compile the pixel and vertex shaders- use vs_5_0 to target DX11 hardware only
	ID3DBlob *MVS, *MPS, *error;

	hr = D3DX11CompileFromFile("skybox_shaders.hlsl", 0, 0, "SkyBoxVS", "vs_4_0", 0, 0, 0, &MVS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("skybox_shaders.hlsl", 0, 0, "SkyBoxPS", "ps_4_0", 0, 0, 0, &MPS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}
	//Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(MVS->GetBufferPointer(), MVS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(MPS->GetBufferPointer(), MPS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))
	{
		return hr;
	}

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), MVS->GetBufferPointer(), MVS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	return S_OK;
}

HRESULT SkyBox::AddTexture()
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, (char*)m_textureName, NULL, NULL, &m_pTexture0, NULL);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}
