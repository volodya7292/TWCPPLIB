#pragma once
#include "TW3DTypes.h"

class TW3DTransform {
public:
	TW3DTransform(TWT::vec3 Position = TWT::vec3(0), TWT::vec3 Rotation = TWT::vec3(0), TWT::vec3 Scale = TWT::vec3(1), bool ChangedInit = true);
	~TW3DTransform() = default;

	TWT::mat4 GetModelMatrix();
	TWT::vec3 GetPosition();
	TWT::vec3 GetRotation();
	TWT::vec3 GetScale();
	void SetPosition(const TWT::vec3& Position);
	void SetRotation(const TWT::vec3& Rotation);
	void SetScale(const TWT::vec3& Scale);
	void AdjustRotation(const TWT::vec3& Rotation);

	bool Changed = false;

private:
	TWT::vec3 position = TWT::vec3(0);
	TWT::vec3 rotation = TWT::vec3(0);
	TWT::vec3 scale = TWT::vec3(1);
};