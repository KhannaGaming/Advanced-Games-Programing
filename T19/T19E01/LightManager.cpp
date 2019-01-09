#include "LightManager.h"



LightManager::LightManager()
{
}


LightManager::~LightManager()
{
}

void LightManager::CreateDirectionalLight(string Name, XMVECTOR Pos, XMVECTOR Colour)
{
	DirectionalLight directionalLight = {Name, Pos, Colour };
	m_vDirectionalLights.push_back(directionalLight);
}

void LightManager::CreateAmbientLight(string Name, XMVECTOR Colour)
{
	AmbientLight ambientLight = {Name, Colour };
	m_vAmbientLights.push_back(ambientLight);
}

void LightManager::CreatePointLight(string Name, XMVECTOR Pos, XMVECTOR Colour)
{
	PointLight pointLight = { Name, Pos, Colour };
	m_vPointLights.push_back(pointLight);
	
}

void LightManager::SetLightColour(string lightName, XMVECTOR Colour)
{
	for (int i = 0; i < m_vDirectionalLights.size(); i++)
	{
		if (m_vDirectionalLights[i].m_Name == lightName)
		{
			m_vDirectionalLights[i].m_Colour = Colour;
			break;
		}
	}
	for (int i = 0; i < m_vAmbientLights.size(); i++)
	{
		if (m_vAmbientLights[i].m_Name == lightName)
		{
			m_vAmbientLights[i].m_Colour = Colour;
			break;
		}
	}
	for (int i = 0; i < m_vPointLights.size(); i++)
	{
		if (m_vPointLights[i].m_Name == lightName)
		{
			m_vPointLights[i].m_Colour = Colour;
			break;
		}
	}
}

void LightManager::SetLightPosition(string lightName, XMVECTOR Pos)
{
	for (int i = 0; i < m_vDirectionalLights.size(); i++)
	{
		if (m_vDirectionalLights[i].m_Name == lightName)
		{
			m_vDirectionalLights[i].m_Pos = Pos;
			break;
		}
	}
	for (int i = 0; i < m_vPointLights.size(); i++)
	{
		if (m_vPointLights[i].m_Name == lightName)
		{
			m_vPointLights[i].m_Pos = Pos;
			break;
		}
	}
}

XMVECTOR LightManager::GetLightColour(string lightName)
{
	for (int i = 0; i < m_vDirectionalLights.size(); i++)
	{
		if (m_vDirectionalLights[i].m_Name == lightName)
		{
			return m_vDirectionalLights[i].m_Colour;			
		}
	}
	for (int i = 0; i < m_vAmbientLights.size(); i++)
	{
		if (m_vAmbientLights[i].m_Name == lightName)
		{
			return m_vAmbientLights[i].m_Colour;
		}
	}
	for (int i = 0; i < m_vPointLights.size(); i++)
	{
		if (m_vPointLights[i].m_Name == lightName)
		{
			return m_vPointLights[i].m_Colour;			
		}
	}
	return XMVectorSet(0, 0, 0, 0);
}

XMVECTOR LightManager::GetLightPosition(string lightName)
{
	for (int i = 0; i < m_vDirectionalLights.size(); i++)
	{
		if (m_vDirectionalLights[i].m_Name == lightName)
		{
			return m_vDirectionalLights[i].m_Pos;
			
		}
	}
	for (int i = 0; i < m_vPointLights.size(); i++)
	{
		if (m_vPointLights[i].m_Name == lightName)
		{
			return m_vPointLights[i].m_Pos;			
		}
	}
	return XMVectorSet(0,0,0,0);
}
