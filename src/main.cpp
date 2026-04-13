#define AE_MAIN

#include "ArnoldEngine.h"

class FpsLayer final : public AE::Core::Layer
{
public:
    FpsLayer()
        : Layer("FpsLayer"),
          m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f)
    {
        m_Shader  = AE::Graphics::Renderer::Shader::Create("assets/shaders/textured.glsl");
        m_Texture = AE::Graphics::Renderer::Texture2D::Create("assets/textures/dirt.png");
        m_CameraController.GetCamera().SetPosition({32.0f, 5.0f, 32.0f});
    }

    void OnUpdate(const AE::Core::Timestep ts) override
    {
        m_CameraController.OnUpdate(ts);

        AE::Graphics::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        AE::Graphics::Renderer::RenderCommand::Clear();

        m_Shader->Bind();
        m_Texture->Bind(0);
        m_Shader->UploadUniformInt("u_Texture", 0);
        m_Shader->UploadUniformFloat3("u_LightDir", glm::normalize(glm::vec3(0.6f, 1.0f, 0.4f)));

        AE::Graphics::Renderer::Renderer::BeginScene(m_CameraController.GetCamera());
        m_World.Render(m_Shader);
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
    AE::Graphics::Renderer::FpsCameraController       m_CameraController;
    AE::World::World                                   m_World;
    std::shared_ptr<AE::Graphics::Renderer::Shader>    m_Shader;
    std::shared_ptr<AE::Graphics::Renderer::Texture2D> m_Texture;
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
