#include "ArnoldEngine.h"

class ExampleLayer : public AE::Core::Layer
{
public:
    ExampleLayer()
        : Layer()
    {
    }

    void OnUpdate() override
    {
        // AE_INFO("ExampleLayer::Update");
    }

    void OnEvent(AE::Events::Event &event) override
    {
        // AE_TRACE("{0}", event);
    }
};

class Sandbox : public AE::Core::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
        PushOverlay(new AE::Graphics::UI::ImGuiLayer());
    }

    ~Sandbox()
    {
        AE_INFO("Application shutdown");
    }
};

AE::Core::Application *AE::Core::CreateApplication()
{
    AE_INFO("Starting application...");
    return new Sandbox();
}