#ifndef __GameObject_h__
#define __GameObject_h__

#include <list>
#include <vector>

#include "IMGUI\imgui.h"
#include "assert.h"
#include "Globals.h"
#include "Assimp\include\assimp/matrix4x4.h"

#include "MathGeoLib\include\Math\float4x4.h"
#include "MathGeoLib\include\Geometry\AABB.h"

#include "rapidjson-1.1.0\include\rapidjson\document.h"
#include "rapidjson-1.1.0\include\rapidjson\prettywriter.h"

class Config;
class Component;
class ComponentMesh;
class ComponentMaterial;
class ComponentTransform;
enum class ComponentType;

class GameObject
{
	public:
		GameObject();
		GameObject(std::string goName, GameObject* goParent);
		GameObject(std::string goName, const math::float4x4& transform);
		GameObject(std::string goName, const math::float4x4& transform, GameObject* goParent);
		GameObject(const GameObject& duplicateGameObject);
		~GameObject();

		void							Update();
		void							DrawProperties();
		void							DrawHierarchy(GameObject* goSelected);
		void							ComputeBBox();
		void							ModelTransform(unsigned shader) const;
		void							UpdateStaticChilds(bool staticState);
		void							Load(Config* config, rapidjson::Value& value);

		bool							Save(Config* config);

	public:
		Component*						AddComponent(ComponentType type);
		Component*						GetComponent(ComponentType type) const;
		std::vector<Component*>			GetComponents(ComponentType type) const;
		std::list<Component*>::iterator RemoveComponent(std::list<Component*>::iterator component);

	public:
		bool							enabled = true;
		bool							drawGOBBox = false;
		bool							duplicating = false;
		bool							toBeDeleted = false;
		bool							toBeCopied = false;
		bool							moveGOUp = false;
		bool							moveGODown = false;
		bool							staticGo = false;

		char							uuid[37];
		char							parentUuid[37];
		
		std::string						name = std::string(DEFAULT_GO_NAME);
		GameObject*						parent = nullptr;


		math::AABB						bbox;

		ComponentTransform*				transform = nullptr;
		ComponentMesh*					mesh = nullptr;
		ComponentMaterial*				material = nullptr;

		std::list<Component*>			components;
		std::list<GameObject*>			goChilds;

};

#endif // __GameObject_h__
