#
# This file is part of the Util library.
# Copyright (C) 2009-2017 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
Cursor.cpp
EventContext.cpp
EventQueue.cpp
SplashScreenWin.cpp
SplashScreenX11.cpp
UI.cpp
WindowEGL.cpp
WindowGLX.cpp
WindowSDL.cpp
WindowX11.cpp
WindowX11Data.cpp
)
# Install the header files
install(FILES
	Cursor.h
	Event.h
	EventContext.h
	EventQueue.h
	SplashScreen.h
	SplashScreenWin.h
	SplashScreenX11.h
	UI.h
	Window.h
	WindowEGL.h
	WindowGLX.h
	WindowSDL.h
	WindowX11.h
	WindowX11Data.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/UI
	COMPONENT headers
)
