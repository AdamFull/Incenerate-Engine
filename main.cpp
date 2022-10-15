#include "engine/engine.h"
#include "engine/system/filesystem/filesystem.h"

#include "engine/SparseVoxelOctree.h"

#include <random>
#include <PerlinNoise.hpp>

#define OGT_VOX_IMPLEMENTATION
#include <ogt_vox.h>

using namespace engine;
using namespace engine::graphics;
using namespace engine::system;

uint32_t ConvertToRGBA(const glm::vec4& color)
{
    auto r = (uint8_t)(color.x * 255.f);
    auto g = (uint8_t)(color.y * 255.f);
    auto b = (uint8_t)(color.z * 255.f);
    auto a = (uint8_t)(color.w * 255.f);

    return (a << 24) | (b << 16) | (g << 8) | r;
}

uint32_t ConvertToRGBA(const ogt_vox_rgba& color)
{
    return (color.a << 24) | (color.b << 16) | (color.g << 8) | color.r;
}

int main()
{
    std::random_device r;

    std::vector<FVertex> vVertices{};
    std::default_random_engine e1(r());
    std::uniform_real_distribution<float> _col(0.f, 1.f);

    siv::PerlinNoise noise{ std::random_device{} };
    noise.reseed(std::mt19937{ 67890u });

    vVertices.reserve(500);

    //for (uint32_t x = 0; x < 500; x++)
    //{
    //    auto xc = (double)x * 0.25;
    //    for (uint32_t z = 0; z < 500; z++)
    //    {
    //        auto zc = (double)z * 0.25;
    //
    //        auto height = static_cast<uint32_t>(noise.octave2D_01(xc, zc, 1) * 20.f);
    //
    //        for (uint32_t y = 0; y < height; y++)
    //        {
    //            uint32_t color;
    //            if (y > 0 && y <= 2)
    //                color = 0xFF010203;
    //            else if (y > 2 && y <= 5)
    //                color = 0xFF6c7071;
    //            else if (y > 5 && y <= 10)
    //                color = 0xFFabaeaf;
    //            else
    //                color = 0xFF008000;
    //
    //            auto yc = (double)y * 0.25;
    //            vVertices.emplace_back(FVertex(glm::vec3(xc, yc, zc), color));
    //        }
    //    }
    //}

    CSparseVoxelOctree svo;

    auto voxfile = fs::read_file("models/monu2.vox");

    const ogt_vox_scene* scene = ogt_vox_read_scene_with_flags(reinterpret_cast<const uint8_t*>(voxfile.c_str()), voxfile.size(), 0);
    if (scene)
    {
        for (uint32_t instance_index = 0; instance_index < scene->num_instances; instance_index++)
        {
            const ogt_vox_instance* instance = &scene->instances[instance_index];
            const ogt_vox_model* model = scene->models[instance->model_index];

            uint32_t voxel_index = 0;
            for (uint32_t z = 0; z < model->size_z; z++) 
            {
                auto zc = (float)z * 0.25f;
                for (uint32_t y = 0; y < model->size_y; y++) 
                {
                    auto yc = (float)y * 0.25f;
                    for (uint32_t x = 0; x < model->size_x; x++, voxel_index++)
                    {
                        uint32_t color_index = model->voxel_data[voxel_index];

                        if (color_index != 0)
                        {
                            auto xc = (float)x * 0.25f;
                            auto color = ConvertToRGBA(scene->palette.color[color_index]);
                            vVertices.emplace_back(FVertex(glm::vec3(xc, yc, zc), color));
                            svo.insert(octreevec_t(x, y, z), color);
                        }
                    }
                }
            }
        }
    }

    ogt_vox_destroy_scene(scene);

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