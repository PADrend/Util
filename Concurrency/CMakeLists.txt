#
# This file is part of the Util library.
# Copyright (C) 2009-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
add_sources(Util
	Concurrency.cpp
	CppMutex.cpp
	CppSemaphore.cpp
	CppThread.cpp
	DummyMutex.cpp
	DummySemaphore.cpp
	DummySpinLock.cpp
	DummyThread.cpp
	POSIXMutex.cpp
	POSIXSemaphore.cpp
	POSIXSpinLock.cpp
	POSIXThread.cpp
	SDLMutex.cpp
	SDLSemaphore.cpp
	SDLThread.cpp
	UserThread.cpp
)
