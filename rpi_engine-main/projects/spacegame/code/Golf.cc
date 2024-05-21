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
        Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);

        PhysicsUpdate(dt);

        // Hit ball
        if (Gamepad->GetButtonState(a).justPressed) {
            vec3 desiredVelocity = vec3(0, 0, this->hitpower);
            linearVelocity = this->transform * vec4(desiredVelocity, 0.0f);
        }

        //Rotate ball smoothly
        float rotX = -Gamepad->GetLeftJoystickX();

        const float rotationSpeed = 1.8f * dt;
        
        rotXSmooth = mix(rotXSmooth, rotX * rotationSpeed, dt * cameraSmoothFactor);

        quat localOrientation = quat(vec3(-rotYSmooth, rotXSmooth, rotZSmooth));
        this->orientation = this->orientation * localOrientation;
        
        mat4 T = translate(this->position) * (mat4)this->orientation;
        this->transform = T * (mat4)quat(vec3(0, 0, rotationZ));

        // update camera view transform
        vec3 desiredCamPos = this->position + vec3(this->transform * vec4(0, camOffsetY, -4.0f, 0));
        this->camPos = mix(this->camPos, desiredCamPos, dt * cameraSmoothFactor);
        cam->view = lookAt(this->camPos, this->camPos + vec3(this->transform[2]), vec3(this->transform[1]));
    }

    void
        GolfBall::PhysicsUpdate(float dt)
    {
        /*
        TODO:

        Friction not taken into account during collision check, low framerate will cause less friction than usual.
        Friction as a whole might need to be changed to make this implementation easier.
        */



        Physics::RaycastPayload payload;

        glm::vec3 dir = (glm::normalize(linearVelocity));
        float len = linearVelocity.length()* dt + radius;        
        
        // Keep last payload with collision to calculate where the ball ended up after collision.
        Physics::RaycastPayload lastPayload;
        
        payload = Physics::Raycast(position + glm::normalize(linearVelocity) * radius, dir, len);
        
        // To calculate remaining velocity after collision.
        unsigned int hitCounter=0;
        
        
        while (payload.hit && len>0) {

            hitCounter++;

            len = (((len - (payload.hitDistance)) * energyRetention));
            dir = glm::normalize(glm::reflect(dir, glm::normalize(lastPayload.hitNormal) * radius));
            // NORMALS NOT NORMILIZED, DOES NOT WORK UNLESS NORMILIZED
            lastPayload = payload;

            payload = Physics::Raycast(payload.hitPoint+ (glm::normalize(lastPayload.hitNormal) * radius), dir, len);
        }

        if (hitCounter!=0) {
            // Remaining velocity after collision.
            this->linearVelocity = glm::reflect((this->linearVelocity), glm::normalize(lastPayload.hitNormal)) * (float)(glm::pow(energyRetention, hitCounter));

            // Calculate where the ball ended up after collision(s).
            this->position = lastPayload.hitPoint + (dir*len) + (glm::normalize(-lastPayload.hitNormal)*radius);
        }
        else {
            // Ordinary movement without collision, friction added
            this->linearVelocity = mix(this->linearVelocity, vec3(0, 0, 0), dt * friction);
            this->position += this->linearVelocity * dt;
        }
        

        // Move ball to ground.
        dir = vec3(0,-1,0);
        len = 1.0;
        
        //Debug::DrawLine(position, position + dir*len, 2.0, vec4(0,1,0,0), vec4(0, 1, 0, 0));
        payload = Physics::Raycast(position, dir, len);
        if (payload.hit) {
            position = payload.hitPoint + vec3(0, radius, 0);
        }


        return;
    }
}