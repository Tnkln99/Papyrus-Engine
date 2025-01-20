//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "DirectXMath.h"
#include <windows.h>

using namespace DirectX;

class SimpleCamera
{
public:
    SimpleCamera();

    void init(XMFLOAT3 position);
    void update(float elapsedSeconds);
    XMMATRIX getViewMatrix();
    XMMATRIX getProjectionMatrix(float fov, float aspectRatio, float nearPlane = 1.0f, float farPlane = 1000.0f);
    void setMoveSpeed(float unitsPerSecond);
    void setTurnSpeed(float radiansPerSecond);

    void onKeyDown(WPARAM key);
    void onKeyUp(WPARAM key);

private:
    void reset();

    struct KeysPressed
    {
        bool m_w;
        bool m_a;
        bool m_s;
        bool m_d;

        bool m_left;
        bool m_right;
        bool m_up;
        bool m_down;
    };

    XMFLOAT3 m_initialPosition;
    XMFLOAT3 m_position;
    float m_yaw;                // Relative to the +z axis.
    float m_pitch;              // Relative to the xz plane.
    XMFLOAT3 m_lookDirection;
    XMFLOAT3 m_upDirection;
    float m_moveSpeed;          // Speed at which the camera moves, in units per second.
    float m_turnSpeed;          // Speed at which the camera turns, in radians per second.

    KeysPressed m_keysPressed;
};
