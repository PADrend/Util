#
# This file is part of the MinSG library.
# Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
# Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#

target_sources(Util PRIVATE
	Profiling/Logger.cpp
	Profiling/Profiler.cpp
)

# Install the header files
install(FILES
	Logger.h
	Profiler.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Profiling
	COMPONENT headers
)
