/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_SERIAL

#include "SerialProvider.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Factory/Factory.h"
#include "../StringUtils.h"
#include "../Timer.h"
#include <serial/serial.h>
#include <sstream>
#include <memory>

namespace Util {

bool SerialProvider::init() {
	static SerialProvider provider;
	return FileUtils::registerFSProvider("serial", PointerHolderCreator<SerialProvider>(&provider));
}


static std::vector<std::string> split(const std::string& source, char delimiter){
    std::vector<std::string> result;
    std::string accum;
	for(const char c : source){
		if(c==delimiter){
			result.emplace_back(accum);
			accum.clear();
		}else{
			accum += c;
		}
	}
	if(!accum.empty())
		result.emplace_back(accum);
    return result;
}

// -------------------------------

struct PortInfo{
	std::string portName,internalPortName;
	uint32_t baudRate;
	serial::bytesize_t bytesize;
	serial::parity_t parity;
	serial::stopbits_t stopbits;
	serial::flowcontrol_t flowcontrol;
};
static PortInfo getPortInfo(const FileName & filename){
	PortInfo info;
	
	const auto parts = split(filename.getFile(),':');
	
	info.portName  = parts.empty() ? "" : parts[0];
#ifdef _WIN32
	info.internalPortName =  "\\\\.\\" + info.portName;
#else
	info.internalPortName = info.portName;
#endif
	info.baudRate = 9600;
	info.bytesize = serial::eightbits;
	info.stopbits = serial::stopbits_one;
	info.flowcontrol = serial::flowcontrol_none;
	info.parity = serial::parity_none;
for(auto& s:parts)
	std::cout << ">"<<s<<"\n";

	if(parts.size()>1){
		info.baudRate = StringUtils::toNumber<uint32_t>(parts[1]);
		if(parts.size()>2){
			switch(StringUtils::toNumber<uint32_t>(parts[2])){
				case 5:	info.bytesize = serial::fivebits;	break;
				case 6:	info.bytesize = serial::sixbits;	break;
				case 7:	info.bytesize = serial::sevenbits;	break;
				case 0:// empty -> default
				case 8:	info.bytesize = serial::eightbits;	break;
				default:
					throw std::invalid_argument("SerialProvider: invalid bytesize :"+filename.toString());
			}
			if(parts.size()<=3 || parts[3].empty() || parts[3]=="n"){
				info.parity = serial::parity_none;
			}else if(parts[3]=="o"){
				info.parity = serial::parity_odd;
			}else if(parts[3]=="e"){
				info.parity = serial::parity_even;
			}else{
				throw std::invalid_argument("SerialProvider: invalid parity :"+filename.toString());
			}
			if(parts.size()<=4 || parts[4].empty() || parts[4]=="1"){
				info.stopbits = serial::stopbits_one;
			}else if(parts[4]=="2"){
				info.stopbits = serial::stopbits_two;
			}else{
				throw std::invalid_argument("SerialProvider: invalid stopbits :"+filename.toString());
			}
			if(parts.size()<=5 || parts[5].empty() || parts[5]=="n"){
				info.flowcontrol = serial::flowcontrol_none;
			}else if(parts[5]=="h"){
				info.flowcontrol = serial::flowcontrol_hardware;
			}else if(parts[5]=="s"){
				info.flowcontrol = serial::flowcontrol_software;
			}else{
				throw std::invalid_argument("SerialProvider: invalid flowcontrol :"+filename.toString());
			}
		}
	}
	return info;
}

bool SerialProvider::isFile(const FileName & filename){
	PortInfo info = getPortInfo(filename);
	
//	std::cout << "Port:" << info.internalPortName <<"\n";
	
	try{
		serial::Serial port( info.internalPortName );

		const bool b = port.isOpen();
		if(b)
			port.close();
//		std::cout << "IsOpen:" << b <<"\n";
		return b;
	}catch(const serial::IOException & e){
		std::cout << "!"<< e.what() << "\n";
		return false;
	}
}


class SerialIOBuffer : public std::streambuf{
		serial::Serial& port;
		static const std::size_t PUT_BACK_SIZE = 8;
        std::vector<char> inBuffer;
        std::vector<char> outBuffer;
        
	public:
		typedef char							char_type;
		typedef std::char_traits<char>			traits_type;
		typedef typename traits_type::int_type	int_type;
		typedef typename traits_type::off_type	off_type;
		typedef typename traits_type::pos_type	pos_type;

		SerialIOBuffer(serial::Serial& port_) : port(port_),inBuffer(1024),outBuffer(1024){
			setg(&inBuffer.back()+1, &inBuffer.back()+1, &inBuffer.back()+1);
			setp(outBuffer.data(),&outBuffer.back()); // leave one byte as extra space!
		}
		virtual ~SerialIOBuffer(){}
		
        SerialIOBuffer(const SerialIOBuffer &) = delete;
        SerialIOBuffer &operator= (const SerialIOBuffer &) = delete;
		
	private:
        int_type underflow()override{
//			std::cout << " underflow ";
			if(gptr() < egptr()) // inBuffer not exhausted
				return traits_type::to_int_type(*gptr());

			if(!port.isOpen() || port.available()==0)
				return traits_type::eof();
  
			char * cursor = inBuffer.data();

			// move last characters to beginning to support put_back
			const size_t numOldCharacters = std::min(static_cast<size_t>(egptr()-eback()),PUT_BACK_SIZE ); 
			for(size_t i=numOldCharacters; i>0; --i){
				*cursor = *(egptr()-i);
				++cursor;
			}
//			std::cout<< "Moving "<< numOldCharacters<<" characters\n";

			const size_t bytesRead = port.read(reinterpret_cast<uint8_t*>(cursor), std::min(port.available(),inBuffer.size()-numOldCharacters));
			cursor += bytesRead;
//						std::cout << " Read "<<bytesRead;


			setg(inBuffer.data(), inBuffer.data()+numOldCharacters, cursor);

			return traits_type::to_int_type(*gptr());
        }
		int_type overflow(int_type c)override{
			if(port.isOpen() && c!=traits_type::eof()){
				*pptr() = c;
				pbump(1);
				if( doFlush() )
					return traits_type::to_int_type(c);
			}
			return traits_type::eof();;
		}
		int sync()override{
			return  doFlush() ? 0 : -1;
		}
		bool doFlush(){
			if(!port.isOpen())
				return false;
			const std::ptrdiff_t bytesAvailable = pptr()-pbase();
			const std::ptrdiff_t bytesWritten = port.write(reinterpret_cast<const uint8_t*>(pbase()), bytesAvailable);
			if(bytesWritten!=bytesAvailable)
				throw std::runtime_error("SerialProvider: Error writing to serial port.");
			std::cout << "Bytes to serial: "<<bytesWritten<<std::endl;
			pbump(-bytesWritten);
			return true;
		}

};

class SerialIOStream : public std::iostream{
		std::unique_ptr<serial::Serial> port;
	public:
		SerialIOStream(std::unique_ptr<serial::Serial>&& port_) : std::iostream(new SerialIOBuffer(*port_.get())),port(std::move(port_)){}
		virtual ~SerialIOStream(){
			if(isOpen())flush();
			close();
		}
		bool isOpen()const	{	return port&&port->isOpen();	}
		void close()		{	if(isOpen()) port->close();	}
};

std::unique_ptr<std::iostream> SerialProvider::open(const FileName & filename){
	const PortInfo info = getPortInfo(filename);
	std::unique_ptr<serial::Serial> port( new serial::Serial( info.internalPortName,info.baudRate,
															serial::Timeout(),info.bytesize,info.parity,
															info.stopbits,info.flowcontrol ));
	if(!port->isOpen())
		throw std::runtime_error("SerialProvider::open: Could not open port: "+filename.toString());
	
	return std::unique_ptr<std::iostream> (new SerialIOStream(std::move(port)));
}

std::unique_ptr<std::istream> SerialProvider::openForReading(const FileName & filename){
	std::unique_ptr<std::iostream> io = open(filename);
	if(!dynamic_cast<std::istream*>(io.get()))
		return nullptr;
	return std::unique_ptr<std::istream>(dynamic_cast<std::istream*>(io.release()) );
}
std::unique_ptr<std::ostream> SerialProvider::openForWriting(const FileName & filename){
	std::unique_ptr<std::iostream> io = open(filename);
	if(!dynamic_cast<std::ostream*>(io.get()))
		return nullptr;
	return std::unique_ptr<std::ostream>(dynamic_cast<std::ostream*>(io.release()) );
}

//AbstractFSProvider::status_t SerialProvider::readFile(const FileName & filename, std::vector<uint8_t> & data){
//	std::unique_ptr<std::iostream> file( open(filename) );
//	std::cout << "<";
//	Timer t;
//	while(t.getSeconds()<5.0);
//	std::cout << ">";
//	
//	while(file->good()){
//		data.push_back( file->get() );
//	}
//	return OK;
//}
AbstractFSProvider::status_t SerialProvider::writeFile(const FileName & filename, const std::vector<uint8_t> & data, bool /*overwrite*/){
	std::unique_ptr<std::iostream> file( open(filename) );
	(*file) << std::string(data.begin(),data.end());
	return OK;
}


}

#endif // UTIL_HAVE_LIB_SERIAL
