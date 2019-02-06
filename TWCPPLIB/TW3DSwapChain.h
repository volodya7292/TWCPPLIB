#pragma once
#include "TW3DFactory.h"
#include "TW3DCommandQueue.h"

namespace TW3D {
	class TW3DSwapChain {
	public:
		TW3DSwapChain(TW3DFactory* factory, TW3DCommandQueue* commandQueue, HWND hwnd, TWT::UInt width, TWT::UInt height, TWT::Bool vsync);
		~TW3DSwapChain();;

		TWT::Bool GetFullscreen();
		void SetFullscreen(TWT::Bool fullscreen);

		TWT::UInt GetCurrentBufferIndex();
		ID3D12Resource* GetBuffer(TWT::UInt index);

		void Resize(TWT::UInt width, TWT::UInt height);
		void Present();

	private:
		IDXGISwapChain4* swapChain;
		DXGI_SWAP_CHAIN_DESC1 desc;

		TWT::Bool vsync;

		const TWT::Bool tearing;

		static const TWT::UInt bufferCount = 3;
	};
}