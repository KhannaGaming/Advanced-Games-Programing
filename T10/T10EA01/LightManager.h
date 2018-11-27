#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

struct DirectionalLight
{
	string m_Name;
	XMVECTOR m_Pos;
	XMVECTOR m_Colour;
};
struct AmbientLight
{
	string m_Name;
	XMVECTOR m_Colour;
};
struct PointLight
{
	string m_Name;
	XMVECTOR m_Pos;
	XMVECTOR m_Colour;
};

class LightManager
{
public:
	LightManager();
	~LightManager();
	void CreateDirectionalLight(string Name, XMVECTOR Pos, XMVECTOR Colour);
	void CreateAmbientLight(string Name, XMVECTOR Colour);
	void CreatePointLight(string Name, XMVECTOR Pos, XMVECTOR Colour);
	void SetLightColour(string lightName, XMVECTOR Colour);
	void SetLightPosition(string lightName, XMVECTOR Pos);
	XMVECTOR GetLightColour(string lightName);
	XMVECTOR GetLightPosition(string lightName);

private:
	vector<DirectionalLight> m_vDirectionalLights;
	vector<AmbientLight> m_vAmbientLights;
	vector<PointLight> m_vPointLights;

};

