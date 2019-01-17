#include "Config.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "imgui_internal.h"
#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(GameObject* goContainer, const math::float4x4& transform) : Component(goContainer, ComponentType::TRANSFORM) {
	AddTransform(transform);
}

ComponentTransform::ComponentTransform(const ComponentTransform& duplicatedTransform) : Component(duplicatedTransform){
	position = duplicatedTransform.position;
	rotation = duplicatedTransform.rotation;
	scale = duplicatedTransform.scale;
	eulerRotation = duplicatedTransform.eulerRotation;
}

ComponentTransform::~ComponentTransform() { }

Component* ComponentTransform::Duplicate() {
	return new ComponentTransform(*this);
}

void ComponentTransform::AddTransform(const math::float4x4& transform) {
	math::float3 translation;
	math::float3 scaling;
	math::Quat aiRotation;
	transform.Decompose(translation, aiRotation, scaling);

	position = { translation.x, translation.y, translation.z };
	scale = { scaling.x, scaling.y, scaling.z };
	rotation = math::Quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);
	RotationToEuler();
}

void ComponentTransform::SetRotation(const Quat& rot) {
	rotation = rot;
	RotationToEuler();
}

void ComponentTransform::RotationToEuler() {
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation.x = math::RadToDeg(eulerRotation.x);
	eulerRotation.y = math::RadToDeg(eulerRotation.y);
	eulerRotation.z = math::RadToDeg(eulerRotation.z);
}

void ComponentTransform::SetPosition(const math::float3& pos) {
	position = pos;
}

void ComponentTransform::SetLocalToWorld(const math::float4x4& localTrans) {
	math::float4x4 world = localTrans;
	world.Decompose(position, rotation, scale);
	RotationToEuler();
}

void ComponentTransform::SetWorldToLocal(const math::float4x4& parentTrans) {
	math::float4x4 world = math::float4x4::FromTRS(position, rotation, scale);
	math::float4x4 local = parentTrans.Inverted() * world;
	local.Decompose(position, rotation, scale);
	RotationToEuler();
}

math::float4x4 ComponentTransform::GetLocalTransform() const {
	return math::float4x4::FromTRS(position, rotation, scale);
}

math::float4x4 ComponentTransform::GetGlobalTransform() const {
	if (goContainer->parent != nullptr && goContainer->parent->transform != nullptr) {
		return goContainer->parent->transform->GetGlobalTransform() * goContainer->transform->GetLocalTransform();
	}

	return GetLocalTransform();
}

void ComponentTransform::SetGlobalTransform(const math::float4x4& global) {
	SetLocalToWorld(global);	
	math::float4x4 parentglobal = math::float4x4::identity;

	if (goContainer->parent != nullptr && goContainer->parent->transform != nullptr) {
		parentglobal = goContainer->parent->transform->GetGlobalTransform();
	}
	SetWorldToLocal(parentglobal);
	goContainer->ComputeBBox();
}

void ComponentTransform::DrawProperties(bool staticGo) {

	if (ImGui::CollapsingHeader("Local Transform")) {

		// https://github.com/ocornut/imgui/issues/211
		if (staticGo) {
			ImGui::PushItemFlag({ ImGuiButtonFlags_Disabled | ImGuiItemFlags_Disabled | ImGuiSelectableFlags_Disabled }, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::DragFloat3("Position", (float*)&position, 0.001f * App->scene->scaleFactor, -100.0f * App->scene->scaleFactor, 100.f * App->scene->scaleFactor)) {
			edited = true;
		}

		if (ImGui::DragFloat3("Rotation", (float*)&eulerRotation, 0.5f, -360, 360.f)) {
			edited = true;
		}

		if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f, 0.1f, 100.f)) {
			edited = true;
		}

		rotation = rotation.FromEulerXYZ(math::DegToRad(eulerRotation.x), math::DegToRad(eulerRotation.y), math::DegToRad(eulerRotation.z));

		ImGui::Text("Select one to edit the GO");
		if (ImGui::Button("Trans")) {
			App->renderer->imGuizmoOp = ImGuizmo::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate")) {
			App->renderer->imGuizmoOp = ImGuizmo::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Scale")) {
			App->renderer->imGuizmoOp = ImGuizmo::SCALE;
			App->renderer->imGuizmoMode = ImGuizmo::LOCAL;
		}
		
		if (App->renderer->imGuizmoOp != ImGuizmo::SCALE) {
			ImGui::RadioButton("Local", &App->renderer->imGuizmoMode, ImGuizmo::LOCAL); ImGui::SameLine();
			ImGui::RadioButton("Global", &App->renderer->imGuizmoMode, ImGuizmo::WORLD);
		}

		if (edited) {
			goContainer->ComputeBBox();
			edited = false;
		}

		if (staticGo) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::Separator();
	}
}

/* RapidJson storage */
void ComponentTransform::Save(Config* config) {
	config->StartObject();

	config->AddComponentType("componentType", componentType);
	config->AddString("parentUuid", parentUuid);
	config->AddString("uuid", uuid);
	config->AddFloat3("position", position);
	config->AddQuat("rotation", rotation);
	config->AddFloat3("eulerRotation", eulerRotation);
	config->AddFloat3("scale", scale);

	config->EndObject();
}

void ComponentTransform::Load(Config* config, rapidjson::Value& value) {
	sprintf_s(uuid, config->GetString("uuid", value));
	sprintf_s(parentUuid, config->GetString("parentUuid", value));
	position = config->GetFloat3("position", value);
	eulerRotation = config->GetFloat3("eulerRotation", value);
	scale = config->GetFloat3("scale", value);
	rotation = config->GetQuat("rotation", value);
}
