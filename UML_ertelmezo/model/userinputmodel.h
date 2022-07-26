#ifndef USERINPUTMODEL_H
#define USERINPUTMODEL_H

#include "idrawingcontainer.h"
#include "common.hpp"
#include "initializer.hpp"
#include "persistence/persistence.h"

#include <map>

namespace UIM{
class Type{
protected:
	Type() {}
public:
	virtual std::string toStr() const = 0;
	
	virtual std::istream& read(std::istream& is) = 0;
	virtual ~Type() = 0;
	
};
inline Type::~Type(){
}
class Integer: public Type{
public:
	int wrappedVal;
	~Integer(){}
	
	std::string toStr() const override{
		return std::to_string(wrappedVal);
	}
	std::istream& read(std::istream& is) override{
		is >> wrappedVal;
		return is;
	}
	operator int&(){
		return wrappedVal;
	}
	explicit operator int() const{
		return wrappedVal;
	}
};
inline std::pair<QString, UIM::Integer*> iPair(QString first, UIM::Integer* second){
	return std::pair<QString, UIM::Integer*>(first, second);
}

class Double: public Type{
public:
	double wrappedVal;
	~Double(){}
	
	std::string toStr() const override{
		return std::to_string(wrappedVal);
	}
	std::istream& read(std::istream& is) override{
		is >> wrappedVal;
		return is;
	}
	operator double&(){
		return wrappedVal;
	}
	explicit operator double() const{
		return wrappedVal;
	}
};
inline std::pair<QString, UIM::Double*> dPair(QString first, UIM::Double* second){
	return std::pair<QString, UIM::Double*>(first, second);
}
///TODO: a több szavas string nem lesz jól beolvasva, mert a read csak egy szót olvas be
/// kijavítani
class String: public Type{
public:
	QString wrappedVal;
	virtual ~String(){}
	
	std::string toStr() const override{
		return wrappedVal.toStdString();
	}
	std::istream& read(std::istream& is) override{
		std::string dummy;
		is >> dummy;
		wrappedVal = QString::fromStdString(dummy);
		return is;
	}
	operator QString&(){
		return wrappedVal;
	}
	explicit operator QString() const{
		return wrappedVal;
	}
};
inline std::pair<QString, UIM::String*> sPair(QString first, UIM::String* second){
	return std::pair<QString, UIM::String*>(first, second);
}
class FileName: public UIM::String{
public:
	FileName(){}
	explicit FileName(QString str){
		wrappedVal = str;
	}
	~FileName(){}
};
class DirName: public UIM::String{
public:
	~DirName(){}
};
}///namespace UIM

inline std::pair<QString, UIM::FileName*> sPair(QString first, UIM::FileName* second){
	return std::pair<QString, UIM::FileName*>(first, second);
}

class UserInputModel : public Initializer{
/// fields:
private:
	Persistence* p;
protected:
	// a leszarmazo osztaly konstruktora  adja  a UIMIdentifier-t,ami a mentési file neve lesz:
	// az UIMIdentifier azonositasra szolgal (a title mezot is lehet talan arra hasznalni kiegeszitskent)
	const QString UIMIdentifier;
	std::map<QString, UIM::Type*> inputs;
public:
	bool hasUIMdefaultValues = false;
	const QString title;
	
/// ctors, dtor, operators:
private:
protected:
public:
	//UserInputModel(UserInputModel* mostDerived);
	UserInputModel(const QString& UIMIdentifier, const QString& _title, bool isMostDerivedSubobject_ = true);
	virtual ~UserInputModel();
	
/// other methods:
private:
protected:
	virtual void init_overrideThis_callTheOtherOne() final;
public:
	const std::map<QString, UIM::Type*>& getInputs() const;
	std::map<QString, UIM::Type*>& getInputs();
	//a file neve, amibe a kitöltött form mezőinek értékét mentjük, és a form legközelebbi megnyitásakor abból ebből a fileból inicializáljuk a formban megjelenő mezőket:
	virtual QString GetUIMIdentifier() const;
	virtual bool saveDataToFile() const;
	virtual bool readDataFromFile();
};




///
/// \brief The GenericFileUIM class
///		fileneveket tárol a heap-en. 
class GenericFileUIM : public UserInputModel{
///fields:
private:
	const IDrawingContainer* dcModel;
	
/// ctors, dtor, operators:
private:
	///GenericFileUIM sem másolható
	GenericFileUIM(const GenericFileUIM& d) = delete;
	GenericFileUIM& operator=(const GenericFileUIM& d) = delete;
protected:
public:
	//UserInputModel(UserInputModel* mostDerived);
	GenericFileUIM(const IDrawingContainer* dcm, const QString& _title, const QString& UIMIdentifier = "GenericFileUIM", bool isMostDerivedSubobject_ = true);
	virtual ~GenericFileUIM();
	
/// other methods:
public:
	///
	/// \brief addInput
	/// \param fileName
	/// \return 
	/// ""-t ad vissza, ha már van filePath-bólkészített kez-el rendelkező kulcsú mező inputs-ban
	/// amúgy a sikeresen felvett kulcs-érték párból a kulcsot adja vissza
	QString addInput(QString filePath);
	bool removeInput(QString fileKey);
	virtual bool readDataFromFile() override;
	virtual void clearInputs();
	virtual bool saveLastDrawingToDataFile(const QString& fileKey);
};

class DrawingPopulationUIM: public UserInputModel{
	///DrawingPopulationUIM nem másolható
	DrawingPopulationUIM(const DrawingPopulationUIM& d) = delete;
	DrawingPopulationUIM& operator=(const DrawingPopulationUIM& d) = delete;
public:
	UIM::FileName concatenatedCsvFileName;
	
	UIM::Double pointModificationRate;
	UIM::Double minRangeRatio;
	UIM::Double maxRangeRatio;
	UIM::Double maxAmplitudeMultiplier;
	UIM::Double maxDeviation;
	UIM::Double minDeviation;

	UIM::Double minScaleX;
	UIM::Double maxScaleX;
	UIM::Double minScaleY;
	UIM::Double maxScaleY;
	UIM::Double scaleChance;
	
	UIM::Integer populationSize;
	
	UIM::Integer rowsNum;
	UIM::Integer colsNum;
public:
	DrawingPopulationUIM(const QString& _title, QString UIMIdentifier = "DrawingPopulationUIM", bool isMostDerivedSubobject_ = true):
				UserInputModel(UIMIdentifier, _title, false)
			{
		inputs.insert(sPair("concatenatedCsvFileName", &concatenatedCsvFileName));
		
		inputs.insert(dPair("pointModificationRate", &pointModificationRate));
		inputs.insert(dPair("minRangeRatio", &minRangeRatio));
		inputs.insert(dPair("maxRangeRatio", &maxRangeRatio));
		inputs.insert(dPair("maxAmplitudeMultiplier", &maxAmplitudeMultiplier));
		inputs.insert(dPair("maxDeviation", &maxDeviation));
		inputs.insert(dPair("minDeviation", &minDeviation));
		
		inputs.insert(dPair("minScaleX", &minScaleX));
		inputs.insert(dPair("maxScaleX", &maxScaleX));
		inputs.insert(dPair("minScaleY", &minScaleY));
		inputs.insert(dPair("maxScaleY", &maxScaleY));
		inputs.insert(dPair("scaleChance", &scaleChance));
		
		inputs.insert(iPair("populationSize", &populationSize));
		
		inputs.insert(iPair("rowsNum", &rowsNum));
		inputs.insert(iPair("colsNum", &colsNum));
		init_callable(isMostDerivedSubobject_);
	}
	virtual ~DrawingPopulationUIM(){}
};

class TrainingUIM : public DrawingPopulationUIM{
	///TrainingUIM nem másolható
	TrainingUIM(const TrainingUIM& d) = delete;
	TrainingUIM& operator=(const TrainingUIM& d) = delete;
public:
	UIM::DirName sourceDir;
	UIM::DirName populatedDir;
	
	//a jelenlegi megoldás kb véletlenszerűen párosítja össze a mintafileokat az elvárt kimenetekkel, ami valószínűleg nem baj, de nem is szép.
	//Szépítésként meg lehetne írni, hogy valahogy explicit módon össze legyenek párosítva.	
	
		// vector expected_value1, ...> megadása helyett sorban 
		//az egységvektorok lesznek az elvárt kimeneti értékek.
		//Azt hiszem a python által egy könyvtár végigiterálása az abc sorrendjében halad,
		// ezért abc sorrendben kapják meg a mintaFileok az egység vektorokat - azaz az elvárt kimeneteket.
	
	//UIM::DirName txtsourceDataDirectory = populatedDir;
	
	UIM::DirName csvOutputDataDirectory;
	
	//UIM::DirName inputSeparateCsvDirectory;
	//UIM::FileName outPutConcatenatedCsvFileName;
	
	UIM::Integer inputSize;
	UIM::Integer colsNum;
	UIM::Integer rowsNum;
	UIM::Integer expOutputSize;
	UIM::Integer epochs_num; 
	UIM::Double testDataRatio;
	UIM::Integer batchSize;
	UIM::String trainedNetworkName;
	//ha hamis, akkor egyszerűen csak betanítjuk a hálót a outPutConcatenatedCsvFileName-ben megadott nevű fileból:
	UIM::Integer doPopulateAndConvertBeforeTraining;
	
	UIM::Integer ttfModelDoLoadAndRetrain;
	UIM::DirName ttfModelLoadDir;
	UIM::DirName ttfModelSaveDir;
	///ki -és be-meneti fileok nem lehetnek egy könyvtárban, mert akkor a populált
	///	minták is feldolgozásra kerülnek az eredetiek mellett a python progik által
public:
	TrainingUIM(const QString& _title, bool isMostDerivedSubobject_ = true):
		DrawingPopulationUIM(_title, "TrainingUIM", false)
			{
		inputs.insert(sPair("sourceDir", &sourceDir));                            
		inputs.insert(sPair("populatedDir", &populatedDir));  
		
		inputs.insert(sPair("csvOutputDataDirectory", &csvOutputDataDirectory)); 
		
		inputs.insert(iPair("inputSize", &inputSize));
		
		inputs.insert(iPair("expOutputSize", &expOutputSize));                    
		inputs.insert(iPair("epochs_num", &epochs_num	));                    
		inputs.insert(dPair("testDataRatio", &testDataRatio));                      
		inputs.insert(iPair("batchSize", &batchSize));                  
		inputs.insert(sPair("trainedNetworkName", &trainedNetworkName));
		inputs.insert(iPair("doPopulateAndConvertBeforeTraining", &doPopulateAndConvertBeforeTraining));
		inputs.insert(sPair("trainedNetworkName", &trainedNetworkName));
		inputs.insert(iPair("doPopulateAndConvertBeforeTraining", &doPopulateAndConvertBeforeTraining));
		
		inputs.insert(iPair("ttfModelDoLoadAndRetrain", &ttfModelDoLoadAndRetrain));     
		inputs.insert(sPair("ttfModelLoadIfSetDir", &ttfModelLoadDir));
		inputs.insert(sPair("ttfModelSaveDir", &ttfModelSaveDir));
		
		init_callable(isMostDerivedSubobject_);
	}
	virtual ~TrainingUIM(){
		
	}
	
};


#endif // USERINPUTMODEL_H
