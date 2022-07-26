#ifndef Persistence_H
#define Persistence_H

#include "model/logger.h"// irasi/olvasasi hibak kiirasahoz
#include "network/network.h"// neurális háló
#include "shape_drawing/drawing.hpp"
#include "streamoperators.h"

#include <QString>
#include <QDir>
#include <QDirIterator>

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <variant>

typedef std::variant<bool*, unsigned int*, int*,float*,double*,std::string*> ptr_variants;
///Singleton Perzisztencia
///1. dolga jól olvasható filebaírást megvalósítani.
///2.: részletes, a javítást könnyítő hibaüzenetek gyártása(írási/olvasási hiba esetén) a Logger osztály felhasználásával
/// az egyes adattagok kiírása előtt kiírja azok nevét, hogy 
/// a későbbi beolvasás során egyértelmű legyen, ha rosz változót írtunk be, és a hibaüzenet tartalmazni is fogja mely változónál volt eltérés,
///  ezért könnyebb lesz a hibaüzenet alapján javítani a hibát.
class Persistence
{
///fields:
private:
	static Persistence* InnerObject;
	static uint refCounter;
	//enum typeTag {INT, DOUBLE, STRING};
	QString logfileDirName = "logfiles";
	std::ifstream writer;
	std::ofstream reader;
	//NetworkData* nd = nullptr;
	NeuralNetwork* n = nullptr;
	
	///fields for iterating through a directory:
	bool iterating = false;
	QDirIterator* dirIterator = nullptr;
	std::ifstream drawingReader;
	QString skipHavingSuffix;
public:
	Logger* l;
	
/// ctors, dtor, operators:
public:
	Persistence(){
		l = new Logger(logfileDirName);
	}
	///TODO: valamiert debugban az alkalmazasbol kiszallasokr(jobbfelul xgombra klikk) eseten nem hivodik meg a destruktor (legalabbis nem all meg a progi a breakpointon ) itt. Kijavitani
	~Persistence(){
		delete l;
		if(n != nullptr)
			delete n;
	}

/// other methods:
private:
	bool isOpen(std::ifstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	bool isOpen(std::ofstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	
	///
	/// \brief readCheckNameTagSep
	///		Ha a nameTag argumentum üres, akkor nem csinál semmit csak igazat ad vissza.
	///		Amúgy beolvas egy stringet, ellenőrzi, hogy nametaggel megegyezik-e ha nem, akkor hibás futást jelez.
	/// 
	///		Ha errOnFail igaz, a hibás futás jelzése azt jelenti, hogy assert-el hibát dob a program.
	///		Ha hamis, akkor csak hamisat adunk vissza.
	///		Ha hibás futást érzékel a program, akkor nem történik több beolvasás.
	/// \param ifs
	/// \param methodName
	/// \param nameTag
	/// \param errOnFail
	/// \return 
	///
	bool readCheckNameTagSep(std::ifstream& ifs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	bool writeNameTagSep(std::ofstream& ofs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	template<class fStreamType>
	bool isStreamValid(fStreamType& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false){
		if(fs.fail()){
			std::stringstream msg;
			msg	<< "ERROR: " << methodName << ": found nametag, but failed while reading/writing variable: "
				<< nameTag.toStdString();
			if(errOnFail){
				l->err(msg.str());
				l->flush();
				assert(!msg.str().c_str());
			}
			else{
				l->warn(msg.str());
				return false;
			}
		}
		return true;
	}
	
	///
	/// \brief loadVariable
	/// ha nametag üres, akkor arra számítunk, hogy a fileban nincs nametag és szeparátor se
	/// \param ifs
	/// \param data
	/// \param nameTag
	/// \param errOnFail
	/// \return 
	///
	template<class T>
	bool loadVariable(std::ifstream& ifs, T& data, QString nameTag, bool errOnFail=false){
		std::string methodName = "persistence::loadVariable(..)";
		//ha errOnFail igaz és pl isOpen(..)-ben hiba történik, akkor a logikai kifejezés nem értékelődik ki, mert közben hibát dobunk
		if(isOpen(ifs, methodName, nameTag, errOnFail)
				&& readCheckNameTagSep(ifs, methodName, nameTag, errOnFail)
				&& (ifs >> data || true)
				&& isStreamValid(ifs, methodName, nameTag, errOnFail) 
		){
			return true;
		}
		else{
			return false;
		}
	}
	template<class T>
	bool loadVariable(std::ifstream& ifs, T* data, QString nameTag, bool errOnFail=false){
		assert(data);
		std::string methodName = "persistence::loadVariable(..)";
		//ha errOnFail igaz és pl isOpen(..)-ben hiba történik, akkor a logikai kifejezés nem értékelődik ki, mert közben hibát dobunk
		if(isOpen(ifs, methodName, nameTag, errOnFail)
				&& readCheckNameTagSep(ifs, methodName, nameTag, errOnFail)
				&& (ifs >> *data || true)
				&& isStreamValid(ifs, methodName, nameTag, errOnFail) 
		){
			return true;
		}
		else{
			return false;
		}
	}
	///
	/// \brief saveVariable
	///		ha nameTag üres, akkor csak a data-t írjuk ki, a nameTagot (nyilván) nem, és ekkor szeparátort sem írunk a fileba.
	/// \param ofs
	/// \param data
	/// \param nameTag
	/// \param errOnFail
	/// \return 
	///
	template<class T>
	bool saveVariable(std::ofstream& ofs, const T& data, QString nameTag, bool errOnFail=false){
		std::string methodName = "persistence::saveVariable(..)";
		//ha errOnFail igaz és pl isOpen(..)-ben hiba történik, akkor a logikai kifejezés nem értékelődik ki, mert közben hibát dobunk
		if(isOpen(ofs, methodName, nameTag, errOnFail)
				&& writeNameTagSep(ofs, methodName, nameTag, errOnFail)
				&& (ofs << data << std::endl || true)
				&& isStreamValid(ofs, methodName, nameTag, errOnFail) 
		){
			return true;
		}
		else{
			return false;
		}
	}
public:
	///egyebek mellett növeli a referenciszámlálót
	static Persistence* GetSingleton();
	///csökkenti a referencia számlálót. ha eléri a nullát, akkor delete-eljük InnerObject-et
	static void releaseSingleton();
	NeuralNetwork* getNetwork(){
		if(n != nullptr)
			return n;
		
		n = new NeuralNetwork();
		std::ifstream ifs("trainedNetwork.txt");
		bool succ =
			loadVariable(ifs, n->layers, "")					 
			//&& loadVariable(ifs, tmp, "")
			&& loadVariable(ifs, n->weightMatrices, "");
		ifs.close();
		if(!succ){
			l->err(str("Persistence::GetNetwork(): couldn't load network from '")+str("trainedNetwork.txt")+str("' file"));
			assert(false);
		}
		return n;
	}
	
	int getDrawingCategoriesNumInDir(QString dir){
		QDirIterator it(dir, QDir::Files);
		int categNum = 0;
		while(it.hasNext()){
			QString fileName = it.next();
			if(it.fileInfo().suffix()!="csv")
				++categNum;
		}
		return categNum;
	}
	void startIterationInDir(QString dir, QString skipHavingSuffix_ = ""){
		skipHavingSuffix = skipHavingSuffix_;
		if(dirIterator)
			delete dirIterator;
		dirIterator = new QDirIterator(dir, QDir::Files, QDirIterator::NoIteratorFlags);
	}
	bool nextDrawingCategoryInDir(){
		if(!dirIterator)
			return false;
		
		if(drawingReader.is_open())
			drawingReader.close();
		
		bool foundNext = false;
		while(!foundNext && dirIterator->hasNext()){
			QString fileName = dirIterator->next();
			if(		skipHavingSuffix == "" 
					|| dirIterator->fileInfo().suffix() != skipHavingSuffix)
			{
				foundNext = true;
			}
		}
		if(foundNext){
			std::string path = dirIterator->filePath().toStdString();
			drawingReader.open(path, std::ios_base::in);
			if(!drawingReader.is_open()){
				std::stringstream ss;
				ss << "Persistence::nextDrawingCategoryInDir(): found next category's file(" << path << "), but couldn't open it";
				l->warn(ss.str());
			}
			return drawingReader.is_open();
		}
		else{
			delete dirIterator;
			dirIterator = nullptr;
			return false;
		}
	}
	QString getActualFileName(){
		if(!dirIterator)
			return "NOFILE";
		return dirIterator->fileName();
	}
	QString getActualFilePath(){
		if(!dirIterator)
			return "NOFILE";
		return dirIterator->filePath();
	}
	int getDrawingNumInCategory(int newResHor, int newResVert){
		if(!dirIterator || !drawingReader.is_open())
			return 0;
		std::ifstream ifs(getActualFilePath().toStdString());
		int categNum = 0;
		Drawing d;
		while(d.load(ifs, newResHor, newResVert, true,true)){
			++categNum;
		}
		return categNum;
	}
	Drawing* nextDrawingInCategory(int newResHor, int newResVert){
		if(!dirIterator || !drawingReader.is_open())
			return nullptr;
		Drawing* ret = Drawing::createDrawingFromFile(drawingReader, newResHor, newResVert, true, true);
		return ret;
	}
	void loadVars(
				const std::vector<std::pair<QString, ptr_variants> >&,
				const std::string& file = "conf.txt"
				);
	bool saveNetwork(){
		if(n == nullptr){
			l->warn("Persistence::saveNetwork(): couldn't save network, because network was null'");
			return false;
		}
		std::ofstream ofs("trainedNetwork.txt");
		bool succ =
				saveVariable(ofs, n->layers, "")
				&& saveVariable(ofs, n->weightMatrices, qstr(""));
		ofs.close();
		if(!succ){
			l->err(str("Persistence::saveNetwork(): couldn't save network to '")+str("trainedNetwork.txt")+str("' file"));
			exit(1);
		}
		return n;
	}
	
	/////a cells-t <filenév>.csv fileból olvassa, a Drawing többi adatát pedig <filenév> fileból:
	//bool loadDrawings(std::vector<Drawing*>& ds, const std::string& fName);
	/////a cells-t <filenév>.csv fileból olvassa, a Drawing többi adatát pedig <filenév> fileból:
	//bool saveDrawings(const std::vector<Drawing*>& ds, const std::string& fName);
};


#endif // Persistence_H
