#pragma once

class GameWorld
{
public:
	GameWorld(); 
	~GameWorld();

	void Init();
	void Update(float aTimeDelta); 
	void Render();
private:
	Tga::Sprite2DInstanceData myTGELogoInstance = {};
	Tga::SpriteSharedData mySharedData = {};
};