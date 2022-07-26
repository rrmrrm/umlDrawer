#include "connectiondata.h"
#include "persistence/streamoperators.h"

#include <fstream>

std::list<ConnectionData> ConnectionData::modellIntraconnections;

ConnectionData::ConnectionData()
{}
ConnectionData::ConnectionData(IdType srcId_, QString signal_, IdType dstId_, QString slot_)
	: srcId(srcId_),
	  signal(signal_),
	  dstId(dstId_),
	  slot(slot_)
{}





void ConnectionData::save(std::ostream& os) const{
	os << srcId << std::endl;
	writeWSpacedStr(os, signal);
	os << dstId << std::endl;
	writeWSpacedStr(os, slot);
}
void ConnectionData::load(std::istream& is){
	is >> srcId;
	readWSpacedStr(is, signal);
	is >> dstId;
	readWSpacedStr(is, slot);
	
	Identifiable* src = Identifiable::findById(srcId);
	Identifiable* dst = Identifiable::findById(dstId);
	assert(src);
	assert(dst);
	
	auto qSrc = dynamic_cast<QObject*>(src);
	auto qDst = dynamic_cast<QObject*>(dst);
	assert(qSrc);
	assert(qDst);
	
	ConnectionData::modellIntraconnections.push_back(ConnectionData(srcId, signal, dstId, slot));
	QObject::connect(qSrc, 
					 signal.toStdString().c_str(), 
					 qDst, 
					 slot.toStdString().c_str()
			);
}

void ConnectionData::saveAllModelIntraConns(std::ostream& os) const{
	os << modellIntraconnections.size() << std::endl;
	for(auto& it : modellIntraconnections){
		it.save(os);
	}
}
void ConnectionData::loadAllModelIntraConns(std::istream& is){
	int siz = 0;
	is >> siz;
	/// a tárolóméret hibás beolvasása esetén nem próbáljuk ekkora mérettel létrehozni a tárolót.
	assert(!is.fail());
	modellIntraconnections.resize(siz);
	for (auto it = modellIntraconnections.begin(); it != modellIntraconnections.end(); ++it) {
		it->load(is);
	}
}

