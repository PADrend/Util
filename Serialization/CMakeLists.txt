#
# This file is part of the Util library.
# Copyright (C) 2009-2017 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
	Serialization.cpp
	StreamerPNG.cpp
	StreamerSDL.cpp
	StreamerSDLImage.cpp
	StreamerTGA.cpp
)
# Install the header files
install(FILES
	AbstractBitmapStreamer.h
	AbstractStreamer.h
	Serialization.h
	StreamerPNG.h
	StreamerSDL.h
	StreamerSDLImage.h
	StreamerTGA.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Serialization
	COMPONENT headers
)
