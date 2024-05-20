//------------------------------------------------------------------------------
//  @file gamepad.cc
//  @copyright (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "gamepad.h"
namespace Input
{
    void Gamepad::Update() {
        const float* axisTemp = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);

        for (int i = 0; i < axisCount; i++) {
            axis[i] = axisTemp[i];
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
