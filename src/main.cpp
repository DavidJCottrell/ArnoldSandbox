#define AE_MAIN

#include "ArnoldEngine.h"
#include "Arnold/Core/Window.h"
#include "Arnold/Events/MouseEvent.h"
#include "Arnold/Events/KeyEvent.h"
#include "Arnold/Events/ApplicationEvent.h"

class FpsLayer final : public AE::Core::Layer
{
public:
    FpsLayer()
        : Layer("FpsLayer"),
          m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f)
    {
        m_CubeShader = AE::Graphics::Renderer::Shader::Create("assets/shaders/textured.glsl");
        m_Texture    = AE::Graphics::Renderer::Texture2D::Create("assets/textures/Rembo.JPEG");
        m_Camera.SetPosition({32.0f, 5.0f, 32.0f});
    }

    void OnUpdate(const AE::Core::Timestep ts) override
    {
        if (m_CursorCaptured)
        {
            const glm::vec3 right      = m_Camera.GetRight();
            const glm::vec3 camForward = m_Camera.GetForward();

            // Project forward onto XZ plane for grounded horizontal movement
            glm::vec3 moveForward(camForward.x, 0.0f, camForward.z);
            if (glm::length(moveForward) > 0.001f)
                moveForward = glm::normalize(moveForward);

            glm::vec3 moveDir(0.0f);
            if (AE::Core::Input::IsKeyPressed(AE_KEY_W))           moveDir += moveForward;
            if (AE::Core::Input::IsKeyPressed(AE_KEY_S))           moveDir -= moveForward;
            if (AE::Core::Input::IsKeyPressed(AE_KEY_D))           moveDir += right;
            if (AE::Core::Input::IsKeyPressed(AE_KEY_A))           moveDir -= right;
            if (AE::Core::Input::IsKeyPressed(AE_KEY_SPACE))       moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
            if (AE::Core::Input::IsKeyPressed(AE_KEY_LEFT_SHIFT))  moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);

            if (glm::length(moveDir) > 0.0f)
                moveDir = glm::normalize(moveDir);

            m_Camera.SetPosition(m_Camera.GetPosition() + moveDir * m_MoveSpeed * ts.GetSeconds());
        }

        AE::Graphics::Renderer::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        AE::Graphics::Renderer::RenderCommand::Clear();

        m_Texture->Bind(0);
        m_CubeShader->UploadUniformInt("u_Texture", 0);

        AE::Graphics::Renderer::Renderer::BeginScene(m_Camera);
        m_World.Render(m_CubeShader);
        AE::Graphics::Renderer::Renderer::EndScene();
    }

    void OnEvent(AE::Events::Event& e) override
    {
        AE::Events::EventHandler handler(e);

        handler.TryHandle<AE::Events::MouseMovedEvent>([this](AE::Events::MouseMovedEvent& ev)
        {
            if (!m_CursorCaptured)
                return false;

            if (m_FirstMouse)
            {
                m_LastMouseX = ev.GetX();
                m_LastMouseY = ev.GetY();
                m_FirstMouse = false;
                return false;
            }

            const float xOffset = ev.GetX() - m_LastMouseX;
            const float yOffset = ev.GetY() - m_LastMouseY;
            m_LastMouseX = ev.GetX();
            m_LastMouseY = ev.GetY();

            m_Camera.ProcessMouseMovement(xOffset, yOffset);
            return false;
        });

        handler.TryHandle<AE::Events::KeyPressedEvent>([this](AE::Events::KeyPressedEvent& ev)
        {
            if (ev.GetKeyCode() == AE_KEY_ESCAPE)
            {
                m_CursorCaptured = !m_CursorCaptured;
                m_FirstMouse = true;
                AE::Core::Application::Get().GetWindow().SetCursorMode(
                    m_CursorCaptured
                        ? AE::Core::CursorMode::Captured
                        : AE::Core::CursorMode::Normal
                );
                return true;
            }
            return false;
        });

        handler.TryHandle<AE::Events::WindowResizeEvent>([this](AE::Events::WindowResizeEvent& ev)
        {
            if (ev.GetWidth() > 0 && ev.GetHeight() > 0)
                m_Camera.SetAspectRatio(static_cast<float>(ev.GetWidth()) / static_cast<float>(ev.GetHeight()));
            return false;
        });
    }

    void OnImGuiRender() override
    {
        const glm::vec3& pos = m_Camera.GetPosition();
        ImGui::Begin("Camera");
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        ImGui::Text("Yaw:   %.1f", m_Camera.GetYaw());
        ImGui::Text("Pitch: %.1f", m_Camera.GetPitch());
        ImGui::Separator();
        ImGui::Text("Cursor: %s", m_CursorCaptured ? "Captured" : "Normal");
        ImGui::Text("[ESC] to toggle cursor capture");
        ImGui::End();
    }

private:
    AE::Graphics::Renderer::PerspectiveCamera          m_Camera;
    AE::World::World                                   m_World;
    std::shared_ptr<AE::Graphics::Renderer::Shader>    m_CubeShader;
    std::shared_ptr<AE::Graphics::Renderer::Texture2D> m_Texture;

    float m_MoveSpeed   = 5.0f;
    float m_LastMouseX  = 640.0f;
    float m_LastMouseY  = 360.0f;
    bool  m_FirstMouse     = true;
    bool  m_CursorCaptured = false;
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
