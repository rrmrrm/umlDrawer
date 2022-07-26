#include "connector.h"
#include "model/plug.h"
#include "persistence/streamoperators.h"

#include <qdebug.h>


Connector::Connector(CEB* containingCE_, CanvasElement_Type ct):
	connectableCEType(ct),
	containingCE(containingCE_)
{
	assert(containingCE_);
}
Connector::Connector(CEB* containingCE_, QRect geometry_, CanvasElement_Type ct):
	connectableCEType(ct),
	geometry(geometry_),
	containingCE(containingCE_)
{
	assert(containingCE_);
}
Connector::Connector(CEB* containingCE_, Direction dir_, CanvasElement_Type ct):
	connectableCEType(ct),
	dir(dir_),
	containingCE(containingCE_)
{
	assert(containingCE_);
}
Connector::~Connector(){
}


std::ostream& Connector::save(std::ostream& os){
	Identifiable::save(os);
	os << connectableCEType << std::endl;
	//os << translation;
	os << dir << std::endl;
	os << geometry;
	//os << std::vector<QPoint>(area.begin(),area.end()) << std::endl;
	os << movedAway << std::endl;
	return os;
}

std::istream& Connector::load(std::istream& is){
	Identifiable::load(is);
	
	int d=0 ; is >> d;//connectableCE/Type konstans
	
	
	int idir = 0; is >> idir; assert(is); dir = (Direction)idir;
	
	is >> geometry;
	is >> movedAway;
	return is;
}

//void Connector::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void Connector::load(std::istream& is){
//	load_chainCallThis(is);
//}
QRect Connector::getGeometry() const{
	return geometry;
}

//std::ostream& Connector::saveAggregations(std::ostream& os){
//	saveAggsChainCallThis(os);
//	return os;
//}
//std::istream& Connector::loadAggregations(std::istream& is){
//	loadAggsChainCallThis(is);
//	return is;
//}
std::ostream& Connector::saveAggregations(std::ostream& os){
	saveContainer(os, connectedPlugs);
	//containingCE.save(os);
	return os;
}
std::istream& Connector::loadAggregations(std::istream& is){
	loadContainer(is, connectedPlugs);
	//containingCE.load(is);
	return is;
}

CanvasElement_Type Connector::getConnectableCEType() const{
	return connectableCEType;
}
Direction Connector::getDir() const{
	return dir;
}
bool Connector::containsPoint(QPoint p) const{
	return geometry.contains(p);
}
void Connector::setDir(Direction dir_){
	assert(!movedAway);
	Direction prevDir = dir;
	dir = dir_;
	emit directionChanged(weak_from_this(), prevDir, dir);
}

void Connector::CW_setGeometry(QRect r){
	geometry = r;
	emit areaChanged(weak_from_this(), r);
}
