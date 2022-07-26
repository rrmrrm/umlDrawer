#include "plug.h"
#include "connector.h"
#include "model/identityDir/connectiondata.h"

Plug::Plug():containingCE(nullptr)
{
	qDebug() << "warning: Plug::Plug(): empty ctor called containingCE will be nullptr\n";
}
Plug::Plug(CEB* cceb) : containingCE(cceb){
}
//Plug::Plug(CEB* cceb, uint id_inCE_) : 
//	//id_inCE(id_inCE_),
//   containingCE(cceb)
//{
//}
Plug::~Plug(){
	detach();
}


std::ostream& Plug::save(std::ostream& os){
	Identifiable::save(os);
	//os << id_inCE << std::endl;
	return os;
}

std::istream& Plug::load(std::istream& is){
	Identifiable::load(is);
	//is >> id_inCE;
	return is;
}
std::ostream& Plug::saveAggregations(std::ostream& os){
	//base::saveAggsChainCallThis(os);
	connectedConn.save(os);
	return os;
}

std::istream& Plug::loadAggregations(std::istream& is){
	//base::loadAggsFinalizeChainCallThis(is);
	connectedConn.load(is, this);
	return is;
}

//std::ostream& Plug::saveAggregations(std::ostream& os){
//	Plug::saveAggsChainCallThis(os);
//	return os;
//}
//
//std::istream& Plug::loadAggregations(std::istream& is){
//	Plug::loadAggsFinalizeChainCallThis(is);
//	return is;
//}

//void Plug::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//
//void Plug::load(std::istream& is){
//	load_chainCallThis(is);
//}

//void Plug::setId(uint id_inCE_){
//	id_inCE = id_inCE_;
//}
//uint Plug::getId(){
//	return id_inCE;
//}

std::weak_ptr<Connector> Plug::getConnectedConn() const{
	return connectedConn;
}
void Plug::attach(Connector& conn){
	connectedConn = conn.weak_from_this();
	auto obs = connectedConn.lock();
	obs->connectedPlugs.push_back(weak_from_this());
	modelConnect(obs.get(),
		SIGNAL(translationChanged(std::weak_ptr<Connector>, QPoint, QPoint)),
		this,
		SLOT(connectorTranslationChanged(std::weak_ptr<Connector>, QPoint, QPoint))
	);
	modelConnect(obs.get(),
		SIGNAL(directionChanged(std::weak_ptr<Connector>, Direction, Direction)),
		this,
		SLOT(connectorDirectionChanged(std::weak_ptr<Connector>, Direction, Direction))
	);
	modelConnect(obs.get(),
		SIGNAL(areaChanged(std::weak_ptr<Connector>, QRect)),
		this,
		SLOT(connectorAreaChanged(std::weak_ptr<Connector>, QRect))
	);
}
void Plug::detach(){
	if(auto obs = connectedConn.lock()){
		disconnect(obs.get(),
			&Connector::translationChanged,
			this,
			&Plug::connectorTranslationChanged
		);
		disconnect(obs.get(),
			&Connector::directionChanged,
			this,
			&Plug::connectorDirectionChanged
		);
		disconnect(obs.get(),
			&Connector::areaChanged,
			this,
			&Plug::connectorAreaChanged
		);
		/// a csatlakozott Connector connectedPlugs vektorából törlöm ezt a Plug-ot
		auto found = std::find_if(
					obs->connectedPlugs.begin(), 
					obs->connectedPlugs.end(),
					[this](Myweak_ptr<Plug>& plug ){
						if(auto pobs = plug.lock())//assyem ey nem muksyik tesytelni
							return shared_from_this().get() == pobs.get();
						return false;
					}
		);
		if(found != obs->connectedPlugs.end()){
			obs->connectedPlugs.erase(found);
		}
		connectedConn.reset();
	}
}

//void Plug::tryResizeConnectedConn(std::weak_ptr<Plug> pl, QSize newSize){
//	(void) pl;
//	if(auto obs = connectedConn.lock()){
//		obs->tryResize(newSize);
//	}
//}
CEB* Plug::getContainingCE(){
	return containingCE;
}


void Plug::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, QPoint prevTr, QPoint newTr){
	emit translationChangedCalled(srcConn, weak_from_this(), prevTr, newTr);
}
void Plug::connectorDirectionChanged(std::weak_ptr<Connector>srcConn, Direction prevDir, Direction newDir){
	emit directionChangedCalled(srcConn, weak_from_this(),prevDir,newDir);
}
void Plug::connectorAreaChanged(std::weak_ptr<Connector> srcConn, QRect newArea){
	emit areaChangedCalled(srcConn, weak_from_this(), newArea);
}


//PlugGeom::PlugGeom():Plug(nullptr)
//{
//	qDebug() << "warning: PlugGeom::PlugGeom(): empty ctor called containingCE will be nullptr\n";
//}
//PlugGeom::PlugGeom(CEB* cceb) : Plug(cceb){
//}
//PlugGeom::PlugGeom(CEB* cceb, uint id_inCE_) : Plug(cceb, id_inCE_){
//}
//PlugGeom::~PlugGeom(){
//	
//}
//
//void PlugGeom::setAttachedSide(Direction newSide){
//	attachedSide = newSide;
//}
//void PlugGeom::setRelToConn(QPoint rel){
//	relToConn = rel;
//}
//
//QPoint PlugGeom::getLastPoint(){
//	if(isAttachedToConn())
//		updateAttP();
//	return lastPoint;
//}
//bool PlugGeom::isAttachedToConn() const{
//	return connectedConn.lock().get();
//}
//QPoint PlugGeom::updateAttP(){
//	if(!isAttachedToConn())
//		return QPoint();
//	auto conn = connectedConn.lock();
//	QRect cg = conn->getGeometry();
//	QPoint attP;
//	switch (attachedSide) {
//	case LEFT:{
//		attP = QPoint(cg.left(), relToConn.y());
//		break;
//	}
//	case RIGHT:{
//		attP = QPoint(cg.right(), relToConn.y());
//		break;
//	}
//	case TOP:{
//		attP = QPoint(relToConn.x(), cg.top());
//		break;
//	}
//	case BOT:{
//		attP = QPoint(relToConn.x(), cg.bottom());
//		break;
//	}
//	}
//	lastPoint = attP;
//	return attP;
//}
