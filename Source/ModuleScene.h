#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "MathGeoLib\include\Math\Quat.h"
#include "MathGeoLib\include\Math\float3.h"
#include "MathGeoLib\include\Math\float4.h"
#include "MathGeoLib\include\Math\float4x4.h"

#include "rapidjson-1.1.0\include\rapidjson\document.h"
#include "rapidjson-1.1.0\include\rapidjson\prettywriter.h"

enum class GeometryType {
	SPHERE = 0,
	TORUS,
	PLANE,
	CUBE
};

class Config;
class GameObject;
class KuadTree;

class ModuleScene : public Module
{
	public:
		ModuleScene();
		~ModuleScene();

		bool			Init() override;
		bool			CleanUp() override;
		
		update_status	Update() override;

		void			DrawHierarchy();
		void			LoadGeometry(GameObject* goParent, GeometryType geometryType);
		void			CreateGameObject(Config* config, rapidjson::Value& value);
		void			SaveScene();
		void			SaveGameObject(Config* config, GameObject* gameObject);
		void			LoadScene();
		void			ClearScene();

	public:
		GameObject*		CreateGameObject(const char* goName = nullptr, GameObject* goParent = nullptr, const math::float4x4& transform = math::float4x4().identity);
		GameObject*		CreateCamera(GameObject* goParent = nullptr, const math::float4x4& transform = math::float4x4().identity);
		GameObject*		GetGameObjectByUUID(GameObject* gameObject, char uuidObjectName[37]);

	public:
		GameObject*			root = nullptr;
		GameObject*			goSelected = nullptr;
		KuadTree*			quadTree = nullptr;

		int					scaleFactor = 1000;
		math::float3		lightPosition = math::float3(0.0f, 1.0 * scaleFactor, 1.0f * scaleFactor);
		float				ambientLight = 0.3f;
};

#endif // __ModuleScene_h__
