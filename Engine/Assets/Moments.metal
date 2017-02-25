#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct uniforms_t
{
    matrix_float4x4 _ModelViewProjectionMatrix;
    matrix_float4x4 _ModelViewMatrix;
};

struct Vertex
{
    float3 position [[attribute(0)]];
};

struct ColorInOut
{
    float4 position [[position]];
};

vertex ColorInOut moments_vertex(Vertex vert [[stage_in]],
                               constant uniforms_t& uniforms [[ buffer(5) ]])
{
    ColorInOut out;
    
    float4 in_position = float4( vert.position, 1.0 );
    out.position = uniforms._ModelViewProjectionMatrix * in_position;
    out.position.z = out.position.z * 0.5 + 0.5; // -1..1 to 0..1 conversion
    return out;
}

fragment float4 moments_fragment( ColorInOut in [[stage_in]] )
{
    float linearDepth = in.position.z;

    float dx = dfdx( linearDepth );
    float dy = dfdy( linearDepth );
    
    float moment1 = linearDepth;
    // Compute second moment over the pixel extents.
    float moment2 = linearDepth * linearDepth + 0.25 * (dx * dx + dy * dy);

    return float4( moment1, moment2, 0.0, 0.0 );
}
