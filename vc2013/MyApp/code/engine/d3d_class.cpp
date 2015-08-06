
#include <iostream>
#include <string>
#include "d3d_class.hpp"
#include "../debug.hpp"

using namespace stuff;

d3d_class::d3d_class()
    : _vsync(false)
    , _factory(NULL)
    , _adapter(NULL)
    , _adapter_output(NULL)
    , _display_modes_count(0)
    , _display_modes(NULL)
    , _adapter_desc(NULL)
    , _adapter_name(NULL)
    , _adapter_mem_system_ded(0)
    , _adapter_mem_system_shared(0)
    , _adapter_mem_video(0)
    , _swap_chain(NULL)
    , _refresh_numer(1)
    , _refresh_denom(1)
    , _device(NULL)
    , _device_context(NULL)
    , _blend_state_on(NULL)
    , _blend_state_off(NULL)
    , _back_buffer_target(NULL)
    , _depth_stencil_tex(NULL)
    , _depth_stencil_state(NULL)
    , _depth_stencil_view(NULL)
    , _raster_state(NULL)
    , _matrix_perspective(NULL)
    , _matrix_orthogonal(NULL)
    , _matrix_world(NULL)
{
}

d3d_class::~d3d_class()
{
}

bool
d3d_class::initialize(unsigned int width
                    , unsigned int height
                    , Windows::UI::Core::CoreWindow^ core_window
                    , float near_dist
                    , float far_dist)
{
    debug::d() << "initializing d3d..." << debug::endl;

    //_vsync = vsync;

    HRESULT result;

    //get a dxgi factory

	result = ::CreateDXGIFactory1(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&_factory));
    if(FAILED(result))
    {
        debug::d() << "failed to create factory" << debug::endl;
        return false;
    }

    //get the primary adapter

    result = _factory->EnumAdapters1(0, &_adapter);
    if(FAILED(result))
    {
        debug::d() << "failed to get primary adapter handle" << debug::endl;
        return false;
    }

    //get the adapter's monitor (the physical screen).
    //some cards may not be connected to a monitor, they act 
    //only as a renderer (ie: the discrete graphic cards
    //like Nvidia optimus or AMD switchable graphics)
    //this can be changed through the proprietary driver
    //settings panel (ie: turn on "high perf" for an app.exe
    //on amd's catalyst control center)

    result = _adapter->EnumOutputs(0, reinterpret_cast<IDXGIOutput**>(&_adapter_output));

    if(FAILED(result))
    {
        debug::d() << "failed to get monitor handle" << debug::endl;
        return false;
    }

    //get the video card description

    _adapter_desc = new DXGI_ADAPTER_DESC;
    result = _adapter->GetDesc(_adapter_desc);
    if(FAILED(result))
    {
        debug::d() << "failed to get adapter description" << debug::endl;
        return false;
    }

    //describe the video card

    std::wstring ws(_adapter_desc->Description);
    _adapter_name = new std::string(ws.begin(), ws.end());

    _adapter_mem_video = static_cast<unsigned int>(
        _adapter_desc->DedicatedVideoMemory / 1024 / 1024);

    _adapter_mem_system_ded = static_cast<unsigned int>(
        _adapter_desc->DedicatedSystemMemory / 1024 / 1024);

    _adapter_mem_system_shared = static_cast<unsigned int>(
        _adapter_desc->SharedSystemMemory / 1024 / 1024);

    debug::d() << "adapter infos:" << debug::endl
        << "  description              " << *_adapter_name << debug::endl
        << "  dedicated video memory   " << _adapter_mem_video
        << " MB" << debug::endl
        << "  dedicated system memory  " << _adapter_mem_system_ded
        << " MB" << debug::endl
        << "  shared system memory     " << _adapter_mem_system_shared
        << " MB" << debug::endl;

#pragma region show monitor sizes, create device & swap chain (desktop only)

    ////get the number of display modes supported by the screen

    //result = _adapter_output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM
    //                                                , 0
    //                                                , &_display_modes_count
    //                                                , NULL);
    //if(FAILED(result))
    //{
    //    debug::d() << "failed to get supported display modes count" << debug::endl;
    //    return false;
    //}

    ////enumerate the possible display modes

    //_display_modes = new DXGI_MODE_DESC1[_display_modes_count];
    //std::memset(_display_modes, 0
    //            , sizeof(DXGI_MODE_DESC1) * _display_modes_count);

    //result = _adapter_output->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM
    //                                                , 0
    //                                                , &_display_modes_count
    //                                                , _display_modes);
    //if(FAILED(result))
    //{
    //    debug::d() << "failed to get supported display modes list";
    //    return false;
    //}

    ////find a suitable display mode from the list

    //debug::d() << "available displays (" << _display_modes_count 
    //    << "):" << debug::endl;
    //for(unsigned int i = 0; i < _display_modes_count; i++)
    //{

    //    unsigned int dm_width = _display_modes[i].Width;
    //    unsigned int dm_height = _display_modes[i].Height;
    //    unsigned int dm_scaling = _display_modes[i].Scaling;
    //    unsigned int dm_refrate_num = _display_modes[i].RefreshRate.Numerator;
    //    unsigned int dm_refrate_den = _display_modes[i].RefreshRate.Denominator;

    //    if(0 == dm_refrate_den) //divide by 0 risk
    //        continue;

    //    unsigned int dm_refrate = dm_refrate_num / dm_refrate_den;

    //    debug::d() << "  "
    //        << '[' << dm_width << 'x' << dm_height << "] "
    //        << '[' << dm_refrate << " Hz] "
    //        << "[scaling " << dm_scaling << ']'
    //        << debug::endl;

        //if(dm_width == width && dm_height == height
        //   && DXGI_MODE_SCALING_CENTERED == dm_scaling)
        //{
        //    //found a correct supported resolution & mode
        //    _refresh_numer = dm_refrate_num;
        //    _refresh_denom = dm_refrate_den;
        //}
    //}

    //if(1 == _refresh_denom)
    //{
    //    debug::d() << "no suitable display mode found" << debug::endl;
    //    return false;
    //}

    //debug::d() << "chosen display mode is " << width << "x"
    //    << height << "@" << _refresh_numer / _refresh_denom
    //    << "hz" << debug::endl;

    //fill the swap chain infos
    //DXGI_SWAP_CHAIN_DESC swap_chain_infos;
    //::ZeroMemory(&swap_chain_infos, sizeof(DXGI_SWAP_CHAIN_DESC));
    //DXGI_SWAP_CHAIN_DESC& sci = swap_chain_infos;

    //sci.BufferCount = 1;                                    //one back buffer
    //sci.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      //we will draw to the back buffer
    //sci.OutputWindow = window_handle;                       //the window to be used
    //sci.SampleDesc.Count = 1;                               //turn off multisampling
    //sci.SampleDesc.Quality = 0;                             //turn off multisampling
    //sci.Windowed = !full_screen;                            //windowed/full-screen 
    //sci.BufferDesc.Width = width;                           //back buffer width
    //sci.BufferDesc.Height = height;                         //back buffer height
    //sci.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     //use 32-bit colors
    //sci.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //unspecified scaling
    //sci.BufferDesc.ScanlineOrdering
    //    = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;             //unspecified scanline
    //sci.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;              //swap erases the back buffer
    //sci.Flags = 0;                                          //no advanced flags

    //if(vsync)
    //{
    //    swap_chain_infos.BufferDesc.RefreshRate.Numerator = _refresh_numer;
    //    swap_chain_infos.BufferDesc.RefreshRate.Denominator = _refresh_denom;
    //}
    //else
    //{
    //    swap_chain_infos.BufferDesc.RefreshRate.Numerator = 0;
    //    swap_chain_infos.BufferDesc.RefreshRate.Numerator = 1;
    //}

    //Create a device, a device context, and a swap chain

//    UINT device_creation_flags = 0;
//    D3D_FEATURE_LEVEL feature_priority = D3D_FEATURE_LEVEL_11_0;
//
//	
//#ifdef _DEBUG
//    //Show useful memory leaks warnings for directx objects
//    device_creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//
//
//    result = D3D11CreateDeviceAndSwapChain
//    (
//        NULL,					  //use default adapter
//        D3D_DRIVER_TYPE_HARDWARE, //hardware or software driver
//        NULL,					  //rasterizer handle, must be null if hardware driver
//        device_creation_flags,	  //flags when creating the device
//        &feature_priority,	      //features priority (directx 10, 9, 11 etc..) default order is 11, 10, 9
//        1,						  //size of features priority array
//        D3D11_SDK_VERSION,		  //must be D3D11_SDK_VERSION
//        &swap_chain_infos,     	  //pointer to infos of the swapchain
//        &_swap_chain,		      //pointer address to swapchain to create
//        &_device,	        	  //pointer address to device to create
//        NULL,					  //this pointer will contain the feature (10, 11, 9...) chosen by Windows
//        &_device_context          //pointer address to device context to create
//    );

#pragma endregion

	UINT device_creation_flags = 0;
    D3D_FEATURE_LEVEL feature_priority = D3D_FEATURE_LEVEL_9_3;

	#ifdef _DEBUG
	    //Show useful memory leaks warnings for directx objects
	    device_creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	result = D3D11CreateDevice(_adapter
							 , D3D_DRIVER_TYPE_UNKNOWN
							 , NULL
							 , device_creation_flags
							 , &feature_priority
                             , 1
							 , D3D11_SDK_VERSION
							 , &_device
                             , NULL
							 , &_device_context);
    if(FAILED(result))
    {
        debug::d() << "failed to create device and device context (code " 
            << static_cast<unsigned int>(result) << ")" << debug::endl;
        return false;
    }

	//describe the swap chain

	DXGI_SWAP_CHAIN_DESC1 sci; //swap chain infos
	//::ZeroMemory(&sci, sizeof(DXGI_ADAPTER_DESC1)); //causes stack corruption

	sci.Width = width;
	sci.Height = height;
	sci.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sci.Stereo = false;
	sci.SampleDesc.Count = 1;
	sci.SampleDesc.Quality = 0;
	sci.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sci.BufferCount = 1;
	sci.Scaling = DXGI_SCALING_STRETCH;
	sci.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sci.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sci.Flags = 0;

	//create the swap chain

	result = _factory->CreateSwapChainForCoreWindow(_device
												  , reinterpret_cast<IUnknown*>(core_window)
												  , &sci
												  , NULL
												  , &_swap_chain);

    if(FAILED(result))
    {
        debug::d() << "failed to create swap chain for core window (code "
            << result << ")" << debug::endl;
        return false;
    }

    //create a blend state to allow transparency
    //this is done by giving infos about how to blend
    //the source pixel with the destination pixel

    //describe the blend state

    D3D11_BLEND_DESC blend_state_infos;
    std::memset(&blend_state_infos, 0, sizeof(D3D11_BLEND_DESC));

    blend_state_infos.RenderTarget[0].BlendEnable = TRUE;
    blend_state_infos.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_state_infos.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_state_infos.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_state_infos.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_state_infos.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blend_state_infos.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_state_infos.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    //create a "transparency enabled" blend state

    result = _device->CreateBlendState(&blend_state_infos, &_blend_state_on);
    if(FAILED(result))
    {
        debug::d() << "failed to create enabled blend state" << debug::endl;
        return false;
    }

    //create a "transparency disabled" blend state

    blend_state_infos.RenderTarget[0].BlendEnable = FALSE;
    result = _device->CreateBlendState(&blend_state_infos, &_blend_state_off);
    if(FAILED(result))
    {
        debug::d() << "failed to create disabled blend state" << debug::endl;
        return false;
    }

    //get the adress of the backbuffer texture

    IID id = __uuidof(ID3D11Texture2D);
    ID3D11Texture2D* back_buffer_texture;
    result = _swap_chain->GetBuffer(0, id, reinterpret_cast<void**>(&back_buffer_texture));
    if(FAILED(result))
    {
        debug::d() << "failed to get back buffer texture ptr" << debug::endl;
        return false;
    }

    //create a render target for the back buffer with this texture

    result = _device->CreateRenderTargetView(back_buffer_texture
                                             , NULL
                                             , &_back_buffer_target);
    if(FAILED(result))
    {
        debug::d() << "failed to create back buffer target" << debug::endl;
        return false;
    }
    back_buffer_texture->Release();
    back_buffer_texture = NULL;

    //describe the depth buffer

    D3D11_TEXTURE2D_DESC depth_buffer_infos;
    ::ZeroMemory(&depth_buffer_infos, sizeof(D3D11_TEXTURE2D_DESC));

    depth_buffer_infos.Width = width;
    depth_buffer_infos.Height = height;
    depth_buffer_infos.MipLevels = 1;
    depth_buffer_infos.ArraySize = 1;
    depth_buffer_infos.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //24 bits for depth, 8 bits for stencil
    depth_buffer_infos.SampleDesc.Count = 1;
    depth_buffer_infos.SampleDesc.Quality = 0;
    depth_buffer_infos.Usage = D3D11_USAGE_DEFAULT;
    depth_buffer_infos.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_infos.CPUAccessFlags = 0;
    depth_buffer_infos.MiscFlags = 0;

    //create a depth texture with these infos

    result = _device->CreateTexture2D(&depth_buffer_infos, NULL, &_depth_stencil_tex);
    if(FAILED(result))
    {
        debug::d() << "failed to create depth buffer texture" << debug::endl;
        return false;
    }

    //describe the depth stencil

    D3D11_DEPTH_STENCIL_DESC stencil_infos;
    ::ZeroMemory(&stencil_infos, sizeof(D3D11_DEPTH_STENCIL_DESC));

    stencil_infos.DepthEnable = true;
    stencil_infos.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    stencil_infos.DepthFunc = D3D11_COMPARISON_LESS;

    stencil_infos.StencilEnable = true;
    stencil_infos.StencilReadMask = 0xFF;
    stencil_infos.StencilWriteMask = 0xFF;

    stencil_infos.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    stencil_infos.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    stencil_infos.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    stencil_infos.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    stencil_infos.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    stencil_infos.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    stencil_infos.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    stencil_infos.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //create and set the depth stencil state

    result = _device->CreateDepthStencilState(&stencil_infos, &_depth_stencil_state);
    if(FAILED(result))
    {
        debug::d() << "failed to create depth stencil state" << debug::endl;
        return false;
    }
    _device_context->OMSetDepthStencilState(_depth_stencil_state, 1);

    //describe the depth stencil view

    D3D11_DEPTH_STENCIL_VIEW_DESC stencil_view_infos;
    ::ZeroMemory(&stencil_view_infos, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    stencil_view_infos.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stencil_view_infos.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    stencil_view_infos.Texture2D.MipSlice = 0;

    //create the depth stencil view

    result = _device->CreateDepthStencilView(_depth_stencil_tex
                                           , &stencil_view_infos
                                           , &_depth_stencil_view);
    if(FAILED(result))
    {
        debug::d() << "failed to create depth stencil viexw" << debug::endl;
        return false;
    }
    _device_context->OMSetRenderTargets(1, &_back_buffer_target, _depth_stencil_view);

    //describe the rasterizer

    D3D11_RASTERIZER_DESC raster_infos;
    raster_infos.AntialiasedLineEnable = false; //no anti aliasing
    raster_infos.CullMode = D3D11_CULL_BACK;    //back faces are not drawn
    raster_infos.DepthBias = 0;                 //adjust this to fix z-fighting
    raster_infos.DepthBiasClamp = 0.0f;         //see above
    raster_infos.DepthClipEnable = true;        //see above
    raster_infos.SlopeScaledDepthBias = 0.0f;   //see above
    raster_infos.FillMode = D3D11_FILL_SOLID;   //solid or wireframe
    raster_infos.FrontCounterClockwise = false; //front faces are clockwise vertices
    raster_infos.MultisampleEnable = false;     //used for MSAA
    raster_infos.ScissorEnable = false;         //culls vertices in a rectangle

    //create the rasterizer state 

    result = _device->CreateRasterizerState(&raster_infos, &_raster_state);
    if(FAILED(result))
    {
        debug::d() << "failed to create rasterizer state" << debug::endl;
        return false;
    }
    _device_context->RSSetState(_raster_state);

    //set the viewport

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    _device_context->RSSetViewports(1, &viewport);

    //XMMATRIX must be 16 byte aligned (automatically done
    //for variables created on the stack (no new / malloc))

    DirectX::XMMATRIX matrix;

    //set the perspective matrix

    float fov = 3.14f / 4;   //  PI/4 is a common FOV angle
    float screen_ratio = static_cast<float>(width)
        / static_cast<float>(height);
    
    matrix = DirectX::XMMatrixPerspectiveFovLH(
          fov
        , screen_ratio
        , near_dist
        , far_dist);

    _matrix_perspective = new DirectX::XMFLOAT4X4;
    DirectX::XMStoreFloat4x4(_matrix_perspective, matrix);

    //set the orthogonal matrix

    matrix = DirectX::XMMatrixOrthographicLH(
          static_cast<float>(width)
        , static_cast<float>(height)
        , near_dist
        , far_dist);

    _matrix_orthogonal = new DirectX::XMFLOAT4X4;
    DirectX::XMStoreFloat4x4(_matrix_orthogonal, matrix);

    //the view matrix is created in the camera class
    //since it is tied to the camera's position and rotation

    //set the world matrix (identity matrix)
    
    matrix = DirectX::XMMatrixIdentity();
    _matrix_world = new DirectX::XMFLOAT4X4;
    DirectX::XMStoreFloat4x4(_matrix_world, matrix);

    return true;
}

void
d3d_class::shutdown()
{
    debug::d() << "shutting down d3d.." << debug::endl;

    _vsync = false;

    if(NULL != _factory)
    {
        _factory->Release();
        _factory = NULL;
    }

    if(NULL != _adapter)
    {
        _adapter->Release();
        _adapter = NULL;
    }

    if(NULL != _adapter_output)
    {
        _adapter_output->Release();
        _adapter_output = NULL;
    }

    if(NULL != _display_modes)
    {
        delete[] _display_modes;
        _display_modes = NULL;
    }

    _display_modes_count = 0;

    if(NULL != _adapter_desc)
    {
        delete _adapter_desc;
        _adapter_desc = NULL;
    }

    if(NULL != _adapter_name)
    {
        delete _adapter_name;
        _adapter_name = NULL;
    }

    _refresh_numer = 1;
    _refresh_denom = 1;

    if(NULL != _swap_chain)
    {
        _swap_chain->Release();
        _swap_chain = NULL;
    }

    if(NULL != _device)
    {
        _device->Release();
        _device = NULL;
    }

    if(NULL != _device_context)
    {
        _device_context->Release();
        _device_context = NULL;
    }

    if(NULL != _blend_state_on)
    {
        _blend_state_on->Release();
        _blend_state_on = NULL;
    }

    if(NULL != _blend_state_off)
    {
        _blend_state_off->Release();
        _blend_state_off = NULL;
    }

    if(NULL != _back_buffer_target)
    {
        _back_buffer_target->Release();
        _back_buffer_target = NULL;
    }

    if(NULL != _depth_stencil_tex)
    {
        _depth_stencil_tex->Release();
        _depth_stencil_tex = NULL;
    }

    if(NULL != _depth_stencil_state)
    {
        _depth_stencil_state->Release();
        _depth_stencil_state = NULL;
    }

    if(NULL != _depth_stencil_view)
    {
        _depth_stencil_view->Release();
        _depth_stencil_view = NULL;
    }

    if(NULL != _raster_state)
    {
        _raster_state->Release();
        _raster_state = NULL;
    }

    if(NULL != _matrix_perspective)
    {
        delete _matrix_perspective;
        _matrix_perspective = NULL;
    }

    if(NULL != _matrix_orthogonal)
    {
        delete _matrix_orthogonal;
        _matrix_orthogonal = NULL;
    }

    if(NULL != _matrix_world)
    {
        delete _matrix_world;
        _matrix_world = NULL;
    }
}

void
d3d_class::begin_scene(float red
                     , float green
                     , float blue
                     , float alpha)
{
    float color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    _device_context->ClearRenderTargetView(_back_buffer_target, color);
    _device_context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void
d3d_class::end_scene()
{
    //if(_vsync)
    //{
    //    _swap_chain->Present(1, 0);
    //}
    //else
    //{
    //    _swap_chain->Present(0, 0);
    //}

    //on WP, vsync is always enabled
    _swap_chain->Present(1, 0);
}

void 
d3d_class::enable_alpha_blending()
{
    float blend_factor[4];

    // Setup the blend factor.
    blend_factor[0] = 0.0f;
    blend_factor[1] = 0.0f;
    blend_factor[2] = 0.0f;
    blend_factor[3] = 0.0f;

    // Turn on the alpha blending.
    _device_context->OMSetBlendState(this->_blend_state_on, blend_factor, 0xffffffff);
}

void
d3d_class::disable_alpha_blending()
{
    float blend_factor[4];

    // Setup the blend factor.
    blend_factor[0] = 0.0f;
    blend_factor[1] = 0.0f;
    blend_factor[2] = 0.0f;
    blend_factor[3] = 0.0f;

    // Turn on the alpha blending.
    _device_context->OMSetBlendState(this->_blend_state_off, blend_factor, 0xffffffff);
}

ID3D11Device*
d3d_class::get_device()
{
    return _device;
}

DirectX::XMFLOAT4X4
d3d_class::get_world_matrix()
{
    return *_matrix_world;
}

DirectX::XMFLOAT4X4 
d3d_class::get_projection_matrix()
{
    return *_matrix_perspective;
}

ID3D11DeviceContext*
d3d_class::get_device_context()
{
    return _device_context;
}
