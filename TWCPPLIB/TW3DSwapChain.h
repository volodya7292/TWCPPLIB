#pragma once
#include "TW3DFactory.h"
#include "TW3DCommandQueue.h"

class TW3DSwapChain { //-V802
public:
	TW3DSwapChain(TW3DFactory* factory, TW3DCommandQueue* commandQueue, HWND hwnd, TWT::uint width, TWT::uint height, bool vsync);
	~TW3DSwapChain();

	DXGI_SWAP_CHAIN_DESC1 GetDescription();
	bool GetFullscreen();
	void SetFullscreen(bool fullscreen);

	TWT::uint GetCurrentBufferIndex();
	ID3D12Resource* GetBuffer(TWT::uint index);

	void Resize(TWT::uint width, TWT::uint height);
	void Present();

	bool VSync;

	static const TWT::uint BufferCount = 3;

private:
	IDXGISwapChain4* swapChain;
	DXGI_SWAP_CHAIN_DESC1 desc;

	const bool tearing;
};