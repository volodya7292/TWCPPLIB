#pragma once
#include "TW3DTypes.h"

class TW3DTransform {
public:
	// Rotation in degrees
	TW3DTransform(TWT::double3 Position = TWT::double3(0), TWT::double3 Rotation = TWT::double3(0), TWT::double3 Scale = TWT::double3(1), bool ChangedInit = true);
	~TW3DTransform() = default;

	TWT::double4x4 GetModelMatrix() const;
	TWT::float4x4 GetModelMatrixReduced(TWT::double3 const& RelativePoint, double ScaleFactor) const;
	TWT::double3 GetPosition() const;
	TWT::double3 GetRotation() const;
	TWT::double3 GetScale() const;
	void SetIdentity();
	void SetPosition(TWT::double3 const& Position);
	// Rotation in degrees
	void SetRotation(TWT::double3 const& Rotation);
	void SetScale(TWT::double3 const& Scale);
	// Rotation in degrees
	void AdjustRotation(TWT::double3 const& Rotation);

	rp3d::Transform GetPhysicalTransform();
	void InitFromPhysicalTransform(rp3d::Transform const& Transform);

	bool Updated = false;

private:
	TWT::double3 position = TWT::double3(0);
	TWT::double3 rotation = TWT::double3(0);
	TWT::double3 scale = TWT::double3(1);
};