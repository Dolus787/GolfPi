#include "config.h"
#include "Golf.h"
#include <iostream>
#include "render/cameramanager.h"
#include "render/physics.h"
#include "render/debugrender.h"
#include "render/particlesystem.h"


using namespace Input;
using namespace glm;
using namespace Render;

namespace Game
{
    
    //-----------------------------------------------------------------------------------------------------------------
    void
        GolfBall::Update(float dt)
    {
        Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);

        PhysicsUpdate(dt);

        // Hit ball
        if (ballStill) {

            // Gamepad control.
            if (Gamepad->GetButtonState(a).held && chargeTime < maxChargeTime) {
                chargeTime += dt;
                charging = true;
            }


            else if(charging){
                vec3 desiredVelocity = vec3(0, 0, 1) * ((chargeTime / maxChargeTime)*hitpower);
                linearVelocity = transform * vec4(desiredVelocity, 0.0f);
                state = PlayState::InPlay;
                hits++;
                charging = false;
                chargeTime = 0.0f;
            }

            // Keyboard control for debug purposes.
            //if (kbd->held[Input::Key::Space] && chargeTime < maxChargeTime) {
            //    chargeTime += dt;
            //    charging = true;
            //}
            //else if (charging) {
            //    vec3 desiredVelocity = vec3(0, 0, 1) * ((chargeTime / maxChargeTime) * hitpower);
            //    linearVelocity = transform * vec4(desiredVelocity, 0.0f);
            //    state = PlayState::InPlay;
            //    hits++;
            //    charging = false;
            //    chargeTime = 0.0f;
            //}

        }

        // Rotate ball and camera (gamepad)
        //float rotX = -Gamepad->GetLeftJoystickX();
        //float rotY = -glm::min(Gamepad->GetRightJoystickY(), 0.4f);


			//switch map
		if (Gamepad->GetButtonState(back).justPressed && !switchMap)
		{
			switchMap = true;
		}
		if (Gamepad->GetButtonState(y).justPressed && !resetMap)
		{
			resetMap = true;
		}

        float rotX = 0.0f, rotY = 0.0f;

        // Rotate x of camera
        if (Gamepad->GetLeftJoystickX() != 0)
        {
            rotX -= 0.5f * Gamepad->GetLeftJoystickX();
        }
        // Rotate camera up
        if (Gamepad->GetRightJoystickY() > 0.1)
        {
            rotY += 1.0f * Gamepad->GetRightJoystickY();
        }
        // Rotate camera down
        if (Gamepad->GetRightJoystickY() < -0.1)
        {
            rotY += 0.3f * Gamepad->GetRightJoystickY();
        }
        // Rotate ball and camera (kbd)
        if (kbd->held[Input::Key::Left]) {
            rotX += 0.5f;
        }
        else if (kbd->held[Input::Key::Right]) {
            rotX -= 0.5f;
        }

        if (kbd->held[Input::Key::Up]) {
            rotY += 1.0f;
        }
        else if (kbd->held[Input::Key::Down]) {
            rotY -= 0.3f;

        }
        // Switch map
        if (kbd->pressed[Input::Key::Tab] && !switchMap)
        {
            switchMap = true;
        }
        if (kbd->pressed[Input::Key::R] && !resetMap)
        {
            resetMap = true;
        }


        const float rotationSpeed = 1.8f * dt;

        rotXSmooth = mix(rotXSmooth, rotX * rotationSpeed, dt * cameraSmoothFactor);

        quat localOrientation = quat(vec3(-rotYSmooth, rotXSmooth, rotZSmooth));
        orientation = orientation * localOrientation;

        mat4 T = translate(position) * (mat4)orientation;
        transform = T * (mat4)quat(vec3(0, 0, rotationZ));

        // update camera view transform
        vec3 desiredCamPos = position + vec3(transform * vec4(0, camOffsetY + (2 * rotY), camOffsetZ + glm::max((rotY * 1.5f), -2.0f), 0));
        camPos = mix(camPos, desiredCamPos, dt * cameraSmoothFactor);
        cam->view = lookAt(camPos, position, vec3(transform[1]));
    }
    //-----------------------------------------------------------------------------------------------------------------
    void
        GolfBall::PhysicsUpdate(float dt)
    {
        /*
        TODO:

        */


        /// NORMALS NOT NORMILIZED, DOES NOT WORK UNLESS NORMILIZED!

        Physics::RaycastPayload payload;

        glm::vec3 dir = (glm::normalize(linearVelocity));
        dir = glm::normalize(dir * vec3(1, 0, 1));
        float len = (glm::length(linearVelocity) * dt);


        // Keep last payload with collision to calculate where the ball ended up after collision.
        Physics::RaycastPayload lastPayload;

        payload = Physics::Raycast(position, dir, len);

        // To calculate remaining velocity after collision.
        unsigned int hitCounter = 0;

        while (payload.hit) {
            // Check collision with goal, if closer than approximate width of the pole. Approximate size is 0.05.
            if (goalPos != nullptr) {
                if (glm::distance(*goalPos, {payload.hitPoint.x, payload.hitPoint.z}) < 0.05f) {
                    HitGoal();
                }
            }

            hitCounter++;
            len = ((len - (payload.hitDistance)) * energyRetention) - (payload.hitDistance * friction);
            dir = glm::normalize(glm::reflect(dir, glm::normalize(lastPayload.hitNormal)) * vec3(1, 0, 1));
            lastPayload = payload;
            payload = Physics::Raycast(payload.hitPoint, dir, len);
        }


        if (glm::length(linearVelocity) < slowLimit) {
            // Sets actual velocity to zero when close enough. This also makes it possible to hit the ball again.
            linearVelocity = vec3(0, 0, 0);
            ballStill = true;
        }
        else {
            // Friction
            float frictionLoss((glm::length(linearVelocity) - ((dt * glm::length(linearVelocity)) * friction)) / glm::length(linearVelocity));
            linearVelocity = (linearVelocity * frictionLoss);
            ballStill = false;
        }

        if (hitCounter != 0) {
            // Remaining velocity after collision.
            linearVelocity = glm::reflect((linearVelocity), glm::normalize(lastPayload.hitNormal)) * (float)(glm::pow(energyRetention, hitCounter));

            // Calculate where the ball ended up after collision(s).
            position = lastPayload.hitPoint + (dir * len);
        }
        else {
            // Ordinary movement without collision, friction added earlier
            position += linearVelocity * dt;
        }

        if (groundLevel == 0) {
            // Move ball to ground.
            dir = vec3(0, -1, 0);
            len = 1.0;

            //Debug::DrawLine(position, position + dir*len, 2.0, vec4(0,1,0,0), vec4(0, 1, 0, 0));
            payload = Physics::Raycast(position, dir, len);
            if (payload.hit) {
                groundLevel = payload.hitPoint.y + radius;
            }
        }
        else {
            position.y = groundLevel;
        }

        return;
    }
    //-----------------------------------------------------------------------------------------------------------------
    void  GolfBall::HitGoal() {
        state = NameSelect;
    }
    //-----------------------------------------------------------------------------------------------------------------


}