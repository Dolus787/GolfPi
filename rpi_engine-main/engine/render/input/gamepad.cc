//------------------------------------------------------------------------------
//  @file gamepad.cc
//  @copyright (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "gamepad.h"
namespace Input
{
    void Gamepad::Update() {

        for (int i = 0; i < buttonCount; i++) {
            buttons[i] = 0;
        }
        
        const float* axisTemp = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
        const unsigned char* buttonsTemp = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

        for (int i = 0; i < axisCount; i++) {
            axis[i] = axisTemp[i];
        }

        for (int i = 0; i < buttonCount; i++) {
            buttons[i] = buttonsTemp[i];
            if (buttons[i] == GLFW_PRESS) {

                //Add new buttons here and header
                if (i == 7) {
                    startPressed = true;
                    startReleased = false;
                }
                if (i == 6) {
                    backPressed = true;
                    backReleased = false;
                }
                if (i == 0) {
                    aPressed = true;
                    aReleased = false;
                }
                if (i == 3) {
                    yPressed = true;
                    yReleased = false;
                }
            }
            else if (buttons[i] == GLFW_RELEASE) {
                //Released only true for one frame
                if (i == 7) {
                    if (startPressed) {
                        startReleased = true;
                    }
                    else {
                        startReleased = false;
                    }
                    startPressed = false;
                }

                if (i == 6 && backPressed) {
                    if (backPressed) {
                        backReleased = true;
                    }
                    else {
                        backReleased = false;
                    }
                    backPressed = false;
                }

                if (i == 0 && aPressed) {
                    if (aPressed) {
                        aReleased = true;
                    }
                    else {
                        aReleased = false;
                    }
                    aPressed = false;
                }
                if (i == 3 && yPressed) {
                    if (yPressed) {
                        yReleased = true;
                    }
                    else {
                        yReleased = false;
                    }
                    yPressed = false;
                }
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
