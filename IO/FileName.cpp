/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "FileName.h"
#include <algorithm>
#include <iostream>

using std::string;

namespace Util {

/*! (static) */
FileName FileName::createDirName(const string & dirname) {
	std::string s=dirname;
	if( !s.empty() && s.at(s.length()-1) != '/' )
		s+='/';
	return FileName(s);
}

/**
 * (internal)
 */
void FileName::parse(const string & s) {
	if(s.empty())
		return;
	std::string s2;
	const size_t fsPos=s.find("://");
	if(fsPos!=string::npos){
		fsName=s.substr(0,fsPos);
		s2=s.substr(fsPos+3);
	}else{
		fsName = "file";
		s2=s;
	}

	std::replace(s2.begin(), s2.end(), '\\', '/');

	size_t index = s2.rfind("/");
	if(index == string::npos){
		setDir("");
		setFile(s2);
	}    else{
		setDir(s2.substr(0,index+1));
		setFile(s2.substr(index+1));
	}
}
/**
 *
 */
string FileName::getEnding()const{
	size_t pointPos=file.rfind('.');
	return pointPos==string::npos ? "" : file.substr(pointPos+1);
}

void FileName::setEnding(const string & ending) {
	string newFile(file);
	string newEnding(ending);
	size_t pos;

	pos = newFile.rfind('.');
	if(pos != string::npos)
		newFile = newFile.substr(0, pos);

	pos = newEnding.rfind('.');
	if(pos != string::npos)
		newEnding = newEnding.substr(pos + 1);

	if(newEnding.length() > 0) {
		newFile.append(".");
		newFile.append(newEnding);
	}
	file = newFile;
}

}
