/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Serialization.h"
#include "AbstractBitmapStreamer.h"
#include "../Factory/Factory.h"
#include "../Graphics/Bitmap.h"
#include "../IO/FileName.h"
#include "../IO/FileUtils.h"
#include "../Macros.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <memory>
#include <cstdint>

namespace Util {
namespace Serialization {

typedef Factory<AbstractBitmapStreamer *, std::string, std::function<AbstractBitmapStreamer * ()>, FallbackPolicies::NULLFallback> streamer_factory_t;

//! Local singleton function for factory creating streams loading bitmaps
static streamer_factory_t & getLoaderFactory() {
	static streamer_factory_t factory;
	return factory;
}

//! Local singleton function for factory creating streams saving bitmaps
static streamer_factory_t & getSaverFactory() {
	static streamer_factory_t factory;
	return factory;
}

bool registerBitmapLoader(const std::string & extension, std::function<AbstractBitmapStreamer * ()> loaderCreator) {
	return getLoaderFactory().registerType(extension, loaderCreator);
}

bool registerBitmapSaver(const std::string & extension, std::function<AbstractBitmapStreamer * ()> saverCreator) {
	return getSaverFactory().registerType(extension, saverCreator);
}

static std::string toLower(const std::string & extension) {
	std::string lowerExtension(extension);
	std::transform(extension.begin(), extension.end(), lowerExtension.begin(), ::tolower);
	return lowerExtension;
}

Reference<Bitmap> loadBitmap(const FileName & url) {
	std::unique_ptr<AbstractBitmapStreamer> loader(getLoaderFactory().create(toLower(url.getEnding())));
	if (loader.get() == nullptr) {
		WARN("No loader available.");
		return nullptr;
	}
	auto stream = FileUtils::openForReading(url);
	if(!stream) {
		WARN("Error opening stream for reading. Path: " + url.toString());
		return nullptr;
	}
	return std::move(loader->loadBitmap(*stream));
}

Reference<Bitmap> loadBitmap(const std::string & extension, const std::string & data) {
	std::unique_ptr<AbstractBitmapStreamer> loader(getLoaderFactory().create(toLower(extension)));
	if (loader.get() == nullptr) {
		WARN("No loader available.");
		return nullptr;
	}
	std::istringstream stream(data);
	return std::move(loader->loadBitmap(stream));
}

bool saveBitmap(const Bitmap & bitmap, const FileName & url) {
	std::unique_ptr<AbstractBitmapStreamer> saver(getSaverFactory().create(toLower(url.getEnding())));
	if (saver.get() == nullptr) {
		WARN("No saver available.");
		return false;
	}
	std::unique_ptr<std::ostream> stream(FileUtils::openForWriting(url));
	if(stream.get() == nullptr) {
		WARN("Error opening stream for writing. Path: " + url.toString());
		return false;
	}
	if (!saver->saveBitmap(bitmap, *stream)) {
		WARN(std::string("Saving failed."));
		return false;
	}
	return true;
}

bool saveBitmap(const Bitmap & bitmap, const std::string & extension, std::ostream & output) {
	std::unique_ptr<AbstractBitmapStreamer> saver(getSaverFactory().create(toLower(extension)));
	if (saver.get() == nullptr) {
		WARN("No saver available.");
		return false;
	}
	if (!saver->saveBitmap(bitmap, output)) {
		WARN("Saving failed.");
		return false;
	}
	return true;
}

}
}
