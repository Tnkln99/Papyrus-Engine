#pragma once

#define NOMINMAX             // This prevents <Windows.h> from defining min/max macros
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

#include <wrl.h>
