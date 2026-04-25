#pragma once
class Entity;
class Component
{
	public:
		Component(Entity* anOwner);
		~Component();

		Entity* GetOwner() { return myOwner; }
	private:
		Entity* myOwner;
};

