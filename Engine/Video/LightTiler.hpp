#ifndef LIGHT_TILER_HPP
#define LIGHT_TILER_HPP

#include <vector>
#if RENDERER_METAL
#import <Metal/Metal.h>
#endif
#include "Vec3.hpp"

struct ID3D12Resource;

namespace ae3d
{
    /// Implements Forward+ light culler
    class LightTiler
    {
    public:
        void Init();
        void SetPointLightPositionAndRadius( int bufferIndex, Vec3& position, float radius );
        void SetSpotLightPositionAndRadius( int bufferIndex, Vec3& position, float radius );
        void UpdateLightBuffers();
        void CullLights( class ComputeShader& shader, const struct Matrix44& projection, const Matrix44& view,  class RenderTexture& depthNormalTarget );
        bool CullerUniformsCreated() const { return cullerUniformsCreated; }
        
#if RENDERER_METAL
        id< MTLBuffer > GetPerTileLightIndexBuffer() { return perTileLightIndexBuffer; }
        id< MTLBuffer > GetPointLightCenterAndRadiusBuffer() { return pointLightCenterAndRadiusBuffer; }
        id< MTLBuffer > GetSpotLightCenterAndRadiusBuffer() { return spotLightCenterAndRadiusBuffer; }
        id< MTLBuffer > GetCullerUniforms() { return uniformBuffer; }
#endif
        /// Destroys graphics API objects.
        void DestroyBuffers();

#if RENDERER_D3D12
        ID3D12Resource* GetPointLightCenterAndRadiusBuffer() const { return pointLightCenterAndRadiusBuffer; }
#endif
        int GetPointLightCount() const { return activePointLights; }
        int GetSpotLightCount() const { return 0; }
        unsigned GetMaxNumLightsPerTile() const;

    private:
        unsigned GetNumTilesX() const;
        unsigned GetNumTilesY() const;

#if RENDERER_METAL
        id< MTLBuffer > uniformBuffer;
        id< MTLBuffer > pointLightCenterAndRadiusBuffer;
        id< MTLBuffer > spotLightCenterAndRadiusBuffer;
        id< MTLBuffer > perTileLightIndexBuffer;
#endif
#if RENDERER_D3D12
        ID3D12Resource* uniformBuffer = nullptr;
        ID3D12Resource* mappedUniformBuffer = nullptr;
        ID3D12Resource* perTileLightIndexBuffer = nullptr;
        ID3D12Resource* pointLightCenterAndRadiusBuffer = nullptr;
        ID3D12Resource* spotLightCenterAndRadiusBuffer = nullptr;
#endif
        std::vector< Vec4 > pointLightCenterAndRadius;
        std::vector< Vec4 > spotLightCenterAndRadius;
        int activePointLights = 0;
        int activeSpotLights = 0;
        bool cullerUniformsCreated = false;
        static const int TileRes = 16;
        static const int MaxLights = 2048;
        static const unsigned MaxLightsPerTile = 544;
    };
}

#endif
