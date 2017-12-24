#
# This file is part of the Util library.
# Copyright (C) 2009-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
	UI/Cursor.cpp
	UI/EventContext.cpp
	UI/EventQueue.cpp
	UI/UI.cpp
	UI/SplashScreenWin.cpp
	UI/SplashScreenX11.cpp
	UI/WindowEGL.cpp
	UI/WindowGLX.cpp
	UI/WindowSDL.cpp
	UI/WindowX11.cpp
	UI/WindowX11Data.cpp
)
