#pragma once
#include "render/model.h"
#include "render/input/gamepad.h"
#include "render/input/keyboard.h"


namespace Game
{
    enum PlayState {
        InPlay,
        BeforePlay,
        NameSelect,
    };

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
        const float camOffsetY = 1.0f;
        const float cameraSmoothFactor = 10.0f;

        PlayState state = PlayState::BeforePlay;

        const float friction = 0.65f;
        
        const float slowLimit = 0.04f;

        bool ballStill = false;
        bool switchMap = false;
        bool charging = false; 

        float hitpower = 10.0f;
        float chargeTime = 0.0f;
        float maxChargeTime = 1.0f;

        glm::vec2 *goalPos = nullptr;
        glm::vec3 *spawnPos = nullptr;

        unsigned short hits = 0;

        float energyRetention = 0.85f;

        float rotationZ = 0.0f;
        float rotXSmooth = 0.0f;
        float rotYSmooth = 0.0f;
        float rotZSmooth = 0.0f;

        Render::ModelId model;

        float groundLevel=0.0f;

        Input::Gamepad *Gamepad;
        Input::Keyboard *kbd;

        void Update(float dt);
        
        void HitGoal();

        void PhysicsUpdate(float dt);

    };

}