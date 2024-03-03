#include "DxgiInfoManager.h"

#include "ChiliWin.h"
#include "Macros.h"
#include "Window.h"

#pragma comment(lib, "dxguid.lib")

DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	
	const HMODULE hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!hModDxgiDebug) 
	{
		throw CHWND_LAST_EXCEPT();
	}

	const DXGIGetDebugInterface debugInterface = (DXGIGetDebugInterface)GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface");
	
	if (!debugInterface)
	{
		throw CHWND_LAST_EXCEPT();
	}

	GFX_THROW_NOINFO(debugInterface(IID_PPV_ARGS(&pDxgiInfoQueue)));
	messageBuffer = malloc(10000);
}

DxgiInfoManager::~DxgiInfoManager()
{
	free(messageBuffer);
}

void DxgiInfoManager::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const UINT64 end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (UINT64 i = next; i < end; i++) 
	{
		memset(messageBuffer, 0, 10000);
		SIZE_T messageLen = 0;
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, nullptr, &messageLen));
		DXGI_INFO_QUEUE_MESSAGE* message = (DXGI_INFO_QUEUE_MESSAGE*)messageBuffer;
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, message, &messageLen));
		messages.emplace_back(message->pDescription);
	}
	return messages;
}
