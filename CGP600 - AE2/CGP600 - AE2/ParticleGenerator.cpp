#include "ParticleGenerator.h"


//Const buffer structs. Pack to 16 bytes. Don't let any single element cross a 16 byte boundary
struct PARTICAL_CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;		// 64 bytes (4x4=16 floats x 4 floats)	
	XMMATRIX World;						// 64 bytes (4x4=16 floats x 4 floats)	
};//TOTAL SIZE = 112 bytes

struct POS_TEX_NORM_VERTEX//This will be added to and renamed in future tutorials
{
	XMFLOAT3 Pos;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

ParticleGenerator::ParticleGenerator(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext, bool shiney, DeltaTime* deltaTime)
{
	m_isShiney = shiney;
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
	m_x, m_y, m_z = 0;
	m_xAngle, m_yAngle, m_zAngle = 0;
	m_scale = 1.0f;
	m_timePrevious = (float(timeGetTime()) / 1000.0f);
	m_untilParticle = 0.0f;
	m_isActive = true;
	m_particalType = ENGINE_PARTICLE;
	m_pDeltaTime = deltaTime;
	for (int i = 0; i < 1000; i++)
	{
		Particle* particle = new Particle();
		particle->color = XMFLOAT4(1.0f, 0.0f, 0.3f, 1.0f);
		particle->gravity = 1.0f;
		particle->position = XMFLOAT3(0.0f, 3.0f, 50.0f);
		particle->m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
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

HRESULT ParticleGenerator::LoadObjModel(char* textureName)
{
	HRESULT hr;
	m_textureName = textureName;

	//Load and compile the pixel and vertex shaders- use vs_5_0 to target DX11 hardware only
	ID3DBlob *MVS, *MPS, *error;
	if (!m_isShiney)
	{
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
	constant_buffer_desc.ByteWidth = 128; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))return hr;

	D3D11_BLEND_DESC b;
	b.RenderTarget[0].BlendEnable = TRUE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = FALSE;
	b.AlphaToCoverageEnable = TRUE;

	m_pD3DDevice->CreateBlendState(&b, &m_pAlphaBlendEnable);
	
	return S_OK;
}

void ParticleGenerator::Draw(XMMATRIX* view, XMMATRIX* projection, XMVECTOR* cameraposition)
{
	XMMATRIX  world;
	PARTICAL_CONSTANT_BUFFER0 partical_cb_values;

	UINT stride = sizeof(POS_TEX_NORM_VERTEX);
	UINT offset = 0;
	float timeNow = (float(timeGetTime()) / 1000.0f);
	float deltaTime = timeNow - m_timePrevious;
	m_timePrevious = timeNow;
	m_untilParticle -= deltaTime;

	if (m_untilParticle <= 0.0f)
	{
		if (m_isActive)//a bool to check if the particle engine is on or off. Make a getter/setter and use it in main
		{
			it = m_free.begin();//point to the beggining of the free list
								//add a new particle to the back of m_active from the front of m_free
			if (m_free.size() != NULL)//safety check
			{
				switch (m_particalType)//the name of my enum
				{
				case ENGINE_PARTICLE:
				{
					m_age = 0.8f;
					m_untilParticle = 0.008f;
					////////////////////////initialise the particle NOTE: all of this is adjustable for different effects////////////////////////
					(*it)->color = XMFLOAT4(0.901f, 0.49f, 0.0f, 1.0f);
					(*it)->gravity = 0.0f;
					(*it)->position = XMFLOAT3(m_x, m_y, m_z);
					(*it)->m_velocity = XMFLOAT3(RandomNegOneToPosOne(), 0.0f, -RandomZeroToOne());
					////////////////////////////////////////////////////////////////////////////////////////////////
					break;
				}
				default:
				{
					break;
				}
				}
				(*it)->age = 0.0f;//set age to 0. this is used for knowing when to delete the particle

								  //////add the particle from the front of the available list to the back of the active list and remove it
				m_active.push_back(*it);
				m_free.pop_front();
			}
		}
		else m_untilParticle = 0.001f;
	}

	if (m_active.size() != NULL)//safety check
	{
		it = m_active.begin();//point the iterator to the front of the active list ready for processing
		while (it != m_active.end())//move all of the particles
		{

			switch (m_particalType)
			{
			case ENGINE_PARTICLE:
			{

				/////////////////////////ALL of this is adjustable for different effects///////////////////////////////////////////////////////////
				(*it)->age += deltaTime;
				(*it)->m_velocity.y -= (*it)->gravity*(deltaTime);
				(*it)->position.x += (*it)->m_velocity.x*(deltaTime);
				(*it)->position.y += (*it)->m_velocity.y*(deltaTime);
				(*it)->position.z += (*it)->m_velocity.z*(deltaTime);
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				break;
			}
			default:
			{
				break;
			}
			}

			world = XMMatrixIdentity();
			world *= XMMatrixScaling(m_scale, m_scale, m_scale);
			world *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xAngle), XMConvertToRadians(m_yAngle), XMConvertToRadians(m_zAngle));
			world *= XMMatrixTranslation((*it)->position.x, (*it)->position.y, (*it)->position.z);

			XMMATRIX A = (world);
			A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMVECTOR det = XMMatrixDeterminant(A);
			//constant buffer stuff for shader
			partical_cb_values.WorldViewProjection = (world) * (*view)*(*projection);
			partical_cb_values.World = XMMatrixTranspose(XMMatrixInverse(&det, A));
			m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &partical_cb_values, 0, 0);

			m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
			m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

			//set the shader objects as active
			m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
			m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
			m_pImmediateContext->IASetInputLayout(m_pInputLayout);


			m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_pImmediateContext->OMSetBlendState(m_pAlphaBlendEnable, 0, 0xffffffff);
			m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
			m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
			m_pImmediateContext->RSSetState(m_pRasterParticle);//set backface culling to on
			m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
			m_pImmediateContext->Draw(6, 0);//draw the particle
			m_pImmediateContext->RSSetState(m_pRasterSolid);//set backface culling to off
			m_pImmediateContext->OMSetBlendState(m_pAlphaBlendDisable, 0, 0xffffffff);
			if ((*it)->age >= m_age)//check the age of the current particle
			{
				it++;
				m_active.front()->age = m_age;
				m_active.front()->position = { (RandomNegOneToPosOne() + m_x * 10)*(RandomZeroToOne() * 10),m_y + 5.0f, /*position.z*/ cameraposition->z + 7.0f };
				m_active.front()->m_velocity = { /*RandomNegOneToPosOne()*/0.0f, 4.50f, RandomNegOneToPosOne() };
				m_free.push_back(m_active.front());//move the (now previously) current active particle to the back of the pool			
				m_active.pop_front();//remove the particle			
			}
			else it++;
		}//end of while
	}//end of if(m_active.size()!=NULL)
}

void ParticleGenerator::SetPos(XMVECTOR position)
{
	m_x = position.x;
	m_y = position.y;
	m_z = position.z;
}

void ParticleGenerator::SetRot(XMVECTOR position)
{
	m_xAngle = position.x;
	m_yAngle = position.y;
	m_zAngle = position.z;
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

HRESULT ParticleGenerator::AddTexture(char* textureName)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,(char*)textureName, NULL, NULL, &m_pTexture0, NULL);
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
	m_yAngle = (float)atan2(dx, dz)*(180.0f / XM_PI);

}

void ParticleGenerator::LookAt_XYZ(float xWorld, float yWorld, float zWorld)
{
	float dx, dy, dz;
	dx = xWorld - m_x;
	dy = yWorld - m_y;
	dz = zWorld - m_z;
	m_xAngle = (float)-atan2(dy, dx - dz)*(180.0f / XM_PI);// *180.0f / XM_PI;
	m_yAngle = (float)atan2(dx, dz)*(180.0f / XM_PI);
}

void ParticleGenerator::MoveForward(float distance)
{
	m_x += (float)sin(m_yAngle* (XM_PI / 180.0f))*distance;
	m_z += (float)cos(m_yAngle* (XM_PI / 180.0f))*distance;

}

void ParticleGenerator::MoveForwardIncY(float distance)
{
	m_x += (float)sin(m_yAngle * (XM_PI / 180.0f)) * distance * cos(m_xAngle * (XM_PI / 180.0f));
	m_y += (float)-sin(m_xAngle * (XM_PI / 180.0f)) * distance;
	m_z += (float)cos(m_yAngle * (XM_PI / 180.0f)) * distance * cos(m_xAngle * (XM_PI / 180.0f));

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
	POS_TEX_NORM_VERTEX vertices[] = //verts for the quad NOTE: could be changed to make different shapes
	{
	{	XMFLOAT3(-1.0f,-1.0f,0.0f),	XMFLOAT2(0.0f,1.0f),	XMFLOAT3(0.0f,0.0f,-1.0f) },
	{	XMFLOAT3(1.0f,1.0f,0.0f),	XMFLOAT2(1.0f,0.0f),	XMFLOAT3(0.0f,0.0f,-1.0f) },
	{	XMFLOAT3(1.0f,-1.0f,0.0f),	XMFLOAT2(1.0f,1.0f),	XMFLOAT3(0.0f,0.0f,-1.0f) },

	{	XMFLOAT3(-1.0f,1.0f,0.0f),	XMFLOAT2(0.0f,0.0f),	XMFLOAT3(0.0f,0.0f,-1.0f) },
	{	XMFLOAT3(1.0f,1.0f,0.0f),	XMFLOAT2(1.0f,0.0f),	XMFLOAT3(0.0f,0.0f,-1.0f) },
	{	XMFLOAT3(-1.0f,-1.0f,0.0f), XMFLOAT2(0.0f,1.0f),	XMFLOAT3(0.0f,0.0f,-1.0f)},
	};

	D3D11_RASTERIZER_DESC rasterize_desc;
	ZeroMemory(&rasterize_desc, sizeof(rasterize_desc));
	rasterize_desc.FillMode = D3D11_FILL_SOLID;
	rasterize_desc.CullMode = D3D11_CULL_BACK;//sets no culling;
	hr = m_pD3DDevice->CreateRasterizerState(&rasterize_desc, &m_pRasterSolid);
	if (FAILED(hr))	return hr;

	rasterize_desc.FillMode = D3D11_FILL_SOLID;
	rasterize_desc.CullMode = D3D11_CULL_NONE;//sets backface culling
	hr = m_pD3DDevice->CreateRasterizerState(&rasterize_desc, &m_pRasterParticle);
	if (FAILED(hr))	return hr;

	// Create the Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;// Both cpu and gpu
	bufferDesc.ByteWidth = sizeof(vertices)/*VERCOUNT*/;
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
	return 0;
}

float ParticleGenerator::RandomZeroToOne()
{
	float randomNumber = -2.0f;
	while (randomNumber >= 1.0f || randomNumber <=0.0f)
	{
		randomNumber = (float)rand()/(float)32767.0f;
	}
	return randomNumber;
}

float ParticleGenerator::RandomNegOneToPosOne()
{
	float randomNumber = -2.0f;
	while (randomNumber > 1.0f || randomNumber <-1.0f)
	{
		randomNumber = ((float)rand() / (float)32767.0f)- ((float)rand() / (float)32767.0f);

	}
	return randomNumber;
}

