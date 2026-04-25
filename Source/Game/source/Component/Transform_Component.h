#pragma once
#include "Component.h"

class Transform_Component : public Component
{
	public:
		Transform_Component(Entity* anOwner);
		~Transform_Component();
	private:
};

