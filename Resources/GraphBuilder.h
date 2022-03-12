/*
	This file is part of the Platform for Algorithm Development and Rendering (PADrend).
	Web page: http://www.padrend.de/
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2014-2022 Sascha Brandt <sascha@brandt.graphics>

	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCEGRAPH_GRAPHBUILDER_H_
#define UTIL_RESOURCEGRAPH_GRAPHBUILDER_H_

#include <vector>
#include <memory>

namespace Util {
class ResourcePass;
using ResourcePassRef = std::shared_ptr<ResourcePass>;

class GraphBuilder {
public:
	GraphBuilder();
	~GraphBuilder();

	void addPass(const ResourcePassRef& pass);

	
private:
	std::vector<ResourcePassRef> passes;
};

} // Util

#endif // UTIL_RESOURCEGRAPH_GRAPHBUILDER_H_