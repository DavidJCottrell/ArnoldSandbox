#include "ArnoldEngine.h"

class Sandbox : public AE::Core::Application
{
public:
    Sandbox(const char *windowTitle) : AE::Core::Application(windowTitle) {}
    ~Sandbox()
    {
        AE_INFO("Application shutdown");
    }
};

AE::Core::Application *AE::Core::CreateApplication()
{
    AE_INFO("Starting application...");
    return new Sandbox("Demo Game");
}