#include "ubo.h"

[numthreads( 8, 8, 1 )]
void CSMain( uint3 globalIdx : SV_DispatchThreadID, uint3 localIdx : SV_GroupThreadID, uint3 groupIdx : SV_GroupID )
{
    float4 color = rwTexture[ globalIdx.xy ];
    
    for (uint i = 0; i < particleCount; ++i)
    {
        //if ((uint)particles[ i ].clipPosition.x + windowWidth / 2 == globalIdx.x && (uint)particles[ i ].clipPosition.y + windowHeight / 2 == globalIdx.y)
        {
            float dist = distance( particles[ i ].clipPosition.xy + float2( windowWidth, windowHeight ) / 2, globalIdx.xy );
            const float radius = 5;
            if (dist < radius)
            {
                color = particles[ i ].color;
            }
        }
    }

    rwTexture[ globalIdx.xy ] = color; 
}
