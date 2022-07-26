#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H

#include <vector>
#include <iostream>

typedef long long int IdType;
class Identifiable
{
	IdType id;
	static IdType nextId;
	static std::vector<IdType> ids;
public:
	static std::vector<Identifiable*> existingObjects;
	static const IdType INVALID_ID;//= -1;
	
protected:
	Identifiable(IdType);
public:
	Identifiable();
	//Identifiable& operator=(const Identifiable& o){}
	Identifiable(const Identifiable& o) = delete;
	Identifiable(Identifiable&& o);
	virtual ~Identifiable();
	
private:
	static IdType requestId();
	IdType freeId();
	//static IdType setNextId();
protected:
	//void save_chainCallThis(std::ostream&);
	//void load_chainCallThis(std::istream&);
public:
	virtual std::ostream& save(std::ostream&);
	virtual std::istream& load(std::istream&);
	void saveIdent(std::ostream&);
	void loadIdent(std::istream&);
	static Identifiable* findById(IdType);
	/// ellenőrzi, hogy az id szabad-e. Ha nem, akkor hibát dob, amúgy visszaadja forcedId-t
	/// ha validateId igaz, akkor azt is ellenőrzi, hogy forcedId != INVALID_ID
	static IdType forceGetId(IdType, bool validateId = false);
	/// ugyanaz mint foceGetId, de ez nem statikus, és az objektum előző id-jét fel szabadítja, és forcedIt kapja helyette
	void forceSetId(IdType, bool validateId = false);
	IdType getId() const;
};


#endif // IDENTIFIABLE_H
