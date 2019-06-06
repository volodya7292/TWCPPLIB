#pragma once
#include "TW3DTypes.h"

class TW3DTransform {
public:
	// Rotation in degrees
	TW3DTransform(TWT::vec3d Position = TWT::vec3d(0), TWT::vec3d Rotation = TWT::vec3d(0), TWT::vec3d Scale = TWT::vec3d(1), bool ChangedInit = true);
	~TW3DTransform() = default;

	TWT::mat4d GetModelMatrix() const;
	TWT::mat4 GetModelMatrixReduced(TWT::vec3d const& RelativePoint, double ScaleFactor) const;
	TWT::vec3d GetPosition() const;
	TWT::vec3d GetRotation() const;
	TWT::vec3d GetScale() const;
	void SetIdentity();
	void SetPosition(TWT::vec3d const& Position);
	// Rotation in degrees
	void SetRotation(TWT::vec3d const& Rotation);
	void SetScale(TWT::vec3d const& Scale);
	// Rotation in degrees
	void AdjustRotation(TWT::vec3d const& Rotation);

	rp3d::Transform GetPhysicalTransform();
	void InitFromPhysicalTransform(rp3d::Transform const& Transform);

	bool Updated = false;

private:
	TWT::vec3d position = TWT::vec3d(0);
	TWT::vec3d rotation = TWT::vec3d(0);
	TWT::vec3d scale = TWT::vec3d(1);
};