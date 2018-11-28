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
#include "text2D.h"
#include "Model.h"

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

///////////////////////////////////////////////////////////////////////
//Globals
HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;
D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pD3DDevice = NULL;
ID3D11DeviceContext*		g_pImmediateContext = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pBackBufferRTView = NULL;
ID3D11Buffer*				g_pVertexBuffer;
ID3D11VertexShader*			g_pVertexShader;
ID3D11PixelShader*			g_pPixelShader;
ID3D11InputLayout*			g_pInputLayout;
ID3D11Buffer*				g_pConstantBuffer0;
ID3D11DepthStencilView*		g_pZBuffer;
camera*						g_pCamera;
ID3D11ShaderResourceView*	g_pTexture0;
ID3D11ShaderResourceView*	g_pTexture1;
ID3D11SamplerState*			g_pSampler0;
Text2D*						g_pText2D;
Text2D*						g_pText2D2;
ID3D11BlendState* g_pAlphaBlendEnable; //14
ID3D11BlendState* g_pAlphaBlendDisable; //14
XMVECTOR g_directional_light_shines_from;
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_color;
XMVECTOR g_point_light_position;
XMVECTOR g_point_light_colour;
Model* g_pModel;
Model* g_pModel2;
Model* g_pModel3;
Model* g_pModel4;
Model* g_pModel5;
vector<Model*> g_vModels;

	//Define vertices of a triangle - screen coordinates -1.0 to +1.0
//Define vertex structure
struct POS_COL_TEX_NORM_VERTEX//This will be added to and renamed in future tutorials
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

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

//Backgournd clear colour
float g_clear_colour[4] = { 0.0f,0.0f,0.0f,1.0f };
float g_x = 0;
float g_y = 0;
float g_rect_width = 600;
float g_rect_height = 600;
const float speed = 0.06f;
float xpos=0;
float ypos=0;
float zpos=10;
float xDegrees = 0;
float yDegrees = 0;
float zDegrees = 0;

bool aPressed = false;
bool sPressed = false;
bool wPressed = false;
bool dPressed = false;
string fileName = "Assets/tiger.obj";



//Const buffer structs. Pack to 16 bytes. Don't let any single element cross a 16 byte boundary
struct CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;		// 64 bytes (4x4=16 floats x 4 floats)
	XMVECTOR directional_light_vector;	//16 bytes
	XMVECTOR directional_light_colour;	//16 bytes
	XMVECTOR ambient_light_colour;		//16 bytes
	XMVECTOR point_light_position;
	XMVECTOR point_light_colour;
};//TOTAL SIZE = 112 bytes

////////////////////////////////////////////////////////////////////////
//Change every tutorial
const char g_TutorialName[100] = "T09E01\0";

////////////////////////////////////////////////////////////////////////
//Forward declarations
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitialiseD3D();
void ShutdownD3D();
void RenderFrame(void);
void Input();
HRESULT InitialiseGraphics(void);



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

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Input();
			RenderFrame();
		}
	}

	ShutdownD3D();

	return(int)msg.wParam;
}


HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	//Give your app your own name
	char Name[100] = "Space Game\0";

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
	
	case WM_KEYDOWN:
		//D3D11_MAPPED_SUBRESOURCE ms;
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(g_hWnd);
			//break;
		}
		if (wParam == 0x41)//A
		{
			aPressed = true;
			//break;
		};
		if (wParam == 0x44)//D
		{
			dPressed = true;
		}
		if (wParam == 0x53)//S
		{
			sPressed = true;
			//break;
		};
		if (wParam == 0x57)//W
		{
			wPressed = true;
		}
		break;
		
	case WM_MOUSEMOVE:
		
		if (g_x > GET_X_LPARAM(lParam))
		{
			g_x = GET_X_LPARAM(lParam);
			g_pCamera->Rotate(-1.0f);

		}
		if (g_x < GET_X_LPARAM(lParam))
		{
			g_x = GET_X_LPARAM(lParam);
			g_pCamera->Rotate(1.0f);

		}
		break;

	case WM_KEYUP:

		if (wParam == 0x41)//A
		{
			aPressed = false;
		}
		if (wParam == 0x44)//D
		{
			dPressed = false;
		}
		if (wParam == 0x53)//S
		{
			sPressed = false;
			//break;
		};
		if (wParam == 0x57)//W
		{
			wPressed = false;
		}

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

	g_pText2D = new Text2D("Assets/font1.png", g_pD3DDevice, g_pImmediateContext);
	g_pText2D2 = new Text2D("Assets/font1.png", g_pD3DDevice, g_pImmediateContext);

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
	for (int i = 0; i < g_vModels.size(); i++)
	{
		if (g_vModels[i])
		{
			delete g_vModels[i];
			g_vModels[i] = nullptr;
		}
	}
	/*if (g_pModel4)
	{
		delete g_pModel4;
		g_pModel4 = nullptr;
	}
	if (g_pModel3)
	{
		delete g_pModel3;
		g_pModel3 = nullptr;
	}
	if (g_pModel2)
	{
		delete g_pModel2;
		g_pModel2 = nullptr;
	}
	if (g_pModel)
	{
		delete g_pModel;
		g_pModel = nullptr;
	}*/

	if (g_pAlphaBlendDisable) g_pAlphaBlendDisable->Release();
	if (g_pAlphaBlendEnable) g_pAlphaBlendEnable->Release();

	if (g_pText2D2) 
	{
		delete g_pText2D2;
		g_pText2D2 = nullptr;
	}

	if (g_pText2D)
	{
		delete g_pText2D;
		g_pText2D = nullptr;
	}

	if (g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = nullptr;
	}
	if (g_pVertexBuffer) g_pVertexBuffer->Release();//03-01
	if (g_pInputLayout) g_pInputLayout->Release();//03-01
	if (g_pConstantBuffer0) g_pConstantBuffer0->Release();
	if (g_pVertexShader) g_pVertexShader->Release();//03-01
	if (g_pPixelShader) g_pPixelShader->Release();//03-01
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pZBuffer) g_pZBuffer->Release();
	if (g_pTexture1)g_pTexture1->Release();
	if (g_pTexture0)g_pTexture0->Release();
	if (g_pSampler0)g_pSampler0->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}

//Init graphics - Tu03
HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;
	
	g_pModel = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel->LoadObjModel((char*)"Assets/sphere.obj" ,-50.0f,5.0f,50.0f);
	g_vModels.push_back(g_pModel);
	g_pModel2 = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel2->LoadObjModel((char*)"Assets/floor.obj", 0.0f, -2.0f, 50.0f);
	g_vModels.push_back(g_pModel2);
	g_pModel3 = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel3->LoadObjModel((char*)"Assets/teapot.obj", 50.0f, 0.0f, 50.0f);
	g_vModels.push_back(g_pModel3);
	g_pModel4 = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel4->LoadObjModel((char*)"Assets/teapot.obj", 50.0f, 0.0f, 100.0f);
	g_vModels.push_back(g_pModel4);
	g_pModel5 = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel5->LoadObjModel((char*)"Assets/teapot.obj", -50.0f, 0.0f, 100.0f);
	g_vModels.push_back(g_pModel5);

	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = /*sizeof(POS_COL_VERTEX) **/ sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);
	//Create the buffer

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));
	
	
	//Unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);

	//Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 144; //MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);

	if (FAILED(hr))return hr;

	CONSTANT_BUFFER0 cb0_values;
	//cb0_values.RedAmount = 0.5f; // 50% of vertex red value 
	//cb0_values.GreenAmount = 0.5f; // 50% of vertex green value 
	//cb0_values.scale = 0.5f;

	//Upload the new values for the constant buffer
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);

	//Load and compile the pixel and vertex shaders- use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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
	hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}

	//Set the shader objects as active
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);

	

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//NOTE the spelling of COLOR. Again, be careful setting the correct dxgi format (using A32) and correct D3D version
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	hr = g_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	g_pCamera = new camera(0.0f, 1.0f, 0.0f,0.0f);

	hr = g_pModel->AddTexture((char*)"Assets/metal.jpg");
	if (FAILED(hr))
	{
		return hr;
	}	
	hr = g_pModel2->AddTexture((char*)"Assets/metal.jpg");
	if (FAILED(hr))
	{
		return hr;
	}
	hr = g_pModel3->AddTexture((char*)"Assets/metal.jpg");
	if (FAILED(hr))
	{
		return hr;
	}
	hr = g_pModel4->AddTexture((char*)"Assets/metal.jpg");
	if (FAILED(hr))
	{
		return hr;

	}
	hr = g_pModel5->AddTexture((char*)"Assets/metal.jpg");
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}


//Render Frame
void RenderFrame(void)
{
	g_pModel->LookAt_XZ(g_pModel3->GetPos().x, g_pModel3->GetPos().z);
	g_pModel->MoveForward(0.001f);
	for (int i = 0; i < g_vModels.size(); i++)
	{
		if (g_pModel->CheckCollision(g_vModels[i]))
		{
			if (i != 1)
			{
				g_pModel->MoveForwardIncY(-0.001f);
			}
		}
	}

	g_pModel3->LookAt_XZ(g_pModel->GetPos().x, g_pModel->GetPos().z);

	//g_pModel3->IncPos(0,  0.001f,0);

	//g_pModel3->MoveForward(0.001f);

	g_pModel4->LookAt_XZ(g_pModel5->GetPos().x, g_pModel5->GetPos().z);
	//g_pModel4->MoveForward(0.001f);

	g_pModel5->LookAt_XZ(g_pModel->GetPos().x, g_pModel->GetPos().z);
	//g_pModel5->MoveForward(0.001f);

	g_pImmediateContext->OMSetBlendState(g_pAlphaBlendEnable, 0, 0xffffffff);
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, g_clear_colour);

	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//Render HERE

	//Upload the new values for the constant buffer
	XMMATRIX   projection, view;

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), g_rect_width / g_rect_height, 1.0, 100.0);
	view = g_pCamera->GetViewMatrix();


	// Draw Objects Here
	for (int i = 0; i < g_vModels.size(); i++)
	{
		g_vModels[i]->Draw(&view, &projection);
	}
	

	g_pImmediateContext->OMSetBlendState(g_pAlphaBlendDisable, 0, 0xffffffff);

	//Display what has just been rendered;
	g_pSwapChain->Present(0, 0);
}

void Input()
{
	if (aPressed == true)
	{
		//zDegrees += 0.01f;
		g_pCamera->Strafe(-0.01f);
		//g_pModel->IncRotation(0.0f, 0.0f, 0.1f);
	}

	if (sPressed == true)
	{
		//yDegrees += 0.01f;
		//zpos -= 0.001f;
		g_pCamera->Forward(-0.01f);
		//g_pModel->IncPos(-0.01f, 0, 0);
	}

	if (wPressed == true)
	{
		//xDegrees -= 0.01f;
		//zpos += 0.001f;
		g_pCamera->Forward(0.01f);
		//g_pModel->IncPos(0.01f, 0, 0);
	}

	if (dPressed == true)
	{
		//zDegrees -= 0.01f;
		g_pCamera->Strafe(0.01f);
		//g_pModel->IncScale(0.01f);
	}
}
