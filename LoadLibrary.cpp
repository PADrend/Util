/*
	This file is part of the Util library.
	Copyright (C) 2017 Sascha Brandt <myeti@mail.upb.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "LoadLibrary.h"
#include "Macros.h"

#if defined(UTIL_HAVE_LIB_SDL2)

COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL_loadso.h>
COMPILER_WARN_POP
#elif defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#include <stdio.h>
#include <dlfcn.h>
#endif

#include <unordered_map>
#include <memory>
#include <sstream>

using namespace Util;

struct LibraryHandle {
	StringIdentifier id;
#if defined(_WIN32)
	HINSTANCE handle;
#else
	void* handle;
#endif
};

typedef std::unordered_map<StringIdentifier, LibraryHandle> LibraryHandles_t;

static LibraryHandles_t & getLibraryHandles() {
	static std::unique_ptr<LibraryHandles_t> libraryHandles(new LibraryHandles_t);
	return *libraryHandles.get();
}

#if defined(UTIL_HAVE_LIB_SDL2)

const StringIdentifier Util::loadLibrary(const std::string& filename) {
	StringIdentifier libraryId(filename);
	auto & libraryHandles = getLibraryHandles();
	auto entry = libraryHandles.find(libraryId);
	
	if (entry == libraryHandles.cend()) {
		void* handle = SDL_LoadObject(filename.c_str());
		if(!handle) {
			WARN(std::string("Util::loadLibrary failed: ") + SDL_GetError());
			return StringIdentifier(0);
		}
		libraryHandles[libraryId] = {libraryId, handle};
	} 
	return libraryId;
}

void* Util::loadFunction(const StringIdentifier& libraryId, const std::string& name) {
	auto & libraryHandles = getLibraryHandles();
	auto entry = libraryHandles.find(libraryId);
	
	if (entry == libraryHandles.cend()) {
		WARN(std::string("Util::loadFunction failed: ") + libraryId.toString() + std::string(" not loaded!"));
		return nullptr;
	}
	
	void* fnHandle = SDL_LoadFunction(entry->second.handle, name.c_str());
	if(!fnHandle) {
		WARN(std::string("Util::loadFunction failed: ") + SDL_GetError());
	}
	return fnHandle;
}

void Util::unloadLibrary(const StringIdentifier& libraryId) {
	auto & libraryHandles = getLibraryHandles();
	auto entry = libraryHandles.find(libraryId);
	
	if (entry != libraryHandles.cend()) {
		SDL_UnloadObject(entry->second.handle);
		libraryHandles.erase(entry);
	}
}

#else

using namespace Util;

const StringIdentifier Util::loadLibrary(const std::string& filename) {
	StringIdentifier libraryId(filename);
	auto & libraryHandles = getLibraryHandles();
	auto entry = libraryHandles.find(libraryId);
	std::ostringstream err;
	
	if (entry == libraryHandles.cend()) {		
		#if defined(_WIN32) || defined(_WIN64)
			HINSTANCE handle = LoadLibrary(filename.c_str());
			if(!handle) {
				DWORD errorMessageID = GetLastError();
				LPSTR messageBuffer = nullptr;
				size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
																		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
				std::string message(messageBuffer, size);
				LocalFree(messageBuffer);
				err << message;
			}
		#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
			std::string libPath = filename;
			if(libPath.find("/") == std::string::npos)
				libPath = "./" + libPath; // need to prepend "./" for some distributions
			void* handle = dlopen(libPath.c_str(), RTLD_NOW|RTLD_LOCAL);
			if(!handle) err << dlerror();
		#else
			void* handle = nullptr;
		#endif
		
		if(!handle) {
			WARN(std::string("Util::loadLibrary failed for file '" + filename + "': " + err.str()));
			return StringIdentifier(0);
		}
		libraryHandles[libraryId] = {libraryId, handle};
	}
	return libraryId;
}

void* Util::loadFunction(const StringIdentifier& libraryId, const std::string& name) {
	auto & libraryHandles = getLibraryHandles();
	auto entry = libraryHandles.find(libraryId);
	std::ostringstream err;
	
	if (entry == libraryHandles.cend()) {
		WARN(std::string("Util::loadFunction failed: ") + libraryId.toString() + std::string(" not loaded!"));
		return nullptr;
	}
	
	#if defined(_WIN32) || defined(_WIN64)
		void* fnHandle = reinterpret_cast<void*>(GetProcAddress(entry->second.handle, name.c_str()));
	#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
		void* fnHandle = dlsym(entry->second.handle, name.c_str());
		if(!fnHandle) err << dlerror();
	#endif
	if(!fnHandle) {
		WARN(std::string("Util::loadFunction failed: ") + err.str());
	}
	return fnHandle;
}

void Util::unloadLibrary(const StringIdentifier& libraryId) {
	auto & libraryHandles = getLibraryHandles();
	auto entry = libraryHandles.find(libraryId);
	
	if (entry != libraryHandles.cend()) {
		#if defined(_WIN32) || defined(_WIN64)
			FreeLibrary(entry->second.handle);
		#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
			dlclose(entry->second.handle);
		#endif
		libraryHandles.erase(entry);
	}
}

#endif
