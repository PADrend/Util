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

#include <unordered_map>
#include <memory>

using namespace Util;

struct LibraryHandle {
  StringIdentifier id;
  void* handle;
};

typedef std::unordered_map<StringIdentifier, LibraryHandle> LibraryHandles_t;

static LibraryHandles_t & getLibraryHandles() {
	static std::unique_ptr<LibraryHandles_t> libraryHandles(new LibraryHandles_t);
	return *libraryHandles.get();
}

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

const StringIdentifier Util::loadLibrary(const std::string& filename) {
	WARN("Util::loadLibrary requires SDL to work!");
	return StringIdentifier(0);
}

void* Util::loadFunction(const StringIdentifier& libraryId, const std::string& name) {
	WARN("Util::loadFunction requires SDL to work!");
	return nullptr;
}

void Util::unloadLibrary(const StringIdentifier& libraryId) {
	WARN("Util::unloadLibrary requires SDL to work!");
}

#endif