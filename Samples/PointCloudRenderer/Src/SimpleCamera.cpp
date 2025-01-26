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
    m_yaw(0.0f),
    m_pitch(0.0f),
    m_lookDirection(0.0f, 0.0f, 1.0f),
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

void SimpleCamera::setMoveSpeed(const float unitsPerSecond)
{
    m_moveSpeed = unitsPerSecond;
}

void SimpleCamera::setTurnSpeed(const float radiansPerSecond)
{
    m_turnSpeed = radiansPerSecond;
}

void SimpleCamera::reset()
{
    m_position = m_initialPosition;
    m_yaw = 0.0f;
    m_pitch = 0.0f;
    m_lookDirection = { 0.0f, 0.0f, 1.0f };
    m_upDirection = { 0.0f, 1.0f, 0.0f };
    // Optionally reset keys pressed
    m_keysPressed = KeysPressed{};
}

void SimpleCamera::update(const float elapsedSeconds)
{
    // Calculate the move vector in camera space
    XMFLOAT3 move(0.0f, 0.0f, 0.0f);

    if (m_keysPressed.m_a)
        move.x -= 1.0f;
    if (m_keysPressed.m_d)
        move.x += 1.0f;
    if (m_keysPressed.m_w)
        move.z += 1.0f; // Positive Z moves forward in left-handed
    if (m_keysPressed.m_s)
        move.z -= 1.0f; // Negative Z moves backward in left-handed

    // Normalize move vector if necessary
    if (fabsf(move.x) > 0.1f || fabsf(move.z) > 0.1f) {
        const XMVECTOR vector = XMVector3Normalize(XMLoadFloat3(&move));
        XMStoreFloat3(&move, vector);
    }

    const float moveInterval = m_moveSpeed * elapsedSeconds;
    const float rotateInterval = m_turnSpeed * elapsedSeconds;

    // Update yaw and pitch
    if (m_keysPressed.m_left)
        m_yaw -= rotateInterval;
    if (m_keysPressed.m_right)
        m_yaw += rotateInterval;
    if (m_keysPressed.m_up)
        m_pitch += rotateInterval;
    if (m_keysPressed.m_down)
        m_pitch -= rotateInterval;

    // Clamp pitch to avoid flipping
    m_pitch = XMMin(m_pitch, XM_PIDIV4);
    m_pitch = XMMax(m_pitch, -XM_PIDIV4);

    // Wrap yaw for continuity
    if (m_yaw > XM_2PI)
        m_yaw -= XM_2PI;
    else if (m_yaw < -XM_2PI)
        m_yaw += XM_2PI;

    // Move the camera in model space
    float x = move.x * cosf(m_yaw) + move.z * sinf(m_yaw);
    float z = -move.x * sinf(m_yaw) + move.z * cosf(m_yaw);
    m_position.x += x * moveInterval;
    m_position.z += z * moveInterval;

    // Determine the look direction
    float r = cosf(m_pitch);
    m_lookDirection.x = r * sinf(m_yaw);
    m_lookDirection.y = sinf(m_pitch);
    m_lookDirection.z = r * cosf(m_yaw);
}

XMMATRIX SimpleCamera::getViewMatrix() const
{
    return XMMatrixLookToLH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_lookDirection), XMLoadFloat3(&m_upDirection));
}

XMMATRIX SimpleCamera::getProjectionMatrix(const float fov, const float aspectRatio, const float nearPlane, const float farPlane)
{
    return XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}

void SimpleCamera::onKeyDown(const Os::Key key)
{
    switch (key)
    {
        case Os::Key::ClvW:
            m_keysPressed.m_w = true;
        break;
        case Os::Key::ClvA:
            m_keysPressed.m_a = true;
        break;
        case Os::Key::ClvS:
            m_keysPressed.m_s = true;
        break;
        case Os::Key::ClvD:
            m_keysPressed.m_d = true;
        break;
        case Os::Key::ArwLeft:
            m_keysPressed.m_left = true;
        break;
        case Os::Key::ArwRight:
            m_keysPressed.m_right = true;
        break;
        case Os::Key::ArwUp:
            m_keysPressed.m_up = true;
        break;
        case Os::Key::ArwDown:
            m_keysPressed.m_down = true;
        break;
        case Os::Key::ClvEsc:
            reset();
        break;
        default:
            break;
    }
}

void SimpleCamera::onKeyUp(const Os::Key key)
{
    switch (key)
    {
        case Os::Key::ClvW:
            m_keysPressed.m_w = false;
        break;
        case Os::Key::ClvA:
            m_keysPressed.m_a = false;
        break;
        case Os::Key::ClvS:
            m_keysPressed.m_s = false;
        break;
        case Os::Key::ClvD:
            m_keysPressed.m_d = false;
        break;
        case Os::Key::ArwLeft:
            m_keysPressed.m_left = false;
        break;
        case Os::Key::ArwRight:
            m_keysPressed.m_right = false;
        break;
        case Os::Key::ArwUp:
            m_keysPressed.m_up = false;
        break;
        case Os::Key::ArwDown:
            m_keysPressed.m_down = false;
        break;
        default:
            break;
    }
}

