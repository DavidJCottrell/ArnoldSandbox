#pragma once

#include "ArnoldEngine.h"

class ExampleLayer : public AE::Core::Layer
{
public:
    ExampleLayer();

    void OnUpdate() override;
    void OnEvent(AE::Events::Event &event) override;
};