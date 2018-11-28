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
#include<WinUser.h>
#include<windowsx.h>
#include <dinput.h>
#include"camera.h"
class DirectInput
{
public:
	DirectInput(HINSTANCE g_hInst, HWND g_hWnd, camera* Camera);
	~DirectInput();
	HRESULT InitInput();
	void ReadInputStates();
	void IsKeyPressed();

private:
	IDirectInput8 * g_direct_input;
	IDirectInputDevice8* g_keyboard_device;
	HINSTANCE m_hInst = NULL;
	HWND m_hWnd = NULL;
	camera* m_pCamera;
	unsigned char g_keyboard_keys_state[256];
};

