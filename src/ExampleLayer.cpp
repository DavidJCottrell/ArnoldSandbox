#include "ExampleLayer.h"

ExampleLayer::ExampleLayer() = default;

void ExampleLayer::OnUpdate()
{
    if (AE::Core::Input::IskeyPressed(AE_KEY_TAB))
    {
        AE_INFO("TAB KEY PRESSED");
    }
}

void ExampleLayer::OnEvent(AE::Events::Event& event)
{
}

void ExampleLayer::OnImGuiRender()
{
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}
