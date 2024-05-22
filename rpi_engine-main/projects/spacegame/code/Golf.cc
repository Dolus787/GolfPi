#include "config.h"
#include "Golf.h"
#include "render/input/inputserver.h"
#include "render/cameramanager.h"
#include "render/physics.h"
#include "render/debugrender.h"
#include "render/particlesystem.h"

using namespace Input;
using namespace glm;
using namespace Render;

namespace Game
{


    void
        GolfBall::Update(float dt)
    {

        /*
        TODO:

        Controls need implementation, charge up etc.

        Check collision with goal.

        */



        Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);

        PhysicsUpdate(dt);

        // Hit ball
        if (Gamepad->GetButtonState(a).justPressed) {
            vec3 desiredVelocity = vec3(0, 0, hitpower);
            linearVelocity = transform * vec4(desiredVelocity, 0.0f);
        }

        //Rotate ball smoothly
        float rotX = -Gamepad->GetLeftJoystickX();

        const float rotationSpeed = 1.8f * dt;
        
        rotXSmooth = mix(rotXSmooth, rotX * rotationSpeed, dt * cameraSmoothFactor);

        quat localOrientation = quat(vec3(-rotYSmooth, rotXSmooth, rotZSmooth));
        orientation = orientation * localOrientation;
        
        mat4 T = translate(position) * (mat4)orientation;
        transform = T * (mat4)quat(vec3(0, 0, rotationZ));

        // update camera view transform
        vec3 desiredCamPos = position + vec3(transform * vec4(0, camOffsetY, -4.0f, 0));
        camPos = mix(camPos, desiredCamPos, dt * cameraSmoothFactor);
        cam->view = lookAt(camPos, camPos + vec3(transform[2]), vec3(transform[1]));
    }

    void
        GolfBall::PhysicsUpdate(float dt)
    {
        /*
        TODO:

        */
        
        
        // NORMALS NOT NORMILIZED, DOES NOT WORK UNLESS NORMILIZED

        Physics::RaycastPayload payload;

        glm::vec3 dir = (glm::normalize(linearVelocity));
        float len = (glm::length(linearVelocity)* dt);

        
        // Keep last payload with collision to calculate where the ball ended up after collision.
        Physics::RaycastPayload lastPayload;
        
        payload = Physics::Raycast(position, dir, len);
        
        // To calculate remaining velocity after collision.
        unsigned int hitCounter=0;
        
        while (payload.hit && hitCounter<4) {

            hitCounter++;
            len = ((len - (payload.hitDistance)) * energyRetention)-(payload.hitDistance*friction);
            dir = glm::reflect(dir, glm::normalize(lastPayload.hitNormal));
            lastPayload = payload;

            payload = Physics::Raycast(payload.hitPoint, dir, len);
        }

        if (hitCounter!=0) {
            // Remaining velocity after collision.
            linearVelocity = glm::reflect((linearVelocity), glm::normalize(lastPayload.hitNormal)) * (float)(glm::pow(energyRetention, hitCounter));
            
            // Calculate where the ball ended up after collision(s).
            position = lastPayload.hitPoint + (dir*len);
        }
        else {
            // Ordinary movement without collision, friction added
            position += linearVelocity * dt;
        }

        if (glm::length(linearVelocity) < slowLimit) {
            // Sets actual velocity to zero when close enough.
            linearVelocity = vec3(0, 0, 0);
            ballStill = true;
        }
        else {
            // Friction.
            float frictionLoss((glm::length(linearVelocity) - ((dt * glm::length(linearVelocity)) * friction))/ glm::length(linearVelocity));
            linearVelocity = (linearVelocity * frictionLoss);
            ballStill = false;
        }
        
        if (groundLevel==0) {
            // Move ball to ground.
            dir = vec3(0,-1,0);
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
}