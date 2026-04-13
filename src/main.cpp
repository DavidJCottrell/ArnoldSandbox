#define AE_MAIN
#include "ArnoldEngine.h"

class MyGame final : public AE::VoxelLayer
{
public:
    MyGame() : VoxelLayer({.blockScale = 1.0f, .seed = 42}) {}
};


// --------- Sandbox ---------


class SandboxGame final : public AE::Core::Application
{
public:
    SandboxGame()
    {
        PushLayer(new MyGame());
    }

    ~SandboxGame() override = default;
};


AE::Core::Application* AE::Core::CreateApplication()
{
    AE_INFO("Starting application...");
    return new SandboxGame();
}
