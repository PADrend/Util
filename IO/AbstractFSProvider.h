/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _ABSTRACT_FS_PROVIDER_H
#define	_ABSTRACT_FS_PROVIDER_H

#include "../ReferenceCounter.h"
#include "../References.h"
#include <cstdint>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

namespace Util{
class FileName;

class AbstractFSProvider : public ReferenceCounter<AbstractFSProvider> {
public:
	AbstractFSProvider();
	virtual ~AbstractFSProvider();

	enum status_t{
		OK,FAILURE,UNSUPPORTED
	};
	static std::string getStatusMessage(const status_t status);

	virtual bool exists(const FileName &)       {   return false;   }
	virtual bool isFile(const FileName &)       {   return false;   }
	virtual bool isDir(const FileName &)        {   return false;   }
	virtual size_t fileSize(const FileName &)   {   return 0;   }

	virtual status_t makeDir(const FileName &)             {   return UNSUPPORTED; }
	virtual status_t makeDirRecursive(const FileName &)    {   return UNSUPPORTED; }

	virtual status_t remove(const FileName &)			   {   return UNSUPPORTED; }
	/*! standard implementation uses dir and remove calls to delete all contained files and dirs for their own*/
	virtual status_t removeRecursive(const FileName &);

	virtual status_t dir(const FileName &/*path*/, std::list<FileName> &/*result*/, uint8_t/*flags*/){	return UNSUPPORTED;	}

	virtual status_t readFile(const FileName &, std::vector<uint8_t> & /*data*/)                              {   return UNSUPPORTED; }
	virtual status_t writeFile(const FileName &, const std::vector<uint8_t> & /*data*/, bool /*overwrite*/)   {   return UNSUPPORTED; }

	virtual std::iostream * open(const FileName & )             {   return nullptr;    }
	virtual std::istream * openForReading(const FileName & )    {   return nullptr;    }
	virtual std::ostream * openForWriting(const FileName & )    {   return nullptr;    }
	virtual std::ostream * openForAppending(const FileName & )  {   return nullptr;    }

	/*! If some kind of internal caching is used, all data should be written to disk
		\note May block if needed	*/
	virtual void flush()  {   return ;    }
};
}

#endif	/* _ABSTRACT_FS_PROVIDER_H */
