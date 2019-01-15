/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_CURL

#ifndef _NET_PROVIDER_H
#define	_NET_PROVIDER_H

#include "AbstractFSProvider.h"

// Forward declaration
typedef void CURL;

namespace Util {

/**
 * File system provider for network protocols (FTP, HTTP, etc.) using libcurl.
 *
 * @author Benjamin Eikel
 * @date 2011-01-21
 * @ingroup io
 */
class NetProvider : public AbstractFSProvider {
	public:
		static bool init();

		NetProvider();
		virtual ~NetProvider();

		status_t readFile(const FileName & url, std::vector<uint8_t> & data) override;

		bool isFile(const FileName & url) override;

	private:
		//! Handle from libcurl that is re-used.
		CURL * handle;
};

}

#endif/* _NET_PROVIDER_H */

#endif /* UTIL_HAVE_LIB_CURL */
