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
#include <string>
#include <fstream>
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
#include "GolfMap.h"
#include <chrono>
#include <string>
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
    MapManager* mapManager = new MapManager();
    mapManager->maps = { 
        GolfMap("C2C202CHW00200200H03W00G", {1,1,0}, 3),
        GolfMap("CHC00020CC00200r1IH0C2HCW2C000G00000",{2,1,3}, 6),
        GolfMap("C22l1r22C200I1I002200000002r1I00I11ll3l2Wr11IH000H0000200020000C222W2C00000000200000000G00", { 1,1,4 }, 9)
    };
    //map for rotation testing purpose
    //mapManager->maps = { GolfMap("00G000C2C0G2O2G0C2C000G00", { 0,0,0 }, 5) };
    int w;
    int h;
    this->window->GetSize(w, h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
    cam->projection = projection;
    mapManager->SpawnMaps();



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
    
    kbd = Input::GetDefaultKeyboard();

    Gamepad = Input::Gamepad();

    const int numLights = 4;
    Render::PointLightId lights[numLights];
    this->window->SetNanoFunc([this](NVGcontext* vg)
    {
        this->RenderUI(vg);
    });



    //GolfBall ball;
    ball.model = LoadModel("assets/golf/GLB/ball-red.glb");
    ball.Gamepad = &Gamepad;
    ball.kbd = Input::GetDefaultKeyboard();
    ball.position = mapManager->maps[mapManager->selectedMap].spawnPos;
    ball.spawnPos = &mapManager->maps[mapManager->selectedMap].spawnPos;
    ball.goalPos = &mapManager->maps[mapManager->selectedMap].goalPos;
    mapIndex = mapManager->selectedMap;
    std::clock_t c_start = std::clock();
    double dt = 0.01667f;

    

    // game loop.
    ReadScore();
    while (this->window->IsOpen()){
        
        auto timeStart = std::chrono::steady_clock::now();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
        
        this->window->Update();

        if (kbd->pressed[Input::Key::Code::End]){
            ShaderResource::ReloadShaders();
        }
        Gamepad.Update();
        //Physics::DebugDrawColliders();
        if (ball.switchMap)
        {
            ball.switchMap = false;
            SwitchMap(mapManager);
        }
        if (ball.resetMap)
        {
            ball.resetMap = false;
            ResetMap();
        }
        if (ball.state == PlayState::InPlay || ball.state == PlayState::BeforePlay) {
            ball.Update(dt);
        }
        else {
            SelectName();
        }

        // Store all drawcalls in the render device
        for (auto const& tile : mapManager->tiles){
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
SpaceGameApp::ResetMap()
{
    ball.state = PlayState::BeforePlay;
    if (ball.spawnPos != nullptr) {
        ball.position = *ball.spawnPos;
        ball.hits = 0;
        ball.linearVelocity = { 0,0,0 };
    }
}
//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::SwitchMap(MapManager* manager)
{
    mapIndex = manager->NextMap();
    ball.state = PlayState::BeforePlay;

    ball.spawnPos = &manager->maps[mapIndex].spawnPos;
    ball.goalPos = &manager->maps[mapIndex].goalPos;

    if (ball.spawnPos != nullptr) {
        ball.position = *ball.spawnPos;
        ball.hits = 0;
        ball.linearVelocity = { 0,0,0 };
    }
}
//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::RenderUI(NVGcontext* vg)
{
    // Render UI

    nvgSave(vg);

    //nvgBeginPath(vg);
    //nvgCircle(vg, 600, 100, 50);
    //NVGpaint paint;
    //paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
    //nvgFillPaint(vg, paint);
    //nvgFill(vg);

    // Header
    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStroke(vg);

    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 200));

    nvgFontSize(vg, 30.0f);
    int width, height;
    this->window->GetSize(width, height);
    if (ball.state == Game::PlayState::BeforePlay) {
        // If before play I want to dislpay the in play info too.
        nvgText(vg, width-200, 30, "High Scores", NULL);
        for (size_t i = 0; i < 5; i++)
        {
            const char* highscoreC = highscores[mapIndex][i].name;
            std::string highscoreS = highscoreC;
            highscoreS[3] = ':';

            // Skip in the ascii table to the numbers.
            highscoreS[4]=(highscores[mapIndex][i].hits) + (char)'0';
            nvgText(vg, width-200, 70+30*i, highscoreS.c_str(), NULL);
        }

        goto inplay;
    }
    if (ball.state == Game::PlayState::InPlay) {
        inplay:
        int ballCharge_I = (int)((ball.chargeTime / ball.maxChargeTime) * 100);
        std::string ballCharge_String = std::to_string(ballCharge_I).append("% charge");
        const char* ballCharge_CString = ballCharge_String.c_str();

        std::string hits_String = std::to_string(ball.hits).append(" hits");
        const char* hits_CString = hits_String.c_str();

        nvgText(vg, 5, 30, ballCharge_CString, NULL);
        nvgText(vg, 5, 70, hits_CString, NULL);

    }
    else if (ball.state == Game::PlayState::NameSelect) {
        nvgFontSize(vg, 100.0f);
        const char* newName = name;
        nvgText(vg, width/2-100, height/2, newName, NULL);
    }


    nvgRestore(vg);
}
//-----------------------------------------------------------------------------------------------------------------
void
SpaceGameApp::SelectName()
{
    //Gamepad control
    /*if (Gamepad->GetButtonState(a).held && chargeTime < maxChargeTime) {
        chargeTime += dt;
        charging = true;
    }
    else if(charging){
        vec3 desiredVelocity = vec3(0, 0, 1) * ((chargeTime / maxChargeTime)*hitpower);
        linearVelocity = transform * vec4(desiredVelocity, 0.0f);
        hits++;
        charging = false;
        chargeTime = 0.0f;
    }*/

    // Keyboard control for debug purposes.
    if (kbd->pressed[Input::Key::Down]) {
        name[charIndex]++;
        if (name[charIndex] > 90) {
            name[charIndex] = 65;
        }
    }

    if (kbd->pressed[Input::Key::Up]) {
        name[charIndex]--;
        if (name[charIndex] < 65) {
            name[charIndex] = 90;
        }
    }

    if (kbd->pressed[Input::Key::Right]) {
        charIndex++;
        if (charIndex > 2) {
            charIndex = 0;
        }
    }

    if (kbd->pressed[Input::Key::Left]) {
        charIndex--;
        if (charIndex < 0) {
            charIndex = 2;
        }
    }

    if (kbd->pressed[Input::Key::RightShift]) {
        
        InsertScore();
        charIndex = 0;
        // Ugly, but it didnt let me do it in the nice way
        name[0] = 'A', name[1] = 'A', name[2] = 'A';
        ball.switchMap = true;
    }
}
//-----------------------------------------------------------------------------------------------------------------
    /*
    Save format:
    No spaces with the order of: map index, name, hits. The numbers in the form of char with the ascii index being the number. 
    New line separates scores. 5 highscores per map.

    It will look like this in the file:

    [?]ARO!
    [?]LOV[?]
    
    */
void
SpaceGameApp::SaveScore() {
    std::ofstream file(fileName);
        // if the score is higher than 127 it has not been entered in yet
        for (size_t i = 0; i < 3; i++){
            for (size_t y = 0; y < 5; y++){
                if (highscores[i][y].hits < 127) {
                    file << (char)i << highscores[i][y].name[0] << highscores[i][y].name[1] << highscores[i][y].name[2] << (char)highscores[i][y].hits << "\n";
                }
            }
        }
    file.close();
}
void
SpaceGameApp::InsertScore() {
    if (ball.hits > 127) {
        // Can't save scores higher than 127 due to saving in chars
        std::cerr << "skill issue";
    }
    else {
        //Find which index to insert at
        unsigned short index;
        for (short i = 4; i >= 0; i--){
            if (highscores[mapIndex][i].hits >= ball.hits) {
                index = i;
            }
        }        
        for (short i = 4; i > index; i--){
            highscores[mapIndex][i] = highscores[mapIndex][i - 1];
        }
        highscores[mapIndex][index] = { name[0], name[1], name[2], ball.hits };
    }
    SaveScore();
}
void
SpaceGameApp::ReadScore() {

    std::ifstream file(fileName);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error opening the file!" << "\n";
        return;
    }

    // Which index for all three maps.
    unsigned short HighscoreIndex[3] = {0, 0, 0};

    while (std::getline(file, line)) {
        // Read all lines.
        // Cast numbers to shorts from chars, automatically converts.
        highscores[(unsigned short)line[0]][HighscoreIndex[(unsigned short)line[0]]] = 
            {line[1], 
            line[2], 
            line[3], 
            (unsigned short)line[4]};

        HighscoreIndex[(unsigned short)line[0]]++;
    }

    file.close();
}
//-----------------------------------------------------------------------------------------------------------------
} // namespace Game