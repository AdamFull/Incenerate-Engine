# Incenerate Engine

## About

This project was developed with the aim of studying the work of modern game engines and studying the vulkan graphics api.  At the moment, there are not very many features implemented in the engine, because one person is engaged in development. In the future, the engine will be ported to android, work is already underway in this direction.  If there are people who like this project, then I will develop it further.
 
### Most interesting resources

Must read resource: https://vulkan-tutorial.com/ and code from this tutorial https://github.com/Overv/VulkanTutorial

Repository with hello triangle example from previous resource but using Vulkan hpp: https://github.com/bwasty/vulkan-tutorial-hpp

Vulkan engine implementation with explanation on youtube: https://www.youtube.com/watch?v=lr93-_cC8v4&t=1s&ab_channel=BrendanGalea  and repo https://github.com/blurrypiano/littleVulkanEngine

SachaWilliems examples. To really hard, explore it after previous resources: https://github.com/SaschaWillems/Vulkan

Vulkan programming guide book. To consolidate the material and study the subtleties of working with Vulkan.

Game engine architecture. Goog book for creating not bad game engine architecture.

### Supported image formats
- [x] ktx
- [x] ktx2
- [x] png
- [x] jpg
- [x] bmp
- [x] pic
- [x] gif
- [ ] dds

### Supported mesh formats
- [x] gltf
- [x] gltf2.0
- [-] obj - deprecatid
- [ ] fbx

### Supported audio formats
- [x] wav/wave
- [x] ogg (in future)

### Supported scripting languages
- [ ] lua (sol2 in future)

### Vulkan support
- [x] Vulkan 1.3
- [x] Vulkan 1.2
- [x] Vulkan 1.1
- [-] Vulkan 1.0 - deprecated

### Graphics features
- [x] Deferred shading
- [x] HDR
- [x] IBL
- [x] PBR (UE4 like)
- [x] Bloom
- [x] DOF
- [ ] Shadow mapping
- [ ] SSAO
- [ ] SSR
- [ ] Instancing
- [ ] Tescellation (Exists, but now working incorrect)
- [ ] GI
- [ ] RTX Shadows

### Editor features
- [x] Pixel-perfect object picking

### Other features
- [x] shader reflection (spirv-cross)
- [x] Bullet3 physics (rigidbody only)
- [x] Animation (gltf only now)

### In future
- [ ] lua (sol2 in future)
- [ ] Material editor (now working with default pbr material)
- [ ] Mesh (scene) metadata (custom materials and custom loading rules)
- [ ] Skinning (working incorrect)
- [ ] Effekseer particles
- [ ] OpenGL 4+ support

### glTF support
[Feature support status](doc/gltf/glTFFeaturesSupportStatus.md)
[Tests passing status](doc/gltf/glTFSamplesSupportStatus.md)
[Sampless that renders/working correct](doc/gltf/glTFTestPassingStatus.md)

### Bugs and problems
- Some gltf meshes loads incorrect, and when i deleting this mesh, it's not completely deleted
- Shadow projection is incorrect
- Async resource loading not working
