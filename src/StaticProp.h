#pragma once
#include "ArnoldEngine.h"
#include <vector>

struct StaticProp
{
    glm::mat4                                          transform;
    std::shared_ptr<AE::Graphics::Renderer::Mesh>     mesh;
    std::shared_ptr<AE::Graphics::Renderer::Material> material;
};

class PropRenderer
{
public:
    void Add(StaticProp prop) { m_Props.push_back(std::move(prop)); }
    void Clear()              { m_Props.clear(); }
    std::size_t Count() const { return m_Props.size(); }

    void Render(const AE::Graphics::Renderer::Camera& cam)
    {
        AE::Graphics::Renderer::Renderer::BeginScene(cam);
        for (const auto& p : m_Props)
            AE::Graphics::Renderer::Renderer::Submit(p.material, p.mesh, p.transform);
        AE::Graphics::Renderer::Renderer::EndScene();
    }

private:
    std::vector<StaticProp> m_Props;
};
