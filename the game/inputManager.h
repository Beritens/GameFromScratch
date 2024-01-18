#pragma once
#include "utility.h"

class InputManager {
public:
    float2 GetMovement() {
        return m_movement;
    }
    void SetMovement(float2 m) {
        m_movement = m;
    }
    float2 GetShootDir() {
        return m_shootDir;
    }
    void SetShootDir(float2 m) {
        m_shootDir = m;
    }
private:
    float2 m_movement;
    float2 m_shootDir;
};