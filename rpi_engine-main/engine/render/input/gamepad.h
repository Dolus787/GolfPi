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
