/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_GLX) and defined(UTIL_HAVE_GLX_GETPROCADDRESSARB)

#include "WindowGLX.h"
#include "WindowX11Data.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <memory>
#include <unordered_map>
#include <stdexcept>

typedef void (*glXFunctionPointer)(void);
typedef glXFunctionPointer (*glXGetProcAddressARBProc)(const GLubyte *);

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

// Taken from glxext.h
#ifndef GLX_ARB_create_context
#define GLX_CONTEXT_DEBUG_BIT_ARB          0x00000001
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB      0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB      0x2092
#define GLX_CONTEXT_FLAGS_ARB              0x2094
#endif

#ifndef GLX_ARB_create_context_profile
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB   0x00000001
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GLX_CONTEXT_PROFILE_MASK_ARB       0x9126
#endif

#ifndef GLX_EXT_swap_control
#define GLX_SWAP_INTERVAL_EXT              0x20F1
#define GLX_MAX_SWAP_INTERVAL_EXT          0x20F2
#endif

namespace Util {
namespace UI {

static bool isExtensionSupported(const char * extensionList, const char * extension) {
	const std::string extensions(extensionList);
	std::size_t pos = extensions.find(extension);
	return (pos != std::string::npos);
}

struct WindowGLX::WindowGLXData {
	Display * display;
	GLXContext context;

	static bool errorOccurred;

	WindowGLXData() :
		display(nullptr), context(nullptr) {
	}

	~WindowGLXData() {
		if(context != nullptr) {
			glXDestroyContext(display, context);
		}
	}

	static int handleError(Display * /*display*/, XErrorEvent * /*event*/) {
		errorOccurred = true;
		return 0;
	}
};
bool WindowGLX::WindowGLXData::errorOccurred = false;

WindowGLX::WindowGLX(const Window::Properties & properties) :
		WindowX11(properties), glxData(new WindowGLXData) {
	glxData->display = x11Data->display;

	// At least GLX version 1.3 is required for glXChooseFBConfig.
	int glxVersionMajor, glxVersionMinor;
	if (!glXQueryVersion(x11Data->display, &glxVersionMajor, &glxVersionMinor)) {
		throw std::runtime_error("Failed to retrieve GLX version.");
	}

	if ((glxVersionMajor < 1) || ((glxVersionMajor == 1) && (glxVersionMinor < 3))) {
		throw std::runtime_error("GLX version less than 1.3 detected.");
	}

	// Define attributes of desired framebuffer configurations
	int fbAttribs[] = 	{
							GLX_X_RENDERABLE, True,
							GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
							GLX_RENDER_TYPE, GLX_RGBA_BIT,
							GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
							GLX_RED_SIZE, 8,
							GLX_GREEN_SIZE, 8,
							GLX_BLUE_SIZE, 8,
							GLX_ALPHA_SIZE, 8,
							GLX_DEPTH_SIZE, 24,
							GLX_DOUBLEBUFFER, True,
							None
						};

	int fbCount;
	GLXFBConfig * fbConfigs = glXChooseFBConfig(x11Data->display, XDefaultScreen(x11Data->display), fbAttribs, &fbCount);
	if (fbConfigs == nullptr) {
		throw std::runtime_error("Failed to retrieve a matching framebuffer configuration.");
	}

	// Use the first framebuffer configuration
	GLXFBConfig fbConfig = fbConfigs[0];
	XVisualInfo * visualInfo = glXGetVisualFromFBConfig(x11Data->display, fbConfig);

	XFree(fbConfigs);

	XSetWindowAttributes windowAttribs;
	x11Data->colorMap = XCreateColormap(x11Data->display, XRootWindow(x11Data->display, visualInfo->screen), visualInfo->visual, AllocNone);
	x11Data->freeColorMap = true;
	windowAttribs.colormap = x11Data->colorMap;
	windowAttribs.background_pixmap = None;
	windowAttribs.border_pixel = 0;
	windowAttribs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	x11Data->window = XCreateWindow(x11Data->display, XRootWindow(x11Data->display, visualInfo->screen), 
								 0, 0, properties.clientAreaWidth, properties.clientAreaHeight, 
								 0, visualInfo->depth, InputOutput,
								 visualInfo->visual, CWBorderPixel | CWColormap | CWEventMask, &windowAttribs);
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

	// The window should receive a message when the close/"X" button is clicked.
	x11Data->deleteMessage = XInternAtom(x11Data->display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(x11Data->display, x11Data->window, &x11Data->deleteMessage, 1);


	XMapWindow(x11Data->display, x11Data->window);
	if (properties.positioned) {
		XMoveWindow(x11Data->display, x11Data->window, properties.posX, properties.posY);
	}

	// Get the GLX extensions supported by the default screen
	const char * glxExtensions = glXQueryExtensionsString(x11Data->display, visualInfo->screen);

	XFree(visualInfo);

	// Get the function pointer to create an advanced OpenGL context.
	glXGetProcAddressARBProc glXGetProcAddressARB = reinterpret_cast<glXGetProcAddressARBProc>(glXGetProcAddress(
			reinterpret_cast<const GLubyte *>("glXGetProcAddressARB")));
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARBProc>(glXGetProcAddressARB(
			reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")));


	// Register an error handler that is called in the case that the context creation fails.
	WindowGLXData::errorOccurred = false;
	int (*oldErrorHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&WindowGLXData::handleError);

	if (!isExtensionSupported(glxExtensions, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
		// Use GLX 1.3 context creation
		glxData->context = glXCreateNewContext(x11Data->display, fbConfig, GLX_RGBA_TYPE, nullptr, True);
	} else {
		// Use advanced context creation
		int profileMask = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
		if (properties.compatibilityProfile) {
			profileMask |= GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
		int contextFlags = GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		if (properties.debug) {
			contextFlags |= GLX_CONTEXT_DEBUG_BIT_ARB;
		}
		const int contextAttribs[] = 	{
											GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
											GLX_CONTEXT_MINOR_VERSION_ARB, 3,
											GLX_CONTEXT_PROFILE_MASK_ARB, profileMask,
											GLX_CONTEXT_FLAGS_ARB, contextFlags,
											None
										};
		glxData->context = glXCreateContextAttribsARB(x11Data->display, fbConfig, nullptr, True, contextAttribs);


		// Make sure that errors are reported
		XSync(x11Data->display, False);
		if (WindowGLXData::errorOccurred || !glxData->context) {
			// Try to create a context that is backwards compatible with OpenGL 1.0.
			// This will create the newest version available.
			WindowGLXData::errorOccurred = false;
			glxData->context = glXCreateContextAttribsARB(x11Data->display, fbConfig, nullptr, True, nullptr);
		}
	}

	// Make sure that errors are reported
	XSync(x11Data->display, False);

	XSetErrorHandler(oldErrorHandler);

	if (WindowGLXData::errorOccurred || !glxData->context) {
		throw std::runtime_error("Failed to create OpenGL context.");
	} else {
		x11Data->freeContext = true;
	}

	// Check if direct rendering is supported
	if (!glXIsDirect(x11Data->display, glxData->context)) {
		throw std::runtime_error("Indirect GLX rendering context obtained only.");
	}
	glXMakeCurrent(x11Data->display, x11Data->window, glxData->context);
}

WindowGLX::~WindowGLX() = default;

void WindowGLX::swapBuffers() {
	glXSwapBuffers(x11Data->display, x11Data->window);
}

int32_t WindowGLX::getSwapInterval() const {
	unsigned int interval;
	glXQueryDrawable(x11Data->display, x11Data->window, GLX_SWAP_INTERVAL_EXT, &interval);
	return static_cast<int32_t>(interval);
}

}
}

#endif /* defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_GLX) and defined(UTIL_HAVE_GLX_GETPROCADDRESSARB) */
