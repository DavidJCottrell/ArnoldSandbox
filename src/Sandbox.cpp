#include "Sandbox.h"
#include "ExampleLayer.h"

Sandbox::Sandbox()
{
    PushLayer(new ExampleLayer());
}

Sandbox::~Sandbox()
{
    AE_INFO("Application shutdown");
}
