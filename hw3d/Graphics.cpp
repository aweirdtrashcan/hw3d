#include "Graphics.h"
#include "dxerr.h"
#include <exception>
#include <sstream>
#include "Macros.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics( HWND hWnd )
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
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
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
	FLOAT color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), color);
}

void Graphics::EndFrame()
{
#ifdef _DEBUG
	infoManager.Set();
#endif
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

void Graphics::DrawTriangle()
{
	UINT stride = 0;
	wrl::ComPtr<ID3D11Buffer> vertexBuffer;

	struct Vertex
	{
		float x;
		float y;
	};

	const Vertex vertexBufferCpu[] =
	{
		{ 0.0f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f } 
	};

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(vertexBufferCpu);
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexBufferSrd = {};
	vertexBufferSrd.pSysMem = vertexBufferCpu;

	GFX_THROW_INFO(pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferSrd, &vertexBuffer));

	GFX_THROW_INFO_ONLY(pContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &stride));
	GFX_THROW_INFO_ONLY(pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	// Vertex Shader

	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pVertexShaderBlob;

	GFX_THROW_INFO(D3DReadFileToBlob(L".\\VertexShader.cso", &pVertexShaderBlob));

	GFX_THROW_INFO(pDevice->CreateVertexShader(
		pVertexShaderBlob->GetBufferPointer(), 
		pVertexShaderBlob->GetBufferSize(), 
		nullptr, 
		&pVertexShader
	));

	// Fragment Shader

	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pPixelShaderBlob;

	GFX_THROW_INFO(D3DReadFileToBlob(L".\\PixelShader.cso", &pPixelShaderBlob));

	GFX_THROW_INFO(pDevice->CreatePixelShader(
		pPixelShaderBlob->GetBufferPointer(),
		pPixelShaderBlob->GetBufferSize(),
		nullptr,
		&pPixelShader
	));

	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	pContext->RSSetViewports(1, &vp);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	pDevice->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(),
		&pInputLayout
	);

	pContext->IASetInputLayout(pInputLayout.Get());

	GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertexBufferCpu), 0));
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infos) noexcept
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
