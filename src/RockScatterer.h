#pragma once
#include "ArnoldEngine.h"
#include "StaticProp.h"

#include <glm/gtc/matrix_transform.hpp>
#include <random>

struct RockConfig
{
    int   count    = 200;
    float minScale = 0.3f;
    float maxScale = 0.8f;
};

class RockScatterer
{
public:
    static void Scatter(
        PropRenderer& renderer,
        const AE::World::World& world,
        const std::shared_ptr<AE::Graphics::Renderer::Material>& material,
        RockConfig cfg  = {},
        uint32_t   seed = 42)
    {
        auto mesh = AE::Graphics::Renderer::Mesh::CreateCube();

        constexpr float worldSize =
            static_cast<float>(AE::World::World::WORLD_SIZE * AE::World::Chunk::SIZE);
        const float bs = world.GetBlockScale();

        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> posDistr(2.0f, worldSize - 2.0f);
        std::uniform_real_distribution<float> scaleDistr(cfg.minScale, cfg.maxScale);
        std::uniform_real_distribution<float> rotDistr(0.0f, glm::two_pi<float>());

        int placed   = 0;
        int attempts = 0;
        const int maxAttempts = cfg.count * 10;

        while (placed < cfg.count && attempts < maxAttempts)
        {
            ++attempts;
            const float wx = posDistr(rng);
            const float wz = posDistr(rng);
            const float surfaceY = FindSurfaceY(world, wx, wz);
            if (surfaceY < 0.0f) continue;

            const float scale = scaleDistr(rng) * bs;
            const float rot   = rotDistr(rng);

            glm::mat4 t = glm::translate(glm::mat4(1.0f),
                glm::vec3(wx * bs, surfaceY * bs, wz * bs));
            t = glm::rotate(t, rot, {0.0f, 1.0f, 0.0f});
            t = glm::scale(t, glm::vec3(scale));

            renderer.Add({t, mesh, material});
            ++placed;
        }
    }

private:
    // Binary-search for the surface Y in block-space.
    // density > 0 = solid, density < 0 = air.
    // Returns the Y of the first air point above the solid surface, or -1 if not found.
    static float FindSurfaceY(const AE::World::World& world, float wx, float wz)
    {
        constexpr float yBottom = 0.0f;
        constexpr float yTop    = 18.0f;

        if (world.SampleDensity(wx, yBottom, wz) < 0.0f) return -1.0f; // all air
        if (world.SampleDensity(wx, yTop,    wz) > 0.0f) return -1.0f; // all solid

        float lo = yBottom, hi = yTop;
        for (int i = 0; i < 16; ++i)
        {
            const float mid = (lo + hi) * 0.5f;
            if (world.SampleDensity(wx, mid, wz) > 0.0f)
                lo = mid;
            else
                hi = mid;
        }
        return hi; // first air point — rock base sits here
    }
};
