#define AE_MAIN
#define GLM_ENABLE_EXPERIMENTAL

#include "ArnoldEngine.h"
#include "glm/gtx/transform.hpp"

class ExampleLayer final : public AE::Core::Layer
{
public:
    ExampleLayer()
        : Layer("ExampleLayer"),
          m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
          m_CameraPosition(0.0f)
    {
        m_SquareVertexArray.reset(AE::Graphics::Renderer::VertexArray::Create());

        constexpr float squareVertices[3 * 4] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f
        };

        // Create the vertex buffer
        std::shared_ptr<AE::Graphics::Renderer::VertexBuffer> squareVB;
        squareVB.reset(AE::Graphics::Renderer::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        // Set the layout of the vertex buffer
        squareVB->SetLayout({
            {AE::Graphics::Renderer::ShaderDataType::Float3, "a_Position"},
        });
        // Add the vertex buffer to the vertex array
        m_SquareVertexArray->AddVertexBuffer(squareVB);


        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        // Create the index buffer
        std::shared_ptr<AE::Graphics::Renderer::IndexBuffer> squareIB;
        squareIB.reset(
            AE::Graphics::Renderer::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

        // Add the index buffer to the vertex array
        m_SquareVertexArray->SetIndexBuffer(squareIB);

        std::string vertexSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec3 v_Position;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Position;

            void main()
            {
                color = vec4(0.2, 0.3, 0.8, 1.0);
            }
        )";

        m_SquareShader = std::make_unique<AE::Graphics::Renderer::Shader>(vertexSrc, fragmentSrc);
    }

    void OnUpdate(const AE::Core::Timestep ts) override
    {
        glm::vec3 moveDir(0.0f);

        /* Convert camera's rotation from degrees to radians.
         * Making it negative ensures the correct directional mapping
        **/
        const float rads = glm::radians(m_CameraRotation);
        const glm::vec2 up(sin(rads), cos(rads));
        const glm::vec2 right(cos(rads), -sin(rads));

        if (AE::Core::Input::IsKeyPressed(AE_KEY_W)) // Up
            moveDir += glm::vec3(up.x, up.y, 0.0f);
        if (AE::Core::Input::IsKeyPressed(AE_KEY_S)) // Down
            moveDir -= glm::vec3(up.x, up.y, 0.0f);
        if (AE::Core::Input::IsKeyPressed(AE_KEY_D)) // Right
            moveDir += glm::vec3(right.x, right.y, 0.0f);
        if (AE::Core::Input::IsKeyPressed(AE_KEY_A)) // Left
            moveDir -= glm::vec3(right.x, right.y, 0.0f);

        // Normalise horizontal movement
        if (length(moveDir) > 0.0f)
            moveDir = normalize(moveDir);

        m_CameraPosition += moveDir * m_CameraMoveSpeed * ts.GetSeconds();

        AE::Graphics::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        AE::Graphics::Renderer::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        AE::Graphics::Renderer::Renderer::Renderer::BeginScene(m_Camera);
        const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        for (int y = 0; y < 20; ++y)
        {
            for (int x = 0; x < 20; ++x)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = translate(glm::mat4(1.0f), pos) * scale;
                AE::Graphics::Renderer::Renderer::Submit(m_SquareShader, m_SquareVertexArray, transform);
            }
        }

        AE::Graphics::Renderer::Renderer::Renderer::EndScene();
    }

    void OnImGuiRender() override
    {
        // Camera Controls
        ImGui::Begin("Camera");
        ImGui::Text("Camera rotation: %.2f", m_CameraRotation);
        ImGui::Text("Camera position: (x: %.2f, y: %.2f)", m_CameraPosition.x, m_CameraPosition.y);
        if (ImGui::Button("Reset Camera"))
        {
            m_CameraRotation = 0.0f;
            m_CameraPosition = {0.0f, 0.0f, 0.0f};
        }
        ImGui::SliderFloat("Rotation", &m_CameraRotation, 0.0f, 360.0f);
        ImGui::End();
    }

private:
    std::shared_ptr<AE::Graphics::Renderer::VertexArray> m_SquareVertexArray;

    std::shared_ptr<AE::Graphics::Renderer::Shader> m_SquareShader;

    AE::Graphics::Renderer::OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 5.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 180.0f;
};


// --------- Sandbox ---------


class SandboxGame final : public AE::Core::Application
{
public:
    SandboxGame()
    {
        PushLayer(new ExampleLayer());
    }

    ~SandboxGame() override
    = default;
};


AE::Core::Application* AE::Core::CreateApplication()
{
    AE_INFO("Starting application...");
    return new SandboxGame();
}
