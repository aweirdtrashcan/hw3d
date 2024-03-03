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
	:
	modelViewProjection(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	)
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
	if (FAILED(hr = pSwap->Present( 0u,DXGI_PRESENT_ALLOW_TEARING )))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}

		throw GFX_EXCEPT(hr);
	}
}

void Graphics::DrawTriangle(float angle, float x, float y)
{
	using namespace DirectX;
	wrl::ComPtr<ID3D11Buffer> vertexBuffer;
	wrl::ComPtr<ID3D11Buffer> indexBuffer;

	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	pContext->RSSetViewports(1, &vp);

	struct Vertex
	{
		struct 
		{
			float x;
			float y;
		};
		struct
		{
			BYTE r;
			BYTE g;
			BYTE b;
			BYTE a;
		};
	};

	const Vertex vertexBufferCpu[] =
	{
		{ { 0.5f, 0.5f }, { 255, 0, 0, 255 } },
		{ { -0.5f, 0.5f }, { 0, 255, 0, 255 } },
		{ { 0.5f, -0.5f }, { 0, 0, 255, 255 } },
		{ { -0.5f, -0.5f }, { 255, 0, 255, 255 } }
	};

	const USHORT indexBufferCpu[] =
	{
		1, 0, 3,
		3, 0, 2
	};

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(vertexBufferCpu);
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexBufferSrd = {};
	vertexBufferSrd.pSysMem = vertexBufferCpu;

	GFX_THROW_INFO(pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferSrd, &vertexBuffer));

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(indexBufferCpu);
	indexBufferDesc.StructureByteStride = sizeof(indexBufferCpu[0]);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexBufferSrd = {};
	indexBufferSrd.pSysMem = indexBufferCpu;

	GFX_THROW_INFO(pDevice->CreateBuffer(&indexBufferDesc, &indexBufferSrd, &indexBuffer));

	pContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX model = XMMatrixRotationZ(angle) * XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(x, y, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH({ 0.0f, 0.0f, -4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	XMMATRIX projection = XMMatrixPerspectiveFovLH(45.f, 800.f / 600.f, 0.1f, 1000.f);
	XMStoreFloat4x4(&modelViewProjection, XMMatrixTranspose(model * view * projection));

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(modelViewProjection);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	wrl::ComPtr<ID3D11Buffer> cb;

	GFX_THROW_INFO(pDevice->CreateBuffer(&cbDesc, nullptr, &cb));

	D3D11_MAPPED_SUBRESOURCE mrs = {};

	GFX_THROW_INFO(pContext->Map(cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mrs));

	memcpy(mrs.pData, &modelViewProjection, sizeof(modelViewProjection));
	
	pContext->Unmap(cb.Get(), 0);

	pContext->VSSetConstantBuffers(0, 1, cb.GetAddressOf());
	
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

	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

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

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indexBufferCpu), 0, 0));
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
