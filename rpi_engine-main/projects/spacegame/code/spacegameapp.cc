//------------------------------------------------------------------------------
// spacegameapp.cc
// (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "spacegameapp.h"
#include <cstring>
#include "imgui.h"
#include "render/renderdevice.h"
#include "render/shaderresource.h"
#include <vector>
#include "render/textureresource.h"
#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "render/debugrender.h"
#include "core/random.h"
#include "render/input/inputserver.h"
#include "render/input/gamepad.h"
#include "core/cvar.h"
#include "render/physics.h"
#include <chrono>
#include <string>
#include "GolfMap.h"
#include <Bits.h>
#include <string>

using namespace Display;
using namespace Render;

namespace Game
{

//------------------------------------------------------------------------------
/**
*/
SpaceGameApp::SpaceGameApp()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SpaceGameApp::~SpaceGameApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SpaceGameApp::Open()
{
	App::Open();
	this->window = new Display::Window;
    this->window->SetSize(1920, 1080);

    if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        RenderDevice::Init();

        
        return true;
	}
	return false;
}


//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::Run()
{
    MapManager* manager = new MapManager();
    manager->maps = { GolfMap("C2Hl1r22C200I1I002200000002r1I00I11ll3l2Wr11r2000H0002200020002C222W2W2C000000H00000000G00", { 1,1,5 }, 9), GolfMap("00G000C2C0G2O2G0C2C000G00", {0,0,0},5), GolfMap() };
    int w;
    int h;
    this->window->GetSize(w, h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
    cam->projection = projection;
    manager->SpawnMaps();

    // Setup asteroids near
    // // load all resources
    //ModelId models[6] = {
    //    LoadModel("assets/space/Asteroid_1.glb"),
    //    LoadModel("assets/space/Asteroid_2.glb"),
    //    LoadModel("assets/space/Asteroid_3.glb"),
    //    LoadModel("assets/space/Asteroid_4.glb"),
    //    LoadModel("assets/space/Asteroid_5.glb"),
    //    LoadModel("assets/space/Asteroid_6.glb")
    //};
    //Physics::ColliderMeshId colliderMeshes[6] = {
    //    Physics::LoadColliderMesh("assets/space/Asteroid_1_physics.glb"),
    //    Physics::LoadColliderMesh("assets/space/Asteroid_2_physics.glb"),
    //    Physics::LoadColliderMesh("assets/space/Asteroid_3_physics.glb"),
    //    Physics::LoadColliderMesh("assets/space/Asteroid_4_physics.glb"),
    //    Physics::LoadColliderMesh("assets/space/Asteroid_5_physics.glb"),
    //    Physics::LoadColliderMesh("assets/space/Asteroid_6_physics.glb")
    //};
    //std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4>> asteroids;
    //
    // // Setup asteroids near
    //for (int i = 0; i < 25; i++)
    //{
    //    std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
    //    size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
    //    std::get<0>(asteroid) = models[resourceIndex];
    //    float span = 20.0f;
    //    glm::vec3 translation = glm::vec3(
    //        Core::RandomFloatNTP() * span,
    //        Core::RandomFloatNTP() * span,
    //        Core::RandomFloatNTP() * span
    //    );
    //    glm::vec3 rotationAxis = normalize(translation);
    //    float rotation = translation.x;
    //    glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
    //    std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
    //    std::get<2>(asteroid) = transform;
    //    asteroids.push_back(asteroid);
    //}
    // // Setup asteroids far
    //for (int i = 0; i < 20; i++)
    //{
    //    std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
    //    size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
    //    std::get<0>(asteroid) = models[resourceIndex];
    //    float span = 80.0f;
    //    glm::vec3 translation = glm::vec3(
    //        Core::RandomFloatNTP() * span,
    //        Core::RandomFloatNTP() * span,
    //        Core::RandomFloatNTP() * span
    //    );
    //    glm::vec3 rotationAxis = normalize(translation);
    //    float rotation = translation.x;
    //    glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
    //    std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
    //    std::get<2>(asteroid) = transform;
    //    asteroids.push_back(asteroid);
    //}

    // Setup skybox
    std::vector<const char*> skybox
    {
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png"
    };
    TextureResourceId skyboxId = TextureResource::LoadCubemap("skybox", skybox, true);
    RenderDevice::SetSkybox(skyboxId);
    
    Input::Keyboard* kbd = Input::GetDefaultKeyboard();

    Input::Gamepad Gamepad = Input::Gamepad();

    const int numLights = 4;
    Render::PointLightId lights[numLights];
    // Setup lights
    for (int i = 0; i < numLights; i++)
    {
        this->window->SetNanoFunc([this](NVGcontext* vg)
        {
            this->RenderNanoVG(vg);
        });

        glm::vec3 translation = glm::vec3(
            Core::RandomFloatNTP() * 20.0f,
            Core::RandomFloatNTP() * 20.0f,
            Core::RandomFloatNTP() * 20.0f
        );
        glm::vec3 color = glm::vec3(
            Core::RandomFloat(),
            Core::RandomFloat(),
            Core::RandomFloat()
        );
        lights[i] = Render::LightServer::CreatePointLight(translation, color, Core::RandomFloat() * 4.0f, 1.0f + (15 + Core::RandomFloat() * 10.0f));
    }


    //GolfBall ball;
    ball.model = LoadModel("assets/golf/GLB/ball-red.glb");
    ball.Gamepad = &Gamepad;
    ball.position = manager->maps[manager->selectedMap].spawnPos;

    std::clock_t c_start = std::clock();
    double dt = 0.01667f;

    

    // game loop
    while (this->window->IsOpen())
	{
        
        auto timeStart = std::chrono::steady_clock::now();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
        
        this->window->Update();

        if (kbd->pressed[Input::Key::Code::End])
        {
            ShaderResource::ReloadShaders();
        }
        Gamepad.Update();
        //Physics::DebugDrawColliders();
        ball.Update(dt);

        // Store all drawcalls in the render device
        for (auto const& tile : manager->tiles)
        {
            RenderDevice::Draw(tile.model, tile.transform);
        }

        RenderDevice::Draw(ball.model, ball.transform);

        // Execute the entire rendering pipeline
        RenderDevice::Render(this->window, dt);

		// transfer new frame to window
		this->window->SwapBuffers();

        auto timeEnd = std::chrono::steady_clock::now();
        dt = std::min(0.04, std::chrono::duration<double>(timeEnd - timeStart).count());

        if (kbd->pressed[Input::Key::Code::Escape]||Gamepad.GetButtonState(Input::button::start).justPressed)
            this->Exit();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::Exit()
{
    this->window->Close();
}
//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::RenderNanoVG(NVGcontext* vg)
{
    // Render UI

    nvgSave(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 600, 100, 50);
    NVGpaint paint;
    paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
    nvgFillPaint(vg, paint);
    nvgFill(vg);

    // Header
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStroke(vg);

    nvgFontSize(vg, 30.0f);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 200));

    int ballCharge_I = (int)((ball.chargeTime / ball.maxChargeTime) * 100);
    std::string ballCharge_String = std::to_string(ballCharge_I).append("% charge");
    const char* ballCharge_CString = ballCharge_String.c_str();

    std::string hits_String = std::to_string(ball.hits).append(" hits");
    const char* hits_CString = hits_String.c_str();

    nvgText(vg, 5, 30, ballCharge_CString, NULL);
    nvgText(vg, 5, 70, hits_CString, NULL);

    nvgRestore(vg);
}

} // namespace Game