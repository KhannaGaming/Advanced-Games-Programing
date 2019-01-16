#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <Windows.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT
#include <xnamath.h>
#include <Xinput.h>
#include<WinUser.h>
#include<windowsx.h>
#include <ctime>

//***************************************************
//MY INCLUDES
//***************************************************
#include"camera.h"
#include "text2D.h"
#include "Model.h"
#include "DirectInput.h"
#include"SkyBox.h"
#include "ParticleGenerator.h"
#include "DeltaTime.h"
#include "AudioManager.h"
#include "FilePaths.h"

#define GAP_DISTANCE 20.0f
#define PLAYER_GAP_DISTANCE 30.0f
#define ASTEROIDS_COUNT 50
#define ASTEROID_MAX_SCALE 100
#define ASTEROID_MIN_SCALE 1
#define ASTEROID_MAX_VELOCITY 20
#define ENEMY_SIGHT_RADIUS 50.0f
/*Release
d3d11.lib
d3dcompiler.lib
d3dx11.lib
d3dx9.lib
dxerr.lib
legacy_stdio_definitions.lib
*/
/*Debug
d3d11.lib
d3dcompiler.lib
d3dx11d.lib
d3dx9d.lib
dxerr.lib
legacy_stdio_definitions.lib
*/

//*********************************************************
//GLOBALS VARIABLES
//*********************************************************
HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;
D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pD3DDevice = NULL;
ID3D11DeviceContext*		g_pImmediateContext = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pBackBufferRTView = NULL;
ID3D11Buffer*				g_pVertexBuffer;
ID3D11InputLayout*			g_pInputLayout;
ID3D11DepthStencilView*		g_pZBuffer;
camera*						g_pCamera;
Text2D*						g_pFPSText2D;
Text2D*						g_pScoreText2D;
ID3D11BlendState* g_pAlphaBlendEnable; 
ID3D11BlendState* g_pAlphaBlendDisable; 
Model* g_SpaceShip;
Model* g_pAsteroid;
Model* g_pAsteroid2;
Model* g_pAsteroid3;
Model* g_pAsteroid4;
Model* g_pReflectionModel;
Model* g_pCube;
Model* g_pLaser;
vector<Model*> g_vModels;
DirectInput* g_pDirectInput;
SkyBox* g_pSkyBox;
ParticleGenerator* g_pParticleGenerator;
SceneNode* g_root_node;
SceneNode* g_SpaceShipNode; 
SceneNode* g_SpaceShipNode2;
SceneNode* g_AsteroidNode;
SceneNode* g_CameraNode;
SceneNode* g_SpaceShipEngineNode;
SceneNode* g_LaserNode;
SceneNode* g_LaserNode2;
SceneNode* g_LaserNode3;
SceneNode* g_LaserNode4;
vector<SceneNode*> g_vAsteroids;
vector<Model*> g_vAsteroidModels;
vector<SceneNode*> g_vPlayerLasers;
vector<SceneNode*> g_vEnemies;
DeltaTime* g_pDeltaTime;
AudioManager* g_pAudioManager;

//Backgournd clear colour
float g_clear_colour[4] = { 0.0f,0.0f,0.0f,1.0f };
float g_rect_width = 1000;
float g_rect_height = 1000;
float farClipPlane = 1000.0f;
int score = 0;
const char g_TutorialName[100] = "Space Runner";

//***************************************************************
//Forward declarations
//***************************************************************
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitialiseD3D();
void ShutdownD3D();
void RenderFrame(void);
HRESULT InitialiseGraphics(void);
void CreateAsteroids();
int RandomNumberGenerator(int maxDistance);
int RandomNumberGeneratorNoNegative(int maxDistance);
float Pythagoras(XMVECTOR v);
float Pythagoras(XMVECTOR v1, XMVECTOR v2);
void CheckVelocities();


//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}
	

	MSG msg = {0};

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create device");
	}
	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}
	g_pDirectInput = new DirectInput(g_hInst, g_hWnd, g_pCamera, g_pDeltaTime);
	if (FAILED(g_pDirectInput->InitInput()))
	{
		DXTRACE_MSG("Failed to create Input");
	}
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			RenderFrame();
		}
	}

	ShutdownD3D();

	return(int)msg.wParam;
}


HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	//Give your app your own name
	char Name[100] = "Space Runner\0";

	//Register class
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW +1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	//Create Window
	g_hInst = hInstance;
	RECT rc = { 0, 0, (LONG)g_rect_width, (LONG)g_rect_height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(	Name, g_TutorialName, WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
							rc.bottom - rc.top, NULL, NULL, hInstance,NULL);
	
	if(!g_hWnd) return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


//Called every time the app recieves a message

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (g_pSwapChain)
		{
			g_pImmediateContext->OMSetRenderTargets(0, 0, 0);

			// Release all outstanding references to the swap chain's buffers.
			g_pBackBufferRTView->Release();

			HRESULT hr;
			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDs.
			hr = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			// Perform error handling here!

			// Get buffer and create a render-target-view.
			ID3D11Texture2D* pBuffer;
			hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(void**)&pBuffer);
			// Perform error handling here!

			hr = g_pD3DDevice->CreateRenderTargetView(pBuffer, NULL,
				&g_pBackBufferRTView);
			// Perform error handling here!
			pBuffer->Release();
			
			float width = LOWORD(lParam);
			float height = HIWORD(lParam);

			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 1;
			sd.BufferDesc.Width = (UINT)width;
			sd.BufferDesc.Height = (UINT)height;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = g_hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = true;

			// Create a Z buffer texture
			D3D11_TEXTURE2D_DESC tex2dDesc;
			ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

			tex2dDesc.Width = (UINT)width;
			tex2dDesc.Height = (UINT)height;
			tex2dDesc.ArraySize = 1;
			tex2dDesc.MipLevels = 1;
			tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
			tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

			ID3D11Texture2D *pZBufferTexture;
			hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);
			if (FAILED(hr)) return hr;


			// Create the Z Buffer
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));

			dsvDesc.Format = tex2dDesc.Format;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
			pZBufferTexture->Release();

			g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);
			 
			g_rect_width = width;
			g_rect_height = height;

			// Set up the viewport.
			D3D11_VIEWPORT vp;
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			g_pImmediateContext->RSSetViewports(1, &vp);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, //comment out this line if you need to test D3D 11.0 
		//functionality on hardware that doesn't support it 
		D3D_DRIVER_TYPE_WARP,//comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	//Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;
	
	//Use the Back Buffer Texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &g_pBackBufferRTView);
	pBackBufferTexture->Release();
	if (FAILED(hr)) return hr;

	// Create a Z buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);
	if (FAILED(hr)) return hr;


	// Create the Z Buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	//Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	//Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	g_pFPSText2D = new Text2D("Assets/font1.png", g_pD3DDevice, g_pImmediateContext);
	g_pScoreText2D = new Text2D("Assets/font1.png", g_pD3DDevice, g_pImmediateContext);

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
	b.AlphaToCoverageEnable = FALSE;

	g_pD3DDevice->CreateBlendState(&b, &g_pAlphaBlendEnable);
	
	


	return S_OK;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
void ShutdownD3D()
{
	if (g_pAudioManager)
	{		
		delete g_pAudioManager;
		g_pAudioManager = nullptr;
	}

	if (g_pDeltaTime)
	{
		delete g_pDeltaTime;
		g_pDeltaTime = nullptr;
	}


	int maxAsteroidModels = g_vAsteroidModels.size();
	for (int i = 0; i < maxAsteroidModels; i++)
	{
		if (g_vAsteroidModels[i])
		{
			delete g_vAsteroidModels[i];
			g_vAsteroidModels[i] = nullptr;
		}
	}
	for (int i = 0; i < maxAsteroidModels; i++)
	{
		g_vAsteroidModels.pop_back();
	}

	int maxAsteroids = g_vAsteroids.size();
	for (int i = 0; i < maxAsteroids; i++)
	{
		if (g_vAsteroids[i])
		{
			delete g_vAsteroids[i];
			g_vAsteroids[i] = nullptr;
		}
	}
	for (int i = 0; i < maxAsteroids; i++)
	{
		g_vAsteroids.pop_back();
	}

	int maxModels = g_vModels.size();
	for (int i = 0; i < maxModels; i++)
	{
		if (g_vModels[i])
		{
			delete g_vModels[i];
			g_vModels[i] = nullptr;
		}
	}
	for (int i = 0; i < maxModels; i++)
	{
		g_vModels.pop_back();
	}

	g_root_node = nullptr;

	if (g_pParticleGenerator)
	{
		delete g_pParticleGenerator;
		g_pParticleGenerator = nullptr;
	}
	
	if (g_pSkyBox)
	{
		delete g_pSkyBox;
		g_pSkyBox = nullptr;
	}
	if (g_pDirectInput)
	{
		delete g_pDirectInput;
		g_pDirectInput = nullptr;
		g_pCamera = nullptr;
	}


	if (g_pAlphaBlendDisable) g_pAlphaBlendDisable->Release();
	if (g_pAlphaBlendEnable) g_pAlphaBlendEnable->Release();



	if (g_pFPSText2D)
	{
		delete g_pFPSText2D;
		g_pFPSText2D = nullptr;
	}
	if (g_pScoreText2D)
	{
		delete g_pScoreText2D;
		g_pScoreText2D = nullptr;
	}

	
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pZBuffer) g_pZBuffer->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}

HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;
	srand((unsigned int)time(NULL));
	//***************************************************************
	//Create Models
	//***************************************************************
	//Choose random skybox to load
	string randomSkybox = "Assets/space" + to_string((rand() % 3)+1) + ".dds";	

	g_pDeltaTime = new DeltaTime();

	g_SpaceShip = new Model(g_pD3DDevice, g_pImmediateContext, false, true);
	g_SpaceShip->LoadObjModel((char*)FilePaths::OBJ::SpaceShip, (char*)FilePaths::Texture::SpaceShip);
	g_vModels.push_back(g_SpaceShip);
	
	g_pAsteroid = new Model(g_pD3DDevice, g_pImmediateContext, false,true);
	g_pAsteroid->LoadObjModel((char*)FilePaths::OBJ::Asteroid, (char*)FilePaths::Texture::Rock1);
	g_vAsteroidModels.push_back(g_pAsteroid);

	g_pAsteroid2 = new Model(g_pD3DDevice, g_pImmediateContext, false,true);
	g_pAsteroid2->LoadObjModel((char*)FilePaths::OBJ::Asteroid, (char*)FilePaths::Texture::Rock2);
	g_vAsteroidModels.push_back(g_pAsteroid2);

	g_pAsteroid3 = new Model(g_pD3DDevice, g_pImmediateContext, false,true);
	g_pAsteroid3->LoadObjModel((char*)FilePaths::OBJ::Asteroid, (char*)FilePaths::Texture::Rock3);
	g_vAsteroidModels.push_back(g_pAsteroid3);

	g_pAsteroid4 = new Model(g_pD3DDevice, g_pImmediateContext, false,true);
	g_pAsteroid4->LoadObjModel((char*)FilePaths::OBJ::Asteroid, (char*)FilePaths::Texture::Rock4);
	g_vAsteroidModels.push_back(g_pAsteroid4);
	
	g_pCube = new Model(g_pD3DDevice, g_pImmediateContext, false,false);
	g_pCube->LoadObjModel((char*)FilePaths::OBJ::Cube, (char*)FilePaths::Texture::Metal);
	g_vModels.push_back(g_pCube);

	g_pLaser = new Model(g_pD3DDevice, g_pImmediateContext, true, false);
	g_pLaser->LoadObjModel((char*)FilePaths::OBJ::Laser, (char*)randomSkybox.c_str());
	g_vModels.push_back(g_pLaser);

	g_pParticleGenerator = new ParticleGenerator(g_pD3DDevice, g_pImmediateContext, false, g_pDeltaTime);
	g_pParticleGenerator->CreateParticle();
	g_pParticleGenerator->LoadObjModel((char*)FilePaths::Texture::EngineParticle);


	//***************************************************************
	//Create Camera
	//***************************************************************
	g_pCamera = new camera(0.0f, 1.0f, 0.0f,0.0f,0.0f);

	//***************************************************************
	//Create Audio
	//***************************************************************
	g_pAudioManager = new AudioManager();
	hr = g_pAudioManager->Init();
	if (FAILED(hr))return hr;
	g_pAudioManager->PlaySoundEffect("Space");

	//***************************************************************
	//Create new SceneNode
	//***************************************************************
	g_root_node = new SceneNode(g_pDeltaTime, true,Tags::Root,g_root_node, g_pAudioManager, 4.0f);
	g_SpaceShipNode = new SceneNode(g_pDeltaTime, true,Tags::SpaceShip,g_root_node, g_pAudioManager, 4.0f);
	g_SpaceShipNode2 = new SceneNode(g_pDeltaTime, true, Tags::SpaceShip, g_root_node, g_pAudioManager, 4.0f);
	g_SpaceShipEngineNode = new SceneNode(g_pDeltaTime, true, Tags::SpaceShip,g_root_node, g_pAudioManager, 4.0f);
	g_CameraNode = new SceneNode(g_pDeltaTime, true,Tags::Camera,g_root_node, g_pAudioManager, 4.0f);
	g_LaserNode = new SceneNode(g_pDeltaTime, false,Tags::Laser,g_root_node, g_pAudioManager, 4.0f);
	g_LaserNode2 = new SceneNode(g_pDeltaTime, false,Tags::Laser,g_root_node, g_pAudioManager, 4.0f);
	g_LaserNode3 = new SceneNode(g_pDeltaTime, false,Tags::Laser,g_root_node, g_pAudioManager, 4.0f);
	g_LaserNode4 = new SceneNode(g_pDeltaTime, false,Tags::Laser,g_root_node, g_pAudioManager, 4.0f);
	g_vPlayerLasers.push_back(g_LaserNode);
	g_vPlayerLasers.push_back(g_LaserNode2);
	g_vPlayerLasers.push_back(g_LaserNode3);
	g_vPlayerLasers.push_back(g_LaserNode4);
	g_vEnemies.push_back(g_SpaceShipNode);
	g_vEnemies.push_back(g_SpaceShipNode2);

	//***************************************************************
	//Set Scenenodes Model
	//***************************************************************
	g_SpaceShipNode->SetModel(g_SpaceShip);
	g_SpaceShipNode2->SetModel(g_SpaceShip);
	g_CameraNode->SetModel(g_pCube);
	g_LaserNode->SetModel(g_pLaser);
	g_LaserNode2->SetModel(g_pLaser);
	g_LaserNode3->SetModel(g_pLaser);
	g_LaserNode4->SetModel(g_pLaser);

	//***************************************************************
	//Add any child nodes
	//***************************************************************
	g_root_node->addChildNode(g_CameraNode);
	g_CameraNode->addChildNode(g_LaserNode);
	g_CameraNode->addChildNode(g_LaserNode2);
	g_CameraNode->addChildNode(g_LaserNode3);
	g_CameraNode->addChildNode(g_LaserNode4);
	g_root_node->addChildNode(g_SpaceShipNode);
	g_root_node->addChildNode(g_SpaceShipNode2);
	g_SpaceShipNode2->addChildNode(g_SpaceShipEngineNode);

	//***************************************************************
	//Set any other transformations
	//***************************************************************
	g_CameraNode->SetScale(0.1f);
	g_SpaceShipNode->SetPos(XMVectorSet(-50, 50, 50.0f,0.0f));
	g_SpaceShipNode2->SetPos(XMVectorSet(50, 0, 50.0f,0.0f));
	g_LaserNode->SetScale(0.3f);
	g_LaserNode2->SetScale(0.3f);
	g_LaserNode3->SetScale(0.3f);
	g_LaserNode4->SetScale(0.3f);
	g_LaserNode->SetOffset(XMVectorSet(-2.0f, -0.2f, 0.0f, 0));
	g_LaserNode2->SetOffset(XMVectorSet(2.0f, -0.2f, 0.0f, 0));
	g_LaserNode3->SetOffset(XMVectorSet(-2.0f, -0.2f, 0.0f, 0));
	g_LaserNode4->SetOffset(XMVectorSet(2.0f, -0.2f, 0.0f, 0));
	g_SpaceShipEngineNode->SetPos(XMVectorSet(3.0f, -1.2f, 0, 0));

	//***************************************************************
	//Any other node stuff
	//***************************************************************
	g_pParticleGenerator->AddTexture((char*)FilePaths::Texture::EngineParticle);
	for (size_t i = 0; i < g_vPlayerLasers.size(); i++)
	{
		g_vPlayerLasers[i]->Activate(false);
	}
	g_CameraNode->SetCamera(g_pCamera);
	g_LaserNode->SetOriginalParentNode(g_CameraNode);
	g_LaserNode2->SetOriginalParentNode(g_CameraNode);
	g_LaserNode3->SetOriginalParentNode(g_CameraNode);
	g_LaserNode4->SetOriginalParentNode(g_CameraNode);
	
	//***************************************************************
	//Other Initialisation
	//***************************************************************
	g_pSkyBox = new SkyBox(g_pD3DDevice, g_pImmediateContext,g_pVertexBuffer,g_pCamera);
	g_pSkyBox->Init(-50.0f, 0.0f, 50.0, randomSkybox.c_str());
	
	//***************************************************************
	//Add textures
	//***************************************************************
	for (size_t i = 0; i < g_vModels.size(); i++)
	{
		g_vModels[i]->AddTexture();	
	}
	for (size_t i = 0; i < g_vAsteroidModels.size(); i++)
	{
		g_vAsteroidModels[i]->AddTexture();
	}

	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//***************************************************************
	//Create Asteroids
	//***************************************************************
	CreateAsteroids();

	for (size_t i = 0; i < g_vEnemies.size(); i++)
	{
		g_vEnemies[i]->ChaseOrFlee(g_vAsteroids[rand()%g_vAsteroids.size()], g_root_node);
	}
	return S_OK;
}


//	Render Frame
void RenderFrame(void)
{
	//***************************************************************
	//Clear back buffer
	//***************************************************************
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, g_clear_colour);

	//***************************************************************
	//Get any input 
	//***************************************************************
	g_CameraNode->SetVelocity(XMVectorZero());
	g_pDirectInput->ReadInputStates();
	g_pDirectInput->CheckKeysPressed(g_CameraNode, g_root_node);

	//***************************************************************
	//Set Velocities and Update each object
	//***************************************************************
	g_CameraNode->Update(g_pSkyBox, score, g_root_node);
	g_pDeltaTime->Update();
	for (size_t i = 0; i < g_vPlayerLasers.size(); i++)
	{
		g_vPlayerLasers[i]->AddVelocity(g_root_node);
		g_vPlayerLasers[i]->Update(g_pSkyBox, score, g_root_node);
	}
	for (size_t i = 0; i < g_vAsteroids.size(); i++)
	{
		g_vAsteroids[i]->Update(g_pSkyBox, score, g_root_node);
	}

	//***************************************************************
	//Apply any other transformations
	//***************************************************************
	for (size_t i = 0; i < g_vEnemies.size(); i++)
	{
		if (Pythagoras(g_CameraNode->GetPos(), g_vEnemies[i]->GetPos()) < ENEMY_SIGHT_RADIUS)
		{
			g_vEnemies[i]->ChaseOrFlee(g_CameraNode, g_root_node);
		}
		else
		{
			g_vEnemies[i]->MoveForwardIncY(g_root_node);
		}
		g_vEnemies[i]->Update(g_pSkyBox, score, g_root_node);
	}
	g_pParticleGenerator->SetRot(g_SpaceShipNode2->GetRotation());
	g_pParticleGenerator->SetPos(g_SpaceShipNode2->GetPos() + g_SpaceShipEngineNode->GetPos());
	g_CameraNode->SetRotation(g_pCamera->GetRot().x, g_pCamera->GetRot().y, g_pCamera->GetRot().z);
	g_CameraNode->SetVelocity(XMVectorZero());

	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//***************************************************************
	//Upload the new values for the constant buffer
	//***************************************************************
	CheckVelocities();
	XMMATRIX   projection, view;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), g_rect_width / g_rect_height, 1.0, farClipPlane);
	view = g_pCamera->GetViewMatrix();


	//***************************************************************
	//Draw
	//***************************************************************
	g_root_node->execute(&XMMatrixIdentity(), &view, &projection, g_root_node);
	g_pSkyBox->Draw(&view, &projection);
	g_pParticleGenerator->Draw(&view, &projection, &g_pCamera->GetPos());

	//***************************************************************
	//Text
	//***************************************************************
	// Enable Alpha blend for text editing and drawing on top 
	g_pImmediateContext->OMSetBlendState(g_pAlphaBlendEnable, 0, 0xffffffff);	
	string FPSText = string("FPS ") + to_string(g_pDeltaTime->GetFPS());
	g_pFPSText2D->AddText(FPSText, -1.0f, +1.0f, 0.05f);
	g_pFPSText2D->RenderText();
	string scoreText = string( "Score ") + to_string(score);
	g_pScoreText2D->AddText(scoreText, -0.25f, +0.9f, 0.1f);
	g_pScoreText2D->RenderText();
	g_pImmediateContext->OMSetBlendState(g_pAlphaBlendDisable, 0, 0xffffffff);

	//***************************************************************
	//Present everything on back buffer
	//***************************************************************
	g_pSwapChain->Present(0, 0);
}

void CreateAsteroids()
{
	for (int i = 0; i < ASTEROIDS_COUNT; i++)
	{
		SceneNode* tempAsteroid = new SceneNode(g_pDeltaTime, true, Tags::Asteroid, g_root_node, g_pAudioManager, 4.0f);
		
		tempAsteroid->SetModel(g_vAsteroidModels[rand()%4]);
		g_root_node->addChildNode(tempAsteroid);
		do
		{
			//***************************************************************
			//Random scale
			//***************************************************************
			float local_scale = 0.0f;
			do { local_scale = (rand() % ASTEROID_MAX_SCALE) / 10.0f; } while (local_scale <= ASTEROID_MIN_SCALE/10.0f);
			tempAsteroid->SetScale(local_scale);

			//***************************************************************
			//Random Rotation
			//***************************************************************
			tempAsteroid->SetRotation((float)RandomNumberGenerator(360), (float)RandomNumberGenerator(360), (float)RandomNumberGenerator(360));

			//***************************************************************
			//Random position within certain bounds
			//***************************************************************
			float distance = GAP_DISTANCE;
			do
			{
				float distanceBetweenEnemies = GAP_DISTANCE;
				do
				{
					tempAsteroid->SetPos(XMVectorSet((float)RandomNumberGenerator((int)g_pSkyBox->GetScale()), (float)RandomNumberGenerator((int)g_pSkyBox->GetScale()), (float)RandomNumberGenerator((int)g_pSkyBox->GetScale()), 0.0f));
					for (size_t i = 0; i < g_vAsteroids.size(); i++)
					{
						XMVECTOR dir = g_vAsteroids[i]->GetPos() - tempAsteroid->GetPos();
						distance = Pythagoras(dir);
						if (distance < GAP_DISTANCE)
						{							
							break;
						}
					}
					for (size_t i = 0; i < g_vEnemies.size(); i++)
					{
						XMVECTOR dir = g_vEnemies[i]->GetPos() - tempAsteroid->GetPos();
						distanceBetweenEnemies = Pythagoras(dir);
						if (distanceBetweenEnemies < GAP_DISTANCE)
						{
							break;
						}
					}

				} while (distanceBetweenEnemies < GAP_DISTANCE);

			} while (distance < GAP_DISTANCE);
		} while (Pythagoras(g_pCamera->GetPos(), tempAsteroid->GetPos()) < PLAYER_GAP_DISTANCE); 

		//***************************************************************
		//Random Velocity
		//***************************************************************
		tempAsteroid->SetVelocity(XMVectorSet((float)RandomNumberGenerator(ASTEROID_MAX_VELOCITY)/10.0f, (float)RandomNumberGenerator(ASTEROID_MAX_VELOCITY)/10.0f, (float)RandomNumberGenerator(ASTEROID_MAX_VELOCITY)/10.0f,0));

		
		tempAsteroid->SetMaxHealth();
		g_vAsteroids.push_back(tempAsteroid);
	}
}

int RandomNumberGenerator(int maxDistance)
{
	int randNum = 0;

	do
	{
		randNum = rand() % maxDistance;
		randNum -= maxDistance/2;
	} while (randNum < (maxDistance / 2) && randNum > maxDistance);

	//Possible Negative numbers
	return randNum;
}

int RandomNumberGeneratorNoNegative(int maxDistance)
{
	int randNum = 0;

	do
	{
		randNum = rand() % maxDistance/ 2;
	} while (randNum < (maxDistance / 2) && randNum > maxDistance);

	//No Possible negative numbers
	return randNum;
}

float Pythagoras(XMVECTOR v)
{
	return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

float Pythagoras(XMVECTOR v1, XMVECTOR v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

void CheckVelocities()
{
	for (size_t i = 0; i < g_vAsteroids.size(); i++)
	{
		if (g_vAsteroids[i]->GetVelocity().x > MAX_VELOCITY)
		{
			g_vAsteroids[i]->SetVelocity( XMVectorSetX(g_vAsteroids[i]->GetVelocity(), MAX_VELOCITY));
		}
		else if (g_vAsteroids[i]->GetVelocity().x < -MAX_VELOCITY)
		{
			g_vAsteroids[i]->SetVelocity( XMVectorSetX(g_vAsteroids[i]->GetVelocity(), -MAX_VELOCITY));
		}

		if (g_vAsteroids[i]->GetVelocity().y > MAX_VELOCITY)
		{
			g_vAsteroids[i]->SetVelocity( XMVectorSetY(g_vAsteroids[i]->GetVelocity(), MAX_VELOCITY));
		}
		else if (g_vAsteroids[i]->GetVelocity().y < -MAX_VELOCITY)
		{
			g_vAsteroids[i]->SetVelocity( XMVectorSetY(g_vAsteroids[i]->GetVelocity(), -MAX_VELOCITY));
		}

		if (g_vAsteroids[i]->GetVelocity().z > MAX_VELOCITY)
		{
			g_vAsteroids[i]->SetVelocity(XMVectorSetZ(g_vAsteroids[i]->GetVelocity(), MAX_VELOCITY));
		}
		else if (g_vAsteroids[i]->GetVelocity().z < -MAX_VELOCITY)
		{
			g_vAsteroids[i]->SetVelocity(XMVectorSetZ(g_vAsteroids[i]->GetVelocity(), -MAX_VELOCITY));
		}
	}
}
