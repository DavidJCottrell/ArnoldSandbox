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
        // --- Cube geometry ---
        // 24 vertices: 4 per face, Float3 position + Float2 UV
        // Face order: front (+Z), back (-Z), top (+Y), bottom (-Y), right (+X), left (-X)
        // UVs: (0,0) bottom-left → (1,1) top-right, consistent across all faces
        constexpr float vertices[] = {
            // Front (+Z)
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
            // Back (-Z)
             0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
            // Top (+Y)
            -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
            // Bottom (-Y)
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,
            // Right (+X)
             0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
            // Left (-X)
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
        };

        m_CubeVertexArray.reset(AE::Graphics::Renderer::VertexArray::Create());

        std::shared_ptr<AE::Graphics::Renderer::VertexBuffer> vb;
        vb.reset(AE::Graphics::Renderer::VertexBuffer::Create(vertices, sizeof(vertices)));
        vb->SetLayout({
            { AE::Graphics::Renderer::ShaderDataType::Float3, "a_Position" },
            { AE::Graphics::Renderer::ShaderDataType::Float2, "a_TexCoord" },
        });
        m_CubeVertexArray->AddVertexBuffer(vb);

        // 36 indices: 6 faces x 2 triangles x 3 vertices
        uint32_t indices[36];
        for (uint32_t face = 0; face < 6; ++face)
        {
            const uint32_t base = face * 4;
            const uint32_t i    = face * 6;
            indices[i + 0] = base + 0; indices[i + 1] = base + 1; indices[i + 2] = base + 2;
            indices[i + 3] = base + 2; indices[i + 4] = base + 3; indices[i + 5] = base + 0;
        }

        std::shared_ptr<AE::Graphics::Renderer::IndexBuffer> ib;
        ib.reset(AE::Graphics::Renderer::IndexBuffer::Create(indices, 36));
        m_CubeVertexArray->SetIndexBuffer(ib);

        // --- Shaders ---
        const std::string vertexSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec2 a_TexCoord;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec2 v_TexCoord;

            void main()
            {
                v_TexCoord  = a_TexCoord;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec2 v_TexCoord;

            uniform sampler2D u_Texture;

            void main()
            {
                color = texture(u_Texture, v_TexCoord);
            }
        )";

        m_CubeShader = std::make_shared<AE::Graphics::Renderer::Shader>(vertexSrc, fragmentSrc);

        // --- Procedural checkerboard texture (no asset file required) ---
        constexpr uint32_t W = 8, H = 8;
        uint32_t pixels[W * H];
        for (uint32_t y = 0; y < H; ++y)
            for (uint32_t x = 0; x < W; ++x)
                pixels[y * W + x] = ((x + y) % 2 == 0) ? 0xFFFFFFFF : 0xFF404040;

        m_Texture = AE::Graphics::Renderer::Texture2D::Create("C:/dev/ArnoldSandbox/assets/textures/grass.jpg");
    }

    void OnUpdate(const AE::Core::Timestep ts) override
    {
        if (m_CursorCaptured)
        {
            const glm::vec3 right = m_Camera.GetRight();

            // Project forward to XZ plane for grounded horizontal movement
            const glm::vec3 camForward = m_Camera.GetForward();
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
        AE::Graphics::Renderer::Renderer::Submit(m_CubeShader, m_CubeVertexArray);
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
    AE::Graphics::Renderer::PerspectiveCamera            m_Camera;
    std::shared_ptr<AE::Graphics::Renderer::VertexArray> m_CubeVertexArray;
    std::shared_ptr<AE::Graphics::Renderer::Shader>      m_CubeShader;
    std::shared_ptr<AE::Graphics::Renderer::Texture2D>   m_Texture;

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
