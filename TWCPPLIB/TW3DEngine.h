#pragma once
#include "TW3DUtils.h"
#include "TW3DRenderer.h"

namespace TW3D {
	struct InitializeInfo {
		// Window main panel width
		TWT::UInt WindowWidth = 1280;
		// Window main panel height
		TWT::UInt WindowHeight = 720;
		// Window title
		TWT::String	Title = "TW3DEngine";
		// Additional threads for application, not for engine
		TWT::UInt AdditionalThreadCount = 0;
	};

	using DefaultHandler       = void(*)();
	using ThreadTickHandler    = TWT::UInt(*)(TWT::UInt ThreadID, TWT::UInt ThreadCount);
	using KeyHandler           = void(*)(TWT::UInt KeyCode);
	using CharHandler          = void(*)(TWT::WChar Symbol);

	void Initialize(const InitializeInfo& info);
	void Start();
	void Shutdown();
	TWT::Bool GetFullScreen();
	void SetFullScreen(TWT::Bool Fullscreen);
	TWT::Bool GetVSync();
	void SetVSync(TWT::Bool VSync);
	TWT::Bool IsKeyDown(TWT::UInt KeyCode);
	TWT::Vector2u GetCursorPosition();
	// Get window global center position without window adjusted bounds
	TWT::Vector2u GetWindowCenterPosition();
	// Get window global top-left position without window adjusted bounds
	TWT::Vector2u GetWindowPosition();
	// Get window main panel size
	TWT::Vector2u GetCurrentWindowSize();

	void SetRenderer(TW3DRenderer* Renderer);

	void SetOnUpdateEvent(DefaultHandler OnUpdate);
	void SetOnCleanupEvent(DefaultHandler OnCleanup);
	void SetOnThreadTickEvent(ThreadTickHandler OnThreadTick);
	void SetOnKeyDownEvent(KeyHandler OnKeyDown);
	void SetOnKeyUpEvent(KeyHandler OnKeyUp);
	void SetOnCharEvent(CharHandler OnChar);

	TW3DResourceManager* GetResourceManager();
}