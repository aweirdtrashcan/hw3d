#pragma once

#define GFX_THROW_FAILED(hrcall) { HRESULT __hr = hrcall; if (FAILED(__hr)) throw Graphics::HrException(__LINE__, __FILE__, __hr); }
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, hr)
#define CHWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )