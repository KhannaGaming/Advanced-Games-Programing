#include "DirectInput.h"



DirectInput::DirectInput(HINSTANCE g_hInst, HWND g_hWnd, camera* Camera)
{
	m_hInst = g_hInst;
	m_hWnd = g_hWnd;
	m_pCamera = Camera;
}


DirectInput::~DirectInput()
{
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	if (g_keyboard_device)
	{
		g_keyboard_device->Unacquire();
		g_keyboard_device->Release();
	}

	if (g_direct_input)g_direct_input->Release();

}

HRESULT DirectInput::InitInput()
{
	HRESULT hr;
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));

	//	You need a DirectInput object in order to create device interfaces
	hr = DirectInput8Create(m_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	if (FAILED(hr))return hr;

	//	Create devices using DirectInput
	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_keyboard_device, NULL);
	if (FAILED(hr))return hr;

	// Each Device needs to be told how to interpret the data that is being sent from the device
	hr = g_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))return hr;

	//	Every input device also needs to be told how it will interact with other applications that use the device via DirectInput
	hr = g_keyboard_device->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))return hr;

	//	You need to ask the device to start providing input for the application
	hr = g_keyboard_device->Acquire();
	if (FAILED(hr))return hr;

	return S_OK;
}

void DirectInput::ReadInputStates()
{
	HRESULT hr;
	hr = g_keyboard_device->GetDeviceState(sizeof(g_keyboard_keys_state), (LPVOID)&g_keyboard_keys_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_keyboard_device->Acquire();
		}
	}
}

void DirectInput::IsKeyPressed()
{
	if (g_keyboard_keys_state[DIK_ESCAPE] & 0x80)
	{
		DestroyWindow(m_hWnd);
	}
	if (g_keyboard_keys_state[DIK_W] & 0x80)
	{
		m_pCamera->Forward(0.01f);
	}
	if (g_keyboard_keys_state[DIK_S] & 0x80)
	{
		m_pCamera->Forward(-0.01f);
	}
	if (g_keyboard_keys_state[DIK_D] & 0x80)
	{
		m_pCamera->Strafe(0.01f);
	}
	if (g_keyboard_keys_state[DIK_A] & 0x80)
	{
		m_pCamera->Strafe(-0.01f);
	}
}
