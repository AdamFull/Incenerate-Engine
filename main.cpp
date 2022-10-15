#include "engine/engine.h"

#include <random>
#include <PerlinNoise.hpp>

using namespace engine;
using namespace engine::graphics;

uint32_t ConvertToRGBA(const glm::vec4& color)
{
    auto r = (uint8_t)(color.x * 255.f);
    auto g = (uint8_t)(color.y * 255.f);
    auto b = (uint8_t)(color.z * 255.f);
    auto a = (uint8_t)(color.w * 255.f);

    return (a << 24) | (b << 16) | (g << 8) | r;
}

int main()
{
    std::random_device r;

    std::vector<FVertex> vVertices{};
    std::default_random_engine e1(r());
    std::uniform_real_distribution<float> _col(0.f, 1.f);

    siv::PerlinNoise noise{ std::random_device{} };
    noise.reseed(std::mt19937{ 67890u });

    for (uint32_t z = 0; z < 100; ++z)
    {
        for (uint32_t y = 0; y < 100; ++y)
        {
            for (uint32_t x = 0; x < 100; ++x)
            {
                auto xc = (double)x * 0.1;
                auto yc = (double)y * 0.1;
                auto zc = (double)z * 0.1;
                auto res = noise.octave3D_01(xc, yc, zc, 1);

                auto colvec = glm::vec4(1.f, 0.f, 0.f, 1.f);

                if (res > 0.5)
                    vVertices.emplace_back(FVertex(glm::vec3(xc, yc, zc), ConvertToRGBA(colvec)));
            }
        }
    }

    FEngineCreateInfo ci;
    ci.eAPI = ERenderApi::eVulkan_1_1;

    ci.window.width = 800;
    ci.window.height = 600;
    ci.window.srName = "my window";

    CEngine::getInstance()->create(ci);

    auto& graphics = CEngine::getInstance()->getGraphics();
    auto& cameractrl = CEngine::getInstance()->getCameraController();


    auto& program = graphics->addStage("voxel_shader");

    auto& vbo = program->getVBO();
    vbo->addVertices(std::move(vVertices));

    program->setRenderFunc([&](CShaderObject* pso, vk::CommandBuffer& cb) 
        {
            auto& camera = cameractrl->getCamera();
            auto projection = camera->getProjection();
            auto view = camera->getView();

            auto& ubo = pso->getUniformBuffer("MatricesBlock");
            ubo->set("proj", projection);
            ubo->set("view", view);
        });

    CEngine::getInstance()->begin_render_loop();

    return 0;
}