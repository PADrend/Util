/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "PixelFormat.h"

namespace Util {
namespace PixelFormat {

const AttributeFormat RG({"RG"}, TypeConstant::UINT8, 2, false, 0);
const AttributeFormat RG_FLOAT({"RG_FLOAT"}, TypeConstant::FLOAT, 2, false, 0);
const AttributeFormat RG_INT32({"RG_INT32"}, TypeConstant::INT32, 2, false, 0);
const AttributeFormat RG_UINT32({"RG_UINT32"}, TypeConstant::UINT32, 2, false, 0);
const AttributeFormat RGB({"RGB"}, TypeConstant::UINT8, 3, false, 0);
const AttributeFormat RGB_FLOAT({"RGB_FLOAT"}, TypeConstant::FLOAT, 3, false, 0);
const AttributeFormat RGB_INT32({"RGB_INT32"}, TypeConstant::INT32, 3, false, 0);
const AttributeFormat RGB_UINT32({"RGB_UINT32"}, TypeConstant::UINT32, 3, false, 0);
const AttributeFormat BGR({"BGR"}, TypeConstant::UINT8, 3, false, INTERNAL_TYPE_BGRA);
const AttributeFormat BGR_FLOAT({"BGR_FLOAT"}, TypeConstant::FLOAT, 3, false,INTERNAL_TYPE_BGRA);
const AttributeFormat BGR_INT32({"BGR_INT32"}, TypeConstant::INT32, 3, false,INTERNAL_TYPE_BGRA);
const AttributeFormat BGR_UINT32({"BGR_UINT32"}, TypeConstant::UINT32, 3, false,INTERNAL_TYPE_BGRA);
const AttributeFormat RGBA({"RGBA"}, TypeConstant::UINT8, 4, false, 0);
const AttributeFormat RGBA_FLOAT({"RGBA_FLOAT"}, TypeConstant::FLOAT, 4, false, 0);
const AttributeFormat RGBA_INT32({"RGBA_INT32"}, TypeConstant::INT32, 4, false, 0);
const AttributeFormat RGBA_UINT32({"RGBA_UINT32"}, TypeConstant::UINT32, 4, false, 0);
const AttributeFormat BGRA({"BGRA"}, TypeConstant::UINT8, 4, false, INTERNAL_TYPE_BGRA);
const AttributeFormat BGRA_FLOAT({"BGRA_FLOAT"}, TypeConstant::FLOAT, 4, false, INTERNAL_TYPE_BGRA);
const AttributeFormat BGRA_INT32({"BGRA_INT32"}, TypeConstant::INT32, 4, false, INTERNAL_TYPE_BGRA);
const AttributeFormat BGRA_UINT32({"BGRA_UINT32"}, TypeConstant::UINT32, 4, false, INTERNAL_TYPE_BGRA);
const AttributeFormat MONO({"MONO"}, TypeConstant::UINT8, 1, false, 0);
const AttributeFormat MONO_FLOAT({"MONO_FLOAT"}, TypeConstant::FLOAT, 1, false, 0);
const AttributeFormat MONO_INT32({"MONO_INT32"}, TypeConstant::INT32, 1, false, 0);
const AttributeFormat MONO_UINT32({"MONO_UINT32"}, TypeConstant::UINT32, 1, false, 0);
const AttributeFormat R11G11B10_FLOAT({"R11G11B10_FLOAT"}, TypeConstant::UINT32, 1, false, INTERNAL_TYPE_R11G11B10_FLOAT);
const AttributeFormat UNKNOWN({"UNKNOWN"}, TypeConstant::UINT8, 0, false, 0);

} /* PixelFormat */
}
