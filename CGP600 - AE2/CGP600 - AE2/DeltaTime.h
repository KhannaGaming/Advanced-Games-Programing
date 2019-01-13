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
	DeltaTime();
	~DeltaTime();
	void Update();
	float GetDeltaTime();
	int GetFPS();

	int frameCounter;
	float timeElapsed;
	float gameTime;
	float startTime;


private:
	float m_deltaTime;
	float m_previousTime;
	float m_currentTime;
	int fps;

};

