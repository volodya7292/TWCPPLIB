#pragma once
#include "TW3DFactory.h"
#include "TW3DCommandQueue.h"

class TW3DSwapChain {
public:
	TW3DSwapChain(TW3DFactory* Factory, TW3DCommandQueue* CommandQueue, HWND HWND, TWT::uint Width, TWT::uint Height, bool VSync);
	~TW3DSwapChain();

	DXGI_SWAP_CHAIN_DESC1 GetDescription();
	bool GetFullScreen();
	void SetFullScreen(bool Fullscreen);

	TWT::uint GetCurrentBufferIndex();
	ID3D12Resource* GetBuffer(TWT::uint Index);

	void Resize(TWT::uint Width, TWT::uint Height);
	void Present();

	bool VSync;

	static const TWT::uint BufferCount = 3;

private:
	IDXGISwapChain4* swap_chain;
	DXGI_SWAP_CHAIN_DESC1 desc;

	const bool tearing;
};