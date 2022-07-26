#include "userinputmodel.h"
#include <QUrl>
#include <fstream>
#include <iostream>

UserInputModel::UserInputModel(const QString& _UIMIdentifier, const QString& _title, bool isMostDerivedSubobject_):
	Initializer(false),
	p(Persistence::GetSingleton()),
	UIMIdentifier(_UIMIdentifier),
	title(_title)
{
	init_callable(isMostDerivedSubobject_);
}
UserInputModel::~UserInputModel(){
}
void UserInputModel::init_overrideThis_callTheOtherOne(){
	hasUIMdefaultValues = readDataFromFile();
}
const std::map<QString, UIM::Type*>& UserInputModel::getInputs() const{
	return inputs;
}
std::map<QString, UIM::Type*>& UserInputModel::getInputs(){
	return inputs;
}
QString UserInputModel::GetUIMIdentifier() const{
	return UIMIdentifier;
}
//TODO: perzitenciara bizni a beolvasast es mentest
bool UserInputModel::saveDataToFile() const{
	std::ofstream ofs(GetUIMIdentifier().toStdString());
	std::string checkString = "-";
	if(!ofs.is_open()){
		assert(!"ERROR: UserInputModel::saveDataToFile(): couldn't open file");
	}
	for(auto var : inputs){
		ofs << var.first.toStdString();
		if(ofs.fail()){
			std::stringstream ss; ss	<< "ERROR: UserInputModel::saveDataToFile(): couldn't write "
									<< var.first.toStdString()
									<< " tag" << std::endl;
			assert(!ss.str().c_str());
		}
		
		ofs << " : ";
		if(ofs.fail()){
			std::stringstream ss; ss	<< "ERROR: UserInputModel::saveDataToFile(): couldn't write "
									<< "\" : \""
									<< " string to file" << std::endl;
			assert(!ss.str().c_str());
		}
		
		assert(var.second);
		ofs << var.second->toStr() << "\n";
		if(ofs.fail()){
			std::stringstream ss; ss	<< "ERROR: UserInputModel::saveDataToFile(): couldn't write "
									<< var.second->toStr()
									<< " variable" << std::endl;
			assert(!ss.str().c_str());
		}
	}
	return true;
}
bool UserInputModel::readDataFromFile(){
	std::ifstream ifs(GetUIMIdentifier().toStdString());
	std::string checkString = "-";
	
	if(!ifs.is_open()){
		std::cerr << ("WARNING: UserInputModel::readDataFromFile(): couldn't open file '")
					<< GetUIMIdentifier().toStdString()
					<< "' " << std::endl;
		
		return false;
	}
	for(auto var : inputs){
		ifs >> checkString;
		if(ifs.fail()){
			std::cerr << "ERROR: UserInputModel::readDataFromFile(): couldn't read a data identifier tag" << std::endl;
			return false;
		}
		if(checkString != var.first.toStdString()){
			std::cerr	<< "ERROR: UserInputModel::readDataFromFile(): read '"
									<< checkString
									<< "' tag, which did't match tag in uim: " 
									<< var.first.toStdString()<< std::endl;
			return false;
		}
		std::string nothing;
		ifs >> nothing;
		if(ifs.fail()){
			std::cerr << "ERROR: UserInputModel::readDataFromFile(): couldn't read "
									<< "\" : \""
									<< " string from file" << std::endl;
			return false;
		}
		
		assert(var.second);
		var.second->read(ifs);
		if(ifs.fail()){
			std::cerr << "ERROR: UserInputModel::readDataFromFile(): couldn't read variable" << std::endl;
			return false;
		}
	}
	return true;
}
GenericFileUIM::GenericFileUIM(const IDrawingContainer* dcm, const QString& _title, const QString& UIMIdentifier, bool isMostDerivedSubobject_)
			:
		UserInputModel(UIMIdentifier, _title, false),
		dcModel(dcm)
{
	init_callable(isMostDerivedSubobject_);
}
GenericFileUIM::~GenericFileUIM(){
	clearInputs();
}
QString GenericFileUIM::addInput(QString filePath){
	QUrl path(filePath);
	QString inputName = path.fileName();
	if(inputs.find(inputName) != inputs.end())
		return "";
	auto newPair = sPair(inputName, new UIM::FileName(filePath));
	inputs.insert(newPair);
	saveDataToFile();
	return inputName;
}
bool GenericFileUIM::removeInput(QString fileKey){
	if(inputs.find(fileKey) == inputs.end())
		return false;
	inputs.erase(fileKey);
	saveDataToFile();
	
	return true;
}
bool GenericFileUIM::readDataFromFile(){
	clearInputs();
	std::ifstream ifs(GetUIMIdentifier().toStdString());
	std::string checkString = "-";
	
	if(!ifs.is_open()){
		std::cerr << ("WARNING: UserInputModel::readDataFromFile(): couldn't open file '")
					<< GetUIMIdentifier().toStdString()
					<< "' " << std::endl;
		
		return false;
	}
	while(ifs >> checkString){
		if(ifs.fail()){
			std::cerr << "ERROR: UserInputModel::readDataFromFile(): couldn't read a data identifier tag" << std::endl;
			return false;
		}
		std::string nothing;
		ifs >> nothing;
		if(ifs.fail()){
			std::cerr << "ERROR: UserInputModel::readDataFromFile(): couldn't read "
									<< "\" : \""
									<< " string from file" << std::endl;
			return false;
		}
		UIM::FileName* newVal = new UIM::FileName;
		assert(newVal);
		newVal->read(ifs);
		if(ifs.fail()){
			std::cerr << "ERROR: UserInputModel::readDataFromFile(): couldn't read variable" << std::endl;
			return false;
		}
		inputs.insert(sPair(QString(checkString.c_str()), newVal));
	}
	return true;
}
void GenericFileUIM::clearInputs(){
	for(auto var : inputs){
		if(var.second != nullptr){
			delete var.second;
		}
	}
	inputs.clear();
}
bool GenericFileUIM::saveLastDrawingToDataFile(const QString& fileKey){
	auto filePathIt = inputs.find(fileKey);
	if(filePathIt == inputs.end())
		return false;
	auto filePath = filePathIt->second;
	
	assert(dynamic_cast<UIM::FileName*>(filePath));
	UIM::FileName fn = *dynamic_cast<UIM::FileName*>(filePath);
	std::ofstream ofs(fn.wrappedVal.toStdString(), std::ios_base::app);
	auto& ds = dcModel->getDrawings();
	if(ds.size() < 1)
		return false;
	ds.back()->print(ofs, true);
	ofs.close();
	return ofs.good();
}
