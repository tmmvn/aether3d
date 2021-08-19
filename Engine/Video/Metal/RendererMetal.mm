#include "Renderer.hpp"
#include "FileSystem.hpp"
#include "GfxDevice.hpp"
#include "Vec3.hpp"

struct Particle
{
    ae3d::Vec4 color;
};

ae3d::Renderer renderer;

id< MTLBuffer > particleBuffer;

void ae3d::BuiltinShaders::Load()
{
    spriteRendererShader.LoadFromLibrary( "sprite_vertex", "sprite_fragment" );
    sdfShader.LoadFromLibrary( "sdf_vertex", "sdf_fragment" );
    skyboxShader.LoadFromLibrary( "skybox_vertex", "skybox_fragment" );
    momentsShader.LoadFromLibrary( "moments_vertex", "moments_fragment" );
    momentsSkinShader.LoadFromLibrary( "moments_skin_vertex", "moments_fragment" );
    depthNormalsShader.LoadFromLibrary( "depthnormals_vertex", "depthnormals_fragment" );
    depthNormalsSkinShader.LoadFromLibrary( "depthnormals_skin_vertex", "depthnormals_fragment" );
    lightCullShader.Load( "light_culler", FileSystem::FileContents(""), FileSystem::FileContents("") );
    particleSimulationShader.Load( "particle_simulation", FileSystem::FileContents(""), FileSystem::FileContents("") );
    uiShader.LoadFromLibrary( "sprite_vertex", "sprite_fragment" );
    
    const unsigned maxParticleCount = 50;
    particleBuffer = [GfxDevice::GetMetalDevice() newBufferWithLength:sizeof( Particle ) * maxParticleCount
                              options:MTLResourceCPUCacheModeDefaultCache];
    particleBuffer.label = @"Particle buffer";
}
