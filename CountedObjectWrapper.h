/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef COUNTED_OBJECT_WRAPPER_H
#define COUNTED_OBJECT_WRAPPER_H

#include "ReferenceCounter.h"
#include <utility>

namespace Util {

template<class _T>
class CountedObjectWrapper : public ReferenceCounter<CountedObjectWrapper<_T>>{
		_T obj;
	public:
		template<typename...args> explicit CountedObjectWrapper(args&&... params) : obj(std::forward<args>(params)...){}
		~CountedObjectWrapper() = default;
		CountedObjectWrapper & operator=(CountedObjectWrapper&) = delete;
		
		_T & operator*()					{	return obj;	}
		const _T & operator*() const		{	return obj;	}
		_T * operator->() 					{	return &obj;	}
		const _T * operator->() const		{	return &obj;	}
		_T & get()  						{	return obj;	}
		const _T & get() const 				{	return obj;	}
};

}

#endif // COUNTED_OBJECT_WRAPPER_H
