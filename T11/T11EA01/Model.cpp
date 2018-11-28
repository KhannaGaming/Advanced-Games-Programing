#include "Model.h"


//Const buffer structs. Pack to 16 bytes. Don't let any single element cross a 16 byte boundary
struct MODEL_CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;		// 64 bytes (4x4=16 floats x 4 floats)
	XMVECTOR directional_light_vector;	//16 bytes
	XMVECTOR directional_light_colour;	//16 bytes
	XMVECTOR ambient_light_colour;		//16 bytes
	//XMVECTOR point_light_position;
	//XMVECTOR point_light_colour;
};//TOTAL SIZE = 112 bytes

Model::Model(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
	m_x, m_y, m_z = 0;
	m_xAngle, m_yAngle, m_zAngle = 0;
	m_scale = 1.0f;

	m_pLightManager = new LightManager();
	m_pLightManager->CreateDirectionalLight("SunLight", XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	//always use a small value for ambient lighting
	m_pLightManager->CreateAmbientLight("AmbientLight", XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f));
}


Model::~Model()
{
	if (m_pTexture0)m_pTexture0->Release();
	if (m_pSampler0)m_pSampler0->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pObject)
	{
		delete m_pObject;
		m_pObject = nullptr;
	}
	if (m_pLightManager)
	{
		delete m_pLightManager;
		m_pLightManager = nullptr;
	}
}

HRESULT Model::LoadObjModel(char * fileName, float xpos, float ypos, float zpos)
{
	HRESULT hr;
	m_pObject = new ObjFileModel(fileName, m_pD3DDevice, m_pImmediateContext);
	if (m_pObject->filename == "File Not Loaded") return S_FALSE;
	SetPos(xpos, ypos, zpos);


	//Load and compile the pixel and vertex shaders- use vs_5_0 to target DX11 hardware only
	ID3DBlob *MVS, *MPS, *error;

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &MVS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &MPS, &error, 0);

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
	constant_buffer_desc.ByteWidth = 112; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))return hr;

	return S_OK;
}

void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	//Point Light
	/*g_point_light_colour = XMVectorSet(0.5f, 0.0f, 0.0f, 0.0f);
	g_point_light_position = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);*/


	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX Rotation;
	Rotation = XMMatrixRotationX(XMConvertToRadians(90));

	XMMATRIX transpose;
	MODEL_CONSTANT_BUFFER0 model_cb_values;
	XMMATRIX  world;

	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationRollPitchYaw(m_xAngle, m_yAngle, m_zAngle);
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	model_cb_values.WorldViewProjection = world * (*view)*(*projection);

	//Point Light
	/*
	XMVECTOR determinant; // inverse function returns determinant, but it isnt used
	XMMATRIX inverse; // if not defined elsewhere
	inverse = XMMatrixInverse(&determinant, world);
	model_cb_values.point_light_colour = g_point_light_colour;
	model_cb_values.point_light_position = XMVector3Transform(g_point_light_position, Rotation);
	model_cb_values.point_light_position *= XMVector3Transform(g_point_light_position, inverse);
	*/
	transpose = XMMatrixTranspose(world);
	model_cb_values.ambient_light_colour = m_pLightManager->GetLightColour("AmbientLight");
	model_cb_values.directional_light_colour = m_pLightManager->GetLightColour("SunLight");
	model_cb_values.directional_light_vector = XMVector3Transform(m_pLightManager->GetLightPosition("SunLight"), transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pObject->Draw();
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
}

void Model::SetPos(float xpos, float ypos, float zpos)
{
	m_x = xpos;
	m_y = ypos;
	m_z = zpos;
}

XMVECTOR Model::GetPos()
{
	XMVECTOR positions = XMVectorSet(m_x, m_y, m_z, 0.0f);
	return positions;
}

void Model::IncPos(float xAmount, float yAmount, float zAmount)
{
	m_x += xAmount;
	m_y += yAmount;
	m_z += zAmount;
}

void Model::IncRotation(float xAmount, float yAmount, float zAmount)
{
	m_xAngle += xAmount;
	m_yAngle += yAmount;
	m_zAngle += zAmount;

}

void Model::IncScale(float scaleAmount)
{
	m_scale += scaleAmount;
}

HRESULT Model::AddTexture(char * fileName)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, fileName, NULL, NULL, &m_pTexture0, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	SetSamplerState();
	return S_OK;
}

void Model::LookAt_XZ(float xWorld, float zWorld)
{
	float dx, dz;
	dx = xWorld - m_x;
	dz = zWorld - m_z;
	m_yAngle = XMConvertToDegrees(atan2(dx, dz));// *(180.0 / XM_PI);

}

void Model::LookAt_XYZ(float xWorld, float yWorld, float zWorld)
{
	float dx, dy, dz;
	dx = xWorld - m_x;
	dy = yWorld - m_y;
	dz = zWorld - m_z;
	m_xAngle = -XMConvertToDegrees(atan2(dy, dx - dz));// *180.0 / XM_PI;
	m_yAngle = XMConvertToDegrees(atan2(dx, dz));
}

void Model::MoveForward(float distance)
{
	m_x += sin(XMConvertToRadians(m_yAngle))*distance;
	m_z += cos(XMConvertToRadians(m_yAngle))*distance;

}

void Model::MoveForwardIncY(float distance)
{
	m_x += sin(XMConvertToRadians(m_yAngle))*distance* cos(m_xAngle * (XM_PI / 180.0));
	m_y += -sin(XMConvertToRadians(m_xAngle))*distance;
	m_z += cos(XMConvertToRadians(m_yAngle))*distance* cos(m_xAngle * (XM_PI / 180.0));
}

HRESULT Model::SetSamplerState()
{
	HRESULT hr;

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
