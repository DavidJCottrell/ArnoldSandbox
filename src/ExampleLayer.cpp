#include "ExampleLayer.h"

ExampleLayer::ExampleLayer() = default;

void ExampleLayer::OnUpdate()
{
}

void ExampleLayer::OnEvent(AE::Events::Event& event)
{
}

void ExampleLayer::OnImGuiRender()
{
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}
