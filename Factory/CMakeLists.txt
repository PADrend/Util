#
# This file is part of the Util library.
# Copyright (C) 2017 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#

# Install the header files
install(FILES
	Factory.h
	FallbackPolicies.h
	LambdaFactory.h
	ObjectCache.h
	WrapperFactory.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Factory
	COMPONENT headers
)
