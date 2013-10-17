/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_EGL)

#include "WindowEGL.h"
#include "WindowX11Data.h"
#include "../StringUtils.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <EGL/egl.h>
#include <memory>
#include <stdexcept>

namespace Util {
namespace UI {

extern Key keySymToKey(KeySym sym);

struct WindowEGL::WindowEGLData {
		EGLDisplay display;
		EGLContext context;
		EGLSurface surface;

		WindowEGLData() :
			display(EGL_NO_DISPLAY), context(EGL_NO_CONTEXT), surface(EGL_NO_SURFACE) {
		}

		~WindowEGLData() {
			if (display != EGL_NO_DISPLAY) {
				eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
				if (surface != EGL_NO_SURFACE) {
					eglDestroySurface(display, surface);
				}
				if (context != EGL_NO_CONTEXT) {
					eglDestroyContext(display, context);
				}
				eglTerminate(display);
			}
		}
};

WindowEGL::WindowEGL(const Window::Properties & properties) :
		WindowX11(properties), eglData(new WindowEGLData) {

	eglData->display = eglGetDisplay(x11Data->display);
	if (eglData->display == EGL_NO_DISPLAY) {
		throw std::runtime_error("Failed to open display.");
	}

	EGLint versionMajor;
	EGLint versionMinor;
	if (eglInitialize(eglData->display, &versionMajor, &versionMinor) == EGL_FALSE) {
		throw std::runtime_error("Failed to initialize display.");
	}

	// EGL version 1.3 is needed for EGL_CONTEXT_CLIENT_VERSION
	if ((versionMajor < 1) || ((versionMajor == 1) && (versionMinor < 3))) {
		throw std::runtime_error("EGL version less than 1.3 detected.");
	}

	// Define attributes of desired framebuffer configurations
	EGLint fbAttribs[] = { EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER, EGL_BUFFER_SIZE, 24, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 8,
							EGL_NATIVE_RENDERABLE, EGL_TRUE, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };
	// Request a single framebuffer configuration
	EGLConfig fbConfig;
	EGLint fbCount;
	if (eglChooseConfig(eglData->display, fbAttribs, &fbConfig, 1, &fbCount) == EGL_FALSE) {
		throw std::runtime_error("Failed to retrieve a matching framebuffer configuration.");
	}

	if (fbCount == 0) {
		// FIXME: Workaround: Use eglGetConfigs instead of eglChooseConfig, because sometimes eglChooseConfig does not find matching configurations.
		EGLConfig fbConfigs[200];
		eglGetConfigs(eglData->display, fbConfigs, 200, &fbCount);
		for (EGLint i = 0; i < fbCount; ++i) {
			EGLint value;
			// We want to render into a window
			eglGetConfigAttrib(eglData->display, fbConfigs[i], EGL_SURFACE_TYPE, &value);
			if (!(value & EGL_WINDOW_BIT)) {
				continue;
			}
			// We want a configuration with a depth buffer
			eglGetConfigAttrib(eglData->display, fbConfigs[i], EGL_DEPTH_SIZE, &value);
			if (value == 0) {
				continue;
			}
			fbConfig = fbConfigs[i];
		}
	}

	if (fbCount == 0) {
		throw std::runtime_error("No matching framebuffer configurations found.");
	}

	EGLint visualID;
	eglGetConfigAttrib(eglData->display, fbConfig, EGL_NATIVE_VISUAL_ID, &visualID);

	const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglData->context = eglCreateContext(eglData->display, fbConfig, EGL_NO_CONTEXT, contextAttribs);
	if (eglData->context == EGL_NO_CONTEXT) {
		throw std::runtime_error("Failed to create OpenGL ES 2.x context. " + 
								 StringUtils::toString(eglGetError()));
	}

	// Create X11 window
	XVisualInfo templateInfo;
	templateInfo.visualid = visualID;
	int visualCount;
	XVisualInfo * visualsInfo = XGetVisualInfo(x11Data->display, VisualIDMask, &templateInfo, &visualCount);
	if (visualsInfo == nullptr) {
		throw std::runtime_error("Failed to find a matching visual.");
	} else if (visualCount != 1) {
		XFree(visualsInfo);
		throw std::runtime_error("More than one visual found.");
	}

	x11Data->colorMap = XCreateColormap(x11Data->display, RootWindow(x11Data->display, visualsInfo[0].screen), visualsInfo[0].visual, AllocNone);
	x11Data->freeColorMap = true;

	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = x11Data->colorMap;
	windowAttribs.background_pixmap = None;
	windowAttribs.border_pixel = 0;
	windowAttribs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	x11Data->window = XCreateWindow(x11Data->display, RootWindow(x11Data->display, visualsInfo[0].screen), 
								 0, 0, properties.clientAreaWidth, properties.clientAreaHeight, 
								 0, visualsInfo[0].depth,
								 InputOutput, visualsInfo[0].visual, CWBorderPixel | CWColormap | CWEventMask, &windowAttribs);

	XFree(visualsInfo);

	if (!x11Data->window) {
		throw std::runtime_error("Failed to create window.");
	} else {
		x11Data->freeWindow = true;
	}

	if (properties.borderless) {
		x11Data->removeWindowBorder();
	}
	if (!properties.resizable) {
		x11Data->fixWindowSize(static_cast<int>(properties.clientAreaWidth), 
							   static_cast<int>(properties.clientAreaHeight));
	}

	x11Data->inputMethod = XOpenIM(x11Data->display, nullptr, nullptr, nullptr);
	if (x11Data->inputMethod == nullptr) {
		throw std::runtime_error("Failed to create input method.");
	}

	x11Data->inputContext = XCreateIC(x11Data->inputMethod,
								   XNInputStyle, XIMPreeditNone | XIMStatusNone,
								   XNClientWindow, x11Data->window,
								   XNFocusWindow, x11Data->window,
								   nullptr);
	if (x11Data->inputContext == nullptr) {
		throw std::runtime_error("Failed to create input context.");
	}

	XStoreName(x11Data->display, x11Data->window, properties.title.c_str());

	XMapWindow(x11Data->display, x11Data->window);
	if (properties.positioned) {
		XMoveWindow(x11Data->display, x11Data->window, properties.posX, properties.posY);
	}

	eglData->surface = eglCreateWindowSurface(eglData->display, fbConfig, x11Data->window, nullptr);
	if (eglData->surface == EGL_NO_SURFACE) {
		throw std::runtime_error("Failed to create window surface.");
	}

	eglMakeCurrent(eglData->display, eglData->surface, eglData->surface, eglData->context);
}

WindowEGL::~WindowEGL() = default;

void WindowEGL::swapBuffers() {
	eglSwapBuffers(eglData->display, eglData->surface);
}

}
}

#endif /* defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_EGL) */
