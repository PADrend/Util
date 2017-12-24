#
# This file is part of the Util library.
# Copyright (C) 2009-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
	Graphics/Bitmap.cpp
	Graphics/BitmapUtils.cpp
	Graphics/ColorLibrary.cpp
	Graphics/EmbeddedFont.cpp
	Graphics/FontRenderer.cpp
	Graphics/NoiseGenerator.cpp
	Graphics/PixelAccessor.cpp
	Graphics/PixelFormat.cpp
)
# Install the header files
install(FILES
	Bitmap.h
	BitmapUtils.h
	Color.h
	ColorLibrary.h
	EmbeddedFont.h
	FontRenderer.h
	NoiseGenerator.h
	PixelAccessor.h
	PixelFormat.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Graphics
	COMPONENT headers
)
