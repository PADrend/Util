/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_SERIAL

#ifndef SERIAL_PROVIDER_H
#define	SERIAL_PROVIDER_H

#include <list>

#include "AbstractFSProvider.h"

namespace Util {


/*! Access the the serial port using a file system provider.
	
		serial://ComportName:BaudRate(*9600*):Parity(n...*none*|o..odd|e..even):Stopbits(*1*|2):flowcontrol(n..*none*|s..software|h..hardware)
	e.g.:
		serial://com2
		serial://com20:14400:n:2:h
		serial://com1:115200:::s

	[SerialProvider] ---|> [AbstractFSProvider]
	@ingroup io
*/
class SerialProvider : public AbstractFSProvider {
	public:
		UTILAPI static bool init();

		
		SerialProvider() : AbstractFSProvider() {}
		virtual ~SerialProvider(){}

		UTILAPI std::unique_ptr<std::iostream> open(const FileName & ) override;
		UTILAPI std::unique_ptr<std::istream> openForReading(const FileName & )override;
		UTILAPI std::unique_ptr<std::ostream> openForWriting(const FileName & )override;
		
		UTILAPI bool isFile(const FileName &)override;

//		status_t readFile(const FileName & file, std::vector<uint8_t> & data) override;
		UTILAPI status_t writeFile(const FileName &, const std::vector<uint8_t> & /*data*/, bool /*overwrite*/) override;

//		/*! Returns a list of available com ports if @p flags contains DIR_FILES.
//			The @p path is ignored.	*/
//		status_t dir(const FileName &/*path*/, std::list<FileName> &result, uint8_t flags) override;
};
}
#endif	/* SERIAL_PROVIDER_H */
#endif	/* UTIL_HAVE_LIB_SERIAL */
