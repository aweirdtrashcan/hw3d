#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "DxgiInfoManager.h"
#include "ChiliException.h"
#include "ChiliTimer.h"
#include <DirectXMath.h>

typedef HRESULT(*PresentFuncPtr)(IDXGISwapChain* swapChain);

class Graphics
{
	friend class Bindable;
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, const std::vector<std::string>& infos = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs);
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics( HWND hWnd, UINT windowWidth, UINT windowHeight );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	~Graphics();
	void BeginFrame(float red, float green, float blue);
	void EndFrame();
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetProjection(DirectX::FXMMATRIX projection) noexcept;
	DirectX::XMMATRIX GetView() const noexcept;
	void SetView(DirectX::FXMMATRIX view) noexcept;
	inline void EnableImgui() { imguiEnabled = true; }
	inline void DisableImgui() { imguiEnabled = false; };
	inline bool IsImguiEnabled() const { return imguiEnabled; }
	void DrawIndexed(UINT indexCount);
	inline bool& GetIsVsyncEnabled() noexcept { return bEnableVSync; }
	void SetPaused(bool bPaused) noexcept { this->bPaused = bPaused; }
	bool GetIsPaused() const noexcept { return bPaused; }
	void OnResize(UINT width, UINT height) noexcept(!IS_DEBUG);
private:
#ifdef _DEBUG
	DxgiInfoManager infoManager;
#endif

	UINT windowWidth, windowHeight;

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	D3D11_VIEWPORT viewport = {};
	bool imguiEnabled = true;

	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 view;
	PresentFuncPtr presentFunctions[2];
	bool bEnableVSync = true;
	bool bPaused = false;
};
