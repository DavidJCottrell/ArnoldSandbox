#include "ArnoldEngine.h"

class Sandbox : public AE::Application
{
public:
    Sandbox() {}
    ~Sandbox() {
        AE_INFO("Application shutdown");
    }
};

AE::Application *AE::CreateApplication()
{
    AE_INFO("Starting application...");
    return new Sandbox();
}