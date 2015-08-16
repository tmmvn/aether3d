#include "RenderTexture.hpp"
#include "GfxDevice.hpp"
#include "System.hpp"

void ae3d::RenderTexture2D::Create( int aWidth, int aHeight, TextureWrap aWrap, TextureFilter aFilter )
{
    if (aWidth <= 0 || aHeight <= 0)
    {
        System::Print( "Render texture has invalid dimension!\n" );
        return;
    }

    width = aWidth;
    height = aHeight;
    wrap = aWrap;
    filter = aFilter;
}

