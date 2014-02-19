/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(_WIN32)

#include "SplashScreenWin.h"
#include "../Concurrency/Concurrency.h"
#include "../Concurrency/Lock.h"
#include "../Concurrency/Mutex.h"

#include "../Graphics/Bitmap.h"
#include "../Graphics/PixelAccessor.h"
#include "../Utils.h"
#include "../Macros.h"

namespace Util {
namespace UI {

// Inspired by:
// http://code.logos.com/blog/2008/09/displaying_a_splash_screen_with_c_part_ii.html
// Reference:
// http://msdn.microsoft.com/en-us/library/ms632680%28v=VS.85%29.aspx

const TCHAR * g_szClassName = "SplashWindow2";


static volatile bool initDone = false;

//! ---|> UserThread
void SplashScreenWin::run() {

//	// needs lib gdi32

	HINSTANCE hInstance = GetModuleHandle(nullptr);

	// get image size
	SIZE imgSize;
	imgSize.cx = splashImage->getWidth();
	imgSize.cy = splashImage->getHeight();

	// get the primary monitor's info
	POINT ptZero;
	ptZero.x = 0;
	ptZero.y = 0;
	HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFO monitorinfo;
	monitorinfo.cbSize = sizeof(monitorinfo);
	GetMonitorInfo(hmonPrimary, &monitorinfo);


	// center the splash screen in the middle of the primary work area
	const RECT & rcWork = monitorinfo.rcWork;
	POINT ptOrigin;
	ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - imgSize.cx) / 2;
	ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - imgSize.cy) / 2;


	{
		// create window class
		WNDCLASSEX wc;// = {0};
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = 0;
		wc.lpfnWndProc   = DefWindowProc;//DefWindowProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
		wc.lpszMenuName  = nullptr;
		wc.lpszClassName = g_szClassName;
		wc.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);
		if(!RegisterClassEx(&wc)) {
			setStatus(ERROR_STATUS);
			MessageBox(nullptr, "Window Registration Failed!", "Error!",
					   MB_ICONEXCLAMATION | MB_OK);
			return;
		}
	}


	// create the window
	HWND hwnd = CreateWindowEx(
			   WS_EX_LAYERED|
			   WS_EX_TOPMOST | // always on top
			   WS_EX_TRANSPARENT, // ignore mouse events
			   g_szClassName,
			   "Splash",
			   WS_POPUP,
//			   WS_OVERLAPPEDWINDOW,
			   ptOrigin.x, ptOrigin.y, imgSize.cx, imgSize.cy,
			   nullptr, nullptr, hInstance, nullptr);

	if(hwnd == nullptr) {
		setStatus(ERROR_STATUS);
		MessageBox(nullptr, "Window Creation Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return ;
	}

	// create a memory DC holding the splash bitmap
	HDC hdcScreen = GetDC(nullptr);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	//    g_hBackgroundDC = CreateCompatibleDC(GetDC(nullptr));
	//HBITMAP g_hBackgroundBitmap = CreateCompatibleBitmap(hdcScreen,imgSize.cx,imgSize.cy);
	//SelectObject(hdcMem,g_hBackgroundBitmap);

	//----------------------------------------
	// create the bitmap

	HBITMAP hbmp = nullptr;
	{
		// prepare structure giving bitmap information (negative height indicates a top-down DIB)
		BITMAPINFO bminfo;
		ZeroMemory(&bminfo, sizeof(bminfo));
		bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bminfo.bmiHeader.biWidth = imgSize.cx;
		bminfo.bmiHeader.biHeight = -(static_cast<LONG>(imgSize.cy));
		bminfo.bmiHeader.biPlanes = 1;
		bminfo.bmiHeader.biBitCount = 32;
		bminfo.bmiHeader.biCompression = BI_RGB;


		// create a DIB section that can hold the image
		void * pvImageBits = nullptr;

		hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);

		ReleaseDC(nullptr, hdcScreen);

		if (hbmp == nullptr) {
			setStatus(ERROR_STATUS);
			MessageBox(nullptr, "CreateDIBSection Failed!", "Error!",
					   MB_ICONEXCLAMATION | MB_OK);
			return ;
		}
		Reference<PixelAccessor> source = PixelAccessor::create(splashImage.get());
		if(source.isNull()) {
			setStatus(ERROR_STATUS);
			MessageBox(nullptr, "createPixelAccessor Failed!", "Error!",
					   MB_ICONEXCLAMATION | MB_OK);
			return ;
		}

		uint8_t * imgPtr = static_cast<uint8_t *>(pvImageBits);
		for(int y=0; y<imgSize.cy; ++y) {
			for(int x=0; x<imgSize.cx; ++x) {
				const Color4ub c = source->readColor4ub(x,y);
				const uint8_t a = c.getA();
				if(a==0){ // (Cl) not sure why this is needed. Without it, white pixels with alpha=0 stay white!
					imgPtr[0]=0;
					imgPtr[1]=0;
					imgPtr[2]=0;
					imgPtr[3]=0;

				}else{
					imgPtr[0]=c.getB();
					imgPtr[1]=c.getG();
					imgPtr[2]=c.getR();
					imgPtr[3]=c.getA();
				}
				imgPtr+=4;
			}
		}
	}
	//------------------------------------------
	HGDIOBJ  hbmpOld = SelectObject(hdcMem,hbmp);

	// use the source image's alpha channel for blending
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	// paint the window (in the right location) with the alpha-blended bitmap
	UpdateLayeredWindow(hwnd, hdcScreen, &ptOrigin, &imgSize,
						hdcMem,  &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA);//ULW_OPAQUE);//ULW_ALPHA);


	// delete temporary objects
	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);
	ReleaseDC(nullptr, hdcScreen);

//	// show the window
	ShowWindow(hwnd, SW_SHOWNORMAL);//nCmdShow);

	initDone = true;
	setStatus(RUNNING_STATUS);
	while(getStatus() == RUNNING_STATUS) {
//		std::cout<< "#";
		Utils::sleep(100);

		MSG Msg;
		while(PeekMessage(&Msg, hwnd, 0, 0,PM_REMOVE) != 0) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
//			std::cout<< "M";
		}
	}
	DestroyWindow(hwnd);
}

//! (ctor)
SplashScreenWin::SplashScreenWin(const std::string & /*splashTitle*/, const Reference<Bitmap> & _splashImage) : 
	SplashScreen(), splashImage(_splashImage) {
	if(splashImage.isNotNull()) {
		splashImage = new Bitmap(*_splashImage.get());
		start();
		while(!initDone && isActive());
	}
}

//! (dtor)
SplashScreenWin::~SplashScreenWin() {
	if(isActive()) {
		setStatus(CLOSING_STATUS);
		join();
	}
}

}
}

#endif /* defined(_WIN32) */
