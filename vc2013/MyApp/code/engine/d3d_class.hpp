
#pragma once

#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include <D3D11.h>
#include <DXGI1_2.h>

class d3d_class
{
    bool                      _vsync;
	IDXGIFactory2*            _factory;
	IDXGIAdapter1*            _adapter;
	IDXGIOutput1*             _adapter_output;
	unsigned int              _display_modes_count;
	DXGI_MODE_DESC1*          _display_modes;
	DXGI_ADAPTER_DESC*        _adapter_desc;
	unsigned int              _adapter_mem_video;
	unsigned int              _adapter_mem_system_shared;
	unsigned int              _adapter_mem_system_ded;
	std::string*              _adapter_name;
	unsigned int              _refresh_numer;
	unsigned int              _refresh_denom;
	IDXGISwapChain1*          _swap_chain;
	ID3D11Device*			  _device;
	ID3D11DeviceContext*	  _device_context;
    ID3D11BlendState*         _blend_state_on;
    ID3D11BlendState*         _blend_state_off;
    ID3D11RenderTargetView*	  _back_buffer_target;
    ID3D11Texture2D*          _depth_stencil_tex;
    ID3D11DepthStencilState*  _depth_stencil_state;
    ID3D11DepthStencilView*   _depth_stencil_view;
    ID3D11RasterizerState*    _raster_state;
    DirectX::XMFLOAT4X4*      _matrix_perspective;
    DirectX::XMFLOAT4X4*      _matrix_orthogonal;
    DirectX::XMFLOAT4X4*      _matrix_world;

public:

	d3d_class();
	~d3d_class();

	bool initialize(unsigned int
			      , unsigned int
				  , Windows::UI::Core::CoreWindow^
				  , float
				  , float);

	void shutdown();
    void enable_alpha_blending();
    void disable_alpha_blending();
    void begin_scene(float r, float g, float b, float a);
	void end_scene();
    ID3D11Device* get_device();
    ID3D11DeviceContext* get_device_context();
    DirectX::XMFLOAT4X4 get_world_matrix();
    DirectX::XMFLOAT4X4 get_projection_matrix();
};
