#include "DeltaTime.h"



DeltaTime::DeltaTime()
{
	m_previousTime = (float(timeGetTime()) / 1000.0f);
	m_deltaTime = 0.0f;
	m_currentTime = 0.0f;
	timeElapsed = 0.0f;
	gameTime = (float(timeGetTime()) / 1000.0f);
	startTime = (float(timeGetTime()) / 1000.0f);
	fps = 0;
}


DeltaTime::~DeltaTime()
{
}

void DeltaTime::Update()
{
	m_currentTime = (float(timeGetTime()) / 1000.0f);
	m_deltaTime = m_currentTime - m_previousTime;
	m_previousTime = m_currentTime;
}

float DeltaTime::GetDeltaTime()
{
	return m_deltaTime;
}

int DeltaTime::GetFPS()
{
	
	frameCounter++;
	gameTime = (float(timeGetTime()) / 1000.0f) - startTime;
	
	if (gameTime - timeElapsed >= 1)
	{
		fps = frameCounter;
		frameCounter = 0;	
		timeElapsed++;
	}
	return fps;
}
