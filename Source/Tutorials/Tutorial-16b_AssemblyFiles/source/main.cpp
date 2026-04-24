// TGP.cpp : Defines the entry point for the application.
//

#include <fstream>
#include <iomanip>
#include <tge/animation/AnimationPlayer.h>
#include <tge/graphics/AmbientLight.h>
#include <tge/graphics/DirectionalLight.h>
#include <tge/graphics/PointLight.h>
#include <tge/input/InputManager.h>
#include <tge/error/ErrorManager.h>
#include <tge/graphics/Camera.h>
#include <tge/graphics/DX11.h>
#include <tge/drawers/ModelDrawer.h>
#include <tge/graphics/FullscreenEffect.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/graphics/RenderTarget.h>
#include <tge/Model/ModelFactory.h>
#include <tge/Model/ModelInstance.h>
#include <tge/graphics/GraphicsStateStack.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/Timer.h>
#include <tge/settings/settings.h>

#ifdef _DEBUG
#pragma comment(lib,"Engine_Debug.lib")
#pragma comment(lib,"External_Debug.lib")
#else
#pragma comment(lib,"Engine_Release.lib")
#pragma comment(lib,"External_Release.lib")
#endif // _DEBUG

#pragma comment (lib, "assimp-vc142-mt.lib")
#pragma comment (lib, "d3d11.lib")

using namespace Tga;

float camSpeed = 1000.f;
float camRotSpeed = 1.f;

struct RenderData
{
    DepthBuffer myIntermediateDepth;
    RenderTarget myIntermediateTexture;

    std::vector<std::shared_ptr<ModelInstance>> myModels;
    std::vector<AnimatedModelInstance*> myAnimatedModels;
    std::vector<std::shared_ptr<PointLight>> myPointLights;
    std::shared_ptr<DirectionalLight> myDirectionalLight;
    std::shared_ptr<AmbientLight> myAmbientLight;
    std::shared_ptr<Camera> myMainCamera;
};

void Render(RenderData& renderData, GraphicsEngine& graphicsEngine)
{
    GraphicsStateStack& graphicsStateStack = graphicsEngine.GetGraphicsStateStack();

    ////////////////////////////////////////////////////////////////////////////////
    //// Cleanup

    renderData.myIntermediateTexture.Clear();
    renderData.myIntermediateDepth.Clear();

    ////////////////////////////////////////////////////////////////////////////////
    //// Update Camera

    graphicsStateStack.SetCamera(*renderData.myMainCamera);

    ////////////////////////////////////////////////////////////////////////////////
    //// Prepare lights

    graphicsStateStack.SetDirectionalLight(*renderData.myDirectionalLight);
    graphicsStateStack.SetAmbientLight(*renderData.myAmbientLight);

    graphicsStateStack.ClearPointLights();
    for (std::shared_ptr<PointLight>& pointLight : renderData.myPointLights)
    {
        graphicsStateStack.AddPointLight(*pointLight);
    }

    ////////////////////////////////////////////////////////////////////////////////
    //// Draw all forward rendered objects

    graphicsStateStack.Push();
    graphicsStateStack.SetBlendState(BlendState::Disabled);
    renderData.myIntermediateTexture.SetAsActiveTarget(&renderData.myIntermediateDepth);

    for (auto& modelInstance : renderData.myModels)
    {
        graphicsEngine.GetModelDrawer().DrawPbr(*modelInstance);
    }
    for (auto& modelInstance : renderData.myAnimatedModels) {
        graphicsEngine.GetModelDrawer().DrawPbr(*modelInstance);
    }
    graphicsStateStack.Pop();

    ////////////////////////////////////////////////////////////////////////////////

    graphicsStateStack.Push();
    graphicsStateStack.SetBlendState(BlendState::Disabled);
    DX11::BackBuffer->SetAsActiveTarget();
    renderData.myIntermediateTexture.SetAsResourceOnSlot(0);
    graphicsEngine.GetFullscreenEffectTonemap().Render();
    graphicsStateStack.Pop();
}

void Go(void);
int main(const int /*argc*/, const char* /*argc*/[])
{
    Go();
    return 0;
}

Tga::InputManager* SInputManager;

// This is where the application starts of for real. By keeping this in it's own file
// we will have the same behaviour for both console and windows startup of the
// application.
//
void Go(void)
{
    Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
    Tga::EngineConfiguration createParameters;

    createParameters.myWinProcCallback = [](HWND, UINT message, WPARAM wParam, LPARAM lParam)
    {
        SInputManager->UpdateEvents(message, wParam, lParam);
        return 0;
    };

    createParameters.myApplicationName = L"TGA 2D Tutorial 16b";
    createParameters.myActivateDebugSystems = Tga::DebugFeature::Fps |
        Tga::DebugFeature::Mem |
        Tga::DebugFeature::Drawcalls |
        Tga::DebugFeature::Cpu |
        Tga::DebugFeature::Filewatcher |
        Tga::DebugFeature::OptimizeWarnings;
    createParameters.myEnableVSync = true;

    if (!Tga::Engine::Start(createParameters))
    {
        ERROR_PRINT("Fatal error! Engine could not start!");
        system("pause");
        return;
    }

    {
        Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();
        bool bShouldRun = true;

        RenderData renderData;
        {
            renderData.myIntermediateDepth = DepthBuffer::Create(DX11::GetResolution());
            renderData.myIntermediateTexture = RenderTarget::Create(DX11::GetResolution(), DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);
        }

        HWND windowHandle = *Tga::Engine::GetInstance()->GetHWND();

        Tga::InputManager myInputManager(windowHandle);
        SInputManager = &myInputManager;
        bool isMouseTrapped = false;


        ModelFactory& modelFactory = ModelFactory::GetInstance();

        // TODO: ModelFactory needs to spit out shared ptrs.
        std::shared_ptr<ModelInstance> mdlPlane = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Plane"));
        mdlPlane->SetScale({ 10 });

        std::shared_ptr<ModelInstance> mdlChest = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(L"Particle_Chest.fbx"));
        mdlChest->SetLocation({ -150, 0, 0 });
        mdlChest->SetRotation({ 0.f, 135.f, 0.f });

        std::shared_ptr<ModelInstance> mdlChestTgm = std::make_shared<ModelInstance>(modelFactory.GetTgmModelInstance(L"Particle_Chest.tgm"));
        mdlChestTgm->SetLocation({ 350, 0, -200 });
        mdlChestTgm->SetRotation({ 0.f, -110.f, 0.f });

        auto dLight = DirectionalLight(
            Transform{
                Vector3f(0, 0, 0),
                Rotator(225, 0, 0)
            },
            Color{ 0.2f, 0.2f, 0.2f },
            0.f
        );

        Tga::AnimatedModelInstance animatedModel = modelFactory.GetAnimatedTgmModelInstance(L"CH_NPC_Undead_17G3_SK.tgm");
        animatedModel.SetLocation({ 0.0f, 100.0f, 0.0f });
        //animatedModel.SetScale({ 10.0f });

        Tga::AnimationPlayer animation = modelFactory.GetAnimationPlayer(L"CH_NPC_Undead@Idle_01_17G3_AN.fbx", animatedModel.GetModel());
        animation.SetIsLooping(true);

        renderData.myAnimatedModels.push_back(&animatedModel);

        std::wstring cubeMap = Settings::ResolveGameAssetPathW("cube_1024_preblurred_angle3_Skansen3.dds");

        auto aLight = AmbientLight(
            cubeMap,
            Color{ 0.0f, 0.0f, 0.0f },
            0.f
        );

        std::shared_ptr<Camera> camera = std::make_shared<Camera>(Camera());

        camera->SetPerspectiveProjection(
            90,
            {
                (float)resolution.x,
                (float)resolution.y
            },
            0.1f,
            50000.0f);

        camera->SetPosition(Vector3f(0.0f, 500.0f, -550.0f));
        camera->SetRotation(Rotator(45, 0, 0));

        renderData.myModels.push_back(mdlPlane);
        renderData.myModels.push_back(mdlChest);
        renderData.myModels.push_back(mdlChestTgm);

        renderData.myMainCamera = camera;
        renderData.myAmbientLight = std::make_shared<AmbientLight>(aLight);
        renderData.myDirectionalLight = std::make_shared<DirectionalLight>(dLight);

        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(0.f, 400.f, 200.f),
                Rotator::Zero
            },
            Color{ 0.2f, 1.0f, 0.2f },
            100000.0f,
            800.f));
        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(-200.f, 400.f, 100.f),
                Rotator::Zero
            },
            Color{ 0.2f, 0.2f, 1.0f },
            100000.0f,
            800.f));
        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(200.f, 400.f, -100.f),
                Rotator::Zero
            },
            Color{ 1.f, 1.0f, 1.0f },
            50000.0f,
            800.f));

        renderData.myPointLights.push_back(std::make_shared<PointLight>(
            Transform{
                Vector3f(-500.f, 400.f, 0.f),
                Rotator(90.f, 0.f, 0.f)
            },
            Color{ 1.f, 0.2f, 0.2f },
            100000.0f,
            700.f));

        MSG msg = { 0 };

        Timer timer;

        animation.Play();
        while (bShouldRun)
        {
            timer.Update();
            myInputManager.Update();

            Transform camTransform = camera->GetTransform();
            Vector3f camMovement = Vector3f::Zero;
            Rotator camRotation = Rotator::Zero;

            animation.Update(timer.GetDeltaTime());
            animatedModel.SetPose(animation);

            {
                // Example of getting information from the pose of a character:

                // Find the bone index:
                std::string boneName = "RightHand";
                size_t boneIndex = animatedModel.GetModel()->GetSkeleton()->JointNameToIndex.find(boneName)->second;

                // Convert pose to model space
                ModelSpacePose pose;
                animatedModel.GetModel()->GetSkeleton()->ConvertPoseToModelSpace(animation.GetLocalSpacePose(), pose);

                // Find the hands transform in world space
                Matrix4x4 animatedModelTransform = animatedModel.GetTransform().GetMatrix();
                Matrix4x4 handToWorldTransform = pose.JointTransforms[boneIndex] * animatedModelTransform;

                // Specify the line's position relative to the hand and convert to world space:
                Vector4f v0 = Vector4f(-10.f, 8.f, 3.f, 1.f) * handToWorldTransform;
                Vector4f v1 = Vector4f(-10.f, -100.f, 3.f, 1.f) * handToWorldTransform;
            } 


            // Only read 3D navigation input if
            // the mouse is currently trapped.
            if (isMouseTrapped)
            {
                if (myInputManager.IsKeyHeld(0x57)) // W
                {
                    camMovement += camTransform.GetMatrix().GetForward() * 1.0f;
                }
                if (myInputManager.IsKeyHeld(0x53)) // S
                {
                    camMovement += camTransform.GetMatrix().GetForward() * -1.0f;
                }
                if (myInputManager.IsKeyHeld(0x41)) // A
                {
                    camMovement += camTransform.GetMatrix().GetRight() * -1.0f;
                }
                if (myInputManager.IsKeyHeld(0x44)) // D
                {
                    camMovement += camTransform.GetMatrix().GetRight() * 1.0f;
                }

                camera->SetPosition(camera->GetTransform().GetPosition() + camMovement * camSpeed * timer.GetDeltaTime());

                const Vector2f mouseDelta = myInputManager.GetMouseDelta();

                camRotation.X = mouseDelta.Y;
                camRotation.Y = mouseDelta.X;

                camTransform.AddRotation(camRotation * camRotSpeed);

                camera->SetRotation(camTransform.GetRotation());
            }

            if (myInputManager.IsKeyPressed(VK_RBUTTON))
            {
                // Capture mouse.
                if (!isMouseTrapped)
                {
                    myInputManager.HideMouse();
                    myInputManager.CaptureMouse();
                    isMouseTrapped = true;
                }
            }

            if (myInputManager.IsKeyReleased(VK_RBUTTON))
            {
                // When we let go, recapture.
                if (isMouseTrapped)
                {
                    myInputManager.ShowMouse();
                    myInputManager.ReleaseMouse();
                    isMouseTrapped = false;
                }
            }

            if (myInputManager.IsKeyPressed(VK_SHIFT))
            {
                // When we hold shift, "sprint".
                camSpeed /= 4;
            }

            if (myInputManager.IsKeyReleased(VK_SHIFT))
            {
                // When we let go, "walk".
                camSpeed *= 4;
            }

            if (myInputManager.IsKeyPressed(VK_ESCAPE))
            {
                PostQuitMessage(0);
            }

            if (!Tga::Engine::GetInstance()->BeginFrame())
            {
                break;
            }

            Render(renderData, Tga::Engine::GetInstance()->GetGraphicsEngine());

            Tga::Engine::GetInstance()->EndFrame();
        }
    }

    Tga::Engine::GetInstance()->Shutdown();

    return;
}

