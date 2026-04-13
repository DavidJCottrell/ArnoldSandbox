#define AE_MAIN

#include "ArnoldEngine.h"

class FpsLayer final : public AE::Core::Layer
{
public:
    FpsLayer()
        : Layer("FpsLayer"),
          m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f)
    {
        auto shader = AE::Graphics::Renderer::Shader::Create("assets/shaders/textured.glsl");
        auto atlas  = AE::Graphics::Renderer::TextureAtlas::Create(
            "assets/textures/grass.jpg",
            "assets/textures/dirt.png");

        m_Material = AE::Graphics::Renderer::Material::Create(shader);
        m_Material->SetTexture(0, atlas->GetTexture());
        m_Material->SetInt("u_Texture", 0);
        m_Material->SetFloat3("u_LightDir", glm::normalize(glm::vec3(0.6f, 1.0f, 0.4f)));

        AE::World::WorldGenerator::GenerateTerrain(m_World, 42);

        m_CameraController.GetCamera().SetPosition({32.0f, 15.0f, 32.0f});
    }

    void OnUpdate(const AE::Core::Timestep ts) override
    {
        m_CameraController.OnUpdate(ts);

        AE::Graphics::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        AE::Graphics::Renderer::RenderCommand::Clear();

        AE::Graphics::Renderer::Renderer::BeginScene(m_CameraController.GetCamera());
        m_World.Render(m_Material);
        AE::Graphics::Renderer::Renderer::EndScene();
    }

    void OnEvent(AE::Events::Event& e) override
    {
        m_CameraController.OnEvent(e);
    }

    void OnImGuiRender() override
    {
        const auto& cam = m_CameraController.GetCamera();
        ImGui::Begin("Camera");
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", cam.GetPosition().x, cam.GetPosition().y, cam.GetPosition().z);
        ImGui::Text("Yaw:   %.1f", cam.GetYaw());
        ImGui::Text("Pitch: %.1f", cam.GetPitch());
        ImGui::Separator();
        ImGui::Text("Cursor: %s", m_CameraController.IsCursorCaptured() ? "Captured" : "Normal");
        ImGui::Text("[ESC] to toggle cursor capture");
        ImGui::End();
    }

private:
    AE::Graphics::Renderer::FpsCameraController      m_CameraController;
    AE::World::World                                  m_World;
    std::shared_ptr<AE::Graphics::Renderer::Material> m_Material;
};


// --------- Sandbox ---------


class SandboxGame final : public AE::Core::Application
{
public:
    SandboxGame()
    {
        PushLayer(new FpsLayer());
    }

    ~SandboxGame() override = default;
};


AE::Core::Application* AE::Core::CreateApplication()
{
    AE_INFO("Starting application...");
    return new SandboxGame();
}
