#include "stdafx.h"

#include "GameWorld.h"
#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/DebugDrawer.h>


GameWorld::GameWorld()
{}

GameWorld::~GameWorld() 
{}

void GameWorld::Init()  
{
	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };
	{
		mySharedData.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/tge_logo_w.dds");

		myTGELogoInstance.myPivot = { 0.5f, 0.5f };
		myTGELogoInstance.myPosition = Tga::Vector2f{ 0.5f, 0.5f }*resolution;
		myTGELogoInstance.mySize = Tga::Vector2f{ 0.75f, 0.75f }*resolution.y;
		myTGELogoInstance.myColor = Tga::Color(1, 1, 1, 1);
	}
}
void GameWorld::Update(float aTimeDelta)
{
	UNREFERENCED_PARAMETER(aTimeDelta);

}

void GameWorld::Render()
{
	auto &engine = *Tga::Engine::GetInstance();
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	// Game update
	{
		spriteDrawer.Draw(mySharedData, myTGELogoInstance);
	}

	// Debug draw pivot
#ifndef _RETAIL
	{
		Tga::DebugDrawer& dbg = engine.GetDebugDrawer();
		Tga::Color c1 = myTGELogoInstance.myColor;
		dbg.DrawCircle(myTGELogoInstance.myPosition, 5.f, (c1.myR + c1.myG + c1.myB) / 3 > 0.3f ? Tga::Color(0, 0, 0, 1) : Tga::Color(1, 1, 1, 1));
	}
#endif
}