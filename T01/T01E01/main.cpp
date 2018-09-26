#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <Windows.h>
#include <dxerr.h>

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


HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

char g_TutorialName[100] = "T01E01\0";


HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



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

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//do something
		}
	}
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
	RECT rc = { 0,0,640,480 };
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
		if (wParam == VK_ESCAPE)
			DestroyWindow(g_hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}