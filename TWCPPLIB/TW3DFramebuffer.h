#pragma once
#include "TW3DTexture.h"

class TW3DResourceManager;

class TW3DFramebuffer {
public:
	TW3DFramebuffer(TW3DResourceManager* ResourceManager, TWT::uint2 Size);
	~TW3DFramebuffer();

	void AddRenderTarget(TWT::uint Index, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	void AddRenderTarget(TWT::uint Index, TW3DRenderTarget* RenderTarget);
	void AddDepthStencil();
	void AddDepthStencil(TW3DTexture* DepthStencil);
	void Resize(TWT::uint2 Size);

	TWT::uint2 GetSize();

	TW3DRenderTarget* GetRenderTarget(TWT::uint Index) const;
	const std::vector<TW3DRenderTarget*> GetRenderTargets() const;
	TW3DTexture* GetDepthStencilTexture() const;

private:
	struct TW3DFBRT {
		TW3DRenderTarget* RenderTarget = nullptr;
		bool DestroyOnFBDestroy = true;
	};

	TW3DResourceManager* resource_manager;
	TWT::uint2 size;

	std::unordered_map<TWT::uint, TW3DFBRT> rts; // uint : Index
	std::pair<TW3DTexture*, bool> depth_stencil; // bool : DestroyOnFBDestroy
};

