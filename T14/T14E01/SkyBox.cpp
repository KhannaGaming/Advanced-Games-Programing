#include "SkyBox.h"
struct SKYBOX_CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;		// 64 bytes (4x4=16 floats x 4 floats)
	XMFLOAT4 colour;
}; //64 bytes


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
	m_scale = 30.0f;

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
	UINT stride = 48;
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SKYBOX_CONSTANT_BUFFER0 skybox_cb_values;
	XMMATRIX  world;
	skybox_cb_values.colour = {1.0f,1.0f,1.0f,1.0f};
	m_x = m_pCamera->GetPos().x;
	m_y = m_pCamera->GetPos().y;
	m_z = m_pCamera->GetPos().z;
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	skybox_cb_values.WorldViewProjection = world * (*view)*(*projection);


	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &skybox_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSkybox, 0);
	m_pImmediateContext->RSSetState(m_pRasterSkybox);
	m_pImmediateContext->Draw(36, 0);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSolid, 0);
	m_pImmediateContext->RSSetState(m_pRasterSolid);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);

}

HRESULT SkyBox::Init(float xpos, float ypos, float zpos)
{
	HRESULT hr;
	m_x = xpos;
	m_y = ypos;
	m_z = zpos;

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
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), MVS->GetBufferPointer(), MVS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	//Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 80; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))return hr;

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


	return S_OK;
}
