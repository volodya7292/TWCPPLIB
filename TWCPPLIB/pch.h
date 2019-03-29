#ifndef PCH_H
#define PCH_H

#define NOMINMAX

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <chrono>
#include <vector>
#include <string>
#include <array>

#include <stdint.h>
#include <dxgi1_6.h>
#include <wincodec.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <d3dx12.h>
#include <d3d12raytracingfallback.h>
#include <D3D12RaytracingHelpers.hpp>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#endif //PCH_H
