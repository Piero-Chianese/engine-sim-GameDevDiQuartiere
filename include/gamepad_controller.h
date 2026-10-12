#ifndef ATG_ENGINE_SIM_GAMEPAD_CONTROLLER_H
#define ATG_ENGINE_SIM_GAMEPAD_CONTROLLER_H

#include <Windows.h>
#include <Xinput.h>

class GamepadController {
public:
    GamepadController();
    ~GamepadController();

    void update(float dt);

    bool isConnected() const { return m_connected; }
    bool wasConnectedThisFrame() const { return m_justConnected; }
    bool wasDisconnectedThisFrame() const { return m_justDisconnected; }

    // Analog triggers (normalized 0.0f to 1.0f)
    float getRightTrigger() const { return m_rightTrigger; }
    float getLeftTrigger() const { return m_leftTrigger; }

    // Analog thumbsticks (normalized -1.0f to 1.0f, circular deadzone applied)
    float getLeftStickX() const { return m_leftStickX; }
    float getLeftStickY() const { return m_leftStickY; }
    float getRightStickX() const { return m_rightStickX; }
    float getRightStickY() const { return m_rightStickY; }

    // Digital button checks
    bool isButtonDown(WORD button) const;
    bool isButtonPressed(WORD button) const;
    bool isButtonReleased(WORD button) const;

    // Vibration / Force feedback (normalized 0.0f to 1.0f)
    void setVibration(float leftMotor, float rightMotor);

private:
    int m_userIndex = -1;
    bool m_connected = false;
    bool m_justConnected = false;
    bool m_justDisconnected = false;

    XINPUT_STATE m_state{};
    WORD m_previousButtons = 0;
    WORD m_currentButtons = 0;

    float m_leftTrigger = 0.0f;
    float m_rightTrigger = 0.0f;
    float m_leftStickX = 0.0f;
    float m_leftStickY = 0.0f;
    float m_rightStickX = 0.0f;
    float m_rightStickY = 0.0f;

    float m_reconnectTimer = 0.0f;

    static constexpr float TRIGGER_THRESHOLD = 30.0f / 255.0f;
    static constexpr float LEFT_STICK_DEADZONE = (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 32767.0f;
    static constexpr float RIGHT_STICK_DEADZONE = (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 32767.0f;
};

#endif /* ATG_ENGINE_SIM_GAMEPAD_CONTROLLER_H */
