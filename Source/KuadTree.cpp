#include "Globals.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ComponentMesh.h"
#include "KuadTree.h"

KuadTree::KuadTree() { 
	quadLimits = math::AABB(math::float3(-2.0f * App->scene->scaleFactor, -2.0f * App->scene->scaleFactor, -2.0f * App->scene->scaleFactor), math::float3(2.0f * App->scene->scaleFactor, 2.0f * App->scene->scaleFactor, 2.0f * App->scene->scaleFactor));
	InitQuadTree(quadLimits, true);
}

KuadTree::~KuadTree() {
	goList.clear();
	Clear();
}

void KuadTree::InitQuadTree(const math::AABB& aabb, bool clearAllGameObjects) {

	if (clearAllGameObjects) {
		goList.clear();
		Clear();
	} else {
		Clear();
	}

	if (App->camera->quadCamera != nullptr) {
		App->camera->quadCamera->frustum.pos.y = aabb.maxPoint.y + 0.01f * App->scene->scaleFactor;
		App->camera->quadCamera->frustum.farPlaneDistance = App->camera->quadCamera->frustum.pos.y + aabb.Size().y;
		App->camera->quadCamera->frustum.orthographicHeight = aabb.Size().x + 0.5f * App->scene->scaleFactor;
		App->camera->quadCamera->frustum.orthographicWidth = App->camera->quadCamera->frustum.orthographicHeight;
	}

	root = new QuadTreeNode(aabb);
}

void KuadTree::Insert(GameObject* gameObject, bool addQuadList) {
	if (gameObject->bbox.Intersects(root->aabb)) {
		if (addQuadList) {
			goList.push_back(gameObject);
		}

		root->Insert(gameObject);
	} else {
		ExpandLimits(gameObject);
	}
}

void KuadTree::ExpandLimits(GameObject* gameObject) {
	//TODO: x2 if the item is outside, we managed to increase the size to fit the object but not worth it, too many ifs
	quadLimits.maxPoint *= 2.0f;
	quadLimits.minPoint *= 2.0f;

	InitQuadTree(quadLimits);
	Insert(gameObject, true);
}

void KuadTree::Remove(GameObject* gameObject) {
	if (root != nullptr) {
		goList.remove(gameObject);

		InitQuadTree(quadLimits);

		for (std::list<GameObject*>::iterator it = goList.begin(); it != goList.end(); ++it) {
			Insert(*it);
		}
	}
}

void KuadTree::Clear() {
	delete root;
	root = nullptr;
}

QuadTreeNode::QuadTreeNode() {
	childs[0] = nullptr;
	childs[1] = nullptr;
	childs[2] = nullptr;
	childs[3] = nullptr;
}

QuadTreeNode::~QuadTreeNode() {
	goList.clear();

	delete childs[0];
	childs[0] = nullptr;
	delete childs[1];
	childs[1] = nullptr;
	delete childs[2];
	childs[2] = nullptr;
	delete childs[3];
	childs[3] = nullptr;
}

QuadTreeNode::QuadTreeNode(const math::AABB& aabb) : aabb(aabb) {
	childs[0] = nullptr;
	childs[1] = nullptr;
	childs[2] = nullptr;
	childs[3] = nullptr;
}

void QuadTreeNode::Insert(GameObject* gameObject) {
	if (IsLeaf() && (goList.size() < maxItems)) {
		goList.push_back(gameObject);
	} else {
		if (IsLeaf()) {
			CreateChilds();
		}

		goList.push_back(gameObject);
		RecalculateSpace();
	}
}

void QuadTreeNode::CreateChilds() {
	math::AABB newAABB;
	math::float3 aabbSize(aabb.Size());
	math::float3 newSize(aabbSize.x * 0.5f, aabbSize.y, aabbSize.z * 0.5f);
	math::float3 aabbCenter(aabb.CenterPoint());
	math::float3 newCenter(aabbCenter);

	newCenter.x = aabbCenter.x + aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z + aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[0] = new QuadTreeNode(newAABB);
	childs[0]->parent = this;

	newCenter.x = aabbCenter.x + aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z - aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[1] = new QuadTreeNode(newAABB);
	childs[1]->parent = this;

	newCenter.x = aabbCenter.x - aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z - aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[2] = new QuadTreeNode(newAABB);
	childs[2]->parent = this;

	newCenter.x = aabbCenter.x - aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z + aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[3] = new QuadTreeNode(newAABB);
	childs[3]->parent = this;
}

void QuadTreeNode::RecalculateSpace() {
	for (std::list<GameObject*>::iterator iterator = goList.begin(); iterator != goList.end();) {
		GameObject* gameObject = *iterator;
		bool intersects[4];
		for (int i = 0; i < 4; ++i) {
			ComponentMesh* componentMesh = (ComponentMesh*)(gameObject)->GetComponent(ComponentType::MESH);
			if (componentMesh != nullptr) {
				intersects[i] = childs[i]->aabb.Intersects(gameObject->bbox);
			}
		}

		if (intersects[0] && intersects[1] && intersects[2] && intersects[3]) {
			++iterator;
		} else {
			iterator = goList.erase(iterator);
			for (int i = 0; i < 4; ++i) {
				if (intersects[i]) {
					childs[i]->Insert(gameObject);
				}
			}
		}
	}
}

bool QuadTreeNode::IsLeaf() const {
	return childs[0] == nullptr;
}