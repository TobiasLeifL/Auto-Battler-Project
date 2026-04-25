#pragma once
#include <unordered_map>
#include "Component/Component.h"
#include "Component/TypeID.hpp"

class Entity
{
	public:
		Entity();
		~Entity();

		template <typename T>
		void AddComponent();

		template <typename T>
		T* GetComponent();

	private:
		std::unordered_map<int,Component*> myComponents;
};

template<typename T>
inline void Entity::AddComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
	T* newComponent = new T(this);
	myComponents.insert({ GetTypeID<T>(), newComponent });
}

template<typename T>
inline T* Entity::GetComponent()
{
	auto curComponent = myComponents.find(GetTypeID<T>());

	if (curComponent != myComponents.end())
	{
		T* returnComp = static_cast<T*>(curComponent->second);
		return returnComp;
	}
	return nullptr;
}
