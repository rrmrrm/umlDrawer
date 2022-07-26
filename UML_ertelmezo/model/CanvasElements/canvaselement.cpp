#include "model/CanvasElements/canvaselement.h"
#include "model/myqline.h"
#include "persistence/streamoperators.h"
#include "model/plug.h"
#include "model/plug.h"

#include <list>
#include <iterator>
#include <QTimer>

template<class Iter, class Container>
Iter myPrev(Iter it, Container& c, bool& succ){
	Iter ret = it;
	if(ret == c.begin()){
		succ = false;
		return ret;
	}
	--ret;
	succ = true;
	return ret;
}
template<class Iter, class Container>
Iter myNext(Iter it, Container& c, bool& succ){
	Iter it2 = it;
	if(it2 == c.end()){
		succ = false;
		return c.end();
	}
	++it2;
	if(it2 == c.end()){
		succ = false;
		return c.end();
	}
	succ = true;
	return it2;
}

LineComponent::LineComponent(QRect geometry_)
	:CEB(CanvasElement_Type::LINE, geometry_),
		pl1(std::make_shared<Plug>(this)),
		pl2(std::make_shared<Plug>(this)),
		connRel1(std::make_shared<Connector>(this, CanvasElement_Type::RELATIONSHIP)),
		connRel2(std::make_shared<Connector>(this, CanvasElement_Type::RELATIONSHIP)),
		connTB1(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX)),
		connTB2(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX))
	  
{
	plugs.push_back(pl1);
	plugs.push_back(pl2);
	connectors.push_back(connRel1);
	connectors.push_back(connRel2);
	connectors.push_back(connTB1);
	connectors.push_back(connTB2);
	//arrangeConns();
}
LineComponent::LineComponent(
		QRect geometry_, 
		Target start_, 
		Target end_
	) 
	:
		CEB(CanvasElement_Type::LINE, geometry_),
		start(start_),
		end(end_),
		pl1(std::make_shared<Plug>(this)),
		pl2(std::make_shared<Plug>(this)),
		connRel1(std::make_shared<Connector>(this, CanvasElement_Type::RELATIONSHIP)),
		connRel2(std::make_shared<Connector>(this, CanvasElement_Type::RELATIONSHIP)),
		connTB1(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX)),
		connTB2(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX))
{
	plugs.push_back(pl1);
	plugs.push_back(pl2);
	connectors.push_back(connRel1);
	connectors.push_back(connRel2);
	connectors.push_back(connTB1);
	connectors.push_back(connTB2);
	draggingLineIt = lineSegments.end();
	/// inicializáció azon része amihez kellenek az örökölt de már felülírt virtuális függvények:
	QTimer::singleShot( 1000, this, &LineComponent::init);
}
LineComponent::~LineComponent(){
	
}
void LineComponent::addLineSegment(MyQLine&& l){
	lineSegments.push_back(std::move(l));
	connRel2->setDir(l.getDir());
}
void LineComponent::addLineSegmentToFront(MyQLine&& l){
	lineSegments.push_front(std::move(l));
	connRel1->setDir(l.getOtherDir());
}

std::ostream& LineComponent::save(std::ostream& os){
	CEB::save(os);
	
	///először ezt a tárolót kell menteni,
	///  mert a betöltési sorrend = mentési sorrend,
	///  és előbb kell a tárolót betölteni,
	///  és aztán a - tárolótól függő- iterátort:
	os << lineSegments ;
	saveIter(os, lineSegments, draggingLineIt);
	
	os << draggedLinePart << std::endl;
	os << margin << std::endl;
	start.save(os);
	end.save(os);
	saveSharedPtr(os, pl1);
	saveSharedPtr(os, pl2);
	saveSharedPtr(os, connRel1);
	saveSharedPtr(os, connRel2);
	saveSharedPtr(os, connTB1);
	saveSharedPtr(os, connTB2);
	return os;
}
std::istream& LineComponent::load(std::istream& is){
	CEB::load(is);
	
	is >> lineSegments;//először ezt a tárolót kell betölteni, mert az iterátor függ ettől
	loadIter(is, lineSegments, draggingLineIt);
	int d; is >> d; draggedLinePart = (LinePart)d;
	is >> margin;
	start.load(is);
	end.load(is);
	loadSharedPtr(is, pl1, this);
	loadSharedPtr(is, pl2, this);
	loadSharedPtr(is, connRel1, this, CanvasElement_Type::RELATIONSHIP);
	loadSharedPtr(is, connRel2, this, CanvasElement_Type::RELATIONSHIP);
	loadSharedPtr(is, connTB1, this, CanvasElement_Type::TEXTBOX);
	loadSharedPtr(is, connTB2, this, CanvasElement_Type::TEXTBOX);
	return is;
}

std::ostream& LineComponent::saveAggregations(std::ostream& os){
	CEB::saveAggregations(os);
	start.saveAggregations(os);
	end.saveAggregations(os);
	pl1->saveAggregations(os);
	pl2->saveAggregations(os);
	connRel1->saveAggregations(os);
	connRel2->saveAggregations(os);
	connTB1->saveAggregations(os);
	connTB2->saveAggregations(os);
	return os;
}
std::istream& LineComponent::loadAggregations(std::istream& is){
	CEB::loadAggregations(is);
	start.loadAggregations(is);
	end.loadAggregations(is);
	pl1->loadAggregations(is);
	pl2->loadAggregations(is);
	connRel1->loadAggregations(is);
	connRel2->loadAggregations(is);
	connTB1->loadAggregations(is);
	connTB2->loadAggregations(is);
	return is;
}

//std::ostream& LineComponent::saveAggregations(std::ostream& os){
//	CEB::saveAggsChainCallThis(os);
//	return os;
//}
//std::istream& LineComponent::loadAggregations(std::istream& is){
//	CEB::loadAggsFinalizeChainCallThis(is);
//	return is;
//}
std::weak_ptr<Plug> LineComponent::getMainPlug() const{
	return pl1;
}
//egy userdefined objektum mentése és betöltése nem kéne
//több legyen a változóinak mentésénél, de mivel  std tipusokat és  qt tipusokat használok ezért ezekre mind külön mentés függvény kell.
//
//ez az állapot megörzésre általánosan igaz.
//
//emellett a teljes modelt nem hiszem hogy jó ötlet lenne újra tölteni amikor csak pár objektumot akarok benne betölteni.
//ekkor viszont a tárolt objektum betöltéséhez és mentéséhez kellhet extra információ a Modeltől.
//
//a qt connectionöket mostmár tudom menteni, 
//de csak akkor ha:
//  1: egymásólt függetlenül lehet őket kezelni, 
//	(ha egyszerre 3at kell betölteni, akkor ezt előre tudnia kell a betöltő kódnak, 
//	mert ekkor ELŐSZÖR létre kell hozni a 3 objetumot, és AZTÁN összekapcsolni a signal-slotjaikkal(mert csak már létező ojjektumokat lehet összekapcsolni)
//	(elméletileg elég savableQObject subobjectjüket létrehozni ha leszármazottak lennének)).
//vagy 2: ha egy std::vector<T*> sablonú tárolóban vannak és a T* ok heap-en vannak allokálva(nem általános tehát a Qt::connection mentést végző kódom), mert ere már írtam eljárásokat. 
//void LineComponent::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void LineComponent::load(std::istream& is){
//	load_chainCallThis(is);
//}
//std::vector<AggregationT<Connector>> LineComponent::getConnectors(){
//	std::vector<AggregationT<Connector>> ret;
//	ret.push_back(connRel1);
//	ret.push_back(connRel2);
//	return ret;
//}
//Direction LineComponent::getAttachedSide(const Target& target) const{
//	auto ceObs = target.canvasElement.lock();
//	assert(ceObs != nullptr);
//	//az target koordinátarendszerében meghatározzuk a középpontjától való
//	// vízszintes és függőleges eltolást:
//	int wMidX = ceObs.get()->width()/2;
//	int wMidY = ceObs.get()->height()/2;
//	int horizontalDisplacement = target.x_inTarget - wMidX;
//	int vertDisplacement = target.y_inTarget - wMidY;
//	
//	Direction ret;
//	//ha a vízszintes irányban van legtávolabb a widget középpontjától a target:
//	if(std::abs(horizontalDisplacement) >= std::abs(vertDisplacement)){
//		if(horizontalDisplacement >= 0)
//			ret = RIGHT;
//		else 
//			ret = LEFT;
//	}
//	else{// ha függőleges irányban van legtávolabba középponttól:
//		if(vertDisplacement >= 0)
//			ret = BOT;
//		else
//			ret = TOP;
//	}
//	return ret;
//}
bool LineComponent::isInDirection(QPoint p0, Direction dir, QPoint p1, bool strict) const{
	switch (dir) {
	case LEFT:{
		return p1.x() < p0.x() || (!strict && p1.x() == p0.x());
		break;
	}
	case RIGHT:{
		return p1.x() > p0.x() || (!strict && p1.x() == p0.x());
		break;
	}
	case TOP:{
		return p1.y() < p0.y() || (!strict && p1.y() == p0.y());
		break;
	}
	case BOT:{
		return p1.y() > p0.y() || (!strict && p1.y() == p0.y());
		break;
	}
	}
	return false;
}

//returns whether d is in exclusive range (r0, r1):
bool inRangeExcl(int r0, int r1, int d){
	return d > r0 && d < r1;
}
bool LineComponent::isWidgetInWay(const CEB* ceb,
				   Direction dir,
				   QPoint p,
				   QPoint bound, 
				   int margin,//a widgetet ennzivel szélesebbnek tekintjük az ütközés detektálásakor
				   QPoint* OUTcollosionPoint)
				{
	bool ret = false;
	QPoint collosionPoint;
	switch (dir) {
	case LEFT:{
		collosionPoint = QPoint(ceb->x() + ceb->width() + margin, p.y());
		ret = inRangeExcl(ceb->y()-margin, ceb->y() + ceb->height() + margin, p.y())
					&& ceb->x() + ceb->width() + margin < p.x()
					&& bound.x() < ceb->x() + ceb->width() + margin;
		break;
	}
	case RIGHT:{
		collosionPoint = QPoint(ceb->x() - margin, p.y());
		ret = inRangeExcl(ceb->y()-margin, ceb->y() + ceb->height() + margin, p.y())
					&& ceb->x() - margin > p.x()
					&& bound.x() > ceb->x() - margin;
		break;
	}
	case TOP:{
		collosionPoint = QPoint(p.x(), ceb->y() + ceb->height() + margin);
		ret = inRangeExcl(ceb->x()-margin, ceb->x() + ceb->width() + margin, p.x())
					&& ceb->y() + ceb->height() + margin < p.y()
					&& bound.y() < ceb->y() + ceb->height() + margin;
		break;
	}
	case BOT:{
		collosionPoint = QPoint(p.x(), ceb->y() - margin);
		ret = inRangeExcl(ceb->x()-margin, ceb->x() + ceb->width() + margin, p.x())
					&& ceb->y() - margin > p.y()
					&& bound.y() > ceb->y() - margin;
		break;
	}
	}
	if(ret){
		if(OUTcollosionPoint != nullptr)
			*OUTcollosionPoint = collosionPoint;
	}
	return ret;
}

void LineComponent::extendInDirIf(QPoint& lineEndToUpdate, Direction dir, QPoint extensionBound){
	if(!isInDirection(lineEndToUpdate, dir, extensionBound, false)){
		return;
	}
	MyQLine newLine;
	switch (dir){
	case LEFT:{
	}
	case RIGHT:{
		//pEnd = QPoint(extensionBound.x(), lineEndToUpdate.y());
		newLine.fromBounds(lineEndToUpdate, extensionBound, HORIZONTAL);
		break;
	}
	case TOP:{
	}
	case BOT:{
		//pEnd = QPoint(lineEndToUpdate.x(), extensionBound.y());
		newLine.fromBounds(lineEndToUpdate, extensionBound, VERTICAL);
		break;
	}
	}
	lineEndToUpdate = newLine.p2();
	addLineSegment(std::move(newLine));
	//lineSegments.push_back(std::move(newLine));
}
//QPoint LineComponent::moveInDim(QPoint p, Dimension dim, QPoint extensionGoal){
//	switch (dim) {
//	case HORIZONTAL:{
//		return QPoint(extensionGoal.x(), p.y());
//		break;
//	}
//	case VERTICAL:{
//		return QPoint(p.x(), extensionGoal.y());
//		break;
//	}
//	}
//	return QPoint(p.x(), extensionGoal.y());
//}
void LineComponent::rotate90ToPoint(Direction& line1Dir, QPoint p0, QPoint p1){
	switch (line1Dir) {
	case LEFT:{
	}
	case RIGHT:{
		if(p1.y() > p0.y())
			line1Dir = BOT;
		else
			line1Dir = TOP;
		break;
	}
	case TOP:{
	}
	case BOT:{
		if(p1.x() > p0.x())
			line1Dir = RIGHT;
		else
			line1Dir = LEFT;
		break;
	}
	}
}
QPoint LineComponent::getWidgetAttPoint(Direction dir, const CEB* ceb, int margin) const{
	switch (dir) {
	case LEFT:{
		return QPoint(ceb->x() - margin, 0);
		break;
	}
	case RIGHT:{
		return QPoint(ceb->x() + ceb->width() + margin, 0);
		break;
	}
	case TOP:{
		return QPoint(0, ceb->y() - margin);
		break;
	}
	case BOT:{
		return QPoint(0, ceb->y() + ceb->height() + margin);
		break;
	}
	}
	return QPoint();
}
QPoint LineComponent::getWidgetAttPoint(const Target& target) const{
	auto ceObs = target.canvasElement.lock();
	assert(ceObs != nullptr);
	//az target koordinátarendszerében meghatározzuk a középpontjától való
	// vízszintes és függőleges eltolást:
	int wMidX = ceObs.get()->width()/2;
	int wMidY = ceObs.get()->height()/2;
	int horizontalDisplacement = target.x_inTarget - wMidX;
	int vertDisplacement = target.y_inTarget - wMidY;
	
	QPoint ret;
	//ha a vízszintes irányban van legtávolabb a widget középpontjától a target:
	if(std::abs(horizontalDisplacement) >= std::abs(vertDisplacement)){
		if(horizontalDisplacement >= 0)
			ret = QPoint(ceObs.get()->geometry.right(), target.getFreshYInCanvas());
		else 
			ret = QPoint(ceObs.get()->geometry.left(), target.getFreshYInCanvas());
	}
	else{// ha függőleges irányban van legtávolabba középponttól:
		if(vertDisplacement >= 0)
			ret = QPoint(target.getFreshXInCanvas(), ceObs.get()->geometry.bottom());
		else
			ret = QPoint(target.getFreshXInCanvas(), ceObs.get()->geometry.top());
	}
	return ret;
}
QPoint LineComponent::adjustCoordInDimension(QPoint p, Dimension dir, QPoint newCoord){
	switch (dir) {
	case HORIZONTAL:{
		return QPoint(newCoord.x(), p.y());
		break;
	}
	case VERTICAL:{
		return QPoint(p.x(), newCoord.y());
		break;
	}
	}
	return QPoint(newCoord.x(), p.y());
}
void LineComponent::attachToElement(const std::list<MyQLine>::iterator& l, LinePart part, std::weak_ptr<Connector> conn, const Target& t){
	assert(l == lineSegments.begin() || l == std::prev(lineSegments.end()));
	auto connO = conn.lock();
	assert(connO);
	auto ceObs = t.canvasElement.lock();
	if(!ceObs)
		return;
	//csak akkor nyújtjuk a vonalat a canvasElelmt oldalához, ha jó irányban illeszkedik hozzá:
	Direction attachedSide = connO->getDir();
	QPoint attPoint = getWidgetAttPoint(t);
	if(part == START){
		l->movePartAlongLine(attPoint, part);
		/// ha rossz irányú a vonal utolsó szegmense, akkor megtoldjuk egy újal:
		if(dirToDim(attachedSide) != dirToDim(l->getDir())){
			addLineSegmentToFront( MyQLine(attPoint, l->p1(), dirToDim(attachedSide)) );
			//lineSegments.push_front(MyQLine(attPoint, l->p1(), dirToDim(attachedSide)));
		}
	}
	if(part == END){
		l->movePartAlongLine(attPoint, part);
		if( dirToDim(attachedSide) != dirToDim(l->getOtherDir()) ){
			addLineSegment(
							   MyQLine(l->p2(), attPoint, dirToDim(attachedSide))
							   );
			//lineSegments.push_back(MyQLine(l->p2(), attPoint, dirToDim(attachedSide)));
		}
	}
	//arrangeConns();
}
///TODO: draggolaskor TVBoxokat moygat
void LineComponent::moveLines(const std::list<MyQLine>::iterator& lineIt, const QPoint& newPlace, LinePart lp){
	if(lineSegments.end() == lineIt){
		qDebug() << "LineComponent::moveLines INVALID LINE: lineIt==lineSegments.end()" << '\n';
		return;
	}
	switch(lp){
	case MIDDLE:{
		break;
	}
	case START:{
		bool prevSucc = false;
		bool prevPrevSucc = false;
		std::list<MyQLine>::iterator prev = myPrev(lineIt, lineSegments, prevSucc);
		if(prevSucc){
			prev->movePartAlongLine(newPlace, END);
			prev->moveTransversal(newPlace);
		}
	
		//moveLine_setPartPos(prev, newPlace, END);
		std::list<MyQLine>::iterator prevprev = myPrev(prev, lineSegments, prevPrevSucc);
		if(prevPrevSucc){
			prevprev->movePartAlongLine(newPlace, END);
			//adjustPart(prevprev, newPlace, END);
		}
		
		bool nextSucc = false;
		lineIt->moveTransversal(newPlace);
		lineIt->movePartAlongLine(newPlace, START);
		
		std::list<MyQLine>::iterator next = myNext(lineIt, lineSegments, nextSucc);
		if(nextSucc){
			next->movePartAlongLine(newPlace, START);
			//adjustPart(next, newPlace, START);
		}
		break;
	}
	case END:{
		bool prevSucc = false;
		std::list<MyQLine>::iterator prev = myPrev(lineIt, lineSegments, prevSucc);
		if(prevSucc){
			prev->movePartAlongLine(newPlace, END);
			//adjustPart(prev, newPlace, END);
		}
		
		bool nextSucc = false;
		bool nextNextSucc = false;
		lineIt->moveTransversal(newPlace);
		lineIt->movePartAlongLine(newPlace, END);
		//moveLine_setPartPos(lineIt, newPlace, END);
		std::list<MyQLine>::iterator next = myNext(lineIt, lineSegments, nextSucc);
		if(nextSucc){
			next->moveTransversal(newPlace);
			next->movePartAlongLine(newPlace, START);
		}
		//moveLine_setPartPos(next, newPlace, START);
		std::list<MyQLine>::iterator nextnext = myNext(next, lineSegments, nextNextSucc);
		if(nextNextSucc){
			nextnext->movePartAlongLine(newPlace, START);
			//adjustPart(nextnext, newPlace, START);
		}
		break;
	}
	}
	//arrangeConns();
}

///TODO: hatékonyabbá tenni reconnect() -et
void LineComponent::reconnect(){
	if(lineSegments.empty())
		return;
	if(auto c1O = pl1->getConnectedConn().lock()){
		QPoint p1(start.getFreshXInCanvas(),start.getFreshYInCanvas());
		moveLines(lineSegments.begin(), p1, START);
		attachToElement(lineSegments.begin(), START, c1O, start);
	}
	if(auto c2O = pl1->getConnectedConn().lock()){
		QPoint p2(end.getFreshXInCanvas(),end.getFreshYInCanvas());
		moveLines(std::prev(lineSegments.end()), p2, END);
		attachToElement(std::prev(lineSegments.end()), END, c2O, end);
	}
	emit linesChanged_CEW();
}

///TODO: vmiert neha rossz connectorbol indul ki a vonal
void LineComponent::recreateLineSegments(){
	auto ce1 = start.canvasElement.lock();
	auto ce2 = end.canvasElement.lock();
	auto linesPreState = lineSegments;
	if( ce1 == nullptr || ce2 == nullptr){
		//auto p1 = start.getCPoint();
		//auto p2 = end.getCPoint();
		addLineSegment(
			MyQLine(
				HORIZONTAL, 
				start.x_inCanvasOriginal, 
				end.x_inCanvasOriginal,
				start.y_inCanvasOriginal
			)
		);
		addLineSegment(
			MyQLine(
				VERTICAL, 
				start.y_inCanvasOriginal, 
				end.y_inCanvasOriginal,
				end.x_inCanvasOriginal
			)
		);
	}
	else{
		/// line1Dir, line2Dir:
		/// 1. és 2. vonalkezdemény nézési iránya.
		/// line1Dir irányba próbáljuk meg megtoldani az 1. szegmentáltvonal végét,
		/// ha ezzel közelebb jutunk a 2 vinalstegmens összekötéséhez:
		
		/// meghatározom a vonal által összekapcsolt két CE összecsatolt Connector-jét:
		auto c1O = pl1->getConnectedConn().lock();
		auto c2O = pl2->getConnectedConn().lock();
		/// meghatározom, hogy a törtvonal első és utolsó szegmense milyen irányba csatlakozzon a megfelelő Connector-okhoz:
		Direction line1Dir;
		Direction line2Dir;
		if(c1O){
			line1Dir = c1O->getDir();
		}
		else{
			///ha pl1 egy connector-hoz se csatlakozik, akkor RIGHT irányú lesz az első(és utolsó) vonalszegmens:
			line1Dir = RIGHT;
		}
		if(c2O){
			line2Dir = c2O->getDir();
		}
		else{
			///ha pl2 nincs egy connector-hoz se csatlakozik, akkor RIGHT irányú lesz az utolsó(és első) vonalszegmens:
			line2Dir = RIGHT;
			
		}
		
		lineSegments.clear();
		// a 2 EntityWidget vonalhoz csatlakozó oldalainak meghatározása, és
		//  vonalkezdemények kiépítése a 2 widgetből(előre definiálni ezen vonalkezdemények hosszát. lehet mondjuk 30 pixel)
		
		// az első vonalkezdeményt fogjuk újabb szegmensekkel kiegészíteni, hogy összeérjen a 2. vonalkezdeménnyel
		
		// vonalkezdeménynek több szegmense is lehet, és a vonalkezdemény iránya 
		// általában a benne levő utolsó vonalszegmens irányát jelenti, de vannak esetek, amikor ettől eltér.
		// szóval pontosabban a vonalkezdemény "nézésének" az irányát jelenti.
		// Nézési irány alatt azt az irányt értem, amelyik irányba próbáljuk meghosszabítani a vonalkezdeményt,
		//  hogy összeköthessük a 2. vonalkedeménnyel(és ha nem ez a jó irány, akkor megváltoztatjuk a nézési irényt)
		
		int minimalHangoutDist = 40;
		int margin = 10;
		int ce1Left = ce1->x();
		int ce1Right = ce1->x() + ce1->width();
		int ce1Top = ce1->y();
		int ce1Bot = ce1->y() + ce1->height();
		
		int ce2Left = ce2->x();
		int ce2Right = ce2->x() + ce2->width();
		int ce2Top = ce2->y();
		int ce2Bot = ce2->y() + ce2->height();
		QPoint line1Start;
		QPoint line1End;
		switch (line1Dir) {
		case LEFT:{
			line1Start = QPoint( ce1Left, start.getFreshYInCanvas());
			line1End = QPoint( line1Start.x() - minimalHangoutDist, line1Start.y());
			break;
		}
		case RIGHT:{
			line1Start = QPoint( ce1Right, start.getFreshYInCanvas());
			line1End = QPoint( line1Start.x() + minimalHangoutDist, line1Start.y());
			break;
		}
		case TOP:{
			line1Start = QPoint( start.getFreshXInCanvas(), ce1Top);
			line1End = QPoint( line1Start.x(), line1Start.y() - minimalHangoutDist);
			break;
		}
		case BOT:{
			line1Start = QPoint( start.getFreshXInCanvas(), ce1Bot);
			line1End = QPoint( line1Start.x(), line1Start.y() + minimalHangoutDist);
			break;
		}
		}
		//az első vonalkezdemény első szegmensét eltároljuk a lineSegments tömbben:
		addLineSegment(
		   MyQLine(
			   line1Start,
			   line1End,
			   dirToDim(line1Dir)
		));
		//lineSegments.push_back(
		//		MyQLine(
		//			line1Start,
		//			line1End,
		//			dirToDim(line1Dir)
		//			)
		//		);
		QPoint line2Start;
		QPoint line2End;
		switch (line2Dir) {
		case LEFT:{
			line2Start = QPoint( ce2Left, end.getFreshYInCanvas());
			line2End = QPoint( line2Start.x() - minimalHangoutDist, line2Start.y());
			break;
		}
		case RIGHT:{
			line2Start = QPoint( ce2Right, end.getFreshYInCanvas());
			line2End = QPoint( line2Start.x() + minimalHangoutDist, line2Start.y());
			break;
		}
		case TOP:{
			line2Start = QPoint( end.getFreshXInCanvas(), ce2Top);
			line2End = QPoint( line2Start.x(), line2Start.y() - minimalHangoutDist);
			break;
		}
		case BOT:{
			line2Start = QPoint( end.getFreshXInCanvas(), ce2Bot);
			line2End = QPoint( line2Start.x(), line2Start.y() + minimalHangoutDist);
			break;
		}
		}
		//(a 2. vonalkezdeményt a két vonal összeérésekor mentjük a lineSegments-be)
		
		// ISMÉTELNI amíg a két vonalkezdemény össze nem ér, de maximum mondjuk 6 iterációig:
		//	 HA
		//	  1.: az 1. vonalkezdemény - line1Dir irányban való -meghosszabbításával az 1. vonalkezdemény irányában a 2 vonalkezdmény távolsága TÉNYLEGESEN csökken ÉS
		//	  2.: nincs útban se az 1. , se a 2. widget
		//	 AKKOR, 
		//			az 1. vonalkezdemény meghosszabbítása line1Dir irányban, hogy pont egy vonalban legyen a 2. vonalkezdeménnyel
		//	 AMÚGY:
		//			HA 1. nem teljesül(távolság nem csökkenne), akkor 
		//				meghatározni, hogy a vonalkezdeményre merőéleges 2 irány közül merre van a 2. vonalkezdemény vége, és
		//				ebben az irányba "fordulni"(AKKOR IS FORDULNI KELL, ha egyik irány sem jobb a másiknál) 
		//				(lehet, hogy ebben az irányban ütköznénk a widgettel, ezért nem húzhatunk vonalat, de meg kell változtatni az irányt, hogy ezesetben a widgettel való ütközést észlelje a program, és megkerülje a widgetet a következő iterációkban.)
		//			AMÚGY HA 2. nem teljesül(útban van az 1, , vagy 2. widget), akkor 
		//				az 1. vonalkezdeményt meghosszabítani, hogy nekiérjen az ütköző widgetnek,  
		//				 aztán merőleges irányban meghosszabítani úgy, hogy a hosszabítás irányában túllógjon (MENNYIVEL?) a widgeten, és
		//				 válasszuk a lehetséges közül azt, amelyik irányba vana 2. vonalkezd. vége(AKKOR IS VÁLASSZUNK irányt, ha mindkét irány egyaránt jó)
		//	ha elértük az iterációlimitet, akkor felhasználó felé jelezni, hogy sikertelen volt a vonal létrehozása
		//  egyirányú egymásmelletti vonalszegmensek összevonása
		//  az ellentétes irányú egymásmelletti szegmenseket széttolni egymástól, aztán merőleges szegmenssel összekötni őket
		//    a szélső szegmenseket nem szabad mozgatni. ha az egyik szegmens szélső szegmens, akkor a másikat kell eltolni.
		uint iter = 0;
		uint allowedIterations = 20;
		bool linesConnected = false;
		while( !linesConnected && iter < allowedIterations){
			bool inDir = isInDirection(line1End, line1Dir, line2End, true);
			QPoint collosionPoint;
			bool ce1Blocking = isWidgetInWay(ce1.get(), line1Dir, line1End, line2End, margin, &collosionPoint);
			bool ce2Blocking = isWidgetInWay(ce2.get(), line1Dir, line1End, line2End, margin, &collosionPoint);
			bool clearPath = !ce1Blocking && !ce2Blocking;
			if(inDir && clearPath){
				extendInDirIf(line1End, line1Dir, line2End);
			}
			else{
				if(!inDir){
					//(!inDir && !clearPath) || (!inDir && clearPath)
					rotate90ToPoint(line1Dir, line1End, line2End);
				}
				else{
					//(inDir && !clearPath)
					extendInDirIf(line1End, line1Dir, collosionPoint);
					rotate90ToPoint(line1Dir, line1End, line2End);
					if(ce1Blocking)
						extendInDirIf(line1End, line1Dir, getWidgetAttPoint(line1Dir, ce1.get(), margin));
					else
						extendInDirIf(line1End, line1Dir, getWidgetAttPoint(line1Dir, ce2.get(), margin));
				}
			}
			linesConnected = (line1End == line2End);
			++iter;
		}
		if(linesConnected){
			//2. vonalkezdemény egyetlen szegmensét hozzáadjuk a vonalszegmensekhez:
			//a 2. vonal a 2. widgettől indul, ezért kell megcserélni a kezdő és végpontját, amikor az első vonalhoz csatolom
			addLineSegment(
							   MyQLine(line2End, line2Start, dirToDim(line2Dir))
							   );
			//lineSegments.push_back(MyQLine(line2End, line2Start, dirToDim(line2Dir)));
		}
		else{
			lineSegments.resize(0);
		}
		// egyirányú egymásmelletti vonalszegmensek összevonása:
		mergeLinesWhenNeeded();
	}
	/////TODO: lehet, hogy a másolás invalidálja az iterátort, ekkor nem fog jól működni az draggingLineIt undo-redo-zása
	//ModelStateManager::addStateOp(new ModelStateOp_composite);
	//ModelStateManager::addStateOp(
	//			new GenericModelStateOp<std::list<MyQLine>>(lineSegments, linesPreState, &lineSegments));
	
	auto prevDraggingLineIt = draggingLineIt;
	//arrangeConns();
	draggingLineIt = lineSegments.end();
	// itt resetelem a draggingLineIt iterátort, mert invalid'l=dott a vonalsyegmensek ujraépítésekor

	//ModelStateManager::addStateOp(new GenericModelStateOp<std::list<MyQLine>::iterator>(prevDraggingLineIt,draggingLineIt,&draggingLineIt));
	//ModelStateManager::finishCompositeStateOp();
	
	emit linesChanged_CEW();
}

void LineComponent::mergeLinesWhenNeeded(){
	int mergeTreshold = 5;
	if(lineSegments.begin() != lineSegments.end() && std::next(lineSegments.begin()) != lineSegments.end()){
		auto l2 = std::next(lineSegments.begin());
		for( ; l2 != lineSegments.end() ; ++l2){
			auto l1 = std::prev(l2);
			if(l1->getDim() == l2->getDim()){
				l2->movePartAlongLine(l1->p1(),START, false);
				l1 = lineSegments.erase(l1);
			}
		}
	}
	for(auto l = lineSegments.begin() ; l != lineSegments.end() ; ++l){
		qDebug() << l->dx() << "\n";
		qDebug() << l->dy();
		if(std::abs(l->dx()) <= mergeTreshold && std::abs(l->dy()) <= mergeTreshold){
			QPoint erasedLineEnd = l->getPart(END);
			l = lineSegments.erase(l);// megjegyzés: itt továbbiteráljuk l-t
			//itt lehet, hogy elértük a lineSegments végét és túliteráltunk az erase-el, 
			// ha ez történt, akkor a ciklusból kiugrunk
			if(l == lineSegments.end())
				break;
			l->moveTransversal(erasedLineEnd, false);
			bool nextSucc;
			auto next = myNext(l, lineSegments, nextSucc);
			if(nextSucc){
				next->movePartAlongLine(l->getPart(END), START, false);
			}
			
			bool prevSucc;
			l = myPrev(l, lineSegments, prevSucc);
		}
	}
	// mégegyszer öszevonom az egymásmelletti párhuzamos vonalakat, 
	// mert az előző ciklusban keletkezhetett pár egymásmelletti párhuzamos vonal:
	if(lineSegments.begin() != lineSegments.end() && std::next(lineSegments.begin()) != lineSegments.end()){
		auto l2 = std::next(lineSegments.begin());
		for( ; l2 != lineSegments.end() ; ++l2){
			auto l1 = std::prev(l2);
			if(l1->getDim() == l2->getDim()){
				l2->movePartAlongLine(l1->p1(),START, false);
				l1 = lineSegments.erase(l1);
			}
		}
	}
	for(const MyQLine& it : lineSegments){
		it.validate();
	}
	//arrangeConns();
	emit linesChanged_CEW();
}
void LineComponent::dragUnselectedOverrideThis(int canvas_x, int canvas_y){
	QPoint asPoint(canvas_x, canvas_y);
	if(!isDragging){
		//ModelStateManager::addStateOp(new ModelStateOp_composite);
		auto linesPreState = lineSegments;
		auto prevDraggingLineIt = draggingLineIt;
		
		double linedivisionRatio = 0;//ha vonal elejérea kattintotak akkor 0, ha végére akkor 1;
		draggingLineIt = getLineItAtCanvasCoords(canvas_x, canvas_y, &linedivisionRatio);

		if(lineSegments.end() != draggingLineIt){
			if(linedivisionRatio <= 0.5){
				draggedLinePart = START;
				bool prevSucc = false;
				myPrev(draggingLineIt, lineSegments, prevSucc);
				if(!prevSucc){
					//ha ez az első szegmens:
					//draggingLineIt->moveTransversal(asPoint);
					draggingLineIt->movePartAlongLine(asPoint, START);
					
					MyQLine newLine = MyQLine(draggingLineIt->p1(), asPoint, draggingLineIt->getOtherDim());
					addLineSegmentToFront(std::move(newLine));
					//lineSegments.push_front(std::move(newLine));
					
					//ezután már az újonna létrehozott vonalat akarjuk draggolni.
					//draggedLinePart vonatkozik az új draggolandó elemre is(az uj vonal ugyanazon oldalát kell draggolni, mint az eredetinek):
					--draggingLineIt;
				}
			}
			else{
				draggedLinePart = END;
				bool nextSucc = false;
				myNext(draggingLineIt, lineSegments, nextSucc);
				//moveLine_setPartPos(draggingLineIt, asPoint, START);
				if(!nextSucc){//ha ez az utolsó szegmens:
					//draggingLineIt->moveTransversal(asPoint);
					draggingLineIt->movePartAlongLine(asPoint, END);
					MyQLine newLine = MyQLine(draggingLineIt->p2(), asPoint, draggingLineIt->getOtherDim());
					addLineSegment(std::move(newLine));
					//lineSegments.push_back(std::move(newLine));
					
					//ezután már az újonna létrehozott vonalat akarjuk draggolni.
					//draggedLinePart vonatkozik az új draggolandó elemre is(az uj vonal ugyanazon oldalát kell draggolni, mint az eredetinek):
					++draggingLineIt;
				}
			}
		}
		//ModelStateManager::addStateOp(
		//	new GenericModelStateOp<std::list<MyQLine>>(lineSegments, linesPreState, &lineSegments)
		//);
		//ModelStateManager::addStateOp(
		//	new GenericModelStateOp<std::list<MyQLine>::iterator>(prevDraggingLineIt, draggingLineIt, &draggingLineIt)
		//);
		//ModelStateManager::finishCompositeStateOp();
	}
	else{
		if(lineSegments.end() != draggingLineIt){
			moveLines(draggingLineIt, asPoint, draggedLinePart);
			/// start és end a törtvonal új végei alapján frissül:
			if(auto sO = start.canvasElement.lock()){
				start.x_inTarget = lineSegments.front().x1() - sO->x();
				start.y_inTarget = lineSegments.front().y1() - sO->y();
			}
			if(auto eO = end.canvasElement.lock()){
				end.x_inTarget = lineSegments.back().x1() - eO->x();
				end.y_inTarget = lineSegments.back().y1() - eO->y();
			}
		}
		else{
			qDebug() << "LineComponent::dragUnselectedOverrideThis INVALID LINE selected" << '\n';
			qDebug() << (&(*draggingLineIt)) << '\n';
			qDebug() << (&(*lineSegments.end())) << '\n';
		}
		
	}
	emit linesChanged_CEW();
	//arrangeConns();
	should_repaint();
}
void LineComponent::dragSelected(int canvas_x, int canvas_y){
	(void) canvas_x;
	(void) canvas_y;
	qDebug() << "LineComponent::dragSelected NOT IMPLEMENTED YET " << '\n';
	
}
void LineComponent::stopDragOverrideThis(QPoint cPos){	
	//ModelStateManager::addStateOp(new ModelStateOp_composite);
	auto linesPreState = lineSegments;
	auto prevDraggingLineIt = draggingLineIt;
	
	if(draggingLineIt != lineSegments.end()){
		bool prevSucc = false;
		bool nextSucc = false;
		
		myPrev(draggingLineIt, lineSegments, prevSucc);
		myNext(draggingLineIt, lineSegments, nextSucc);
		// ha draggingLineIt az első vonalszegmens, és a végét draggoljuk, akkor az új ponton levő widget-hez próbálunk csatlakozni:
		if(!prevSucc && draggedLinePart == START){
			//stoppedOn.lock()->tryConnToThis(weak_from_this(), cPos, pl1);
			emit connectToCEAt(weak_from_this(), cPos, pl1);
		}
		// ha draggingLineIt az utolsó vonalszegmens, és az elejét draggoljuk, akkor az új ponton levő widget-hez próbálunk csatlakozni:
		if(!nextSucc && draggedLinePart == END){
			//stoppedOn.lock()->tryConnToThis(weak_from_this(), cPos, pl2);
			emit connectToCEAt(weak_from_this(), cPos, pl2);
		}
	}
	mergeLinesWhenNeeded();
	draggingLineIt = lineSegments.end();
	
	//ModelStateManager::addStateOp(
	//	new GenericModelStateOp<std::list<MyQLine>>(lineSegments, linesPreState, &lineSegments)
	//);
	//ModelStateManager::addStateOp(
	//	new GenericModelStateOp<std::list<MyQLine>::iterator>(prevDraggingLineIt, draggingLineIt, &draggingLineIt)
	//);
	//ModelStateManager::finishCompositeStateOp();
}
QRect createRect(Direction dir, QPoint pDirSideMid, int lLongitudinal, int lLAteral){
	int w = -1, h= -1;
	switch(dir){
	case RIGHT:{
		w=lLongitudinal; h= lLAteral; 
		return QRect(pDirSideMid-QPoint(w,h/2), pDirSideMid+QPoint(0,h/2)); break;
	}
	case LEFT:{
		w=lLongitudinal; h= lLAteral; 
		return QRect(pDirSideMid-QPoint(0,h/2), pDirSideMid+QPoint(w,h/2)); break;
	}
	case BOT:{
		w=lLAteral; h= lLongitudinal; 
		return QRect(pDirSideMid-QPoint(w/2,h), pDirSideMid+QPoint(w/2,0)); break;
	}
	case TOP:{
		w=lLAteral; h= lLongitudinal; 
		return QRect(pDirSideMid-QPoint(w/2,0), pDirSideMid+QPoint(w/2,h)); break;
	}
	}
	return QRect();
}
//std::weak_ptr<Connector> LineComponent::tryConnToThis(std::weak_ptr<CEB> srcOwner, QPoint pOnCanv, std::weak_ptr<Plug> plug){
//	auto src = srcOwner.lock();
//	auto obsPl = plug.lock();
//	if(src == nullptr || obsPl == nullptr)
//		return std::shared_ptr<Connector>();
//	std::list<MyQLine>::iterator it = getLineItAtCanvasCoords(pOnCanv.x(), pOnCanv.y(), nullptr);
//	if(it != lineSegments.end()){
//		if( std::dynamic_pointer_cast<UMLRelationship>(src) 
//				&& (it == lineSegments.begin() || it == std::prev(lineSegments.end())) )
//		{
//		}
//		if(std::dynamic_pointer_cast<TextComponent>(src)){
//		}
//	}
//	return std::shared_ptr<Connector>();
//}

std::list<MyQLine>::iterator LineComponent::getLineItAtCanvasCoords(int cx, int cy, double* OUTlinedivisionRatio){
	std::list<MyQLine>::iterator  l0 = lineSegments.begin();
	std::list<MyQLine>::iterator l = l0;
	int margin = 12;
	double ratio = 0;
	bool p = false;
	while( !p && l != lineSegments.end()){
		qDebug() << l->x1() << ' ';
		qDebug() << l->y1() << ' ';
		qDebug() << l->x2() << ' ';
		qDebug() << l->y2() << '\n';
		switch(l->getDir()){
		case LEFT:{
			if(l->x1() - l->x2() != 0)
				ratio = (double)(l->x1() - cx) / (double)(l->x1() - l->x2());
			p = QRect(l->p2()-QPoint(margin,margin), l->p1()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		case TOP:{
			if(l->y1() - l->y2() != 0)
				ratio = (double)(l->y1() - cy) / (double)(l->y1() - l->y2());
			p = QRect(l->p2()-QPoint(margin,margin), l->p1()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		case RIGHT:{
			if(l->x2() - l->x1() != 0)
				ratio = (double)(cx - l->x1()) / (double)(l->x2() - l->x1());
			p = QRect(l->p1()-QPoint(margin,margin), l->p2()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		case BOT:{
			if(l->y2() - l->y1() != 0)
				ratio = (double)(cy - l->y1()) / (double)(l->y2() - l->y1());
			p = QRect(l->p1()-QPoint(margin,margin), l->p2()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		}
		// itt biztosítom, hogy l értéke megfelelő legyen amikor kilépünk a ciklusból:
		if(!p)
			++l;
	}
	if(p && OUTlinedivisionRatio != nullptr)
		*OUTlinedivisionRatio = ratio;
	
	return l;
}
/////TODO: kijavitani kiegesyiteni
//std::weak_ptr<Connector> LineComponent::getConnFirstAtCanvasCoords(QPoint coords){
//	auto it = getLineItAtCanvasCoords(coords.x(), coords.y(), nullptr);
//	if(it == lineSegments.end())
//		return std::weak_ptr<Connector>();
//	/// TODO: megoldani, hogy a lineok mindkét fajta connectorát visszaadhassa ez a függvény.
//	///  ha a bázis connectors vektorában számontartanám, a lineSegmentek connectorait,
//	///  vagy nem is a lineSementeké lenneének a connectorok, hanem eleve a connectors vektorban lennének, akkor  jobb ez könyebb lenne.
//	//return it->connText->weak_from_this();
//	return std::weak_ptr<Connector>();
//}
void LineComponent::connectionAllowed(std::weak_ptr<CEB> connectorOwnerOwner, std::weak_ptr<Connector> connSrc, QPoint p, std::weak_ptr<Plug> plug){
	auto observedSrc = connectorOwnerOwner.lock();
	Target t(0,0,std::shared_ptr<CEB>());
	if(observedSrc == nullptr){
		t = Target(p.x(), p.y(), observedSrc);
	}
	else{
		t = Target(p.x() - observedSrc.get()->x(), p.y() - observedSrc.get()->y(), connectorOwnerOwner);
	}
	auto plObs = plug.lock();
	assert(plObs);
	auto connObs = connSrc.lock();
	assert(connObs);
	if(plObs == pl1){
		if(auto observedStart = start.canvasElement.lock()){
			pl1->detach();
			//disconnect(observedStart.get(),SIGNAL(moved()), this, SLOT(connectedConnectorMoved()));
		}
		start = t;
		pl1->attach(*connObs.get());
		if(!lineSegments.empty()){
			attachToElement(lineSegments.begin(), START, connSrc, t);
		}
	}
	else{
		if(auto observedEnd = end.canvasElement.lock()){
			pl2->detach();
			//disconnect(observedEnd.get(), SIGNAL(moved()), this, SLOT(connectedConnectorMoved()));
		}
		end = t;
		pl2->attach(*connObs.get());
		if(!lineSegments.empty()){
			attachToElement(std::prev(lineSegments.end()), END, connSrc, t);
		}
	}
	if(observedSrc){
		//connect( observedSrc.get(), SIGNAL(moved()), this, SLOT(connectedConnectorMoved()) );
	}
}
void LineComponent::copyFrom(const CEB* o){
	CEB::copyFrom(o);
	const LineComponent* olc = dynamic_cast<const LineComponent*>(o);
	assert(olc);
	
	draggedLinePart = olc->draggedLinePart;
	margin = olc->margin;
	lineSegments = olc->lineSegments;
	draggingLineIt = lineSegments.end();
	/// start nem állítom be;
	/// end nem állítom be;
	///  pl1 nem állítom be
	///  pl2 nem állítom be
	/// 
	/// connRel1 nem állítom be
	/// connRel2 nem állítom be
	/// connTB1  nem állítom be
	/// connTB2  nem állítom be
}

void LineComponent::init(){
	if(auto startObserved = start.canvasElement.lock()){
		forwardPlugSig(pl1.get());
		startObserved->tryConnToThis(weak_from_this(), start.getPoint_FreshInCanvas(), pl1);

	}
	if(auto endObserved = end.canvasElement.lock()){
		forwardPlugSig(pl2.get());
		endObserved->tryConnToThis(weak_from_this(), end.getPoint_FreshInCanvas(), pl2);

	}
	recreateLineSegments();
	//arrangeConns();
	//emit linesChanged_CEW();

}
void LineComponent::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
	(void) prevTr;
	(void) newTr;
	reconnect();
}
void LineComponent::connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction prevDir, Direction newDir){
	(void) srcConn;
	(void) srcPlug;
	(void) prevDir;
	(void) newDir;
	reconnect();
}
void LineComponent::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcConn;
	(void) srcPlug;
	(void) newArea;
	reconnect();
}

TextComponent::TextComponent(QRect geometry_) 
	: 
	  CEB(CanvasElement_Type::TEXTBOX, geometry_),
	  pl(std::make_shared<Plug>(this))
{
	plugs.push_back(pl);
	forwardPlugSig(pl.get());
}
TextComponent::~TextComponent(){
	
	
}

std::weak_ptr<Plug> TextComponent::getMainPlug() const{
	return pl;
}
void TextComponent::CEW_textChanged(QString newt){
	text = newt;
}
void TextComponent::connectionAllowed(std::weak_ptr<CEB> ceT, std::weak_ptr<Connector> conn, QPoint p, std::weak_ptr<Plug> plug){
	(void) p;

	emit ConnectedTo_C(weak_from_this(), plug, ceT, conn);
}
void TextComponent::copyFrom(const CEB* o){
	CEB::copyFrom(o);
	const TextComponent* otc = dynamic_cast<const TextComponent*>(o);
	assert(otc);
	
	text = otc->text;
	/// pl-t nem állítom be
	letterSpacing = otc->letterSpacing;
	charSize = otc->charSize;
}


/// TODO: aggregációit is menteni és betölteni!
std::ostream& TextComponent::save(std::ostream& os){
	CEB::save(os);
	writeWSpacedStr(os, text);
	saveSharedPtr(os, pl);
	os << letterSpacing << std::endl;
	os << charSize << std::endl;
	return os;
};

std::istream& TextComponent::load(std::istream& is){
	CEB::load(is);
	readWSpacedStr(is, text);
	loadSharedPtr(is, pl, this);
	is >> letterSpacing;
	is >> charSize;
	return is;
}

std::ostream& TextComponent::saveAggregations(std::ostream& os){
	CEB::saveAggregations(os);
	pl->saveAggregations(os);
	return os;
}

std::istream& TextComponent::loadAggregations(std::istream& is){
	CEB::loadAggregations(is);
	pl->loadAggregations(is);
	return is;
		
	}

//void TextComponent::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr){
//	(void) srcConn;
//	(void) srcPlug;
//}


///UMLComponentBase::UMLComponentBase(
///			CanvasElement_Type type_,
///			QRect geometry_ ) 
///	: 
///	  CEB(type_, geometry_),
///	  pl(std::make_shared<Plug>(this))
///{
///}
///std::weak_ptr<Plug> UMLComponentBase::getMainPlug() const{
///	return pl;
///}
QRect mrect(QPoint p0, int w, int h){
	return QRect(p0.x(),p0.y(), w,h).normalized();
}


UMLClassComponent::UMLClassComponent(QRect geometry_) 
	:
		CEB(CanvasElement_Type::UMLCLASS, geometry_),
		connTextTitle(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX)),
		connTextFields(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX)),
		connTextMethods(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOX)),
		connLeft(std::make_shared<Connector>(this, LEFT, CanvasElement_Type::LINE)),
		connRight(std::make_shared<Connector>(this, RIGHT, CanvasElement_Type::LINE)),
		connTop(std::make_shared<Connector>(this, TOP, CanvasElement_Type::LINE)),
		connBottom(std::make_shared<Connector>(this, BOT, CanvasElement_Type::LINE)),
		pl(std::make_shared<Plug>(this))
{
	forwardPlugSig(pl.get());
	plugs = {pl};
	connectors = {connTextTitle, connTextFields, connTextMethods,
				  connLeft, connRight, connBottom, connTop};
	qDebug() << "UMLClassComponent::UMLClassComponent(QRect geometry_): plugs.at(0).lock().getId():" << plugs.at(0).lock()->getId();
}

std::weak_ptr<Plug> UMLClassComponent::getMainPlug() const{
	return pl;
}
std::ostream& UMLClassComponent::saveAggregations(std::ostream& os){
	CEB::saveAggregations(os);
	
	pl->saveAggregations(os);
	connTextTitle->saveAggregations(os);
	connTextFields->saveAggregations(os);
	connTextMethods->saveAggregations(os);
	connLeft->saveAggregations(os);
	connRight->saveAggregations(os);
	connTop->saveAggregations(os);
	connBottom->saveAggregations(os);
	return os;
}
std::istream& UMLClassComponent::loadAggregations(std::istream& is){
	CEB::loadAggregations(is);
	
	pl->loadAggregations(is);
	connTextTitle->loadAggregations(is);
	connTextFields->loadAggregations(is);
	connTextMethods->loadAggregations(is);
	connLeft->loadAggregations(is);
	connRight->loadAggregations(is);
	connTop->loadAggregations(is);
	connBottom->loadAggregations(is);
	return is;
}
std::ostream& UMLClassComponent::save(std::ostream& os){
	CEB::save(os);
	
	const std::string umlToken = "UMLClassComponent";
		os << umlToken << std::endl;
		
	saveSharedPtr(os, pl);
	saveSharedPtr(os, connTextTitle);
	saveSharedPtr(os, connTextFields);
	saveSharedPtr(os, connTextMethods);
	os << textHeight << std::endl;
	saveSharedPtr(os, connLeft);
	saveSharedPtr(os, connRight);
	saveSharedPtr(os, connTop);
	saveSharedPtr(os, connBottom);
	return os;
}
std::istream& UMLClassComponent::load(std::istream& is){
	CEB::load(is);
	
	const std::string umlToken = "UMLClassComponent";
	std::string checkStr = "";
		is >> checkStr; assert(checkStr == umlToken);
	
	loadSharedPtr(is, pl, this);
	loadSharedPtr(is, connTextTitle, this, CanvasElement_Type::TEXTBOX);
	loadSharedPtr(is, connTextFields, this, CanvasElement_Type::TEXTBOX);
	loadSharedPtr(is, connTextMethods, this, CanvasElement_Type::TEXTBOX);
	is >> textHeight;
	loadSharedPtr(is, connLeft, this, CanvasElement_Type::LINE);
	loadSharedPtr(is, connRight, this, CanvasElement_Type::LINE);
	loadSharedPtr(is, connTop, this, CanvasElement_Type::LINE);
	loadSharedPtr(is, connBottom, this, CanvasElement_Type::LINE);
	return is;
}
//std::ostream& UMLClassComponent::saveAggregations(std::ostream& os){
//	UMLClassComponent::saveAggregations(os);
//	return os;
//}
//std::istream& UMLClassComponent::loadAggregations(std::istream& is){
//	UMLClassComponent::loadAggregations(is);
//	return is;
//}
//void UMLClassComponent::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void UMLClassComponent::load(std::istream& is){
//	load_chainCallThis(is);
//}

std::weak_ptr<Connector> UMLClassComponent::getConnTitle() const{
	return connTextTitle;
}
std::weak_ptr<Connector> UMLClassComponent::getConnFields() const{
	return connTextFields;
}
std::weak_ptr<Connector> UMLClassComponent::getConnMethods() const{
	return connTextMethods;
}
std::weak_ptr<Connector> UMLClassComponent::getConnLeft() const{
	return connLeft;
}
std::weak_ptr<Connector> UMLClassComponent::getConnRight() const{
	return connRight;
}
std::weak_ptr<Connector> UMLClassComponent::getConnTop() const{
	return connTop;
}
std::weak_ptr<Connector> UMLClassComponent::getConnBottom() const{
	return connBottom;
}
void UMLClassComponent::copyFrom(const CEB* o){
	CEB::copyFrom(o);
	const UMLClassComponent* occ = dynamic_cast<const UMLClassComponent*>(o);
	assert(occ);
	
	textHeight = occ->textHeight;
	/// pl-t nem állítom be
	/// connTextTitle-t nem állítom be    
	/// connTextFields-t nem állítom be    
	/// connTextMethods-t nem állítom be  	   
	/// connLeft-t nem állítom be          
	/// connRight-t nem állítom be        
	/// connTop-t nem állítom be           
	/// connBottom-t nem állítom be        
}


UMLRelationship::UMLRelationship(Relationship_Type rtype_, QRect geometry_) : 
		CEB(CanvasElement_Type::RELATIONSHIP, geometry_),
		dir(Direction::TOP),
		pl(std::make_shared<Plug>(this)),
		rtype(rtype_)
{
	forwardPlugSig(pl.get());
	plugs = {pl};
	connectors = {};
}

std::weak_ptr<Plug> UMLRelationship::getMainPlug() const{
	return pl;
}
Relationship_Type UMLRelationship::getRtype() const{
	return rtype;
}
void UMLRelationship::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
	(void) prevTr;
	(void) newTr;
}
void UMLRelationship::connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction prevDir, Direction newDir){
	(void) srcConn;
	(void) srcPlug;
	(void) prevDir;
	(void) newDir;
	auto srcObs = srcConn.lock();
	assert(srcObs);
	dir = newDir;
}
void UMLRelationship::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcConn;
	(void) srcPlug;
	(void) newArea;
	auto srcObs = srcConn.lock();
	assert(srcObs);
	//changeGeometry(
	//			newArea//.boundingRect()
	//			);
	dir = srcObs.get()->getDir();
}
//void UMLRelationship::adjustGeometryToDir(){
//	switch (dir) {
//	case TOP:
//	case BOT:{
//		setGeometrySizeCentered(QSize(thickness, length));
//		break;
//	}
//	case RIGHT:
//	case LEFT:{
//		setGeometrySizeCentered(QSize(length, thickness));
//		break;
//	}
//	default:assert(false);
//	}
//}
void UMLRelationship::connectionAllowed(std::weak_ptr<CEB> connectorOwner, std::weak_ptr<Connector> srcConn, QPoint p, std::weak_ptr<Plug> plug){
	(void) p;
	if(auto srcObs = srcConn.lock()){
		dir = srcObs.get()->getDir();
		//adjustGeometryToDir();
		emit ConnectedTo_C(weak_from_this(), plug, connectorOwner, srcConn);
	}
}
Direction UMLRelationship::getDir() const{
	return dir;
}  
std::shared_ptr<CEB> UMLRelationship::modellCopy(std::shared_ptr<CEB> fromDerived) const{
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
		switch(rtype){
		case ERROR_RT:{
			assert(false);
			break;
		}
		case CIRCLE:{
			ret = std::make_shared<UMLCircle>(geometry);
			break;
		}
		case EXTENSION:{
			ret = std::make_shared<UMLExtensionSpecifier>(geometry);
			break;
		}
		case AGGREGATION:{
			ret = std::make_shared<UMLAggregationSpecifier>(geometry);
			break;
		}
		case COMPOSITION:{
			ret = std::make_shared<UMLCompositionSpecifier>(geometry);
			break;
		}
		}		
	}
	
	///2.: báziseljárás meghívása(kimarad):
	/// a modellCopy eljárás a csak CEB osztályban és a származtatási hierarchibánan lefele menet van használatban.
	/// szóval itt nem kell meghívni semmilyen bázisra
	CEB::modellCopy(ret);	
	///3.: másolás: 
	///csak az objektum leg leszármazottabb részobjektumában kell kell meghívn a copyFrom-ot. 
	/// derivedFrom pedig pontosan akkor üres, ha a legleszármazottab részobjektumban vagyunk(így)
	if(!fromDerived)
		ret->copyFrom(this);
	return ret;
}
void UMLRelationship::copyFrom(const CEB* o){
	CEB::copyFrom(o);
	const UMLRelationship* orm = dynamic_cast<const UMLRelationship*>(o);
	assert(orm);
	
	dir = orm->dir;
	length = orm->length;
	thickness = orm->thickness;
	/// pl-t nem állítom be
	/// rtype-t nem állítom be     
}
std::ostream& UMLRelationship::save(std::ostream& os){
	CEB::save(os);
	saveSharedPtr(os, pl);
	os << dir << " " << length << " " << thickness << std::endl;
	
	return os;
}
std::istream& UMLRelationship::load(std::istream& is){
	CEB::load(is);
	loadSharedPtr(is, pl, this);
	
	int dirInt; is >> dirInt;
	dir = (Direction)dirInt;
	is >> length >> thickness;
	
	return is;
}
std::ostream& UMLRelationship::saveAggregations(std::ostream& os){
	CEB::saveAggregations(os);
	pl->saveAggregations(os);
	return os;
}
std::istream& UMLRelationship::loadAggregations(std::istream& is){
	CEB::loadAggregations(is);
	pl->loadAggregations(is);
	return is;
}

