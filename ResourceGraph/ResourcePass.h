/*
 * This file is part of the proprietary part of the
 * Platform for Algorithm Development and Rendering (PADrend).
 * Web page: http://www.padrend.de/
 * Copyright (C) 2022 Sascha Brandt <sascha@brandt.graphics>
 *
 * PADrend consists of an open source part and a proprietary part.
 * For the proprietary part of PADrend all rights are reserved.
 */
#ifndef UTIL_RESOURCEGRAPH_RESOURCEPASS_H_
#define UTIL_RESOURCEGRAPH_RESOURCEPASS_H_

#include "../StringIdentifier.h"
#include "../Resources/ResourceFormat.h"

#include <unordered_set>

namespace Util {
class GraphBuilder;

struct ResourceDescriptor {
	StringIdentifier name;
	ResourceFormat format;
	// TODO: dimensions, flags, ...
	// TODO: what about read/write resources?
};
// TODO: should the pass declare on which device/queue it will/wants to run?

class ResourcePass {
public:
	virtual void setup() {}
	virtual void execute() {}

	void addReadDependency(const ResourceDescriptor& resource);
	void addWriteDependency(const ResourceDescriptor& resource);
private:
	std::unordered_set<ResourceDescriptor> readResources;
	std::unordered_set<ResourceDescriptor> writeResources;
};

} // namespace Util

#endif // UTIL_RESOURCEGRAPH_RESOURCEPASS_H_