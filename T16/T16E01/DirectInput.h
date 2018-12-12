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
#include<xinput.h>
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
	IDirectInput8 * m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	IDirectInputDevice8* m_mouse_device;
	DIMOUSESTATE m_mouse_state;
	XINPUT_STATE m_xbox_state;
	HINSTANCE m_hInst = NULL;
	HWND m_hWnd = NULL;
	camera* m_pCamera;
	unsigned char g_keyboard_keys_state[256];
	float m_mouse_horizontal_move_speed;
	float m_mouse_vertical_move_speed;
	float m_controller_vertical_move_speed;
	float m_controller_horizontal_move_speed;



};

