#pragma once

#include "ArnoldEngine.h"

class ExampleLayer final : public AE::Core::Layer
{
public:
    ExampleLayer();

    void OnUpdate() override;
    void OnEvent(AE::Events::Event& event) override;
    void OnImGuiRender() override;

    bool HandleKeyPressed(AE::Events::KeyPressedEvent& e);
};
