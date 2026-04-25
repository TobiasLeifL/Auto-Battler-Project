#include "stdafx.h"
#include "GameWorld.h"
#include "Entity/Entity.h"
#include "Component/Transform_Component.h"

GameWorld::GameWorld()
{
	
}

GameWorld::~GameWorld()
{
	
}

void GameWorld::Init()  
{
	//Testing
	Entity* newEntity = new Entity();
	newEntity->AddComponent<Transform_Component>();
	Transform_Component* transformComp = newEntity->GetComponent<Transform_Component>();
	transformComp;

	Entity* newEntityTwo = new Entity();
	Transform_Component* transformCompTwo = newEntityTwo->GetComponent<Transform_Component>();
	transformCompTwo;
	//

}

void GameWorld::Update(float aTimeDelta)
{
	UNREFERENCED_PARAMETER(aTimeDelta);
}

void GameWorld::Render()
{

}