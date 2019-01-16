#include "DeltaTime.h"



DeltaTime::DeltaTime()
{
	m_previousTime = (float(timeGetTime()) / 1000.0f);
	m_deltaTime = 0.0f;
	m_currentTime = 0.0f;
	m_timeElapsed = 0.0f;
	m_gameTime = (float(timeGetTime()) / 1000.0f);
	m_startTime = (float(timeGetTime()) / 1000.0f);
	m_fps = 0;
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
	m_frameCounter++;
	m_gameTime = (float(timeGetTime()) / 1000.0f) - m_startTime;
	
	if (m_gameTime - m_timeElapsed >= 1)
	{
		m_fps = m_frameCounter;
		m_frameCounter = 0;	
		m_timeElapsed++;
	}
	return m_fps;
}
