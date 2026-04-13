#define AE_MAIN
#include "ArnoldEngine.h"
#include "StaticProp.h"
#include "RockScatterer.h"

#include <glm/gtc/matrix_transform.hpp>

class MyGame final : public AE::VoxelLayer
{
public:
    MyGame() : VoxelLayer({.blockScale = 1.0f, .seed = 42})
    {
        auto player = std::make_shared<AE::PlayerController>();
        SetPlayerController(player);
    }

    void OnAttach() override
    {
        auto shader   = AE::Graphics::Renderer::Shader::Create("assets/shaders/prop.glsl");
        auto material = AE::Graphics::Renderer::Material::Create(shader);
        material->SetTexture(0, AE::Graphics::Renderer::Texture2D::Create("assets/textures/rock_face.jpg"));
        material->SetInt("u_Texture", 0);
        material->SetFloat3("u_LightDir", glm::normalize(glm::vec3(0.6f, 1.0f, 0.4f)));

        RockScatterer::Scatter(m_Props, GetWorld(), material, {.count = 200}, 42);
    }

    void OnVoxelUpdate(AE::Core::Timestep) override
    {
        m_Props.Render(GetScene().GetCamera().GetCamera());
    }

private:
    PropRenderer m_Props;
};


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
