// dear imgui: Renderer Backend for Vulkan
// This needs to be used along with a Platform Backend (e.g. GLFW, SDL, Win32, custom..)

// Implemented features:
//  [X] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.
//  [x] Renderer: Multi-viewport / platform windows. With issues (flickering when creating a new viewport).
//  [!] Renderer: User texture binding. Use 'vk::DescriptorSet' as ImTextureID. Read the FAQ about ImTextureID! See https://github.com/ocornut/imgui/pull/914 for discussions.

// Important: on 32-bit systems, user texture binding is only supported if your imconfig file has '#define ImTextureID ImU64'.
// This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
// To build this on 32-bit systems and support texture changes:
// - [Solution 1] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'ImTextureID=ImU64' (this is what we do in our .vcxproj files)
// - [Solution 2] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'IMGUI_USER_CONFIG="my_imgui_config.h"' and inside 'my_imgui_config.h' add '#define ImTextureID ImU64' and as many other options as you like.
// - [Solution 3] IDE/msbuild: edit imconfig.h and add '#define ImTextureID ImU64' (prefer solution 2 to create your own config file!)
// - [Solution 4] command-line: add '/D ImTextureID=ImU64' to your cl.exe command-line (this is what we do in our batch files)

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// The aim of imgui_impl_vulkan.h/.cpp is to be usable in your engine without any modification.
// IF YOU FEEL YOU NEED TO MAKE ANY CHANGE TO THIS CODE, please share them and your feedback at https://github.com/ocornut/imgui/

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2022-XX-XX: Platform: Added support for multiple windows via the ImGuiPlatformIO interface.
//  2021-10-15: Vulkan: Call vkCmdSetScissor() at the end of render a full-viewport to reduce likehood of issues with people using vk::DynamicState::eScissor in their app without calling vkCmdSetScissor() explicitly every frame.
//  2021-06-29: Reorganized backend to pull data from a single structure to facilitate usage with multiple-contexts (all g_XXXX access changed to bd->XXXX).
//  2021-03-22: Vulkan: Fix mapped memory validation error when buffer sizes are not multiple of vk::PhysicalDeviceLimits::nonCoherentAtomSize.
//  2021-02-18: Vulkan: Change blending equation to preserve alpha in output buffer.
//  2021-01-27: Vulkan: Added support for custom function load and IMGUI_IMPL_VULKAN_NO_PROTOTYPES by using ImGui_ImplVulkan_LoadFunctions().
//  2020-11-11: Vulkan: Added support for specifying which subpass to reference during vk::Pipeline creation.
//  2020-09-07: Vulkan: Added vk::Pipeline parameter to ImGui_ImplVulkan_RenderDrawData (default to one passed to ImGui_ImplVulkan_Init).
//  2020-05-04: Vulkan: Fixed crash if initial frame has no vertices.
//  2020-04-26: Vulkan: Fixed edge case where render callbacks wouldn't be called if the ImDrawData didn't have vertices.
//  2019-08-01: Vulkan: Added support for specifying multisample count. Set ImGui_ImplVulkan_InitInfo::MSAASamples to one of the vk::SampleCountFlagBits values to use, default is non-multisampled as before.
//  2019-05-29: Vulkan: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: Vulkan: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-04-04: *BREAKING CHANGE*: Vulkan: Added ImageCount/MinImageCount fields in ImGui_ImplVulkan_InitInfo, required for initialization (was previously a hard #define IMGUI_VK_QUEUED_FRAMES 2). Added ImGui_ImplVulkan_SetMinImageCount().
//  2019-04-04: Vulkan: Added vk::Instance argument to ImGui_ImplVulkanH_CreateWindow() optional helper.
//  2019-04-04: Vulkan: Avoid passing negative coordinates to vkCmdSetScissor, which debug validation layers do not like.
//  2019-04-01: Vulkan: Support for 32-bit index buffer (#define ImDrawIdx unsigned int).
//  2019-02-16: Vulkan: Viewport and clipping rectangles correctly using draw_data->FramebufferScale to allow retina display.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-08-25: Vulkan: Fixed mishandled vk::SurfaceCapabilitiesKHR::maxImageCount=0 case.
//  2018-06-22: Inverted the parameters to ImGui_ImplVulkan_RenderDrawData() to be consistent with other backends.
//  2018-06-08: Misc: Extracted imgui_impl_vulkan.cpp/.h away from the old combined GLFW+Vulkan example.
//  2018-06-08: Vulkan: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-03-03: Vulkan: Various refactor, created a couple of ImGui_ImplVulkanH_XXX helper that the example can use and that viewport support will use.
//  2018-03-01: Vulkan: Renamed ImGui_ImplVulkan_Init_Info to ImGui_ImplVulkan_InitInfo and fields to match more closely Vulkan terminology.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback, ImGui_ImplVulkan_Render() calls ImGui_ImplVulkan_RenderDrawData() itself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2017-05-15: Vulkan: Fix scissor offset being negative. Fix new Vulkan validation warnings. Set required depth member for buffer image copy.
//  2016-11-13: Vulkan: Fix validation layer warnings and errors and redeclare gl_PerVertex.
//  2016-10-18: Vulkan: Add location decorators & change to use structs as in/out in glsl, update embedded spv (produced with glslangValidator -x). Null the released resources.
//  2016-08-27: Vulkan: Fix Vulkan example for use when a depth buffer is active.

#include "imgui_impl_vulkan.h"
#include <stdio.h>

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#endif

// Forward Declarations
bool ImGui_ImplVulkan_CreateDeviceObjects();
void ImGui_ImplVulkan_DestroyDeviceObjects();
void ImGui_ImplVulkanH_DestroyFrame(vk::Device device, ImGui_ImplVulkanH_Frame* fd, const vk::AllocationCallbacks* allocator);
void ImGui_ImplVulkanH_DestroyFrameSemaphores(vk::Device device, ImGui_ImplVulkanH_FrameSemaphores* fsd, const vk::AllocationCallbacks* allocator);
void ImGui_ImplVulkanH_DestroyFrameRenderBuffers(vma::Allocator vmaAllocator, ImGui_ImplVulkanH_FrameRenderBuffers* buffers, const vk::AllocationCallbacks* allocator);
void ImGui_ImplVulkanH_DestroyWindowRenderBuffers(vma::Allocator vmaAllocator, ImGui_ImplVulkanH_WindowRenderBuffers* buffers, const vk::AllocationCallbacks* allocator);
void ImGui_ImplVulkanH_DestroyAllViewportsRenderBuffers(vma::Allocator vmaAllocator, const vk::AllocationCallbacks* allocator);
void ImGui_ImplVulkanH_CreateWindowSwapChain(vk::PhysicalDevice physical_device, vk::Device device, ImGui_ImplVulkanH_Window* wd, const vk::AllocationCallbacks* allocator, int w, int h, uint32_t min_image_count);
void ImGui_ImplVulkanH_CreateWindowCommandBuffers(vk::PhysicalDevice physical_device, vk::Device device, ImGui_ImplVulkanH_Window* wd, uint32_t queue_family, const vk::AllocationCallbacks* allocator);

//-----------------------------------------------------------------------------
// SHADERS
//-----------------------------------------------------------------------------

// Forward Declarations
static void ImGui_ImplVulkan_InitPlatformInterface();
static void ImGui_ImplVulkan_ShutdownPlatformInterface();

// glsl_shader.vert, compiled with:
// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
/*
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;
    gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
}
*/
static uint32_t __glsl_shader_vert_spv[] =
{
    0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
};

// glsl_shader.frag, compiled with:
// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
/*
#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
}
*/
static uint32_t __glsl_shader_frag_spv[] =
{
    0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
};

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
// FIXME: multi-context support is not tested and probably dysfunctional in this backend.
static ImGui_ImplVulkan_Data* ImGui_ImplVulkan_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}

static uint32_t ImGui_ImplVulkan_MemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    vk::PhysicalDeviceMemoryProperties prop = v->PhysicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
        if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
            return i;
    return 0xFFFFFFFF; // Unable to find memoryType
}

static void check_vk_result(vk::Result err)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    if (!bd)
        return;
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    if (v->CheckVkResultFn)
        v->CheckVkResultFn(err);
}

static void CreateOrResizeBuffer(vk::Buffer& buffer, vma::Allocation& buffer_memory, vk::DeviceSize& p_buffer_size, size_t new_size, vk::BufferUsageFlagBits usage)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    vk::Result err;
    if (buffer && buffer_memory)
        v->vmaAllocator.destroyBuffer(buffer, buffer_memory);

    vk::DeviceSize vertex_buffer_size_aligned = ((new_size - 1) / bd->BufferMemoryAlignment + 1) * bd->BufferMemoryAlignment;
    vk::BufferCreateInfo buffer_info = {};
    buffer_info.size = vertex_buffer_size_aligned;
    buffer_info.usage = usage;
    buffer_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo alloc_create_info = {};
    alloc_create_info.usage = vma::MemoryUsage::eCpuOnly;

    err = v->vmaAllocator.createBuffer(&buffer_info, &alloc_create_info, &buffer, &buffer_memory, nullptr);
    check_vk_result(err);

    vk::MemoryRequirements req;
    v->Device.getBufferMemoryRequirements(buffer, &req);
    p_buffer_size = req.size;
}

static void ImGui_ImplVulkan_SetupRenderState(ImDrawData* draw_data, vk::Pipeline pipeline, vk::CommandBuffer command_buffer, ImGui_ImplVulkanH_FrameRenderBuffers* rb, int fb_width, int fb_height)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();

    // Bind pipeline:
    {
        command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }

    // Bind Vertex And Index Buffer:
    if (draw_data->TotalVtxCount > 0)
    {
        vk::Buffer vertex_buffers[1] = { rb->VertexBuffer };
        vk::DeviceSize vertex_offset[1] = { 0 };
        command_buffer.bindVertexBuffers(0, 1, vertex_buffers, vertex_offset);
        command_buffer.bindIndexBuffer(rb->IndexBuffer, 0, sizeof(ImDrawIdx) == 2 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);
    }

    // Setup viewport:
    {
        vk::Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (float)fb_width;
        viewport.height = (float)fb_height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        command_buffer.setViewport(0, 1, &viewport);
    }

    // Setup scale and translation:
    // Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    {
        float scale[2];
        scale[0] = 2.0f / draw_data->DisplaySize.x;
        scale[1] = 2.0f / draw_data->DisplaySize.y;
        float translate[2];
        translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
        translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];
        command_buffer.pushConstants(bd->PipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(float) * 0, sizeof(float) * 2, scale);
        command_buffer.pushConstants(bd->PipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(float) * 2, sizeof(float) * 2, translate);
    }
}

// Render function
void ImGui_ImplVulkan_RenderDrawData(ImDrawData* draw_data, vk::CommandBuffer command_buffer, vk::Pipeline pipeline)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    if (!pipeline)
        pipeline = bd->Pipeline;

    // Allocate array to store enough vertex/index buffers. Each unique viewport gets its own storage.
    ImGui_ImplVulkan_ViewportData* viewport_renderer_data = (ImGui_ImplVulkan_ViewportData*)draw_data->OwnerViewport->RendererUserData;
    IM_ASSERT(viewport_renderer_data != NULL);
    ImGui_ImplVulkanH_WindowRenderBuffers* wrb = &viewport_renderer_data->RenderBuffers;
    if (wrb->FrameRenderBuffers == NULL)
    {
        wrb->Index = 0;
        wrb->Count = v->ImageCount;
        wrb->FrameRenderBuffers = (ImGui_ImplVulkanH_FrameRenderBuffers*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameRenderBuffers) * wrb->Count);
        memset(wrb->FrameRenderBuffers, 0, sizeof(ImGui_ImplVulkanH_FrameRenderBuffers) * wrb->Count);
    }
    IM_ASSERT(wrb->Count == v->ImageCount);
    wrb->Index = (wrb->Index + 1) % wrb->Count;
    ImGui_ImplVulkanH_FrameRenderBuffers* rb = &wrb->FrameRenderBuffers[wrb->Index];

    if (draw_data->TotalVtxCount > 0)
    {
        // Create or resize the vertex/index buffers
        size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
        size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
        if (!rb->VertexBuffer || rb->VertexBufferSize < vertex_size)
            CreateOrResizeBuffer(rb->VertexBuffer, rb->VertexBufferMemory, rb->VertexBufferSize, vertex_size, vk::BufferUsageFlagBits::eVertexBuffer);
        if (!rb->IndexBuffer || rb->IndexBufferSize < index_size)
            CreateOrResizeBuffer(rb->IndexBuffer, rb->IndexBufferMemory, rb->IndexBufferSize, index_size, vk::BufferUsageFlagBits::eIndexBuffer);

        // Upload vertex/index data into a single contiguous GPU buffer
        ImDrawVert* vtx_dst = NULL;
        ImDrawIdx* idx_dst = NULL;
        vk::Result err = v->vmaAllocator.mapMemory(rb->VertexBufferMemory, (void**)(&vtx_dst));
        check_vk_result(err);
        err = v->vmaAllocator.mapMemory(rb->IndexBufferMemory, (void**)(&idx_dst));
        check_vk_result(err);
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtx_dst += cmd_list->VtxBuffer.Size;
            idx_dst += cmd_list->IdxBuffer.Size;
        }
        std::array<vma::Allocation, 2> allocations = { rb->VertexBufferMemory, rb->IndexBufferMemory };
        std::array<VkDeviceSize, 2> offsets = { 0, 0 };
        std::array<VkDeviceSize, 2> sizes = { VK_WHOLE_SIZE, VK_WHOLE_SIZE };

        v->vmaAllocator.flushAllocations(allocations, offsets, sizes);
        v->vmaAllocator.unmapMemory(rb->VertexBufferMemory);
        v->vmaAllocator.unmapMemory(rb->IndexBufferMemory);
    }

    // Setup desired Vulkan state
    ImGui_ImplVulkan_SetupRenderState(draw_data, pipeline, command_buffer, rb, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplVulkan_SetupRenderState(draw_data, pipeline, command_buffer, rb, fb_width, fb_height);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
                if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
                if (clip_max.x > fb_width) { clip_max.x = (float)fb_width; }
                if (clip_max.y > fb_height) { clip_max.y = (float)fb_height; }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle
                vk::Rect2D scissor;
                scissor.offset.x = (int32_t)(clip_min.x);
                scissor.offset.y = (int32_t)(clip_min.y);
                scissor.extent.width = (uint32_t)(clip_max.x - clip_min.x);
                scissor.extent.height = (uint32_t)(clip_max.y - clip_min.y);
                command_buffer.setScissor(0, 1, &scissor);

                // Bind DescriptorSet with font or user texture
                vk::DescriptorSet desc_set = static_cast<VkDescriptorSet>(pcmd->TextureId);
                if (sizeof(ImTextureID) < sizeof(ImU64))
                {
                    // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit. Do a flaky check that other textures haven't been used.
                    IM_ASSERT(pcmd->TextureId == (ImTextureID)bd->FontDescriptorSet);
                    desc_set = bd->FontDescriptorSet;
                }
                command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bd->PipelineLayout, 0, 1, &desc_set, 0, NULL);

                // Draw
                command_buffer.drawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
    // Our last values will leak into user/application rendering IF:
    // - Your app uses a pipeline with vk::DynamicState::eViewport or vk::DynamicState::eScissor dynamic state
    // - And you forgot to call vkCmdSetViewport() and vkCmdSetScissor() yourself to explicitely set that state.
    // If you use vk::DynamicState::eViewport or vk::DynamicState::eScissor you are responsible for setting the values before rendering.
    // In theory we should aim to backup/restore those values but I am not sure this is possible.
    // We perform a call to vkCmdSetScissor() to set back a full viewport which is likely to fix things for 99% users but technically this is not perfect. (See github #4644)
    vk::Rect2D scissor = { { 0, 0 }, { (uint32_t)fb_width, (uint32_t)fb_height } };
    command_buffer.setScissor(0, 1, &scissor);
}

bool ImGui_ImplVulkan_CreateFontsTexture(vk::CommandBuffer command_buffer)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    size_t upload_size = width * height * 4 * sizeof(char);

    vk::Result err;

    // Create the Image:
    {
        vk::ImageCreateInfo info = {};
        info.imageType = vk::ImageType::e2D;
        info.format = vk::Format::eR8G8B8A8Unorm;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = vk::SampleCountFlagBits::e1;
        info.tiling = vk::ImageTiling::eOptimal;
        info.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        info.sharingMode = vk::SharingMode::eExclusive;
        info.initialLayout = vk::ImageLayout::eUndefined;

        vma::AllocationCreateInfo alloc_create_info = {};
        alloc_create_info.usage = vma::MemoryUsage::eUnknown; //Device local

        err = v->vmaAllocator.createImage(&info, &alloc_create_info, &bd->FontImage, &bd->FontMemory, nullptr);
        check_vk_result(err);
    }

    // Create the Image View:
    {
        vk::ImageViewCreateInfo info = {};
        info.image = bd->FontImage;
        info.viewType = vk::ImageViewType::e2D;
        info.format = vk::Format::eR8G8B8A8Unorm;
        info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.layerCount = 1;
        err = v->Device.createImageView(&info, v->Allocator, &bd->FontView);
        check_vk_result(err);
    }

    // Create the Descriptor Set:
    bd->FontDescriptorSet = (vk::DescriptorSet)ImGui_ImplVulkan_AddTexture(bd->FontSampler, bd->FontView, vk::ImageLayout::eShaderReadOnlyOptimal);

    // Create the Upload Buffer:
    {
        vk::BufferCreateInfo buffer_info = {};
        buffer_info.size = upload_size;
        buffer_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
        buffer_info.sharingMode = vk::SharingMode::eExclusive;

        vma::AllocationCreateInfo alloc_create_info = {};
        alloc_create_info.usage = vma::MemoryUsage::eCpuOnly;

        err = v->vmaAllocator.createBuffer(&buffer_info, &alloc_create_info, &bd->UploadBuffer, &bd->UploadBufferMemory, nullptr);
        check_vk_result(err);

        vk::MemoryRequirements req;
        v->Device.getBufferMemoryRequirements(bd->UploadBuffer, &req);
        bd->BufferMemoryAlignment = (bd->BufferMemoryAlignment > req.alignment) ? bd->BufferMemoryAlignment : req.alignment;
        check_vk_result(err);
    }

    // Upload to Buffer:
    {
        char* map = NULL;
        err = v->vmaAllocator.mapMemory(bd->UploadBufferMemory, (void**)(&map));
        check_vk_result(err);
        memcpy(map, pixels, upload_size);
        
        v->vmaAllocator.flushAllocation(bd->UploadBufferMemory, 0, upload_size);
        v->vmaAllocator.unmapMemory(bd->UploadBufferMemory);
    }

    // Copy to Image:
    {
        std::array<vk::ImageMemoryBarrier, 1> copy_barrier{};
        copy_barrier[0].dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        copy_barrier[0].oldLayout = vk::ImageLayout::eUndefined;
        copy_barrier[0].newLayout = vk::ImageLayout::eTransferDstOptimal;
        copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].image = bd->FontImage;
        copy_barrier[0].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        copy_barrier[0].subresourceRange.levelCount = 1;
        copy_barrier[0].subresourceRange.layerCount = 1;
        command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags{}, 0, nullptr, 0, nullptr, copy_barrier.size(), copy_barrier.data());

        vk::BufferImageCopy region = {};
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = width;
        region.imageExtent.height = height;
        region.imageExtent.depth = 1;
        command_buffer.copyBufferToImage(bd->UploadBuffer, bd->FontImage, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        std::array<vk::ImageMemoryBarrier, 1> use_barrier{};
        use_barrier[0].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        use_barrier[0].dstAccessMask = vk::AccessFlagBits::eShaderRead;
        use_barrier[0].oldLayout = vk::ImageLayout::eTransferDstOptimal;
        use_barrier[0].newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].image = bd->FontImage;
        use_barrier[0].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        use_barrier[0].subresourceRange.levelCount = 1;
        use_barrier[0].subresourceRange.layerCount = 1;
        command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, use_barrier.data());
    }

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)bd->FontDescriptorSet);

    return true;
}

static void ImGui_ImplVulkan_CreateShaderModules(vk::Device device, const vk::AllocationCallbacks* allocator)
{
    // Create the shader modules
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    if (!bd->ShaderModuleVert)
    {
        vk::ShaderModuleCreateInfo vert_info = {};
        vert_info.codeSize = sizeof(__glsl_shader_vert_spv);
        vert_info.pCode = (uint32_t*)__glsl_shader_vert_spv;
        vk::Result err = device.createShaderModule(&vert_info, allocator, &bd->ShaderModuleVert);
        check_vk_result(err);
    }
    if (!bd->ShaderModuleFrag)
    {
        vk::ShaderModuleCreateInfo frag_info = {};
        frag_info.codeSize = sizeof(__glsl_shader_frag_spv);
        frag_info.pCode = (uint32_t*)__glsl_shader_frag_spv;
        vk::Result err = device.createShaderModule(&frag_info, allocator, &bd->ShaderModuleFrag);
        check_vk_result(err);
    }
}

static void ImGui_ImplVulkan_CreateFontSampler(vk::Device device, const vk::AllocationCallbacks* allocator)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    if (bd->FontSampler)
        return;

    // Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling.
    vk::SamplerCreateInfo info = {};
    info.magFilter = vk::Filter::eLinear;
    info.minFilter = vk::Filter::eLinear;
    info.mipmapMode = vk::SamplerMipmapMode::eLinear;
    info.addressModeU = vk::SamplerAddressMode::eRepeat;
    info.addressModeV = vk::SamplerAddressMode::eRepeat;
    info.addressModeW = vk::SamplerAddressMode::eRepeat;
    info.minLod = -1000;
    info.maxLod = 1000;
    info.maxAnisotropy = 1.0f;
    vk::Result err = device.createSampler(&info, allocator, &bd->FontSampler);
    check_vk_result(err);
}

static void ImGui_ImplVulkan_CreateDescriptorSetLayout(vk::Device device, const vk::AllocationCallbacks* allocator)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    if (bd->DescriptorSetLayout)
        return;

    ImGui_ImplVulkan_CreateFontSampler(device, allocator);
    std::array<vk::Sampler, 1> sampler = { bd->FontSampler };
    std::array<vk::DescriptorSetLayoutBinding, 1> binding{};
    binding[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    binding[0].descriptorCount = 1;
    binding[0].stageFlags = vk::ShaderStageFlagBits::eFragment;
    binding[0].pImmutableSamplers = sampler.data();
    vk::DescriptorSetLayoutCreateInfo info = {};
    info.bindingCount = binding.size();
    info.pBindings = binding.data();
    vk::Result err = device.createDescriptorSetLayout(&info, allocator, &bd->DescriptorSetLayout);
    check_vk_result(err);
}

static void ImGui_ImplVulkan_CreatePipelineLayout(vk::Device device, const vk::AllocationCallbacks* allocator)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    if (bd->PipelineLayout)
        return;

    // Constants: we are using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix
    ImGui_ImplVulkan_CreateDescriptorSetLayout(device, allocator);
    std::array<vk::PushConstantRange, 1> push_constants{};
    push_constants[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
    push_constants[0].offset = sizeof(float) * 0;
    push_constants[0].size = sizeof(float) * 4;
    std::array<vk::DescriptorSetLayout, 1> set_layout = { bd->DescriptorSetLayout };
    vk::PipelineLayoutCreateInfo layout_info = {};
    layout_info.setLayoutCount = set_layout.size();
    layout_info.pSetLayouts = set_layout.data();
    layout_info.pushConstantRangeCount = push_constants.size();
    layout_info.pPushConstantRanges = push_constants.data();
    vk::Result  err = device.createPipelineLayout(&layout_info, allocator, &bd->PipelineLayout);
    check_vk_result(err);
}

static void ImGui_ImplVulkan_CreatePipeline(vk::Device device, const vk::AllocationCallbacks* allocator, vk::PipelineCache pipelineCache, vk::RenderPass renderPass, vk::SampleCountFlagBits MSAASamples, vk::Pipeline* pipeline, uint32_t subpass)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_CreateShaderModules(device, allocator);

    std::array<vk::PipelineShaderStageCreateInfo, 2> stage{};
    stage[0].stage = vk::ShaderStageFlagBits::eVertex;
    stage[0].module = bd->ShaderModuleVert;
    stage[0].pName = "main";
    stage[1].stage = vk::ShaderStageFlagBits::eFragment;
    stage[1].module = bd->ShaderModuleFrag;
    stage[1].pName = "main";

    std::array<vk::VertexInputBindingDescription, 1> binding_desc{};
    binding_desc[0].stride = sizeof(ImDrawVert);
    binding_desc[0].inputRate = vk::VertexInputRate::eVertex;

    std::array<vk::VertexInputAttributeDescription, 3> attribute_desc{};
    attribute_desc[0].location = 0;
    attribute_desc[0].binding = binding_desc[0].binding;
    attribute_desc[0].format = vk::Format::eR32G32Sfloat;
    attribute_desc[0].offset = IM_OFFSETOF(ImDrawVert, pos);
    attribute_desc[1].location = 1;
    attribute_desc[1].binding = binding_desc[0].binding;
    attribute_desc[1].format = vk::Format::eR32G32Sfloat;
    attribute_desc[1].offset = IM_OFFSETOF(ImDrawVert, uv);
    attribute_desc[2].location = 2;
    attribute_desc[2].binding = binding_desc[0].binding;
    attribute_desc[2].format = vk::Format::eR8G8B8A8Unorm; 
    attribute_desc[2].offset = IM_OFFSETOF(ImDrawVert, col);

    vk::PipelineVertexInputStateCreateInfo vertex_info = {};
    vertex_info.vertexBindingDescriptionCount = binding_desc.size();
    vertex_info.pVertexBindingDescriptions = binding_desc.data();
    vertex_info.vertexAttributeDescriptionCount = attribute_desc.size();
    vertex_info.pVertexAttributeDescriptions = attribute_desc.data();

    vk::PipelineInputAssemblyStateCreateInfo ia_info = {};
    ia_info.topology = vk::PrimitiveTopology::eTriangleList;

    vk::PipelineViewportStateCreateInfo viewport_info = {};
    viewport_info.viewportCount = 1;
    viewport_info.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo raster_info = {};
    raster_info.polygonMode = vk::PolygonMode::eFill;
    raster_info.cullMode = vk::CullModeFlagBits::eNone;
    raster_info.frontFace = vk::FrontFace::eCounterClockwise;
    raster_info.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo ms_info = {};
    ms_info.rasterizationSamples = (MSAASamples != vk::SampleCountFlagBits::e1) ? MSAASamples : vk::SampleCountFlagBits::e1;

    std::array<vk::PipelineColorBlendAttachmentState, 1> color_attachment{};
    color_attachment[0].blendEnable = VK_TRUE;
    color_attachment[0].srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    color_attachment[0].dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    color_attachment[0].colorBlendOp = vk::BlendOp::eAdd;
    color_attachment[0].srcAlphaBlendFactor = vk::BlendFactor::eOne;
    color_attachment[0].dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    color_attachment[0].alphaBlendOp = vk::BlendOp::eAdd;
    color_attachment[0].colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineDepthStencilStateCreateInfo depth_info = {};

    vk::PipelineColorBlendStateCreateInfo blend_info = {};
    blend_info.attachmentCount = color_attachment.size();
    blend_info.pAttachments = color_attachment.data();

    std::array<vk::DynamicState, 2> dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.dynamicStateCount = dynamic_states.size();
    dynamic_state.pDynamicStates = dynamic_states.data();

    ImGui_ImplVulkan_CreatePipelineLayout(device, allocator);

    vk::GraphicsPipelineCreateInfo info = {};
    info.flags = bd->PipelineCreateFlags;
    info.stageCount = stage.size();
    info.pStages = stage.data();
    info.pVertexInputState = &vertex_info;
    info.pInputAssemblyState = &ia_info;
    info.pViewportState = &viewport_info;
    info.pRasterizationState = &raster_info;
    info.pMultisampleState = &ms_info;
    info.pDepthStencilState = &depth_info;
    info.pColorBlendState = &blend_info;
    info.pDynamicState = &dynamic_state;
    info.layout = bd->PipelineLayout;
    info.renderPass = renderPass;
    info.subpass = subpass;
    vk::Result err = device.createGraphicsPipelines(pipelineCache, 1, &info, allocator, pipeline);
    check_vk_result(err);
}

bool ImGui_ImplVulkan_CreateDeviceObjects()
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    vk::Result err;

    if (!bd->FontSampler)
    {
        vk::SamplerCreateInfo info = {};
        info.magFilter = vk::Filter::eLinear;
        info.minFilter = vk::Filter::eLinear;
        info.mipmapMode = vk::SamplerMipmapMode::eLinear;
        info.addressModeU = vk::SamplerAddressMode::eRepeat;
        info.addressModeV = vk::SamplerAddressMode::eRepeat;
        info.addressModeW = vk::SamplerAddressMode::eRepeat;
        info.minLod = -1000;
        info.maxLod = 1000;
        info.maxAnisotropy = 1.0f;
        err = v->Device.createSampler(&info, v->Allocator, &bd->FontSampler);
        check_vk_result(err);
    }

    if (!bd->DescriptorSetLayout)
    {
        std::array<vk::Sampler, 1> sampler = {bd->FontSampler};
        std::array<vk::DescriptorSetLayoutBinding, 1> binding = {};
        binding[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        binding[0].descriptorCount = sampler.size();
        binding[0].stageFlags = vk::ShaderStageFlagBits::eFragment;
        binding[0].pImmutableSamplers = sampler.data();
        vk::DescriptorSetLayoutCreateInfo info = {};
        info.bindingCount = binding.size();
        info.pBindings = binding.data();
        err = v->Device.createDescriptorSetLayout(&info, v->Allocator, &bd->DescriptorSetLayout);
        check_vk_result(err);
    }

    if (!bd->PipelineLayout)
    {
        // Constants: we are using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix
        std::array<vk::PushConstantRange, 1> push_constants = {};
        push_constants[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
        push_constants[0].offset = sizeof(float) * 0;
        push_constants[0].size = sizeof(float) * 4;
        std::array<vk::DescriptorSetLayout, 1> set_layout = { bd->DescriptorSetLayout };
        vk::PipelineLayoutCreateInfo layout_info = {};
        layout_info.setLayoutCount = set_layout.size();
        layout_info.pSetLayouts = set_layout.data();
        layout_info.pushConstantRangeCount = push_constants.size();
        layout_info.pPushConstantRanges = push_constants.data();
        err = v->Device.createPipelineLayout(&layout_info, v->Allocator, &bd->PipelineLayout);
        check_vk_result(err);
    }

    ImGui_ImplVulkan_CreatePipeline(v->Device, v->Allocator, v->PipelineCache, bd->RenderPass, v->MSAASamples, &bd->Pipeline, bd->Subpass);

    return true;
}

void    ImGui_ImplVulkan_DestroyFontUploadObjects()
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    if (bd->UploadBuffer && bd->UploadBufferMemory)
    {
        v->vmaAllocator.destroyBuffer(bd->UploadBuffer, bd->UploadBufferMemory);
        bd->UploadBuffer = VK_NULL_HANDLE;
        bd->UploadBufferMemory = VK_NULL_HANDLE;
    }
}

void    ImGui_ImplVulkan_DestroyDeviceObjects()
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    ImGui_ImplVulkanH_DestroyAllViewportsRenderBuffers(v->vmaAllocator, v->Allocator);
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    if (bd->ShaderModuleVert)     { v->Device.destroyShaderModule(bd->ShaderModuleVert, v->Allocator); bd->ShaderModuleVert = VK_NULL_HANDLE; }
    if (bd->ShaderModuleFrag)     {  v->Device.destroyShaderModule(bd->ShaderModuleFrag, v->Allocator); bd->ShaderModuleFrag = VK_NULL_HANDLE; }
    if (bd->FontView)             {  v->Device.destroyImageView(bd->FontView, v->Allocator); bd->FontView = VK_NULL_HANDLE; }

    if (bd->FontImage && bd->FontMemory) 
    { 
        v->vmaAllocator.destroyImage(bd->FontImage, bd->FontMemory);
        bd->FontImage = VK_NULL_HANDLE; 
        bd->FontMemory = VK_NULL_HANDLE;
    }

    if (bd->FontSampler)          {  v->Device.destroySampler(bd->FontSampler, v->Allocator); bd->FontSampler = VK_NULL_HANDLE; }
    if (bd->DescriptorSetLayout)  {  v->Device.destroyDescriptorSetLayout(bd->DescriptorSetLayout, v->Allocator); bd->DescriptorSetLayout = VK_NULL_HANDLE; }
    if (bd->PipelineLayout)       {  v->Device.destroyPipelineLayout(bd->PipelineLayout, v->Allocator); bd->PipelineLayout = VK_NULL_HANDLE; }
    if (bd->Pipeline)             {  v->Device.destroyPipeline(bd->Pipeline, v->Allocator); bd->Pipeline = VK_NULL_HANDLE; }
}

bool    ImGui_ImplVulkan_LoadFunctions(PFN_vkVoidFunction(*loader_func)(const char* function_name, void* user_data), void* user_data)
{
    // Load function pointers
    // You can use the default Vulkan loader using:
    //      ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void*) { return vkGetInstanceProcAddr(your_vk_isntance, function_name); });
    // But this would be equivalent to not setting VK_NO_PROTOTYPES.
    return true;
}

bool    ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info, vk::RenderPass render_pass)
{

    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == NULL && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    ImGui_ImplVulkan_Data* bd = IM_NEW(ImGui_ImplVulkan_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_vulkan";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    IM_ASSERT(info->Instance);
    IM_ASSERT(info->PhysicalDevice);
    IM_ASSERT(info->Device);
    IM_ASSERT(info->Queue);
    IM_ASSERT(info->DescriptorPool);
    IM_ASSERT(info->MinImageCount >= 2);
    IM_ASSERT(info->ImageCount >= info->MinImageCount);
    IM_ASSERT(info->vmaAllocator);
    IM_ASSERT(render_pass);

    bd->VulkanInitInfo = *info;
    bd->RenderPass = render_pass;
    bd->Subpass = info->Subpass;

    ImGui_ImplVulkan_CreateDeviceObjects();

    // Our render function expect RendererUserData to be storing the window render buffer we need (for the main viewport we won't use ->Window)
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->RendererUserData = IM_NEW(ImGui_ImplVulkan_ViewportData)();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplVulkan_InitPlatformInterface();

    return true;
}

void ImGui_ImplVulkan_Shutdown()
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    IM_ASSERT(bd != NULL && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    // First destroy objects in all viewports
    ImGui_ImplVulkan_DestroyDeviceObjects();

    // Manually delete main viewport render data in-case we haven't initialized for viewports
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    if (ImGui_ImplVulkan_ViewportData* vd = (ImGui_ImplVulkan_ViewportData*)main_viewport->RendererUserData)
        IM_DELETE(vd);
    main_viewport->RendererUserData = NULL;

    // Clean up windows
    ImGui_ImplVulkan_ShutdownPlatformInterface();

    io.BackendRendererName = NULL;
    io.BackendRendererUserData = NULL;
    IM_DELETE(bd);
}

void ImGui_ImplVulkan_NewFrame()
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplVulkan_Init()?");
    IM_UNUSED(bd);
}

void ImGui_ImplVulkan_SetMinImageCount(uint32_t min_image_count)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    IM_ASSERT(min_image_count >= 2);
    if (bd->VulkanInitInfo.MinImageCount == min_image_count)
        return;

    IM_ASSERT(0); // FIXME-VIEWPORT: Unsupported. Need to recreate all swap chains!
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    v->Device.waitIdle();
    ImGui_ImplVulkanH_DestroyAllViewportsRenderBuffers(v->vmaAllocator, v->Allocator);

    bd->VulkanInitInfo.MinImageCount = min_image_count;
}

// Register a texture
// FIXME: This is experimental in the sense that we are unsure how to best design/tackle this problem, please post to https://github.com/ocornut/imgui/pull/914 if you have suggestions.
vk::DescriptorSet ImGui_ImplVulkan_AddTexture(vk::Sampler sampler, vk::ImageView image_view, vk::ImageLayout image_layout)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;

    // Create Descriptor Set:
    vk::DescriptorSet descriptor_set;
    {
        vk::DescriptorSetAllocateInfo alloc_info = {};
        alloc_info.descriptorPool = v->DescriptorPool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &bd->DescriptorSetLayout;
        vk::Result err = v->Device.allocateDescriptorSets(&alloc_info, &descriptor_set);
        check_vk_result(err);
    }

    // Update the Descriptor Set:
    {
        std::array<vk::DescriptorImageInfo, 1> desc_image{};
        desc_image[0].sampler = sampler;
        desc_image[0].imageView = image_view;
        desc_image[0].imageLayout = image_layout;
        std::array<vk::WriteDescriptorSet, 1> write_desc{};
        write_desc[0].dstSet = descriptor_set;
        write_desc[0].descriptorCount = desc_image.size();
        write_desc[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        write_desc[0].pImageInfo = desc_image.data();
        v->Device.updateDescriptorSets(write_desc.size(), write_desc.data(), 0, NULL);
    }
    return descriptor_set;
}

//-------------------------------------------------------------------------
// Internal / Miscellaneous Vulkan Helpers
// (Used by example's main.cpp. Used by multi-viewport features. PROBABLY NOT used by your own app.)
//-------------------------------------------------------------------------
// You probably do NOT need to use or care about those functions.
// Those functions only exist because:
//   1) they facilitate the readability and maintenance of the multiple main.cpp examples files.
//   2) the upcoming multi-viewport feature will need them internally.
// Generally we avoid exposing any kind of superfluous high-level helpers in the backends,
// but it is too much code to duplicate everywhere so we exceptionally expose them.
//
// Your engine/app will likely _already_ have code to setup all that stuff (swap chain, render pass, frame buffers, etc.).
// You may read this code to learn about Vulkan, but it is recommended you use you own custom tailored code to do equivalent work.
// (The ImGui_ImplVulkanH_XXX functions do not interact with any of the state used by the regular ImGui_ImplVulkan_XXX functions)
//-------------------------------------------------------------------------

vk::SurfaceFormatKHR ImGui_ImplVulkanH_SelectSurfaceFormat(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface, const vk::Format* request_formats, int request_formats_count, vk::ColorSpaceKHR request_color_space)
{
    IM_ASSERT(request_formats != NULL);
    IM_ASSERT(request_formats_count > 0);

    // Per Spec Format and View Format are expected to be the same unless VK_IMAGE_CREATE_MUTABLE_BIT was set at image creation
    // Assuming that the default behavior is without setting this bit, there is no need for separate Swapchain image and image view format
    // Additionally several new color spaces were introduced with Vulkan Spec v1.0.40,
    // hence we must make sure that a format with the mostly available color space, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is found and used.

    std::vector<vk::SurfaceFormatKHR> avail_format;
    avail_format = physical_device.getSurfaceFormatsKHR(surface);

    // First check if only one format, vk::Format::eUndefined, is available, which would imply that any format is available
    if (avail_format.size() == 1)
    {
        if (avail_format[0].format == vk::Format::eUndefined)
        {
            vk::SurfaceFormatKHR ret;
            ret.format = request_formats[0];
            ret.colorSpace = request_color_space;
            return ret;
        }
        else
        {
            // No point in searching another format
            return avail_format[0];
        }
    }
    else
    {
        // Request several formats, the first found will be used
        for (int request_i = 0; request_i < request_formats_count; request_i++)
            for (uint32_t avail_i = 0; avail_i < avail_format.size(); avail_i++)
                if (avail_format[avail_i].format == request_formats[request_i] && avail_format[avail_i].colorSpace == request_color_space)
                    return avail_format[avail_i];

        // If none of the requested image formats could be found, use the first available
        return avail_format[0];
    }
}

vk::PresentModeKHR ImGui_ImplVulkanH_SelectPresentMode(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface, const vk::PresentModeKHR* request_modes, int request_modes_count)
{
    IM_ASSERT(request_modes != NULL);
    IM_ASSERT(request_modes_count > 0);

    // Request a certain mode and confirm that it is available. If not use vk::PresentModeKHR::eFifo which is mandatory
    std::vector<vk::PresentModeKHR> avail_modes;
    avail_modes = physical_device.getSurfacePresentModesKHR(surface);
    //for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
    //    printf("[vulkan] avail_modes[%d] = %d\n", avail_i, avail_modes[avail_i]);

    for (int request_i = 0; request_i < request_modes_count; request_i++)
        for (uint32_t avail_i = 0; avail_i < avail_modes.size(); avail_i++)
            if (request_modes[request_i] == avail_modes[avail_i])
                return request_modes[request_i];

    return vk::PresentModeKHR::eFifo; // Always available
}

void ImGui_ImplVulkanH_CreateWindowCommandBuffers(vk::PhysicalDevice physical_device, vk::Device device, ImGui_ImplVulkanH_Window* wd, uint32_t queue_family, const vk::AllocationCallbacks* allocator)
{
    IM_ASSERT(physical_device && device);
    (void)physical_device;
    (void)allocator;

    // Create Command Buffers
    vk::Result err;
    for (uint32_t i = 0; i < wd->ImageCount; i++)
    {
        ImGui_ImplVulkanH_Frame* fd = &wd->Frames[i];
        ImGui_ImplVulkanH_FrameSemaphores* fsd = &wd->FrameSemaphores[i];
        {
            vk::CommandPoolCreateInfo info = {};
            info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
            info.queueFamilyIndex = queue_family;
            err = device.createCommandPool(&info, allocator, &fd->CommandPool);
            check_vk_result(err);
        }
        {
            vk::CommandBufferAllocateInfo info = {};
            info.commandPool = fd->CommandPool;
            info.level = vk::CommandBufferLevel::ePrimary;
            info.commandBufferCount = 1;
            err = device.allocateCommandBuffers(&info, &fd->CommandBuffer);
            check_vk_result(err);
        }
        {
            vk::FenceCreateInfo info = {};
            info.flags = vk::FenceCreateFlagBits::eSignaled;
            err = device.createFence(&info, allocator, &fd->Fence);
            check_vk_result(err);
        }
        {
            vk::SemaphoreCreateInfo info = {};
            err = device.createSemaphore(&info, allocator, &fsd->ImageAcquiredSemaphore);
            check_vk_result(err);
            err = device.createSemaphore(&info, allocator, &fsd->RenderCompleteSemaphore);
            check_vk_result(err);
        }
    }
}

int ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(vk::PresentModeKHR present_mode)
{
    if (present_mode == vk::PresentModeKHR::eMailbox)
        return 3;
    if (present_mode == vk::PresentModeKHR::eFifo || present_mode == vk::PresentModeKHR::eFifoRelaxed)
        return 2;
    if (present_mode == vk::PresentModeKHR::eImmediate)
        return 1;
    IM_ASSERT(0);
    return 1;
}

// Also destroy old swap chain and in-flight frames data, if any.
void ImGui_ImplVulkanH_CreateWindowSwapChain(vk::PhysicalDevice physical_device, vk::Device device, ImGui_ImplVulkanH_Window* wd, const vk::AllocationCallbacks* allocator, int w, int h, uint32_t min_image_count)
{
    vk::Result err;
    vk::SwapchainKHR old_swapchain = wd->Swapchain;
    wd->Swapchain = VK_NULL_HANDLE;
    device.waitIdle();

    // We don't use ImGui_ImplVulkanH_DestroyWindow() because we want to preserve the old swapchain to create the new one.
    // Destroy old Framebuffer
    for (uint32_t i = 0; i < wd->ImageCount; i++)
    {
        ImGui_ImplVulkanH_DestroyFrame(device, &wd->Frames[i], allocator);
        ImGui_ImplVulkanH_DestroyFrameSemaphores(device, &wd->FrameSemaphores[i], allocator);
    }
    IM_FREE(wd->Frames);
    IM_FREE(wd->FrameSemaphores);
    wd->Frames = NULL;
    wd->FrameSemaphores = NULL;
    wd->ImageCount = 0;
    if (wd->RenderPass)
        device.destroyRenderPass(wd->RenderPass, *allocator);
    if (wd->Pipeline)
        device.destroyPipeline(wd->Pipeline, *allocator);

    // If min image count was not specified, request different count of images dependent on selected present mode
    if (min_image_count == 0)
        min_image_count = ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(wd->PresentMode);

    // Create Swapchain
    {
        vk::SwapchainCreateInfoKHR info = {};
        info.surface = wd->Surface;
        info.minImageCount = min_image_count;
        info.imageFormat = wd->SurfaceFormat.format;
        info.imageColorSpace = wd->SurfaceFormat.colorSpace;
        info.imageArrayLayers = 1;
        info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        info.imageSharingMode = vk::SharingMode::eExclusive;           // Assume that graphics family == present family
        info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        info.presentMode = wd->PresentMode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = old_swapchain;
        vk::SurfaceCapabilitiesKHR cap = physical_device.getSurfaceCapabilitiesKHR(wd->Surface);

        if (info.minImageCount < cap.minImageCount)
            info.minImageCount = cap.minImageCount;
        else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
            info.minImageCount = cap.maxImageCount;

        if (cap.currentExtent.width == 0xffffffff)
        {
            info.imageExtent.width = wd->Width = w;
            info.imageExtent.height = wd->Height = h;
        }
        else
        {
            info.imageExtent.width = wd->Width = cap.currentExtent.width;
            info.imageExtent.height = wd->Height = cap.currentExtent.height;
        }
        err = device.createSwapchainKHR(&info, allocator, &wd->Swapchain);
        check_vk_result(err);
        err = device.getSwapchainImagesKHR(wd->Swapchain, &wd->ImageCount, NULL);
        check_vk_result(err);
        std::array<vk::Image, 16> backbuffers{};
        IM_ASSERT(wd->ImageCount >= min_image_count);
        IM_ASSERT(wd->ImageCount < backbuffers.size());
        err = device.getSwapchainImagesKHR(wd->Swapchain, &wd->ImageCount, backbuffers.data());
        check_vk_result(err);

        IM_ASSERT(wd->Frames == NULL);
        wd->Frames = (ImGui_ImplVulkanH_Frame*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_Frame) * wd->ImageCount);
        wd->FrameSemaphores = (ImGui_ImplVulkanH_FrameSemaphores*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameSemaphores) * wd->ImageCount);
        memset(wd->Frames, 0, sizeof(wd->Frames[0]) * wd->ImageCount);
        memset(wd->FrameSemaphores, 0, sizeof(wd->FrameSemaphores[0]) * wd->ImageCount);
        for (uint32_t i = 0; i < wd->ImageCount; i++)
            wd->Frames[i].Backbuffer = backbuffers[i];
    }
    if (old_swapchain)
        device.destroySwapchainKHR(old_swapchain, allocator);

    // Create the Render Pass
    {
        vk::AttachmentDescription attachment = {};
        attachment.format = wd->SurfaceFormat.format;
        attachment.samples = vk::SampleCountFlagBits::e1;
        attachment.loadOp = wd->ClearEnable ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
        attachment.storeOp = vk::AttachmentStoreOp::eStore;
        attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachment.initialLayout = vk::ImageLayout::eUndefined;
        attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        vk::AttachmentReference color_attachment = {};
        color_attachment.attachment = 0;
        color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment;
        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
        dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dependency.dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;
        vk::RenderPassCreateInfo info = {};
        info.attachmentCount = 1;
        info.pAttachments = &attachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;
        err = device.createRenderPass(&info, allocator, &wd->RenderPass);
        check_vk_result(err);
        
        // We do not create a pipeline by default as this is also used by examples' main.cpp,
        // but secondary viewport in multi-viewport mode may want to create one with:
        //ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, vk::SampleCountFlagBits::e1, &wd->Pipeline, bd->Subpass);
    }

    // Create The Image Views
    {
        vk::ImageViewCreateInfo info = {};
        info.viewType = vk::ImageViewType::e2D;
        info.format = wd->SurfaceFormat.format;
        info.components.r = vk::ComponentSwizzle::eR;
        info.components.g = vk::ComponentSwizzle::eG;
        info.components.b = vk::ComponentSwizzle::eB;
        info.components.a = vk::ComponentSwizzle::eA;
        vk::ImageSubresourceRange image_range = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
        info.subresourceRange = image_range;
        for (uint32_t i = 0; i < wd->ImageCount; i++)
        {
            ImGui_ImplVulkanH_Frame* fd = &wd->Frames[i];
            info.image = fd->Backbuffer;
            err = device.createImageView(&info, allocator, &fd->BackbufferView);
            check_vk_result(err);
        }
    }

    // Create Framebuffer
    {
        vk::ImageView attachment[1];
        vk::FramebufferCreateInfo info = {};
        info.renderPass = wd->RenderPass;
        info.attachmentCount = 1;
        info.pAttachments = attachment;
        info.width = wd->Width;
        info.height = wd->Height;
        info.layers = 1;
        for (uint32_t i = 0; i < wd->ImageCount; i++)
        {
            ImGui_ImplVulkanH_Frame* fd = &wd->Frames[i];
            attachment[0] = fd->BackbufferView;
            err = device.createFramebuffer(&info, allocator, &fd->Framebuffer);
            check_vk_result(err);
        }
    }
}

// Create or resize window
void ImGui_ImplVulkanH_CreateOrResizeWindow(vk::Instance instance, vk::PhysicalDevice physical_device, vk::Device device, ImGui_ImplVulkanH_Window* wd, uint32_t queue_family, const vk::AllocationCallbacks* allocator, int width, int height, uint32_t min_image_count)
{
    (void)instance;
    ImGui_ImplVulkanH_CreateWindowSwapChain(physical_device, device, wd, allocator, width, height, min_image_count);
    //ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, vk::SampleCountFlagBits::e1, &wd->Pipeline, g_VulkanInitInfo.Subpass);
    ImGui_ImplVulkanH_CreateWindowCommandBuffers(physical_device, device, wd, queue_family, allocator);
}

void ImGui_ImplVulkanH_DestroyWindow(vk::Instance instance, vk::Device device, ImGui_ImplVulkanH_Window* wd, const vk::AllocationCallbacks* allocator)
{
    device.waitIdle(); // FIXME: We could wait on the Queue if we had the queue in wd-> (otherwise VulkanH functions can't use globals)
    //vkQueueWaitIdle(bd->Queue);

    for (uint32_t i = 0; i < wd->ImageCount; i++)
    {
        ImGui_ImplVulkanH_DestroyFrame(device, &wd->Frames[i], allocator);
        ImGui_ImplVulkanH_DestroyFrameSemaphores(device, &wd->FrameSemaphores[i], allocator);
    }
    IM_FREE(wd->Frames);
    IM_FREE(wd->FrameSemaphores);
    wd->Frames = NULL;
    wd->FrameSemaphores = NULL;
    device.destroyPipeline(wd->Pipeline, allocator);
    device.destroyRenderPass(wd->RenderPass, allocator);
    device.destroySwapchainKHR(wd->Swapchain, allocator);
    instance.destroySurfaceKHR(wd->Surface, allocator);

    *wd = ImGui_ImplVulkanH_Window();
}

void ImGui_ImplVulkanH_DestroyFrame(vk::Device device, ImGui_ImplVulkanH_Frame* fd, const vk::AllocationCallbacks* allocator)
{
    device.destroyFence(fd->Fence, allocator);
    device.freeCommandBuffers(fd->CommandPool, 1, &fd->CommandBuffer);
    device.destroyCommandPool(fd->CommandPool, allocator);
    fd->Fence = VK_NULL_HANDLE;
    fd->CommandBuffer = VK_NULL_HANDLE;
    fd->CommandPool = VK_NULL_HANDLE;

    device.destroyImageView(fd->BackbufferView, allocator);
    device.destroyFramebuffer(fd->Framebuffer, allocator);
}

void ImGui_ImplVulkanH_DestroyFrameSemaphores(vk::Device device, ImGui_ImplVulkanH_FrameSemaphores* fsd, const vk::AllocationCallbacks* allocator)
{
    device.destroySemaphore(fsd->ImageAcquiredSemaphore, allocator);
    device.destroySemaphore(fsd->RenderCompleteSemaphore, allocator);
}

void ImGui_ImplVulkanH_DestroyFrameRenderBuffers(vma::Allocator vmaAllocator, ImGui_ImplVulkanH_FrameRenderBuffers* buffers, const vk::AllocationCallbacks* allocator)
{
    if (buffers->VertexBuffer && buffers->VertexBufferMemory)
    { 
        vmaAllocator.destroyBuffer(buffers->VertexBuffer, buffers->VertexBufferMemory);
        buffers->VertexBuffer = VK_NULL_HANDLE; 
        buffers->VertexBufferMemory = VK_NULL_HANDLE;
    }
    
    if (buffers->IndexBuffer && buffers->IndexBufferMemory)
    { 
        vmaAllocator.destroyBuffer(buffers->IndexBuffer, buffers->IndexBufferMemory);
        buffers->IndexBuffer = VK_NULL_HANDLE; 
        buffers->IndexBufferMemory = VK_NULL_HANDLE;
    }

    buffers->VertexBufferSize = 0;
    buffers->IndexBufferSize = 0;
}

void ImGui_ImplVulkanH_DestroyWindowRenderBuffers(vma::Allocator vmaAllocator, ImGui_ImplVulkanH_WindowRenderBuffers* buffers, const vk::AllocationCallbacks* allocator)
{
    for (uint32_t n = 0; n < buffers->Count; n++)
        ImGui_ImplVulkanH_DestroyFrameRenderBuffers(vmaAllocator, &buffers->FrameRenderBuffers[n], allocator);
    IM_FREE(buffers->FrameRenderBuffers);
    buffers->FrameRenderBuffers = NULL;
    buffers->Index = 0;
    buffers->Count = 0;
}

void ImGui_ImplVulkanH_DestroyAllViewportsRenderBuffers(vma::Allocator vmaAllocator, const vk::AllocationCallbacks* allocator)
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int n = 0; n < platform_io.Viewports.Size; n++)
        if (ImGui_ImplVulkan_ViewportData* vd = (ImGui_ImplVulkan_ViewportData*)platform_io.Viewports[n]->RendererUserData)
            ImGui_ImplVulkanH_DestroyWindowRenderBuffers(vmaAllocator, &vd->RenderBuffers, allocator);
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplVulkan_CreateWindow(ImGuiViewport* viewport)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_ViewportData* vd = IM_NEW(ImGui_ImplVulkan_ViewportData)();
    viewport->RendererUserData = vd;
    ImGui_ImplVulkanH_Window* wd = &vd->Window;
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;

    // Create surface
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    VkInstance vk_instance = v->Instance;
    VkSurfaceKHR surface_khr;
    vk::Result err = (vk::Result)platform_io.Platform_CreateVkSurface(viewport, (ImU64)vk_instance, (const void*)v->Allocator, (ImU64*)&surface_khr);
    wd->Surface = surface_khr;
    check_vk_result(err);

    // Check for WSI support
    vk::Bool32 res = v->PhysicalDevice.getSurfaceSupportKHR(v->QueueFamily, wd->Surface);
    if (res != VK_TRUE)
    {
        IM_ASSERT(0); // Error: no WSI support on physical device
        return;
    }

    // Select Surface Format
    const std::array<vk::Format, 4> requestSurfaceImageFormat = { vk::Format::eB8G8R8A8Unorm, vk::Format::eR8G8B8A8Unorm, vk::Format::eB8G8R8Unorm, vk::Format::eR8G8B8Unorm };
    const vk::ColorSpaceKHR requestSurfaceColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(v->PhysicalDevice, wd->Surface, requestSurfaceImageFormat.data(), requestSurfaceImageFormat.size(), requestSurfaceColorSpace);

    // Select Present Mode
    // FIXME-VULKAN: Even thought mailbox seems to get us maximum framerate with a single window, it halves framerate with a second window etc. (w/ Nvidia and SDK 1.82.1)
    std::array<vk::PresentModeKHR, 3> present_modes = { vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eImmediate, vk::PresentModeKHR::eFifo };
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(v->PhysicalDevice, wd->Surface, present_modes.data(), present_modes.size());
    //printf("[vulkan] Secondary window selected PresentMode = %d\n", wd->PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    wd->ClearEnable = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? false : true;
    ImGui_ImplVulkanH_CreateOrResizeWindow(v->Instance, v->PhysicalDevice, v->Device, wd, v->QueueFamily, v->Allocator, (int)viewport->Size.x, (int)viewport->Size.y, v->MinImageCount);
    vd->WindowOwned = true;
}

static void ImGui_ImplVulkan_DestroyWindow(ImGuiViewport* viewport)
{
    // The main viewport (owned by the application) will always have RendererUserData == NULL since we didn't create the data for it.
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    if (ImGui_ImplVulkan_ViewportData* vd = (ImGui_ImplVulkan_ViewportData*)viewport->RendererUserData)
    {
        ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
        if (vd->WindowOwned)
            ImGui_ImplVulkanH_DestroyWindow(v->Instance, v->Device, &vd->Window, v->Allocator);
        ImGui_ImplVulkanH_DestroyWindowRenderBuffers(v->vmaAllocator, &vd->RenderBuffers, v->Allocator);
        IM_DELETE(vd);
    }
    viewport->RendererUserData = NULL;
}

static void ImGui_ImplVulkan_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_ViewportData* vd = (ImGui_ImplVulkan_ViewportData*)viewport->RendererUserData;
    if (vd == NULL) // This is NULL for the main viewport (which is left to the user/app to handle)
        return;
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    vd->Window.ClearEnable = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? false : true;
    ImGui_ImplVulkanH_CreateOrResizeWindow(v->Instance, v->PhysicalDevice, v->Device, &vd->Window, v->QueueFamily, v->Allocator, (int)size.x, (int)size.y, v->MinImageCount);
}

static void ImGui_ImplVulkan_RenderWindow(ImGuiViewport* viewport, void*)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_ViewportData* vd = (ImGui_ImplVulkan_ViewportData*)viewport->RendererUserData;
    ImGui_ImplVulkanH_Window* wd = &vd->Window;
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    vk::Result err;
    
    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
    ImGui_ImplVulkanH_FrameSemaphores* fsd = &wd->FrameSemaphores[wd->SemaphoreIndex];
    {
        {
          err = v->Device.acquireNextImageKHR(wd->Swapchain, UINT64_MAX, fsd->ImageAcquiredSemaphore, VK_NULL_HANDLE, &wd->FrameIndex);
          check_vk_result(err);
          fd = &wd->Frames[wd->FrameIndex];
        }
        for (;;)
        {
            err = v->Device.waitForFences(1, &fd->Fence, VK_TRUE, 100);
            if (err == vk::Result::eSuccess) break;
            if (err == vk::Result::eTimeout) continue;
            check_vk_result(err);
        }
        {
            v->Device.resetCommandPool(fd->CommandPool, vk::CommandPoolResetFlags{});
            vk::CommandBufferBeginInfo info = {};
            info.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            err = fd->CommandBuffer.begin(&info);
            check_vk_result(err);
        }
        {
            ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            memcpy(&wd->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));

            vk::RenderPassBeginInfo info = {};
            info.renderPass = wd->RenderPass;
            info.framebuffer = fd->Framebuffer;
            info.renderArea.extent.width = wd->Width;
            info.renderArea.extent.height = wd->Height;
            info.clearValueCount = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? 0 : 1;
            info.pClearValues = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? NULL : &wd->ClearValue;
            fd->CommandBuffer.beginRenderPass(&info, vk::SubpassContents::eInline);
        }
    }

    ImGui_ImplVulkan_RenderDrawData(viewport->DrawData, fd->CommandBuffer, wd->Pipeline);

    {
        fd->CommandBuffer.endRenderPass();
        {
            vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            vk::SubmitInfo info = {};
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &fsd->ImageAcquiredSemaphore;
            info.pWaitDstStageMask = &wait_stage;
            info.commandBufferCount = 1;
            info.pCommandBuffers = &fd->CommandBuffer;
            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &fsd->RenderCompleteSemaphore;

            fd->CommandBuffer.end();
            err = v->Device.resetFences(1, &fd->Fence);
            check_vk_result(err);
            err = v->Queue.submit(1, &info, fd->Fence);
            check_vk_result(err);
        }
    }
}

static void ImGui_ImplVulkan_SwapBuffers(ImGuiViewport* viewport, void*)
{
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_ViewportData* vd = (ImGui_ImplVulkan_ViewportData*)viewport->RendererUserData;
    ImGui_ImplVulkanH_Window* wd = &vd->Window;
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;

    vk::Result err;
    uint32_t present_index = wd->FrameIndex;

    ImGui_ImplVulkanH_FrameSemaphores* fsd = &wd->FrameSemaphores[wd->SemaphoreIndex];
    vk::PresentInfoKHR info = {};
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &fsd->RenderCompleteSemaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &present_index;
    err = v->Queue.presentKHR(&info);
    if (err == vk::Result::eErrorOutOfDateKHR || err == vk::Result::eSuboptimalKHR)
        ImGui_ImplVulkanH_CreateOrResizeWindow(v->Instance, v->PhysicalDevice, v->Device, &vd->Window, v->QueueFamily, v->Allocator, (int)viewport->Size.x, (int)viewport->Size.y, v->MinImageCount);
    else
        check_vk_result(err);

    wd->FrameIndex = (wd->FrameIndex + 1) % wd->ImageCount;         // This is for the next vkWaitForFences()
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}

void ImGui_ImplVulkan_InitPlatformInterface()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        IM_ASSERT(platform_io.Platform_CreateVkSurface != NULL && "Platform needs to setup the CreateVkSurface handler.");
    platform_io.Renderer_CreateWindow = ImGui_ImplVulkan_CreateWindow;
    platform_io.Renderer_DestroyWindow = ImGui_ImplVulkan_DestroyWindow;
    platform_io.Renderer_SetWindowSize = ImGui_ImplVulkan_SetWindowSize;
    platform_io.Renderer_RenderWindow = ImGui_ImplVulkan_RenderWindow;
    platform_io.Renderer_SwapBuffers = ImGui_ImplVulkan_SwapBuffers;
}

void ImGui_ImplVulkan_ShutdownPlatformInterface()
{
    ImGui::DestroyPlatformWindows();
}
