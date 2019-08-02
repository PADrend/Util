#
# This file is part of the Util library.
# Copyright (C) 2009-2015 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
cmake_minimum_required(VERSION 3.1.0)

add_executable(WindowEvents
	WindowEventsMain.cpp
)

set_target_properties(WindowEvents PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_link_libraries(WindowEvents PRIVATE Util)
