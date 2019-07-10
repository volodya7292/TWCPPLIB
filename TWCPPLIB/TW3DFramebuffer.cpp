#include "pch.h"
#include "TW3DFramebuffer.h"
#include "TW3DResourceManager.h"

TW3DFramebuffer::TW3DFramebuffer(TWT::String Name, TW3DResourceManager* ResourceManager, TWT::uint2 Size, TW3DFramebufferType Type) :
	name(Name), resource_manager(ResourceManager), type(Type), size(Size) {
}

TW3DFramebuffer::~TW3DFramebuffer() {
	for (auto const& [name, rt] : rts)
		if (rt.DestroyOnFBDestroy)
			delete rt.RenderTarget;

	if (depth_stencil.first && depth_stencil.second)
		delete depth_stencil.first;
}

void TW3DFramebuffer::AddRenderTarget(TWT::uint Index, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	if (rts.size() == 8) {
		TWU::TW3DLogError("[TW3DFramebuffer::AddRenderTarget] Maximum render target count limit(8) exceeded!"s);
		return;
	}

	TW3DFBRT rt;
	if (type == TW3D_FRAMEBUFFER_CUBE)
		rt.RenderTarget = resource_manager->CreateRenderTargetCube(name + "_RT"s + Index, size.x, Format, ClearValue);
	else
		rt.RenderTarget = resource_manager->CreateRenderTarget(name + "_RT"s + Index, size, Format, ClearValue);
	rt.DestroyOnFBDestroy = true;

	rts[Index] = rt;
}

void TW3DFramebuffer::AddRenderTarget(TWT::uint Index, TW3DRenderTarget* RenderTarget) {
	if (rts.size() == 8) {
		TWU::TW3DLogError("[TW3DFramebuffer::AddRenderTarget] Maximum render target count limit(8) exceeded!"s);
		return;
	}

	RenderTarget->Resize(size);

	TW3DFBRT rt;
	rt.RenderTarget = RenderTarget;
	rt.DestroyOnFBDestroy = false;

	rts[Index] = rt;
}

void TW3DFramebuffer::AddDepthStencil() {
	if (depth_stencil.first)
		TWU::TW3DLogError("DepthStencil texture already exists!"s);

	if (type == TW3D_FRAMEBUFFER_CUBE)
		depth_stencil.first = resource_manager->CreateDepthStencilCubeTexture(name + "_DS"s, size.x);
	else
		depth_stencil.first = resource_manager->CreateDepthStencilTexture(name + "_DS"s, size);
	depth_stencil.second = true;
}

void TW3DFramebuffer::AddDepthStencil(TW3DTexture* DepthStencil) {
	if (depth_stencil.first)
		TWU::TW3DLogError("DepthStencil texture already exists!"s);

	depth_stencil.first = DepthStencil;
	depth_stencil.second = false;
}

void TW3DFramebuffer::Resize(TWT::uint2 Size) {
	size = Size;

	for (auto const& [name, rt] : rts)
		rt.RenderTarget->Resize(Size);
}

TWT::uint2 TW3DFramebuffer::GetSize() {
	return size;
}

TW3DRenderTarget* TW3DFramebuffer::GetRenderTarget(TWT::uint Index) const {
	return rts.at(Index).RenderTarget;
}

const std::vector<TW3DRenderTarget*> TW3DFramebuffer::GetRenderTargets() const {
	std::vector<TW3DRenderTarget*> ordered_rts(rts.size());

	for (auto const& [index, rt] : rts)
		ordered_rts[index] = rt.RenderTarget;

	return ordered_rts;
}

TW3DTexture* TW3DFramebuffer::GetDepthStencilTexture() const {
	return depth_stencil.first;
}