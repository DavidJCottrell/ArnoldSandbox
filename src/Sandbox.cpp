#include "Sandbox.h"
#include "ExampleLayer.h"

Sandbox::Sandbox()
{
    PushLayer(new ExampleLayer());
    PushOverlay(new AE::Graphics::UI::ImGuiLayer());
}

Sandbox::~Sandbox()
{
    AE_INFO("Application shutdown");
}
