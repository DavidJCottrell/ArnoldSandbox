#include "ArnoldEngine.h"

class Sandbox : public AE::Application
{
public:
    Sandbox() {}
    ~Sandbox() {}
};

AE::Application *AE::CreateApplication()
{
    return new Sandbox();
}