/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef FILENAME_H
#define FILENAME_H

#include <string>
#include <sstream>
namespace Util{

/**
 * @ingroup io
 */
class FileName{

		/**
		 * @name Factories
		 */
		//@{
	public:
		UTILAPI static FileName createDirName(const std::string & dirname);
		//@}

		/**
		 * @name Main
		 */
		//@{
	public:
		FileName() :
			fsName(), dir(), file() {
		}
		explicit FileName(const std::string & _file) :
			fsName(), dir(), file() {
			parse(_file);
		}
		explicit FileName(const char * _file) :
			fsName(), dir(), file() {
			parse(_file);
		}

		friend std::ostream & operator<<(std::ostream & s, const FileName & f) {
			return s << f.toString();
		}
		friend std::string operator+(const std::string & s, const FileName & f) {
			return s + f.toString();
		}
	private:
		std::string fsName;
		std::string dir;
		std::string file;
		//@}

		/**
		 * @name Information
		 */
		//@{
	public:
		bool empty()const						{   return fsName.empty() && dir.empty() && file.empty(); }
		const std::string & getDir()const		{   return dir; }
		const std::string & getFile()const		{	return file;    }
		const std::string & getFSName()const	{   return fsName; }
		std::string getPath()const				{   return dir + file;  }
		std::string toString()const				{   return empty() ? "" : fsName + "://" + dir + file;  }
		std::string toShortString()const		{   return empty() ? "" : (fsName=="file" ? dir+file : fsName + "://" + dir + file);  }
		UTILAPI std::string getEnding()const;
		//@}

		/**
		 * @name Modification
		 */
		//@{
	public:
		void setFile(const std::string & newFile)    {   file = newFile; }
		void setDir(const std::string & newDir)  {
			dir = newDir;
			if(!dir.empty() && dir.back() != '/') {
				dir += '/';
			}
		}
		void setFSName(const std::string & newFSName) {
			fsName = newFSName;
		}

		/*!
		 * modifies the filename such that it has the given ending, examples:
		 * "file" --> "file.ending"
		 * "file.xy" --> "file.ending"
		 * "file.ab.xy" --> "file.ab.ending"
		 * "" --> ".ending"
		 * ".xy" --> ".ending"
		 * @param ending the new ending to be used for this filename can be specified either as ".ending" or as "ending"
		 * @note if ending is specified as ".xy.ab", only ".ab" is used
		 * @note ending
		 */
		UTILAPI void setEnding(const std::string & ending);

		FileName& operator=(const std::string & s) {
			parse(s);
			return *this;
		}
	private:
		UTILAPI void parse(const std::string & s);
		//@}

		/**
		 * @name Comparators
		 */
		//@{
	public:
		bool operator==(const FileName & other)const{
			return dir == other.dir && file == other.file && fsName == other.fsName;
		}
		bool operator==(const std::string & s)const{
			return toString()==s;
		}
		bool operator!=(const FileName & other)const{
			return !(*this==other);
		}
		bool operator!=(const std::string & s)const{
			return toString()!=s;
		}
		bool operator<(const FileName& other)const{
			return (dir!=other.dir) ? (dir<other.dir) : (file<other.file);
		}
		bool operator>(const FileName& other)const{
			return (dir!=other.dir) ? (dir>other.dir) : (file>other.file);
		}
		bool operator<=(const FileName& other)const{
			return !(*this>other);
		}
		bool operator>=(const FileName& other)const{
			return !(*this<other);
		}
		//@}
};
}
#endif // FILENAME_H
