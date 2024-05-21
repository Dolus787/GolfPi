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
    GolfBall::GolfBall()
    {

    }

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
        Physics::RaycastPayload payload;
        glm::vec3 dir;
        float len;
        // Cool shit
        dir = (glm::normalize(linearVelocity));
        len = linearVelocity.length() * dt;
        
        Physics::RaycastPayload lastPayload;
        glm::vec3 futurePos = dir * len;
        glm::vec3 startLine = position + glm::normalize(linearVelocity) * radius;
        
        payload = Physics::Raycast(position, dir, len);
        
        unsigned int hitCounter=0;
        
        
        while (payload.hit) {

            hitCounter++;

            len = (((len - (payload.hitDistance)) * energyLoss) + radius);
            dir = glm::normalize(glm::reflect(dir, glm::normalize(glm::normalize(lastPayload.hitNormal) * radius)));
            
            lastPayload = payload;

            payload = Physics::Raycast(payload.hitPoint, dir, len);
        }
        if (hitCounter!=0) {
            
            this->linearVelocity = glm::reflect((this->linearVelocity), glm::normalize(lastPayload.hitNormal)) * (float)(glm::pow(energyLoss, hitCounter));

            this->position = lastPayload.hitPoint + (glm::normalize(lastPayload.hitNormal)*radius);
            //this->position = lastPayload.hitPoint + (dir*len) + (glm::normalize(lastPayload.hitNormal)*radius);
        }
        else {
            this->linearVelocity = mix(this->linearVelocity, vec3(0, 0, 0), dt * friction);
            this->position += this->linearVelocity * dt;
        }
        
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