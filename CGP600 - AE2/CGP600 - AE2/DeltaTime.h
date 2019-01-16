#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>

class DeltaTime
{
public:
	//*********************************************************
	//METHODS
	//*********************************************************
	DeltaTime();
	void Update();
	float GetDeltaTime();
	int GetFPS();

private:
	//*********************************************************
	//VARIABLES
	//*********************************************************
	int		m_frameCounter;
	float	m_timeElapsed;
	float	m_gameTime;
	float	m_startTime;
	float	m_deltaTime;
	float	m_previousTime;
	float	m_currentTime;
	int		m_fps;
};

