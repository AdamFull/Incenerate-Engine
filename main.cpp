#include "engine/engine.h"


using namespace engine;
using namespace engine::graphics;

int main()
{
    std::vector<FVertex> vVertices{};
    vVertices.emplace_back(FVertex(glm::vec3(-1.f, -1.f, 0.5f), 0xFF0000FF));
    vVertices.emplace_back(FVertex(glm::vec3(1.f, -1.f, 0.5f), 0x00FF00FF ));
    vVertices.emplace_back(FVertex(glm::vec3(-1.f, 1.f, 0.5f), 0x0000FFFF ));
    vVertices.emplace_back(FVertex(glm::vec3(1.f, 1.f, 0.5f), 0xFF00FFFF ));
    vVertices.emplace_back(FVertex(glm::vec3(-1.f, -1.f, -0.5f), 0xFF00FFFF ));
    vVertices.emplace_back(FVertex(glm::vec3(1.f, -1.f, -0.5f), 0xFF00FFFF ));
    vVertices.emplace_back(FVertex(glm::vec3(-1.f, 1.f, -0.5f), 0xFF00FFFF ));
    vVertices.emplace_back(FVertex(glm::vec3(1.f, 1.f, -0.5f), 0xFF00FFFF ));

    std::vector<uint32_t> vIndices {
        //Top
        2, 6, 7,
        2, 3, 7,

        //Bottom
        0, 4, 5,
        0, 1, 5,

        //Left
        0, 2, 6,
        0, 4, 6,

        //Right
        1, 3, 7,
        1, 5, 7,

        //Front
        0, 2, 3,
        0, 1, 3,

        //Back
        4, 6, 7,
        4, 5, 7 
    };


    FEngineCreateInfo ci;
    ci.eAPI = ERenderApi::eVulkan_1_1;

    ci.window.width = 800;
    ci.window.height = 600;
    ci.window.srName = "my window";

    CEngine::getInstance()->create(ci);

    auto& graphics = CEngine::getInstance()->getGraphics();
    auto& cameractrl = CEngine::getInstance()->getCameraController();


    auto& program = graphics->addStage("test_shader");

    auto& vbo = program->getVBO();
    vbo->addVertices(std::move(vVertices));
    vbo->addIndices(std::move(vIndices));

    program->setRenderFunc([&](CShaderObject* pso, vk::CommandBuffer& cb) 
        {
            auto& camera = cameractrl->getCamera();
            auto projection = camera->getProjection();
            auto view = camera->getView();

            auto& ubo = pso->getUniformBuffer("FUniformData");
            ubo->set("projection", projection);
            ubo->set("view", view);
        });

    CEngine::getInstance()->begin_render_loop();

    return 0;
}