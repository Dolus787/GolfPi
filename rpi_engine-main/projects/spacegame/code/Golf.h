#pragma once
#include "render/model.h"

#include "render/input/gamepad.h"

namespace Render
{
    struct ParticleEmitter;
}

namespace Game
{

    struct GolfBall
    {
        float radius = 0.05;
        glm::vec3 position = glm::vec3(0);
        glm::quat orientation = glm::identity<glm::quat>();
        glm::vec3 camPos = glm::vec3(0, 1.0f, -2.0f);
        glm::mat4 transform = glm::mat4(1);
        glm::vec3 linearVelocity = glm::vec3(0);

        const float normalSpeed = 1.0f;
        const float boostSpeed = normalSpeed * 2.0f;
        const float friction = 1.0f;
        const float camOffsetY = 1.0f;
        const float cameraSmoothFactor = 10.0f;

        float hitpower = 5.0f;

        float energyRetention = 0.85f;

        float rotationZ = 0.0f;
        float rotXSmooth = 0.0f;
        float rotYSmooth = 0.0f;
        float rotZSmooth = 0.0f;

        Render::ModelId model;

        Input::Gamepad *Gamepad;

        void Update(float dt);

        void PhysicsUpdate(float dt);

    };

}