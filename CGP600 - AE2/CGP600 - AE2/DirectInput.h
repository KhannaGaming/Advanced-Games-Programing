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
#include <xinput.h>
#include <WinUser.h>
#include <windowsx.h>
#include <dinput.h>
#include "camera.h"
#include "SceneNode.h"
#include "DeltaTime.h"

#define DIM_LEFT 0
#define DIM_RIGHT 1
#define DIM_CENTRE 2
#define DIM_OTHER 3

class DirectInput
{
public:
	//***************************************************
	//METHODS
	//***************************************************
	DirectInput(HINSTANCE g_hInst, HWND g_hWnd, camera* Camera, DeltaTime* deltaTime, AudioManager* audioManager);
	~DirectInput();
	HRESULT	InitInput();
	void	ReadInputStates();
	void	CheckKeysPressed(SceneNode* g_cam_node, SceneNode* g_root_node);
	bool	IsKeyPressed(unsigned char DI_keycode);
	void	SetVibrations(XMVECTOR dir);
	float	GetPlayerSpeed() { return m_playerMoveSpeed; };

private:
	//***************************************************
	//POINTERS
	//***************************************************
	IDirectInput8 *			m_direct_input;
	IDirectInputDevice8*	m_keyboard_device;
	IDirectInputDevice8*	m_mouse_device;
	camera*					m_pCamera;
	DeltaTime*				m_pDeltaTime;
	AudioManager*			m_pAudioManager;

	//***************************************************
	//VARIABLES
	//***************************************************
	DIMOUSESTATE			m_mouse_state;
	XINPUT_STATE			m_xbox_state;
	XINPUT_VIBRATION		m_xbox_vibration_state;
	HINSTANCE				m_hInst = NULL;
	HWND					m_hWnd = NULL;
	unsigned char			m_keyboard_keys_state[256];
	float					m_mouse_horizontal_move_speed;
	float					m_mouse_vertical_move_speed;
	float					m_controller_vertical_move_speed;
	float					m_controller_horizontal_move_speed;
	float					m_vibration_cooldown;
	float					m_cur_vibration_cooldown;
	float					m_leftStickVibration, m_rightStickVibration;
	float					m_playerMoveSpeed;
	float					m_mouse_click_cooldown;
	float					m_cur_mouse_click_cooldown;
	bool					m_fired;
};

