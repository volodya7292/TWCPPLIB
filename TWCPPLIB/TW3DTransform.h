#pragma once
#include "TW3DTypes.h"

class TW3DTransform {
public:
	TW3DTransform(TWT::vec3d Position = TWT::vec3d(0), TWT::vec3d Rotation = TWT::vec3d(0), TWT::vec3d Scale = TWT::vec3d(1), bool ChangedInit = true);
	~TW3DTransform() = default;

	TWT::mat4d GetModelMatrix();
	TWT::mat4 GetModelMatrixReduced(TWT::vec3d const& RelativePoint, double ScaleFactor);
	TWT::vec3d GetPosition();
	TWT::vec3d GetRotation();
	TWT::vec3d GetScale();
	void SetPosition(TWT::vec3d const& Position);
	void SetRotation(TWT::vec3d const& Rotation);
	void SetScale(TWT::vec3d const& Scale);
	void AdjustRotation(TWT::vec3d const& Rotation);

	bool Changed = false;

private:
	TWT::vec3d position = TWT::vec3d(0);
	TWT::vec3d rotation = TWT::vec3d(0);
	TWT::vec3d scale = TWT::vec3d(1);
};