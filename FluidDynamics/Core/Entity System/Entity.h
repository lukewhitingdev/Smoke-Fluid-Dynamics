#pragma once
#include "Component.h"
#include <vector>
#include <map>

class Entity
{
public:
	Entity() : id(0), name("N/A") {
		id = reinterpret_cast<unsigned long long>(this); 
		components = std::map<ComponentTypes, std::vector<Component*>>();
	};
	Entity(const char* name) : id(0), name(name) { id = reinterpret_cast<unsigned long long>(this); };
	virtual ~Entity() = default;

	void setID(int ID) { id = ID; }
	unsigned long long getID() { return id; }

	void setName(const char* NAME) { name = NAME; }
	const char* getName() { return name; }

	template<typename T> 
	T* addComponent()
	{
		// If this fails to convert check newest component for proper inheritence.
		Component comp = static_cast<Component>(T());

		// If we dont already have a vector for this component type then make one.
		if(components.find(comp.getType()) == components.end())
		{
			components.emplace(std::make_pair<ComponentTypes, std::vector<Component*>>(comp.getType(), std::vector<Component*>()));
		}

		T* pointer = new T();
		static_cast<Component*>(pointer)->setParent(this);

		components.at(comp.getType()).push_back((Component*)pointer);
		return pointer;
	}

	template<typename T>
	T* getComponent()
	{
 		Component comp = (Component)T();

		// If we dont have a component vector of this type we dont have any at all.
		if (components.find(comp.getType()) == components.end())
		{
			return nullptr;
		}

		std::vector<Component*>* componentVector = &components.at(comp.getType());

		for(int i = 0; i < componentVector->size(); i++)
		{
			Component* component = componentVector->at(i);
			if(component->getType() == comp.getType())
			{
				return (T*)component;
			}
		}

		return nullptr;
	}

	template<typename T>
	std::vector<T*> getAllComponents()
	{
		std::vector<T*> output = std::vector<T*>();
		Component comp = (Component)T();

		// If we dont have a component vector of this type we dont have any at all.
		if (components.find(comp.getType()) == components.end())
		{
			return output;
		}

		std::vector<Component*>* componentVector = &components.at(comp.getType());

		for (int i = 0; i < componentVector->size(); i++)
		{
			Component* component = componentVector->at(i);
			if (component->getType() == comp.getType())
			{
				output.emplace_back((T*)component);
			}
		}

		return output;
	}

	template<typename T>
	void removeComponent()
	{
		Component comp = (Component)T();

		// If we dont have a component vector of this type we dont have any at all.
		if (components.find(comp.getType()) == components.end())
		{
			return;
		}

		std::vector<Component*>* componentVector = &components.at(comp.getType());

		for (int i = 0; i < componentVector->size(); i++)
		{
			Component* component = componentVector->at(i);
			if (component->getType() == comp.getType())
			{
				componentVector->erase(componentVector->begin() + i);
				delete component;
				return;
			}
		}
	}

	template<typename T>
	void removeAllComponents()
	{
		Component comp = (Component)T();

		// If we dont have a component vector of this type we dont have any at all.
		if (components.find(comp.getType()) == components.end())
		{
			return;
		}

		std::vector<Component*>* componentVector = &components.at(comp.getType());

		componentVector->clear();
	}

protected:
	unsigned long long id;
	const char* name;
	std::map<ComponentTypes, std::vector<Component*>> components;
};