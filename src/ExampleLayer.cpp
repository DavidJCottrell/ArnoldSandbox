#include "ExampleLayer.h"

ExampleLayer::ExampleLayer() : Layer()
{
}

void ExampleLayer::OnUpdate()
{
}

void ExampleLayer::OnEvent(AE::Events::Event &event)
{
    AE_TRACE("{0}", event);
}