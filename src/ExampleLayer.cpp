#include "ExampleLayer.h"

ExampleLayer::ExampleLayer() = default;

void ExampleLayer::OnUpdate()
{
}

void ExampleLayer::OnEvent(AE::Events::Event &event)
{
    AE_TRACE("{0}", event);
}
