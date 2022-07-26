#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QDir>
#include <QDateTime>

//#include <common.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

class Logger
{
	std::ofstream logOs;
	std::ofstream warnOs;
	std::ofstream errOs;
	
	void my_assert(bool expr, QString failMsgPart1="", QString failMsgPart2="", QString failMsgPart3="", QString failMsgPart4=""){
		if(!expr){
			QString qstr = failMsgPart1
				+ failMsgPart2
				+ failMsgPart3
				+ failMsgPart4;
			assert(!qstr.toStdString().c_str());
		}
	}
public:
	Logger(QString logfileDirName)
	{
		QString logOsFileName = "log.log";
		QString warnOsFileName = "warnings.log";
		QString errOsFileName = "errors.log";
		
		QDir dir;
		if( !QDir(logfileDirName).exists() ){
			my_assert(dir.mkdir(logfileDirName), "ERROR: Logger::Logger(): could not create '", logfileDirName, " directory");
		}
		/// a log fileba eredeti tartalmához hozzáfűzzük az újakat,
		/// a hiba és figyelmeztetés tartalmát viszont fölülírjuk:
		logOs.open( QDir(logfileDirName).filePath(logOsFileName).toStdString(), std::ios_base::app);
		logOs << std::endl;
		printTimeStamp(logOs);
		
		warnOs.open( QDir(logfileDirName).filePath(warnOsFileName).toStdString() );
		errOs.open( QDir(logfileDirName).filePath(errOsFileName).toStdString() );
		
	}
	~Logger(){
		logOs.close();
		warnOs.close();
		errOs.close();
	}
	void printTimeStamp(std::ostream& os){
		QDateTime qDT = QDateTime::currentDateTime();
		os << qDT.date().toString().toStdString() << " ";
		os << qDT.time().hour() << " ";
		os << qDT.time().minute() << " ";
		os << qDT.time().second() << " ";
		os << std::endl;
	}
	void flush(){
		logOs.flush();
		warnOs.flush();
		errOs.flush();
	}
	bool log(std::string msg){
		std::cout << msg << std::endl;
		logOs << msg << std::endl;
		return !logOs.fail() && !logOs.bad();
	}
	bool warn(std::string msg){
		std::cout << "WARNING: " << msg << std::endl;
		std::cerr << "WARNING: " << msg << std::endl;
		warnOs << msg << std::endl;
		flush();
		return !warnOs.fail() && !warnOs.bad();
	}
	bool err(std::string msg){
		std::cerr << "ERROR: " << msg << std::endl;
		errOs << msg << std::endl;
		flush();
		return !errOs.fail() && !errOs.bad();
	}
};

#endif // LOGGER_H
