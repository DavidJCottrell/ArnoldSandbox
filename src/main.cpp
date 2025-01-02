#define AE_MAIN

#include "ArnoldEngine.h"
#include "Sandbox.h"

AE::Core::Application *AE::Core::CreateApplication()
{
    AE_INFO("Starting application...");
    return new Sandbox();
}