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

#include "SimpleCamera.h"

SimpleCamera::SimpleCamera() :
    m_initialPosition(0.0f, 0.0f, 0.0f),
    m_position(m_initialPosition),
    m_yaw(XM_PI),
    m_pitch(0.0f),
    m_lookDirection(0.0f, 0.0f, -1.0f),
    m_upDirection(0.0f, 1.0f, 0.0f),
    m_moveSpeed(20.0f),
    m_turnSpeed(XM_PIDIV2),
    m_keysPressed{}
{
}

void SimpleCamera::init(XMFLOAT3 position)
{
    m_initialPosition = position;
    reset();
}

void SimpleCamera::setMoveSpeed(float unitsPerSecond)
{
    m_moveSpeed = unitsPerSecond;
}

void SimpleCamera::setTurnSpeed(float radiansPerSecond)
{
    m_turnSpeed = radiansPerSecond;
}

void SimpleCamera::reset()
{
    m_position = m_initialPosition;
    m_yaw = XM_PI;
    m_pitch = 0.0f;
    m_lookDirection = { 0.0f, 0.0f, -1.0f };
    m_upDirection = { 0.0f, 1.0f, 0.0f };
    // Optionally reset keys pressed
    m_keysPressed = KeysPressed{};
}

void SimpleCamera::update(float elapsedSeconds)
{
    // Calculate the move vector in camera space.
    XMFLOAT3 move(0.0f, 0.0f, 0.0f);

    if (m_keysPressed.m_a)
        move.x -= 1.0f;
    if (m_keysPressed.m_d)
        move.x += 1.0f;
    if (m_keysPressed.m_w)
        move.z -= 1.0f;
    if (m_keysPressed.m_s)
        move.z += 1.0f;

    if (fabsf(move.x) > 0.1f && fabsf(move.z) > 0.1f)
    {
        XMVECTOR vector = XMVector3Normalize(XMLoadFloat3(&move));
        move.x = XMVectorGetX(vector);
        move.z = XMVectorGetZ(vector);
    }

    float moveInterval = m_moveSpeed * elapsedSeconds;
    float rotateInterval = m_turnSpeed * elapsedSeconds;

    if (m_keysPressed.m_left)
        m_yaw += rotateInterval;
    if (m_keysPressed.m_right)
        m_yaw -= rotateInterval;
    if (m_keysPressed.m_up)
        m_pitch += rotateInterval;
    if (m_keysPressed.m_down)
        m_pitch -= rotateInterval;

    m_pitch = XMMin(m_pitch, XM_PIDIV4);
    m_pitch = XMMax(m_pitch, -XM_PIDIV4);

    // Move the camera in model space.
    float x = move.x * -cosf(m_yaw) - move.z * sinf(m_yaw);
    float z = move.x * sinf(m_yaw) - move.z * cosf(m_yaw);
    m_position.x += x * moveInterval;
    m_position.z += z * moveInterval;

    // Determine the look direction.
    float r = cosf(m_pitch);
    m_lookDirection.x = r * sinf(m_yaw);
    m_lookDirection.y = sinf(m_pitch);
    m_lookDirection.z = r * cosf(m_yaw);
}

XMMATRIX SimpleCamera::getViewMatrix()
{
    return XMMatrixLookToRH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_lookDirection), XMLoadFloat3(&m_upDirection));
}

XMMATRIX SimpleCamera::getProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    return XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane);
}

void SimpleCamera::onKeyDown(WPARAM key)
{
    switch (key)
    {
    case 'W':
        m_keysPressed.m_w = true;
        break;
    case 'A':
        m_keysPressed.m_a = true;
        break;
    case 'S':
        m_keysPressed.m_s = true;
        break;
    case 'D':
        m_keysPressed.m_d = true;
        break;
    case VK_LEFT:
        m_keysPressed.m_left = true;
        break;
    case VK_RIGHT:
        m_keysPressed.m_right = true;
        break;
    case VK_UP:
        m_keysPressed.m_up = true;
        break;
    case VK_DOWN:
        m_keysPressed.m_down = true;
        break;
    case VK_ESCAPE:
        reset();
        break;
    }
}

void SimpleCamera::onKeyUp(WPARAM key)
{
    switch (key)
    {
    case 'W':
        m_keysPressed.m_w = false;
        break;
    case 'A':
        m_keysPressed.m_a = false;
        break;
    case 'S':
        m_keysPressed.m_s = false;
        break;
    case 'D':
        m_keysPressed.m_d = false;
        break;
    case VK_LEFT:
        m_keysPressed.m_left = false;
        break;
    case VK_RIGHT:
        m_keysPressed.m_right = false;
        break;
    case VK_UP:
        m_keysPressed.m_up = false;
        break;
    case VK_DOWN:
        m_keysPressed.m_down = false;
        break;
    }
}
