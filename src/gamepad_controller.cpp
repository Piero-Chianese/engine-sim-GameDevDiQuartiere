#include "../include/gamepad_controller.h"

#include <cmath>
#include <algorithm>

#pragma comment(lib, "Xinput.lib")

GamepadController::GamepadController() {
    ZeroMemory(&m_state, sizeof(XINPUT_STATE));
}

GamepadController::~GamepadController() {
    if (m_connected) {
        setVibration(0.0f, 0.0f);
    }
}

void GamepadController::update(float dt) {
    m_justConnected = false;
    m_justDisconnected = false;

    // Check connection if currently not connected
    if (!m_connected) {
        m_reconnectTimer += dt;
        if (m_reconnectTimer < 0.5f) {
            return;
        }
        m_reconnectTimer = 0.0f;

        for (DWORD i = 0; i < 4; ++i) {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            if (XInputGetState(i, &state) == ERROR_SUCCESS) {
                m_userIndex = static_cast<int>(i);
                m_connected = true;
                m_justConnected = true;
                break;
            }
        }

        if (!m_connected) {
            return;
        }
    }

    m_previousButtons = m_currentButtons;

    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));
    DWORD result = XInputGetState(static_cast<DWORD>(m_userIndex), &state);

    if (result != ERROR_SUCCESS) {
        m_connected = false;
        m_justDisconnected = true;
        m_userIndex = -1;
        m_currentButtons = 0;
        m_previousButtons = 0;
        m_leftTrigger = 0.0f;
        m_rightTrigger = 0.0f;
        m_leftStickX = 0.0f;
        m_leftStickY = 0.0f;
        m_rightStickX = 0.0f;
        m_rightStickY = 0.0f;
        return;
    }

    m_state = state;
    m_currentButtons = state.Gamepad.wButtons;

    // Analog Left Trigger (deadzone filtering)
    if (state.Gamepad.bLeftTrigger > 30) {
        m_leftTrigger = (state.Gamepad.bLeftTrigger - 30.0f) / (255.0f - 30.0f);
    } else {
        m_leftTrigger = 0.0f;
    }

    // Analog Right Trigger (deadzone filtering)
    if (state.Gamepad.bRightTrigger > 30) {
        m_rightTrigger = (state.Gamepad.bRightTrigger - 30.0f) / (255.0f - 30.0f);
    } else {
        m_rightTrigger = 0.0f;
    }

    // Analog Left Thumbstick (circular deadzone filtering)
    float lx = state.Gamepad.sThumbLX / 32767.0f;
    float ly = state.Gamepad.sThumbLY / 32767.0f;
    float magL = std::sqrt(lx * lx + ly * ly);
    if (magL > LEFT_STICK_DEADZONE) {
        float factor = (magL - LEFT_STICK_DEADZONE) / (1.0f - LEFT_STICK_DEADZONE);
        factor = (std::min)(1.0f, factor) / magL;
        m_leftStickX = lx * factor;
        m_leftStickY = ly * factor;
    } else {
        m_leftStickX = 0.0f;
        m_leftStickY = 0.0f;
    }

    // Analog Right Thumbstick (circular deadzone filtering)
    float rx = state.Gamepad.sThumbRX / 32767.0f;
    float ry = state.Gamepad.sThumbRY / 32767.0f;
    float magR = std::sqrt(rx * rx + ry * ry);
    if (magR > RIGHT_STICK_DEADZONE) {
        float factor = (magR - RIGHT_STICK_DEADZONE) / (1.0f - RIGHT_STICK_DEADZONE);
        factor = (std::min)(1.0f, factor) / magR;
        m_rightStickX = rx * factor;
        m_rightStickY = ry * factor;
    } else {
        m_rightStickX = 0.0f;
        m_rightStickY = 0.0f;
    }
}

bool GamepadController::isButtonDown(WORD button) const {
    return m_connected && ((m_currentButtons & button) != 0);
}

bool GamepadController::isButtonPressed(WORD button) const {
    return m_connected && ((m_currentButtons & button) != 0) && ((m_previousButtons & button) == 0);
}

bool GamepadController::isButtonReleased(WORD button) const {
    return m_connected && ((m_currentButtons & button) == 0) && ((m_previousButtons & button) != 0);
}

void GamepadController::setVibration(float leftMotor, float rightMotor) {
    if (!m_connected || m_userIndex < 0) return;

    leftMotor = (std::max)(0.0f, (std::min)(1.0f, leftMotor));
    rightMotor = (std::max)(0.0f, (std::min)(1.0f, rightMotor));

    XINPUT_VIBRATION vib;
    vib.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f);
    vib.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);
    XInputSetState(static_cast<DWORD>(m_userIndex), &vib);
}
