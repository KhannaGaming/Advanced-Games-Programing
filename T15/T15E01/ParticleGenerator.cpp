#include "ParticleGenerator.h"


//Const buffer structs. Pack to 16 bytes. Don't let any single element cross a 16 byte boundary
struct PARTICAL_CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;		// 64 bytes (4x4=16 floats x 4 floats)	
	XMVECTOR Color; //16 bytes
	//XMVECTOR point_light_position;
	//XMVECTOR point_light_colour;
};//TOTAL SIZE = 112 bytes




ParticleGenerator::ParticleGenerator(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext, bool shiney)
{
	isShiney = shiney;
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
	m_x, m_y, m_z = 0;
	m_xAngle, m_yAngle, m_zAngle = 0;
	m_scale = 3.0f;

	for (int i = 0; i < 100; i++)
	{
		Particle* particle = new Particle();
		particle->color = XMFLOAT4(1.0f, 0.0f, 0.3f, 1.0f);
		particle->gravity = 1.0f;
		particle->position = XMFLOAT3(0.0f, 3.0f, 50.0f);
		particle->velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_free.push_back(particle);
	}
}


ParticleGenerator::~ParticleGenerator()
{
	while (!m_free.empty())delete m_free.front(), m_free.pop_front();
	while (!m_active.empty())delete m_active.front(), m_active.pop_front();

	
	if (m_pRasterSolid) m_pRasterSolid->Release();

	if (m_pTexture0)m_pTexture0->Release();
	if (m_pSampler0)m_pSampler0->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
}

HRESULT ParticleGenerator::LoadObjModel(char * fileName, float xpos, float ypos, float zpos,char* textureName)
{
	HRESULT hr;
	m_textureName = textureName;
	SetPos(xpos, ypos, zpos);


	//Load and compile the pixel and vertex shaders- use vs_5_0 to target DX11 hardware only
	ID3DBlob *MVS, *MPS, *error;
	if (!isShiney)
	{
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
	}
	else 
	{
		hr = D3DX11CompileFromFile("reflect_shader.hlsl", 0, 0, "ReflectionVS", "vs_4_0", 0, 0, 0, &MVS, &error, 0);

		if (error != 0)//Check for shader compilation error
		{
			OutputDebugStringA((char*)error->GetBufferPointer());
			error->Release();
			if (FAILED(hr))//Don't fail if error is just a warning
			{
				return hr;
			}
		}

		hr = D3DX11CompileFromFile("reflect_shader.hlsl", 0, 0, "ReflectionPS", "ps_4_0", 0, 0, 0, &MPS, &error, 0);

		if (error != 0)//Check for shader compilation error
		{
			OutputDebugStringA((char*)error->GetBufferPointer());
			error->Release();
			if (FAILED(hr))//Don't fail if error is just a warning
			{
				return hr;
			}
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
	//	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
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
	constant_buffer_desc.ByteWidth = 176; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))return hr;

	
	return S_OK;
}

void ParticleGenerator::Draw(XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraposition)
{

	

	//DrawOne(&test, view, projection, cameraposition);
	//Point Light
	/*g_point_light_colour = XMVectorSet(0.5f, 0.0f, 0.0f, 0.0f);
	g_point_light_position = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);*/


	/*m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (!isShiney)
	{

		XMMATRIX Rotation;
		Rotation = XMMatrixRotationX(XMConvertToRadians(90));

		XMMATRIX transpose;
		MODEL_CONSTANT_BUFFER0 model_cb_values;
		XMMATRIX  world;

		world = XMMatrixScaling(m_scale, m_scale, m_scale);
		world *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xAngle), XMConvertToRadians(m_yAngle), XMConvertToRadians(m_zAngle));
		world *= XMMatrixTranslation(m_x, m_y, m_z);

		XMMATRIX A = world;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR det = XMMatrixDeterminant(A);		
		model_cb_values.World = XMMatrixTranspose(XMMatrixInverse(&det,A));
		model_cb_values.WorldViewProjection = world * (*view)*(*projection);

		m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	}
	else
	{
		XMMATRIX transpose;
		REFLECTION_CONSTANT_BUFFER0 reflect_cb_values;
		XMMATRIX  world;

		world = XMMatrixScaling(m_scale, m_scale, m_scale);
		world *= XMMatrixTranslation(m_x, m_y, m_z);

		reflect_cb_values.WorldView = world * (*view);
		reflect_cb_values.WorldViewProjection = world * (*view)*(*projection);

		m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &reflect_cb_values, 0, 0);
	}

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);


	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);*/
}

void ParticleGenerator::SetPos(float xpos, float ypos, float zpos)
{
	m_x = xpos;
	m_y = ypos;
	m_z = zpos;
}

XMVECTOR ParticleGenerator::GetPos()
{
	XMVECTOR positions = XMVectorSet(m_x, m_y, m_z, 0.0f);
	return positions;
}

void ParticleGenerator::IncPos(float xAmount, float yAmount, float zAmount)
{
	m_x += xAmount;
	m_y += yAmount;
	m_z += zAmount;
}

void ParticleGenerator::IncRotation(float xAmount, float yAmount, float zAmount)
{
	m_xAngle += xAmount;
	m_yAngle += yAmount;
	m_zAngle += zAmount;

}

void ParticleGenerator::IncScale(float scaleAmount)
{
	m_scale += scaleAmount;
}

HRESULT ParticleGenerator::AddTexture()
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,(char*)m_textureName, NULL, NULL, &m_pTexture0, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	SetSamplerState();
	return S_OK;
}

void ParticleGenerator::LookAt_XZ(float xWorld, float zWorld)
{
	float dx, dz;
	dx = xWorld - m_x;
	dz = zWorld - m_z;
	m_yAngle = atan2(dx, dz)*(180.0 / XM_PI);

}

void ParticleGenerator::LookAt_XYZ(float xWorld, float yWorld, float zWorld)
{
	float dx, dy, dz;
	dx = xWorld - m_x;
	dy = yWorld - m_y;
	dz = zWorld - m_z;
	m_xAngle = -atan2(dy, dx - dz)*(180.0 / XM_PI);// *180.0 / XM_PI;
	m_yAngle = atan2(dx, dz)*(180.0 / XM_PI);
}

void ParticleGenerator::MoveForward(float distance)
{
	m_x += sin(m_yAngle* (XM_PI / 180.0))*distance;
	m_z += cos(m_yAngle* (XM_PI / 180.0))*distance;

}

void ParticleGenerator::MoveForwardIncY(float distance)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0));
	m_y += -sin(m_xAngle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance * cos(m_xAngle * (XM_PI / 180.0));

}




HRESULT ParticleGenerator::SetSamplerState()
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

int ParticleGenerator::CreateParticle()
{
	HRESULT hr = S_OK;
	XMFLOAT3 vertices[6] = //verts for the quad NOTE: could be changed to make different shapes
	{
		XMFLOAT3(-1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,1.0f,0.0f),
		XMFLOAT3(-1.0f,1.0f,0.0f),
		XMFLOAT3(-1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,-1.0f,0.0f),
		XMFLOAT3(1.0f,1.0f,0.0f),
	};
	D3D11_RASTERIZER_DESC rasterize_desc;
	ZeroMemory(&rasterize_desc, sizeof(rasterize_desc));
	rasterize_desc.FillMode = D3D11_FILL_SOLID;
	rasterize_desc.CullMode = D3D11_CULL_NONE;//sets no culling;
	hr = m_pD3DDevice->CreateRasterizerState(&rasterize_desc, &m_pRasterSolid);
	if (FAILED(hr))	return hr;
	rasterize_desc.CullMode = D3D11_CULL_BACK;//sets backface culling
	hr = m_pD3DDevice->CreateRasterizerState(&rasterize_desc, &m_pRasterParticle);
	if (FAILED(hr))	return hr;
	// Create the Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;// Both cpu and gpu
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * 6/*VERCOUNT*/;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //edit on the cpu

	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);
	if (FAILED(hr))return hr;

	//copy verts to buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//mapping = locking the buffer which allows writin
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));//copy data
	//unlock the buffer
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	CompileShaders();

	return 0;
}

HRESULT ParticleGenerator::CompileShaders()
{
	HRESULT hr;

	//Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 80; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))return hr;


	if (FAILED(hr))return hr;
	//Load and compile the pixel and vertex shaders- use vs_5_0 to target DX11 hardware only
	ID3DBlob *MVS, *MPS, *error;
	hr = D3DX11CompileFromFile("particle_shader.hlsl", 0, 0, "ParticleVS", "vs_4_0", 0, 0, 0, &MVS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("particle_shader.hlsl", 0, 0, "ParticlePS", "ps_4_0", 0, 0, 0, &MPS, &error, 0);

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
			{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			//{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), MVS->GetBufferPointer(), MVS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	return S_OK;

}

void ParticleGenerator::DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraposition)
{
	UINT stride = sizeof(XMFLOAT3);

	XMMATRIX  world;
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xAngle), XMConvertToRadians(m_yAngle), XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(one->position.x, one->position.y, one->position.z);

	PARTICAL_CONSTANT_BUFFER0 partical_cb_values;
	partical_cb_values.Color =XMVectorSet(one->color.x, one->color.y, one->color.z, 0.0f);
	partical_cb_values.WorldViewProjection = world * (*view) *(*projection);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &partical_cb_values, 0, 0);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);


	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
	m_pImmediateContext->Draw(6, 0);
}

