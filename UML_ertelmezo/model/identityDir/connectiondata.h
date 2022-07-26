#ifndef CONNECTIONDATA_H
#define CONNECTIONDATA_H

#include "identifiable.h"

#include <QString>
#include <QObject>

#include <list>
#include <assert.h>

class Identifiable;

/// TODO: amikor QObject::disconnect hívódik meg,
///  akkor a modellIntraconnections listából törölni kell a megfelelö ConnectionData-t

struct ConnectionData{

	/// tárolom az összes connection-t, ami a modell réteg két eleme között van létrehozva.
	/// ez a modell állapotának mentéséhez és betöltéséhez kell, 
	///  konkrétebban két Uml elem közötti Connector-Plug összekapcsoltság mentéséhez és betöltéséhez.
	static std::list<ConnectionData> modellIntraconnections;
	IdType srcId;
	QString signal;
	IdType dstId;
	QString slot;
	
public:
	ConnectionData();
	ConnectionData(IdType srcId, QString signal, IdType dstId, QString slot);
public:
	void save(std::ostream& os) const;
	void load(std::istream& is);
	void saveAllModelIntraConns(std::ostream& os) const;
	void loadAllModelIntraConns(std::istream& is);
	
	
};

template<class T1, class T2>
void modelConnect(T1* source, QString signal, T2* target, QString slot){
	static_assert(std::is_base_of_v<QObject,T1>, "Hiba! modelConnect: Csak QObject ES Identifiable leszarmazottait lehet osszekapcsolni");
	static_assert(std::is_base_of_v<QObject, T2>, "Hiba! modelConnect: Csak QObject ES Identifiable leszarmazottait lehet osszekapcsolni");
	static_assert(std::is_base_of_v<Identifiable, T1>, "Hiba! modelConnect: Csak QObject ES Identifiable leszarmazottait lehet osszekapcsolni");
	static_assert(std::is_base_of_v<Identifiable, T2>, "Hiba! modelConnect: Csak QObject ES Identifiable leszarmazottait lehet osszekapcsolni");
	
	ConnectionData::modellIntraconnections.push_back(ConnectionData(source->getId(), signal, target->getId(), slot));
	
	QObject::connect(source, signal.toStdString().c_str(), target, slot.toStdString().c_str());
}

#endif // CONNECTIONDATA_H
