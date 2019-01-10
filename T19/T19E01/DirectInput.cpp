#include "DirectInput.h"
#include <atomic>


DirectInput::DirectInput(HINSTANCE g_hInst, HWND g_hWnd, camera* Camera)
{
	m_hInst = g_hInst;
	m_hWnd = g_hWnd;
	m_pCamera = Camera;
	m_mouse_horizontal_move_speed = 0.5f;
	m_mouse_vertical_move_speed = 0.5f;
	m_controller_vertical_move_speed = 0.02f;
	m_controller_horizontal_move_speed = 0.03f;

}


DirectInput::~DirectInput()
{
	
	m_pCamera = nullptr;
	
	if (m_mouse_device)
	{
		m_mouse_device->Unacquire();
		m_mouse_device->Release();
	}
	if (m_keyboard_device)
	{
		m_keyboard_device->Unacquire();
		m_keyboard_device->Release();
	}

	if (m_direct_input)m_direct_input->Release();

}

HRESULT DirectInput::InitInput()
{
	HRESULT hr;
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));
	ZeroMemory(&m_mouse_state, sizeof(m_mouse_state));
	ZeroMemory(&m_xbox_state, sizeof(XINPUT_STATE));

	//	You need a DirectInput object in order to create device interfaces
	hr = DirectInput8Create(m_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if (FAILED(hr))return hr;

	//	Create devices using DirectInput
	hr = m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard_device, NULL);
	if (FAILED(hr))return hr;
	hr = m_direct_input->CreateDevice(GUID_SysMouse, &m_mouse_device, NULL);
	if (FAILED(hr))return hr;

	// Each Device needs to be told how to interpret the data that is being sent from the device
	hr = m_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))return hr;
	hr = m_mouse_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))return hr;

	//	Every input device also needs to be told how it will interact with other applications that use the device via DirectInput
	hr = m_keyboard_device->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))return hr;
	hr = m_mouse_device->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hr))return hr;

	//	You need to ask the device to start providing input for the application
	hr = m_keyboard_device->Acquire();
	if (FAILED(hr))return hr;
	hr = m_mouse_device->Acquire();
	if (FAILED(hr))return hr;

	return S_OK;
}

void DirectInput::ReadInputStates()
{
	HRESULT hr;
	hr = m_keyboard_device->GetDeviceState(sizeof(g_keyboard_keys_state), (LPVOID)&g_keyboard_keys_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_keyboard_device->Acquire();
		}
	}
	hr = m_mouse_device->GetDeviceState(sizeof(m_mouse_state), (LPVOID)&m_mouse_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_mouse_device->Acquire();
		}
	}
	DWORD dwResult = XInputGetState(0, &m_xbox_state);	
}

void DirectInput::CheckKeysPressed(SceneNode* g_cam_node, SceneNode* g_root_node)
{
	// Keyboard
	if (g_keyboard_keys_state[DIK_ESCAPE] & 0x80)
	{
		DestroyWindow(m_hWnd);
	}
	if (g_keyboard_keys_state[DIK_W] & 0x80)
	{
		float moveAmount = 0.01f;
		m_pCamera->Forward(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Forward(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (g_keyboard_keys_state[DIK_S] & 0x80)
	{
		float moveAmount = -0.01f;
		m_pCamera->Forward(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Forward(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (g_keyboard_keys_state[DIK_D] & 0x80)
	{
		float moveAmount = 0.01f;
		m_pCamera->Strafe(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Strafe(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (g_keyboard_keys_state[DIK_A] & 0x80)
	{
		float moveAmount = -0.01f;
		m_pCamera->Strafe(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Strafe(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (g_keyboard_keys_state[DIK_Q] & 0x80)
	{
		float moveAmount = 0.01f;
		m_pCamera->Up(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Up(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (g_keyboard_keys_state[DIK_E] & 0x80)
	{
		float moveAmount = -0.01f;
		m_pCamera->Up(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Up(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	

	//	Mouse
	if (m_mouse_state.lX > 0)
	{
		m_pCamera->RotateAroundY(m_mouse_horizontal_move_speed);
	}
	else if (m_mouse_state.lX < 0)
	{
		m_pCamera->RotateAroundY(-m_mouse_horizontal_move_speed);
	}
	if (m_mouse_state.lY < 0)
	{
		m_pCamera->RotateAroundX(m_mouse_vertical_move_speed);
	}
	else if (m_mouse_state.lY > 0)
	{
		m_pCamera->RotateAroundX(-m_mouse_vertical_move_speed);
	}

	// Controller
	if (m_xbox_state.Gamepad.sThumbLY>XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		float moveAmount = 0.01f;
		m_pCamera->Forward(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Forward(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	else if (m_xbox_state.Gamepad.sThumbLY<-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		float moveAmount = -0.01f;
		m_pCamera->Forward(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Forward(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (m_xbox_state.Gamepad.sThumbLX>XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		float moveAmount = 0.01f;
		m_pCamera->Strafe(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Strafe(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	else if (m_xbox_state.Gamepad.sThumbLX<-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		float moveAmount = -0.01f;
		m_pCamera->Strafe(moveAmount);

		// set camera node to the position of the cameera 
		g_cam_node->SetPos(m_pCamera->GetPos());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera postiion
		g_root_node->update_collision_tree(&identity, 1.0f);

		if (g_cam_node->check_collision(g_root_node) == true)
		{
			//if there is a collision, restore camera and camera node positions
			m_pCamera->Strafe(-moveAmount);
			g_cam_node->SetPos(m_pCamera->GetPos());//15
		}
	}
	if (m_xbox_state.Gamepad.sThumbRY>XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		m_pCamera->RotateAroundX(m_controller_vertical_move_speed);
	}
	else if (m_xbox_state.Gamepad.sThumbRY<-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		m_pCamera->RotateAroundX(-m_controller_vertical_move_speed);
	}
	if (m_xbox_state.Gamepad.sThumbRX>XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		m_pCamera->RotateAroundY(m_controller_horizontal_move_speed);
	}
	else if (m_xbox_state.Gamepad.sThumbRX<-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		m_pCamera->RotateAroundY(-m_controller_horizontal_move_speed);
	}
}
bool DirectInput::IsKeyPressed(unsigned char DI_keycode)
{	
		return g_keyboard_keys_state[DI_keycode];
}