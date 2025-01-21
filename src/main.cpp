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
          m_CameraPosition(0.0f),
          m_SquarePosition(0.0f)
    {
        // ----------- TRIANGLE -----------
        m_TriangleVertexArray.reset(AE::Graphics::Renderer::VertexArray::Create());

        constexpr float triangleVertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 0.7f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.5f, 0.0f, 0.8f, 0.7f, 1.0f, 1.0f
        };

        std::shared_ptr<AE::Graphics::Renderer::VertexBuffer> squareVertexBuffer;
        squareVertexBuffer.reset(
            AE::Graphics::Renderer::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));
        const AE::Graphics::Renderer::BufferLayout layout = {
            {AE::Graphics::Renderer::ShaderDataType::Float3, "a_Position"},
            {AE::Graphics::Renderer::ShaderDataType::Float4, "a_Color"},
        };
        squareVertexBuffer->SetLayout(layout);
        m_TriangleVertexArray->AddVertexBuffer(squareVertexBuffer);

        uint32_t indices[3] = {0, 1, 2};
        std::shared_ptr<AE::Graphics::Renderer::IndexBuffer> squareIndexBuffer;
        squareIndexBuffer.reset(
            AE::Graphics::Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_TriangleVertexArray->SetIndexBuffer(squareIndexBuffer);


        // ----------- SQUARE -----------
        m_SquareVertexArray.reset(AE::Graphics::Renderer::VertexArray::Create());

        constexpr float squareVertices[3 * 4] = {
            -0.75f, -0.75f, 0.0f,
            0.75f, -0.75f, 0.0f,
            0.75f, 0.75f, 0.0f,
            -0.75f, 0.75f, 0.0f
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
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec3 v_Position;
            out vec4 v_Color;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Position;
            in vec4 v_Color;

            void main()
            {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }
        )";

        m_TriangleShader = std::make_unique<AE::Graphics::Renderer::Shader>(vertexSrc, fragmentSrc);

        std::string vertexSrc2 = R"(
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

        std::string fragmentSrc2 = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Position;

            void main()
            {
                color = vec4(0.2, 0.3, 0.8, 1.0);
            }
        )";

        m_SquareShader = std::make_unique<AE::Graphics::Renderer::Shader>(vertexSrc2, fragmentSrc2);
    }

    void OnUpdate(const AE::Core::Timestep ts) override
    {
        // Up/Down Camera Movement
        if (AE::Core::Input::IsKeyPressed(AE_KEY_UP)) m_CameraPosition.y += m_CameraMoveSpeed * ts;
        else if (AE::Core::Input::IsKeyPressed(AE_KEY_DOWN)) m_CameraPosition.y -= m_CameraMoveSpeed * ts;
        // Left/Right Camera Movement
        if (AE::Core::Input::IsKeyPressed(AE_KEY_LEFT)) m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (AE::Core::Input::IsKeyPressed(AE_KEY_RIGHT)) m_CameraPosition.x += m_CameraMoveSpeed * ts;

        // Camera Rotation
        if (AE::Core::Input::IsKeyPressed(AE_KEY_A))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        else if (AE::Core::Input::IsKeyPressed(AE_KEY_D))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        // Square Movement
        if (AE::Core::Input::IsKeyPressed(AE_KEY_I)) m_SquarePosition.y += m_SquareMoveSpeed * ts;
        else if (AE::Core::Input::IsKeyPressed(AE_KEY_K)) m_SquarePosition.y -= m_SquareMoveSpeed * ts;
        // Left/Right Square Movement
        if (AE::Core::Input::IsKeyPressed(AE_KEY_J)) m_SquarePosition.x -= m_SquareMoveSpeed * ts;
        else if (AE::Core::Input::IsKeyPressed(AE_KEY_L)) m_SquarePosition.x += m_SquareMoveSpeed * ts;

        AE::Graphics::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        AE::Graphics::Renderer::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        AE::Graphics::Renderer::Renderer::Renderer::BeginScene(m_Camera);

        const glm::mat4 transform = translate(glm::mat4(1.0f), m_SquarePosition);

        AE::Graphics::Renderer::Renderer::Renderer::Submit(m_SquareShader, m_SquareVertexArray, transform);
        AE::Graphics::Renderer::Renderer::Renderer::Submit(m_TriangleShader, m_TriangleVertexArray);

        AE::Graphics::Renderer::Renderer::Renderer::EndScene();
    }

private:
    std::shared_ptr<AE::Graphics::Renderer::VertexArray> m_TriangleVertexArray;
    std::shared_ptr<AE::Graphics::Renderer::VertexArray> m_SquareVertexArray;

    std::shared_ptr<AE::Graphics::Renderer::Shader> m_TriangleShader;
    std::shared_ptr<AE::Graphics::Renderer::Shader> m_SquareShader;

    AE::Graphics::Renderer::OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 5.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 180.0f;

    glm::vec3 m_SquarePosition;
    float m_SquareMoveSpeed = 1.0f;

    glm::vec3 m_TrianglePosition;
};

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
