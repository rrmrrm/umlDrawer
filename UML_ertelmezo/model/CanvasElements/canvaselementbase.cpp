#include "canvaselementbase.h"
#include "model/CanvasElements/canvaselement.h"
#include "model/plug.h"
#include "model/connector.h"
#include "model/target.h"
#include "persistence/streamoperators.h"
#include "model/identityDir/connectiondata.h"

#include <QObject>

CanvasElementStateOp::CanvasElementStateOp(std::weak_ptr<Plug> plug_,
			std::weak_ptr<Connector> prevConnectedConn_,
			std::weak_ptr<Connector> newConnectedConn_)
	:
	  plug(plug_),
	  prevConnectedConn(prevConnectedConn_),
	  newConnectedConn(newConnectedConn_)
{
}
void CanvasElementStateOp::undo(){
	auto obsPl = plug.lock();
	assert(obsPl);
	
	obsPl.get()->detach();
	auto opcc = prevConnectedConn.lock();
	if(opcc)
		obsPl.get()->attach(*opcc.get());
}
void CanvasElementStateOp::redo(){
	auto obsPl = plug.lock();
	assert(obsPl);
	
	obsPl.get()->detach();
	auto oncc = newConnectedConn.lock();
	if(oncc)
		obsPl.get()->attach(*oncc.get());
}




CEB::CEB(
		CanvasElement_Type type_,
		QRect geometry_) 
	: 
	  CEtype(type_),
	  geometry(geometry_),
	  lastDragPoint(0,0)
{
}
CEB::~CEB(){
	for(auto p: plugs){
		if(auto pO = p.lock()){
			pO->detach();
		}
	}
}
//std::shared_ptr<CEB> CEB::create(CanvasElement_Type CEtype){
//	switch(CEtype){
//	case LINE: {
//		return std::make_shared<LineComponent>();
//		break;
//	}
//	case TEXTBOX: {
//		return std::make_shared<TextComponent>();
//		break;
//	}
//	case UMLCLASS: {
//		return std::make_shared<UMLClassComponent>();
//		break;
//	}
//	case RELATIONSHIP: {
//		return createRel(geometry);
//		break;
//	}
//	default:{
//		assert(false);
//	}
//	}
//}

void CEB::moveTopLeft(int cX, int cY){
	geometry.moveTopLeft(QPoint(cX, cY));
	emit geometryChanged_CEW();
}
void CEB::forwardPlugSig(Plug* pl){
	modelConnect(pl,
		SIGNAL(translationChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QPoint, QPoint)),
		this,
		SLOT(connectorTranslationChanged(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QPoint, QPoint))
	);
	modelConnect(pl,
		SIGNAL(directionChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, Direction, Direction)),
		this,
		SLOT(connectorDirectionChanged(std::weak_ptr<Connector>, std::weak_ptr<Plug>, Direction, Direction))
	);
	modelConnect(pl,
		SIGNAL(areaChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QRect)),
		this,
		SLOT(connectorAreaChanged(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QRect))
	);
}
std::ostream& CEB::saveAggregations(std::ostream& os){
	//base::saveAggsChainCallThis(..)
	saveContainer(os, connectors);
	saveContainer(os, plugs);
	return os;
}
std::istream& CEB::loadAggregations(std::istream& is){
	//base::loadAggsFinalizeChainCallThis(..)
	loadContainer(is, connectors);
	loadContainer(is, plugs);
	return is;
}
std::ostream& CEB::save(std::ostream& os){
	const std::string idToken = "Identifiable";
	const std::string cebToken = "CEB";
	
		os << idToken << std::endl;
	Identifiable::save(os);
		os << cebToken << std::endl;
	os << CEtype << std::endl;
	os << isSelected_ << std::endl;
	
	os << geometry << std::endl;
	os << dragStartX << std::endl;
	os << dragStartY << std::endl;
	os << lastDragPoint << std::endl;
	os << isDragging << std::endl;
	return os;
}
std::istream& CEB::load(std::istream& is){
	const std::string idToken = "Identifiable";
	const std::string cebToken = "CEB";
	std::string checkStr = "";
	
		is >> checkStr; assert(checkStr == idToken);
	Identifiable::load(is);
		is >> checkStr; assert(checkStr == cebToken);
	//CEType konstans az objektum létrehozásakor helyesen beálítódik. a helyes beállításáról létrehozásakor gondoskodunk. nem kel betölteni:
	int icet = 0; is >> icet; assert(is);// CEtype = (CanvasElement_Type)icet;
	is >> isSelected_;
	
	is >> geometry;
	is >> dragStartX;
	is >> dragStartY;
	is >> lastDragPoint;
	is >> isDragging;///TODO: itt isDragging=true valamiért minden CE esetén. ez valszeg hiba
	return is;
}
void CEB::dragUnselectedOverrideThis(int canvas_x, int canvas_y){
	for(auto conn : connectors){
		
	}
	emit drag_CEW(canvas_x, canvas_y);
}
void CEB::stopDragOverrideThis(QPoint cPos){
	emit connectToCEAt(weak_from_this(), cPos, getMainPlug());
}

//std::ostream& CEB::saveAggregations(std::ostream& os){
//	CEB::saveAggsChainCallThis(os);
//	return os;
//}
//std::istream& CEB::loadAggregations(std::istream& is){
//	CEB::loadAggsFinalizeChainCallThis(is);
//	return is;
//}
//void CEB::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void CEB::load(std::istream& is){
//	load_chainCallThis(is);
//}
//void CEB::connectPlugToConnn(std::weak_ptr<Connector> targetConn,  std::weak_ptr<Plug> plDragged){
//	auto cObs = targetConn.lock();
//	auto plObs = plDragged.lock();
//	assert(cObs);
//	assert(plObs);
//	
//	auto plContO = plObs->getContainingCE();
//	assert(plContO);
//	if(cObs->getConnectableCEType() == plContO->CEtype){
//		plObs->detach();
//		plObs->attach(*cObs);
//		emit connectPlugToConn_C(targetConn, plDragged);
//		//emit canvas_connectPlugToConn(targetConn, plDragged);//regi
//	}
//}

QRect CEB::getGeometry(){
	return geometry;
}
// ///TODO: eyelket megcsinalni, a gemotetrit a neyet frissiti.
// ///a CEB innentol neyetmodel, eyut'n a model oldalon el;rheto lesy minden geometriai info
//std::weak_ptr<Plug> CEB::getPlugAt(QPoint p){
//	for(auto pl : plugs){
//		if(auto plO = pl.lock()){
//			if(plO->getGeometry().contains(p)){
//				return pl;
//			}
//		}
//	}
//	return std::weak_ptr<Plug>();
//}
//std::weak_ptr<Plug> CEB::getPlugAt(int cX, int cY){
//	return getPlugAt(QPoint(cX, cY));
//}
std::weak_ptr<Connector> CEB::getConnAt(QPoint p){
	for(auto conn : connectors){
		if(auto connO = conn.lock()){
			if(connO->getGeometry().contains(p)){
				return conn;
			}
		}
	}
	return std::weak_ptr<Connector>();
}
std::weak_ptr<Connector> CEB::getConnAt(int cX, int cY){
	return getConnAt(QPoint(cX, cY));
}

//std::weak_ptr<Plug> CEB::getMainPlug() const{
//	assert(plugs.size() > 0);
//	return plugs[0];
//}
std::vector<AggregationT<Connector>> CEB::getConnectors(){
	return connectors;
}
bool CEB::containsPoint(QPoint p) const{
	return geometry.contains(p);
}
/// TODO: minden leszármazó canvasElement-ben feltölteni és managel-ni a connector-s vektort, hogy ez az eljárás általánosan használható legyen:
std::weak_ptr<Connector> CEB::getConnFirstAtCanvasCoords(QPoint coords){
	auto conns = getConnectors();
	for(int i = 0 ; i < (int)conns.size() ; ++i){
		if(auto co = conns[i].lock()){
			if(co->containsPoint(coords)){
				return co->weak_from_this();
			}
		}
	}
	return std::weak_ptr<Connector>();
}
/// TODO: minden leszármazó canvasElement-ben feltölteni és managel-ni a connector-s vektort, hogy ez az eljárás általánosan használható legyen:
std::vector<AggregationT<Connector>> CEB::getConnsAllAtCanvasCoords(QPoint coords){
	std::vector<AggregationT<Connector>> ret;
	for(int i = 0 ; i < (int)connectors.size() ; ++i){
		if(auto co = connectors[i].lock()){
			if(co->containsPoint(coords)){
				ret.push_back(co->weak_from_this());
			}
		}
	}
	return ret;
}
int CEB::x() const {return geometry.x();}
int CEB::y() const {return geometry.y();}
int CEB::width() const {return geometry.width();}
int CEB::height() const {return geometry.height();}
void CEB::dragUnselected(int canvas_x, int canvas_y){
	bool first = !isDragging;
	dragUnselectedOverrideThis(canvas_x, canvas_y);
	if(first){
		isDragging = true;
	}
	//emit drag_CEW(canvas_x, canvas_y);
	
	//emit geometryChanged();
	emit should_repaint();
	
}
void CEB::select(){ isSelected_ = true; }
void CEB::deselect(){ isSelected_ = false;}
bool CEB::isSelected(){ return isSelected_;}
void CEB::dragSelected(int canvas_x, int canvas_y){
	(void) canvas_x;
	(void) canvas_y;
	qDebug() << "CEW_Base::dragSelected is NOT IMPLEMENTED YET" << '\n';
}
void CEB::stopDrag(QPoint cPos){
	stopDragOverrideThis(cPos);
	isDragging = false;
	lastDragPoint = cPos;
}

std::weak_ptr<Connector> CEB::tryConnToThis(std::weak_ptr<CEB> srcOwner, QPoint pOnCanv, std::weak_ptr<Plug> plug){
	auto observedPlug = plug.lock();
	if(!observedPlug)
		return std::shared_ptr<Connector>();
	auto srcObserved = srcOwner.lock();
	if(srcObserved == nullptr)
		return std::shared_ptr<Connector>();
	for(auto& it : getConnectors()){
		if(!it.lock())
			continue;
		if(it.lock()->getGeometry().contains(pOnCanv) && it.lock()->getConnectableCEType() == srcObserved->CEtype){
			auto prevConnectedConn = observedPlug.get()->getConnectedConn();
			observedPlug.get()->detach();
			observedPlug.get()->attach(*it.lock());
			
			//ModelStateManager::addStateOp(new CanvasElementStateOp(
			//		observedPlug,
			//		prevConnectedConn,
			//		observedPlug.get()->getConnectedConn()
			//));
			srcObserved->connectionAllowed(weak_from_this(), it, pOnCanv, plug);
			return it;
		}
	}
	return std::shared_ptr<Connector>();
}
void CEB::forceConnToConnector(
			std::weak_ptr<CEB> srcOwner, 
			std::weak_ptr<Connector> target,
			std::weak_ptr<Plug> plug){
	auto srcObserved = srcOwner.lock();
	if(srcObserved == nullptr)
		return;
	auto to = target.lock();
	if(to == nullptr)
		return;
	auto observedPlug = plug.lock();
	if(!observedPlug)
		return;
	
	auto prevConnectedConn = observedPlug.get()->getConnectedConn();
	observedPlug.get()->detach();
	observedPlug.get()->attach(*to.get());
	
	//ModelStateManager::addStateOp(new CanvasElementStateOp(
	//		observedPlug,
	//		prevConnectedConn,
	//		observedPlug.get()->getConnectedConn()
	//));
	srcObserved->connectionAllowed(weak_from_this(), to, QPoint(0,0), plug);
}
void CEB::connectionAllowed(std::weak_ptr<CEB> ceT, 
							std::weak_ptr<Connector> connT, 
							QPoint, 
							std::weak_ptr<Plug> plug){
	emit ConnectedTo_C(weak_from_this(), plug, ceT, connT);
}

std::weak_ptr<Connector> CEB::mouseDoublePressed_My(Target t){
	auto targetCEO = t.canvasElement.lock();///TODO: ugyis t.canvasElement.lock()-nak hivom meg ezt a fuggvenyet, ezert nem kell megkülönböztetni shared_from_this-től. a kettő ugynaz lesz.
	auto ceo = shared_from_this();
	assert(targetCEO);
	assert(ceo);
	assert(targetCEO.get() == ceo.get());
	auto pressedConns = getConnFirstAtCanvasCoords(
				QPoint(t.x_inCanvasOriginal, t.y_inCanvasOriginal));
	//auto pressedConns = getConnsAllAtCanvasCoords(
	//			QPoint(t.x_inCanvasOriginal, t.y_inCanvasOriginal));
	return pressedConns;
	//emit mouseDoublePressedSig(t);
}
	   
std::shared_ptr<CEB> CEB::modellCopy(std::shared_ptr<CEB> fromDerived) const{
	/// heterogén típus létrehozása, és bizonyos adatok lemásolása
	/// alábbi sorrendben(a leszármaztatási fa egy nem-levél osztályában(pl a RelationShip-ben) így kell kinéznie aezen eljárás felülírt változatának:
	/// 1.: ha fromDerived üres, akkor a visszatérési shared_ptr létrehozása switchcase-ben
	/// 2.: származt. fában közvetlen megelőző bázis függvényének meghívása az előzőleg létrehozott shared_ptr átadásával "fromDerived" paraméterben.
	/// 3.: a shared_ptr-be a szükséges adatok kimásolása this-ből
	/// ezt a mintát követve:
	/// 
	///1.:
	std::shared_ptr<CEB> ret;
	if(fromDerived){
		/// egy leszármazott osztályból hívtuk meg ezt az eljárást,
		///  ahol létrehoztuk a visszatérési share_ptr-t,
		///  így itt már nem kell létrehozni:
		ret = fromDerived;
	}
	else{
		switch(CEtype){
		case ERROR:{
			assert(false);
			break;
		}
		case LINE:{
			ret = std::make_shared<LineComponent>(geometry);
			break;
		}
		case TEXTBOX:{
			ret = std::make_shared<TextComponent>(geometry);
			break;
		}
		case UMLCLASS:{
			ret = std::make_shared<UMLClassComponent>(geometry);
			break;
		}
		case RELATIONSHIP:{
			/// CEtype==RELATIONSHIP esetén 
			/// a RELATIONSHIP osztályban felülírt modellCopy eljárása fogja ezt a modellCopy eljárást meghívni, 
			///  és meg kell adnia fromDerived-ot.
			/// ezt az if igaz ága kezeli már. ha ide jutunk, akkor hibát dobunk, mert rosszul hívtuk meg ezt az eljárást a leszármazott ReltionShip osztályban:
			assert(false);
			break;
		}
		}		
	}
	
	///2.: báziseljárás meghívása(kimarad):
	/// a modellCopy eljárás a csak CEB osztályban és a származtatási hierarchibánan lefele menet van használatban.
	/// szóval itt nem kell meghívni semmilyen bázisra
	
	/// 3.: másolás: t
	/// csak az objektum leg leszármazottabb részobjektumában kell kell meghívn a copyFrom-ot. 
	/// derivedFrom pedig pontosan akkor üres, ha a legleszármazottab részobjektumban vagyunk(ld: CEB::modellCopy leírásában)
	if(!fromDerived)
		ret->copyFrom(this);
	return ret;
}
void CEB::copyFrom(const CEB* o){
	/// nem masolom isSelected_-t
	/// nem masolom a CEtype-t
	/// nem masolom a plugs-t
	geometry = o->geometry;
	/// nem masolom a dragStartX-t
	/// nem masolom a dragStartY-t
	lastDragPoint = o->lastDragPoint;
	/// nem masolom a isDragging-t
	/// nem masolom a connectors-t
}
void CEB::CEW_setGeometry(QRect r){
	geometry = r;
}
void CEB::connectorTransformed(std::weak_ptr<Connector> src, QSize newSize){
	(void) src;
	(void) newSize;
}
void CEB::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
	(void) prevTr;
	(void) newTr;
}
void CEB::connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction prevDir, Direction newDir){
	(void) srcConn;
	(void) srcPlug;
	(void) prevDir;
	(void) newDir;
}
void CEB::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcConn;
	(void) srcPlug;
	(void) newArea;
}
