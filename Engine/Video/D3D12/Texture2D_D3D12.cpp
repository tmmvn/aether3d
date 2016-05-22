#include "Texture2D.hpp"
#include <vector>
#include <map>
#include <d3d12.h>
#include <d3dx12.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.c"
#include "GfxDevice.hpp"
#include "FileWatcher.hpp"
#include "FileSystem.hpp"
#include "Macros.hpp"
#include "System.hpp"
#include "DescriptorHeapManager.hpp"

extern ae3d::FileWatcher fileWatcher;
bool HasStbExtension( const std::string& path ); // Defined in TextureCommon.cpp
void TransitionResource( GpuResource& gpuResource, D3D12_RESOURCE_STATES newState );

namespace MathUtil
{
    int GetMipmapCount( int width, int height );
}

namespace GfxDeviceGlobal
{
    extern ID3D12Device* device;
    extern ID3D12CommandAllocator* commandListAllocator;
    extern ID3D12GraphicsCommandList* graphicsCommandList;
    extern ID3D12CommandQueue* commandQueue;
}

namespace Texture2DGlobal
{
    std::vector< ID3D12Resource* > textures;
    std::vector< ID3D12Resource* > uploadBuffers;
    ae3d::Texture2D defaultTexture;
    
    std::map< std::string, ae3d::Texture2D > pathToCachedTexture;
#if DEBUG
    std::map< std::string, std::size_t > pathToCachedTextureSizeInBytes;
    
    void PrintMemoryUsage()
    {
        std::size_t total = 0;
        
        for (const auto& path : pathToCachedTextureSizeInBytes)
        {
            ae3d::System::Print("%s: %d B\n", path.first.c_str(), path.second);
            total += path.second;
        }
        
        ae3d::System::Print( "Total texture usage: %d KiB\n", total / 1024 );
    }
#endif
}

void ae3d::Texture2D::DestroyTextures()
{
    for (std::size_t i = 0; i < Texture2DGlobal::textures.size(); ++i)
    {
        AE3D_SAFE_RELEASE( Texture2DGlobal::textures[ i ] );
    }

    for (std::size_t i = 0; i < Texture2DGlobal::uploadBuffers.size(); ++i)
    {
        AE3D_SAFE_RELEASE( Texture2DGlobal::uploadBuffers[ i ] );
    }
}

void InitializeTexture( GpuResource& gpuResource, D3D12_SUBRESOURCE_DATA* subResources, int subResourceCount )
{
    D3D12_HEAP_PROPERTIES heapProps;
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    ID3D12Resource* uploadBuffer = nullptr;
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize( gpuResource.resource, 0, subResourceCount );

    const auto buffer = CD3DX12_RESOURCE_DESC::Buffer( uploadBufferSize );
    HRESULT hr = GfxDeviceGlobal::device->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &uploadBuffer ) );
    AE3D_CHECK_D3D( hr, "Failed to create texture upload resource" );

    if (hr == S_OK)
    {
        uploadBuffer->SetName( L"Texture Upload Buffer" );
        Texture2DGlobal::uploadBuffers.push_back( uploadBuffer );

        hr = GfxDeviceGlobal::graphicsCommandList->Reset( GfxDeviceGlobal::commandListAllocator, nullptr );
        AE3D_CHECK_D3D( hr, "command list reset in InitializeTexture" );

        TransitionResource( gpuResource, D3D12_RESOURCE_STATE_COPY_DEST );
        UpdateSubresources( GfxDeviceGlobal::graphicsCommandList, gpuResource.resource, uploadBuffer, 0, 0, subResourceCount, subResources );
        TransitionResource( gpuResource, D3D12_RESOURCE_STATE_GENERIC_READ );

        hr = GfxDeviceGlobal::graphicsCommandList->Close();
        AE3D_CHECK_D3D( hr, "command list close in InitializeTexture" );

        ID3D12CommandList* ppCommandLists[] = { GfxDeviceGlobal::graphicsCommandList };
        GfxDeviceGlobal::commandQueue->ExecuteCommandLists( 1, &ppCommandLists[ 0 ] );
    }
}

void TexReload( const std::string& path )
{
    auto& tex = Texture2DGlobal::pathToCachedTexture[ path ];

    tex.Load( ae3d::FileSystem::FileContents( path.c_str() ), tex.GetWrap(), tex.GetFilter(), tex.GetMipmaps(), tex.GetColorSpace(), tex.GetAnisotropy() );
}

ae3d::Texture2D* ae3d::Texture2D::GetDefaultTexture()
{
    if (Texture2DGlobal::defaultTexture.GetWidth() == 0)
    {
        Texture2DGlobal::defaultTexture.width = 32;
        Texture2DGlobal::defaultTexture.height = 32;
// TODO: Init default texture.
    }
    
    return &Texture2DGlobal::defaultTexture;
}

void ae3d::Texture2D::Load( const FileSystem::FileContentsData& fileContents, TextureWrap aWrap, TextureFilter aFilter, Mipmaps aMipmaps, ColorSpace aColorSpace, float aAnisotropy )
{
    filter = aFilter;
    wrap = aWrap;
    mipmaps = aMipmaps;
    anisotropy = aAnisotropy;
    colorSpace = aColorSpace;

    if (!fileContents.isLoaded)
    {
        *this = Texture2DGlobal::defaultTexture;
        return;
    }
    
    const bool isCached = Texture2DGlobal::pathToCachedTexture.find( fileContents.path ) != Texture2DGlobal::pathToCachedTexture.end();

    if (isCached && handle == 0)
    {
        *this = Texture2DGlobal::pathToCachedTexture[ fileContents.path ];
        return;
    }
    
    // First load
    //    fileWatcher.AddFile( fileContents.path, TexReload );

    const bool isDDS = fileContents.path.find( ".dds" ) != std::string::npos || fileContents.path.find( ".DDS" ) != std::string::npos;
    
    if (HasStbExtension( fileContents.path ))
    {
        LoadSTB( fileContents );
    }
    else if (isDDS)
    {
        *this = Texture2DGlobal::defaultTexture;
        LoadDDS( fileContents.path.c_str() );
    }
    else
    {
        ae3d::System::Print("Unknown texture file extension: %s\n", fileContents.path.c_str() );
    }
    
    const UINT mipLevelCount = mipmaps == Mipmaps::Generate ? static_cast< UINT >( MathUtil::GetMipmapCount( width, height ) ) : 1;
    
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = mipLevelCount; 
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.PlaneSlice = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    
    srv = DescriptorHeapManager::AllocateDescriptor( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    handle = static_cast< unsigned >( srv.ptr );

    GfxDeviceGlobal::device->CreateShaderResourceView( gpuResource.resource, &srvDesc, srv );

    if (mipmaps == Mipmaps::Generate)
    {
        uavs.resize( mipLevelCount );

        for (UINT i = 0; i < mipLevelCount; ++i)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = i;
            uavDesc.Texture2D.PlaneSlice = 0;

            uavs[ i ] = DescriptorHeapManager::AllocateDescriptor( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
            GfxDeviceGlobal::device->CreateUnorderedAccessView( gpuResource.resource, nullptr, &uavDesc, uavs[ i ] );
        }
    }

    Texture2DGlobal::pathToCachedTexture[ fileContents.path ] = *this;
#if DEBUG
    Texture2DGlobal::pathToCachedTextureSizeInBytes[ fileContents.path ] = static_cast< std::size_t >(width * height * 4 * (mipmaps == Mipmaps::Generate ? 1.0f : 1.33333f));
    //Texture2DGlobal::PrintMemoryUsage();
#endif
}

void ae3d::Texture2D::LoadDDS( const char* /*path*/ )
{
    ae3d::System::Print( ".dds loading not implemented in d3d12!\n" );
}

void ae3d::Texture2D::LoadSTB( const FileSystem::FileContentsData& fileContents )
{
    int components;
    unsigned char* data = stbi_load_from_memory( fileContents.data.data(), static_cast<int>(fileContents.data.size()), &width, &height, &components, 4 );
    System::Assert( width > 0 && height > 0, "Invalid texture dimension" );

    if (data == nullptr)
    {
        const std::string reason( stbi_failure_reason() );
        System::Print( "%s failed to load. stb_image's reason: %s\n", fileContents.path.c_str(), reason.c_str() );
        return;
    }

    opaque = (components == 3 || components == 1);

    const UINT mipLevelCount = mipmaps == Mipmaps::Generate ? static_cast< UINT >(MathUtil::GetMipmapCount( width, height )) : 1;

    D3D12_RESOURCE_DESC descTex = {};
    descTex.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    descTex.Width = width;
    descTex.Height = static_cast< UINT >( height );
    descTex.DepthOrArraySize = 1;
    descTex.MipLevels = static_cast< UINT16 >( mipLevelCount );
    descTex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    descTex.SampleDesc.Count = 1;
    descTex.SampleDesc.Quality = 0;
    descTex.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    descTex.Flags = mipLevelCount > 1 ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;
    
    HRESULT hr = GfxDeviceGlobal::device->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &descTex,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &gpuResource.resource ) );
    AE3D_CHECK_D3D( hr, "Unable to create texture resource" );

    wchar_t wstr[ 128 ];
    std::mbstowcs( wstr, fileContents.path.c_str(), 128 );
    gpuResource.resource->SetName( wstr );
    gpuResource.usageState = D3D12_RESOURCE_STATE_COPY_DEST;
    Texture2DGlobal::textures.push_back( gpuResource.resource );

    const int bytesPerPixel = 4;
    D3D12_SUBRESOURCE_DATA texResource;
    texResource.pData = data;
    texResource.RowPitch = width * bytesPerPixel;
    texResource.SlicePitch = texResource.RowPitch * height;

    InitializeTexture( gpuResource, &texResource, 1 );

    stbi_image_free( data );
}
