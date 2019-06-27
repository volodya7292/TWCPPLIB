#ifndef PCH_H
#define PCH_H

#define NOMINMAX

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <cstdint>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <queue>
#include <mutex>
#include <array>
#include <map>

#include <comdef.h>
#include <dxgi1_6.h>
#include <wincodec.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <d3dx12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <reactphysics/reactphysics3d.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#endif //PCH_H
