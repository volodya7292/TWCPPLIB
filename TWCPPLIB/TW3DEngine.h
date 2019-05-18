#pragma once
#include "TW3DUtils.h"
#include "TW3DRenderer.h"

class TW3D {
public:
	struct InitializeInfo {
		// Window main panel width
		TWT::UInt WindowWidth = 1280;
		// Window main panel height
		TWT::UInt WindowHeight = 720;
		// Window title
		TWT::String	WindowTitle = "TW3DEngine";
		// Additional threads for application, not for engine
		TWT::UInt AdditionalThreadCount = 0;
		// Log file
		TWT::String LogFilename = "";
	};

	enum KeyActionType {
		KEY_ACTION_DOWN,
		KEY_ACTION_UP,
	};

	using DefaultHandler       = void(*)();
	using ThreadTickHandler    = TWT::UInt(*)(TWT::UInt ThreadID, TWT::UInt ThreadCount);
	using KeyHandler           = void(*)(TWT::UInt KeyCode, KeyActionType Type);
	using CharHandler          = void(*)(TWT::WChar Symbol);

	static void Initialize(const InitializeInfo& info);
	static void Start();
	static void Shutdown();
	static TWT::Bool GetFullScreen();
	static void SetFullScreen(TWT::Bool Fullscreen);
	static TWT::Bool GetVSync();
	static void SetVSync(TWT::Bool VSync);
	static void SetWindowTitle(const TWT::String&	WindowTitle);
	static TWT::Bool IsKeyDown(TWT::UInt KeyCode);
	static TWT::Vector2u GetCursorPosition();
	// Get window global center position without window adjusted bounds
	static TWT::Vector2u GetWindowCenterPosition();
	// Get window global top-left position without window adjusted bounds
	static TWT::Vector2u GetWindowPosition();
	// Get window main panel size
	static TWT::Vector2u GetCurrentWindowSize();

	static TWT::Float GetFPS();
	static TWT::Float GetDeltaTime();

	static void SetRenderer(TW3DRenderer* Renderer);

	static void SetOnUpdateEvent(DefaultHandler OnUpdate);
	static void SetOnCleanupEvent(DefaultHandler OnCleanup);
	static void SetOnThreadTickEvent(ThreadTickHandler OnThreadTick);
	static void SetOnKeyEvent(KeyHandler OnKey);
	static void SetOnCharEvent(CharHandler OnChar);

	static TW3DResourceManager* GetResourceManager();
};