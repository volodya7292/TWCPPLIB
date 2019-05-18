#pragma once
#include "TW3DUtils.h"
#include "TW3DRenderer.h"

class TW3D {
public:
	struct InitializeInfo {
		// Window main panel width
		TWT::uint WindowWidth = 1280;
		// Window main panel height
		TWT::uint WindowHeight = 720;
		// Window title
		TWT::String	WindowTitle = "TW3DEngine";
		// Additional threads for application, not for engine
		TWT::uint AdditionalThreadCount = 0;
		// Log file
		TWT::String LogFilename = "";
	};

	enum KeyActionType {
		KEY_ACTION_DOWN,
		KEY_ACTION_UP,
	};

	using DefaultHandler       = void(*)();
	using ThreadTickHandler    = TWT::uint(*)(TWT::uint ThreadID, TWT::uint ThreadCount);
	using KeyHandler           = void(*)(TWT::uint KeyCode, KeyActionType Type);
	using CharHandler          = void(*)(TWT::wchar Symbol);

	static void Initialize(const InitializeInfo& info);
	static void Start();
	static void Shutdown();
	static bool GetFullScreen();
	static void SetFullScreen(bool Fullscreen);
	static bool GetVSync();
	static void SetVSync(bool VSync);
	static void SetWindowTitle(const TWT::String&	WindowTitle);
	static bool IsKeyDown(TWT::uint KeyCode);
	static TWT::vec2u GetCursorPosition();
	// Get window global center position without window adjusted bounds
	static TWT::vec2u GetWindowCenterPosition();
	// Get window global top-left position without window adjusted bounds
	static TWT::vec2u GetWindowPosition();
	// Get window main panel size
	static TWT::vec2u GetCurrentWindowSize();

	static float GetFPS();
	static float GetDeltaTime();

	static void SetRenderer(TW3DRenderer* Renderer);

	static void SetOnUpdateEvent(DefaultHandler OnUpdate);
	static void SetOnCleanupEvent(DefaultHandler OnCleanup);
	static void SetOnThreadTickEvent(ThreadTickHandler OnThreadTick);
	static void SetOnKeyEvent(KeyHandler OnKey);
	static void SetOnCharEvent(CharHandler OnChar);

	static TW3DResourceManager* GetResourceManager();
};