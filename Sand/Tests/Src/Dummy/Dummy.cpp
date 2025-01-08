#include "Snd/Pch.h"
#include "Dummy.h"

namespace Snd::Dummy
{
	HWND createDummyWindowHandle(const HINSTANCE hInstance)
	{
		WNDCLASS wc = { };
		wc.lpfnWndProc = DefWindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = "Dummy Window Class";

		RegisterClass(&wc);

		const HWND hwnd = CreateWindowEx(
			0,
			"Dummy Window Class",
			"Dummy Window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);
		return hwnd;
	}
}

