#pragma once

#include <GLFW/glfw3.h>
#include "vec2.hpp"
#include <iostream>
#include <vector>
namespace Input
{
    enum button {
        // Don't remove last, add ahead of it.
        a,
        b,
        x,
        y,
        leftShoulder,
        rightShoulder,
        back,
        start,
        leftStick,
        rightStick,
        up,
        right,
        down,
        left,

        __NUM_BUTTONS__
    };

    struct buttonState {
        buttonState(){
            justPressed = false, held = false, justReleased = false, released = true;
        }
        bool justPressed, held, justReleased, released;
    };

    struct Gamepad
    {
        Gamepad();
        

        std::vector<buttonState> buttonstates;

        float axis[6]{ {0} };
        unsigned char buttons[14]{ {0} };

        int axisCount = 0;
        int buttonCount = 0;
        void Update();
        
        buttonState GetButtonState(button button) {return buttonstates[button];}

        float GetLeftJoystickX();
        float GetLeftJoystickY();
        float GetRightJoystickX();
        float GetRightJoystickY();
        float GetRightTrigger();
        float GetLeftTrigger();
    };


}



//#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
////#include "x86_64-win32-msvc/include/libgamepad.hpp"
//#include <Xinput.h>
//namespace Input
//{
//
//struct Gamepad
//{
//    Gamepad() {
//        XINPUT_STATE state;
//        ZeroMemory(&state, sizeof(XINPUT_STATE));
//
//        // Simply get the state of the controller from XInput.
//        dwResult = XInputGetState(i, &state);
//        float LX = state.Gamepad.sThumbLX;
//        float LY = state.Gamepad.sThumbLY;
//
//        //determine how far the controller is pushed
//        float magnitude = sqrt(LX * LX + LY * LY);
//
//        //determine the direction the controller is pushed
//        float normalizedLX = LX / magnitude;
//        float normalizedLY = LY / magnitude;
//
//        float normalizedMagnitude = 0;
//
//        //check if the controller is outside a circular dead zone
//        if (magnitude > INPUT_DEADZONE)
//        {
//            //clip the magnitude at its expected maximum value
//            if (magnitude > 32767) magnitude = 32767;
//
//            //adjust magnitude relative to the end of the dead zone
//            magnitude -= INPUT_DEADZONE;
//
//            //optionally normalize the magnitude with respect to its expected range
//            //giving a magnitude value of 0.0 to 1.0
//            normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);
//        }
//        else //if the controller is in the deadzone zero out the magnitude
//        {
//            magnitude = 0.0;
//            normalizedMagnitude = 0.0;
//        }
//    }
//};
//
//} // namespace Input
//#else
//#include <linux/joystick.h>
//namespace Input
//{
//
//    struct Gamepad
//    {
//        // TODO
//    };
//
//}
//#endif
//------------------------------------------------------------------------------
/**
    @file

    @copyright
    (C) 2022 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
