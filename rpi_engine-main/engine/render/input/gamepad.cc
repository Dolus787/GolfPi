//------------------------------------------------------------------------------
//  @file gamepad.cc
//  @copyright (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "gamepad.h"
namespace Input
{
    Gamepad::Gamepad() {
        for (int i = 0; i !=__NUM_BUTTONS__; i++) {
            buttonstates.push_back(buttonState());
        }
    }
    void Gamepad::Update() {
        const float* axisTemp = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
        const unsigned char* buttonsTemp = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

        for (int i = 0; i < axisCount; i++) {
            axis[i] = axisTemp[i];
        }
        std::cout << "Buttons: " << buttonCount << "\n";

        for (int i = 0; i < buttonCount; i++) {
            buttons[i] = buttonsTemp[i];
            if (buttons[i] == GLFW_PRESS) {
                if (buttonstates[i].justPressed || buttonstates[i].held) {
                    buttonstates[i].held = true;
                    buttonstates[i].justPressed = false;
                }
                else {
                    buttonstates[i].justPressed = true;
                }
                buttonstates[i].justReleased = false, buttonstates[i].released = false;
            }
            else if (buttons[i] == GLFW_RELEASE) {
                if (buttonstates[i].justReleased || buttonstates[i].released) {
                    buttonstates[i].released = true;
                    buttonstates[i].justReleased = false;
                }
                else {
                    buttonstates[i].justReleased = true;
                }
                buttonstates[i].justPressed = false, buttonstates[i].held = false;
            }
        }
    }
    
    float Gamepad::GetLeftJoystickX() {
        if (axisCount == 0){ 
            std::cout << "NO CONTROLLER \n";
            return 0;
        }
        return axis[0];
    }
    float Gamepad::GetLeftJoystickY() {
        if (axisCount == 0) {
            std::cout << "NO CONTROLLER \n";
            return 0;
        }
        return axis[1];
    }
    float Gamepad::GetRightJoystickX() {
        if (axisCount == 0) {
            std::cout << "NO CONTROLLER \n";
            return 0;
        }
        return axis[2];
    }
    float Gamepad::GetRightJoystickY() {
        if (axisCount == 0) {
            std::cout << "NO CONTROLLER \n";
            return 0;
        }
        return axis[3];
    }
    float Gamepad::GetLeftTrigger() {
        if (axisCount == 0) {
            std::cout << "NO CONTROLLER \n";
            return 0;
        }
        return axis[4];
    }
    float Gamepad::GetRightTrigger() {
        if (axisCount == 0) {
            std::cout << "NO CONTROLLER \n";
            return 0;
        }
        return axis[5];
    }
} // namespace Input
