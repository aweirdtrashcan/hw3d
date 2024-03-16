#include "Graphics.h"
#include "dxerr.h"
#include <exception>
#include <sstream>
#include "Macros.h"
#include <d3dcompiler.h>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics( HWND hWnd, UINT windowWidth, UINT windowHeight )
	:
	projection(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	),
	windowHeight(windowHeight),
	windowWidth(windowWidth)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = windowWidth;
	sd.BufferDesc.Height = windowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	UINT swapchainFlags = 0;

#ifdef _DEBUG
	swapchainFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create device and front/back buffers, and swap chain and rendering context
	GFX_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapchainFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&pSwap,
			&pDevice,
			nullptr,
			&pContext
		)
	);

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView));

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	wrl::ComPtr<ID3D11DepthStencilState> pDSVState;

	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSVState));

	pContext->OMSetDepthStencilState(pDSVState.Get(), 0);

	wrl::ComPtr<ID3D11Texture2D> depthTexture;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.Height = windowHeight;
	texDesc.Width = windowWidth;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	
	GFX_THROW_INFO(pDevice->CreateTexture2D(&texDesc, nullptr, &depthTexture));

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	viewDesc.Texture2D.MipSlice = 0;
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	GFX_THROW_INFO(pDevice->CreateDepthStencilView(depthTexture.Get(), &viewDesc, &pDSV));

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (FLOAT)windowWidth;
	viewport.Height = (FLOAT)windowHeight;

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::BeginFrame(float red, float green, float blue)
{
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDSV.Get());
	pContext->RSSetViewports(1, &viewport);
	
	FLOAT color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (imguiEnabled) 
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void Graphics::EndFrame()
{
#ifdef _DEBUG
	infoManager.Set();
#endif

	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;
	if (FAILED(hr = pSwap->Present( 1u,0u )))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}

		throw GFX_EXCEPT(hr);
	}
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return DirectX::XMLoadFloat4x4(&projection);
}

void Graphics::SetProjection(DirectX::FXMMATRIX projection) noexcept
{
	DirectX::XMStoreFloat4x4(&this->projection, projection);
}

DirectX::XMMATRIX Graphics::GetView() const noexcept
{
	return DirectX::XMLoadFloat4x4(&view);
}

void Graphics::SetView(DirectX::FXMMATRIX view) noexcept
{
	DirectX::XMStoreFloat4x4(&this->view, view);
}

void Graphics::DrawIndexed(UINT indexCount)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(indexCount, 0, 0));
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, const std::vector<std::string>& infos) noexcept
	:
	Graphics::Exception(line, file),
	hr(hr)
{
	// Join all info messages with newlines into single string
	for (const std::string& m : infos) 
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << GetErrorCode() << ")\n"
		<< "[Error String] " << GetErrorString() << "\n"
		<< "[Description]" << GetErrorDescription() << "\n";
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << "\n\n";
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorStringA(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs)
	:
	Exception(line, file)
{
	for (const std::string& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << "\n"
		<< "\n[Error Info]\n" << GetErrorInfo() << "\n" 
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
