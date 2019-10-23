#
# This file is part of the Util library.
# Copyright (C) 2009-2017 Benjamin Eikel <benjamin@eikel.org>
# Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
	Resources/AttributeAccessor.cpp
	Resources/ResourceAccessor.cpp
	Resources/ResourceFormat.cpp
)
# Install the header files
install(FILES
	DataStrategy.h
	Resource.h
	ResourceAllocator.h
	ResourceFormat.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Resources
	COMPONENT headers
)
install(FILES
	Accessors/AttributeAccessor.h
	Accessors/ResourceAccessor.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Resources/Accessors
	COMPONENT headers
)
