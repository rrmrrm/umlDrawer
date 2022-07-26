#include "identifiable.h"

#include <QDebug>

#include <fstream>

IdType Identifiable::nextId = 0;
std::vector<IdType> Identifiable::ids = std::vector<IdType>();
std::vector<Identifiable*> Identifiable::existingObjects;
const IdType Identifiable::INVALID_ID = -1;


Identifiable::Identifiable(IdType forcedId)
	:id(forceGetId(forcedId)) 
{
	existingObjects.push_back(this);
	if(nextId > forcedId){
		qDebug() << "WARNING: Identifiable::Identifiable(IdType forcedId): nextId > forcedId\n";
	}
	/// hogy ne ütközhessen az így beállított id 
	/// egy később automatikusan kiosztott id-vel:
	if(nextId < forcedId)
		nextId = forcedId + 1;
}
Identifiable::Identifiable()
	:id(requestId()) 
{
	existingObjects.push_back(this);
}
Identifiable::Identifiable(Identifiable&& o)
	:id( forceGetId(o.freeId(), true) ){//forceGetId(.., true): validáljuk o.id-t mielőtt ez az objektum átvanné id-t.
}
Identifiable::~Identifiable(){
	auto found = std::find(existingObjects.begin(),
						   existingObjects.end(),
						   this);
	assert(found != existingObjects.end());
	*found = nullptr;
	existingObjects.erase(found);
	freeId();
	//freeId(id);
}

IdType Identifiable::requestId(){
	IdType next = nextId;
	bool p = true;
	while(p){
		auto found = std::find(ids.begin(), ids.end(), next);
		p = (found != ids.end());
		++next;
	}
	if(!p){
		--next;
	}
	ids.push_back(next);
	nextId = next + 1;
	return next;
}
IdType Identifiable::freeId(){
	/// ha id = INVALID_ID akkor már meg lett hívva erreaz objektumra a freeId(). 
	///  ekkor nem kell semmit csinálni az eljárásban:
	if(id == INVALID_ID)
		return id;
	
	auto found = std::find(ids.begin(),ids.end(), id);
	assert(found != ids.end());
	IdType erasedId = *found;
	id = INVALID_ID;//jelezzük, hogy ennek az objektumnak már nincs az ids-tárolóban az id-je
	ids.erase(found);
	
	return erasedId;
}


std::ostream& Identifiable::save(std::ostream& os){
	os << id << std::endl;
	assert(os);
	return os;
}

std::istream& Identifiable::load(std::istream& is){
	IdType newId;
	is >> newId;assert(is);
	freeId();
	id = forceGetId(newId);
	return is;
}

//void Identifiable::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void Identifiable::load(std::istream& is){
//	load_chainCallThis(is);
//}
void Identifiable::saveIdent(std::ostream& os){
	Identifiable::save(os);
}
void Identifiable::loadIdent(std::istream& is){
	Identifiable::load(is);
}
Identifiable* Identifiable::findById(IdType id){
	std::vector<Identifiable*>::iterator found = std::find_if(
				Identifiable::existingObjects.begin(),
				Identifiable::existingObjects.end(),
				[&id](Identifiable* ident){return id == ident->getId();}
	);
	if(found == Identifiable::existingObjects.end()){
		qDebug() << "WARNING: ConnectionSaver::findById(..): no Identifiable object with " 
				 << id << " id\n";
		return nullptr;
	}
	else{
		return *found;
	}
}
/// TODO: foreceGetID statikus és feltételezi, hogy id=forcegetId() formában fogják használni. 
/// egyszerűbb lenne, ha nem lenne statikus és értelmes neve lenne. 
/// utóbbi forceSetId ra is vonatkozik.
IdType Identifiable::forceGetId(IdType forcedId, bool validateId){
	if(validateId){
		assert(forcedId != INVALID_ID);
	}
	auto found = std::find(ids.begin(), ids.end(), forcedId);
	assert(found == ids.end());//használt id-t nem foglalhatunk le, ilyen esetben hibát dobunk;
	if(forcedId < nextId){
		
	}
	else{
		if(forcedId == nextId){
			++nextId;//már itt megnöveljük nextId-t, hiszen ezt az értéket le akrarja majd a hívó foglalni
		}
		else{
			
		}
	}
	ids.push_back(forcedId);
	return forcedId;
}
void Identifiable::forceSetId(IdType forcedId, bool validateId){
	freeId();
	id = forceGetId(forcedId, validateId);
}
IdType Identifiable::getId() const{
	return id;
}
