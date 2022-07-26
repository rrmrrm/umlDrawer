#include "persistence.h"
Persistence* Persistence::InnerObject = nullptr;
uint Persistence::refCounter = 0;

//#include "model/model.h"

#include <fstream>
#include <sstream>
#include <variant>

bool Persistence::isOpen(std::ifstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail){
	if(!fs.is_open()){
		std::stringstream msg;
		msg	<< "ERROR: " << methodName << ": file hadn't been opened. variable name:"
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
bool Persistence::isOpen(std::ofstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail){
	if(!fs.is_open()){
		std::stringstream msg;
		msg	<< "ERROR: " << methodName << ": file hadn't been opened. variable name:"
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
bool Persistence::readCheckNameTagSep(std::ifstream& ifs, const std::string methodName, const QString& nameTag, bool errOnFail){
	std::string readNameTag = "";
	/// Ha nameTag üres, akkor azt várjuk, hogy a fileban sincs benne, így az ellenőrzését kihagyjuk. 
	/// Ha nem üres, akkor összevetjük a file ketkez szavával:
	if(nameTag != "" && !nameTag.isEmpty()){
		ifs >> readNameTag;
		if(ifs.fail() || readNameTag != nameTag.toStdString()){
			std::stringstream msg;
			msg	<< "ERROR: " << methodName << ": nameTag mismatch; expected: '" 
				<< nameTag.toStdString() 
				<< "', found: '"
				<< readNameTag
				<< "'";
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
		std::string separator;
		if(!(ifs >> separator)){
			std::stringstream msg;
			msg	<< "ERROR: " << methodName << ": nameTag found, but separator is missing";
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
	}
	return true;
}
bool Persistence::writeNameTagSep(std::ofstream& ofs, const std::string methodName, const QString& nameTag, bool errOnFail){
	//ha nameTag üres, akkor kiírjuk és a szeparátort is:
	if(nameTag != "" && !nameTag.isEmpty()){
		ofs << nameTag.toStdString() << std::endl;
		if(ofs.fail()){
			std::stringstream ss;
			ss	<< "ERROR: " << methodName << ": couldn't write '"
				<< nameTag.toStdString()
				<< "' nametag to file";
			if(errOnFail){
				l->err(ss.str());
				l->flush();
				assert(!ss.str().c_str());
			}
			else{
				l->warn(ss.str());
				return false;
			}
		}
		if(!(ofs << " : " << std::endl)){
			std::stringstream ss;
			ss	<< "ERROR: " << methodName << ": wrote nametag, but couldn't write separator to file";
			if(errOnFail){
				l->err(ss.str());
				l->flush();
				assert(!ss.str().c_str());
			}
			else{
				l->warn(ss.str());
				return false;
			}
		}
	}
	return true;
}
Persistence* Persistence::GetSingleton(){
	if(InnerObject == nullptr)
		InnerObject = new Persistence();
	++Persistence::refCounter;
	return InnerObject;
}
void Persistence::releaseSingleton(){
	assert( refCounter > 0);
	--refCounter;
	if(refCounter == 0 && InnerObject != nullptr)
		delete InnerObject;
}

void Persistence::loadVars(
		const std::vector<std::pair<QString, ptr_variants> >& vars,
		const std::string& file
		)
{
	std::ifstream ifs(file);
	for(auto var : vars){
		QString& varName = var.first;
		std::visit(
				[this, &ifs, varName](auto& v){
					loadVariable(ifs, v, varName, true);
				}, 
				var.second
			);
	}
	//loadVariable(ifs, m->resHorizontal, "resHorizontal:", true);
	//loadVariable(ifs, m->resVertical, "resVertical:", true);
	//loadVariable(ifs, m->marginInPixels, "marginInPixels:", true);
	//loadVariable(ifs, m->drawingBlurSD, "drawingBlurSD:", true);
	//loadVariable(ifs, m->testNetwork, "testNetwork:", true);
	//loadVariable(ifs, m->drawingsOutFileName, "drawingsOutFileName:", true);
	ifs.close();
}
