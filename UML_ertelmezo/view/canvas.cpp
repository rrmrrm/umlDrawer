
#include "canvas.h"
#include "common.hpp"
#include "shape_drawing/drawing.hpp"
#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"
//#include "view/umlcomponentwidget.h"
#include "view/umlclasswidget.h"
#include "canvaselementwidgetcreator.hpp"
#include "model/plug.h"
//#include "model/canvasevent.h"

#include <QWidget>
#include <QColor>
#include <QBrush>
#include <QRect>
#include <QPaintEvent>
#include <QObjectList>
#include <qdebug.h>

#include <iostream>
Canvas::Canvas(int wid, Model* m_, QWidget *parent):
	QWidget(parent),
	m(m_)
		{
	//_layout = new QGridLayout();
	(void)parent;
	//isDrawingSet = false;
	setFixedWidth(wid);
	setFixedHeight(wid);
	//setLayout(_layout);
	setMouseTracking(true);
	
}
Canvas::~Canvas(){
}

CEW_Base* Canvas::viewFor(std::weak_ptr<CEB> ceb){
	if(!ceb.lock())
		return nullptr;
	
	for(auto it: findChildren<CEW_Base*>()){
		if((it->ce.lock()) && it->ce.lock() == ceb.lock()){
			return it;
		}
	}
	return nullptr;
}
CEW_Base* Canvas::viewFor(CEB* ceb){
	if(!ceb)
		return nullptr;
	
	for(auto it: findChildren<CEW_Base*>()){
		if((it->ce.lock()) && it->ce.lock().get() == ceb){
			return it;
		}
	}
	return nullptr;
}
ConnWidget* Canvas::viewFor(std::weak_ptr<Connector> conn){
	if(!conn.lock())
		return nullptr;
	
	for(auto it: findChildren<ConnWidget*>()){
		if((it->getModel().lock()) && it->getModel().lock() == conn.lock()){
			return it;
		}
	}
	return nullptr;
}
void Canvas::save(std::ostream& os){
	
}
void Canvas::load(std::istream& is){
//	canvas->load(is);
	
}
//std::ostream& Canvas::saveAggregations(std::ostream& os){
//	canvas->saveAggregations(os);
//	
//}
//std::istream& Canvas::loadAggregations(std::istream& is){
//	canvas->loadAggregations(is);
//	
//}

void Canvas::paintEvent(QPaintEvent* ev){
	(void)ev;

	if(permitRepaintNum <= 0)
		return;
	if(permitRepaintNum>0)
		--permitRepaintNum;
	//cout << "Canvas::paintEvent(...)" << endl;
	QPainter p(this);
	//qDebug() << "Canvas::paintEvent(..)";
	QColor defCol(255,255,255);
	QBrush defBrush(defCol,Qt::SolidPattern);
	p.fillRect(rectToRepaint, defBrush);
	double radius=3;
	
	if(		m->getLastDrawing()->isBoundsSetup() &&
			rectToRepaint.intersects(m->getLastDrawing()->getBoundingRect()) &&
			m->getLastDrawing()->isVisible()
			)	{
		
		for(auto gesture : m->getLastDrawing()->getGestures()){
			for(auto point : gesture.getPoints()){
				int x = point.first;
				int y = point.second;
				QPointF point0(x - radius, y - radius);
				QPointF point1(x + radius, y + radius);
				QRectF rect(point0,point1);
				p.drawEllipse(rect);
			}
		}
	}
	else{
	}
	srand(3);
	auto drawings = m->getDrawings();
	if(drawings.size() > 0){
		auto draw = drawings[drawings.size()-1]->getCells();
		double scale = 10.0;
		///a kép megjelenítésének helyét letisztítom
		/// (egy teli téglalpot rajzolok oda):
		if(draw.size() > 0){
			QRectF clearRect( QPoint(0,0),QPoint(scale*(int)(draw.size()), scale*(int)(draw.at(0).size())) );
			p.fillRect(clearRect, QBrush(QColor(0,0,0), Qt::SolidPattern));
		}
		
	}
}

void Canvas::Model_CanvasElementAdded(std::weak_ptr<CEB> ce){
	auto obsCE = ce.lock();
	if(!obsCE)
		return;
	uint newEntityInd = CEWs.size();
	CEWs.push_back(createDerived(ce, this));
	auto entityWidget = CEWs[newEntityInd];
	
	connect(entityWidget, SIGNAL(mousePressEventAccepted(QMouseEvent*)), this, SLOT(EntityWidgetmousePressEvent(QMouseEvent*)));
	connect(entityWidget, SIGNAL(mouseMoveEventAccepted(QMouseEvent*)), this, SLOT(EntityWidgetmouseMoveEvent(QMouseEvent*)));
	connect(entityWidget, SIGNAL(mouseReleaseEventAccepted(QMouseEvent*)), this, SLOT(EntityWidgetmouseReleaseEvent(QMouseEvent*)));
	connect(
		entityWidget, 
		SIGNAL(setGeometry_CEB(QRect)), 
		obsCE.get(),
		SLOT(CEW_setGeometry(QRect))
	);
	connect(obsCE.get(), SIGNAL(geometryChanged_CEW()), entityWidget, SLOT(CE_geometryChanged()));
	connect(obsCE.get(), SIGNAL(destroyView_CEW()), entityWidget, SLOT(CE_destroyThis()));
	connect(obsCE.get(), SIGNAL(should_repaint()), entityWidget, SLOT(smartUpdate()));
	connect(obsCE.get(), SIGNAL(resizeEdgeSig(int, int/*, Direction*/)), entityWidget, SLOT(resizeEdge(int, int/*, Direction*/)));
	connect(obsCE.get(), SIGNAL(drag_CEW(int,int)), entityWidget, SLOT(CE_Drag(int,int)));
	connect(
		obsCE.get(), 
		SIGNAL(ConnectedTo_C(
					std::weak_ptr<CEB>,
					std::weak_ptr<Plug>,
					std::weak_ptr<CEB>, 
					std::weak_ptr<Connector>)
			),
		this, 
		SLOT(CE_ConnectedTo(
				 std::weak_ptr<CEB>,
				 std::weak_ptr<Plug>,
				 std::weak_ptr<CEB>, 
				 std::weak_ptr<Connector>)
			 )
	);

	
	entityWidget->smartUpdate();
	entityWidget->show();
}
void Canvas::CE_ConnectedTo(
		std::weak_ptr<CEB> ceS,
		std::weak_ptr<Plug> pl,
		std::weak_ptr<CEB> ceT, 
		std::weak_ptr<Connector> conn)
{
	CEW_Base* cewS = viewFor(ceS);
	CEW_Base* cewT = viewFor(ceT);
	ConnWidget* cw = viewFor(conn);
	cewS->C_connectedTo(pl, cewT, cw);
}
void Canvas::removeCanvasElementWidgets(){
	for(auto cew : CEWs){
		delete cew;
	}
	CEWs.clear();
}
void Canvas::Update(QRect updatedRect){
	++permitRepaintNum;
	rectToRepaint = updatedRect;
	this->repaint(updatedRect);
}
void Canvas::Update(){
	++permitRepaintNum;
	rectToRepaint = geometry();
	this->repaint();
	for(auto& c : findChildren<CEW_Base*>()){
		c->smartUpdate();
	}
}
//Direction Canvas::cewEdgeAt(QPoint cPoint, CEW_Base** cewOut){
//	int m = 4;
//	assert(cewOut);
//	assert(*cewOut == nullptr);
//	for(auto it : findChildren<CEW_Base*>()){
//		QRect ig = it->getGeomInC();
//		QRect lr = QRect(ig.left()-m, ig.top(), 2*m, ig.height());
//		QRect rr = QRect(ig.right()-m, ig.top(), 2*m, ig.height());
//		QRect tr = QRect(ig.left(), ig.top()-m, ig.width(), 2*m);
//		QRect br = QRect(ig.left(), ig.bottom()-m, ig.width(), 2*m);
//		
//		if(lr.contains(cPoint)){
//			*cewOut = it;
//			return LEFT;
//		}
//		if(tr.contains(cPoint)){
//			*cewOut = it;
//			return TOP;
//		}
//		if(rr.contains(cPoint)){
//			*cewOut = it;
//			return RIGHT;
//		}
//		if(br.contains(cPoint)){
//			*cewOut = it;
//			return BOT;
//		}
//	}
//	*cewOut = nullptr;
//	return LEFT;
//}
std::weak_ptr<CEB> Canvas::srcAsCE(){
	CEW_Base* asCEW = dynamic_cast<CEW_Base*>(sender());
	if(asCEW){
		if(!asCEW->ce.lock()){
			qDebug() << "WARNING: Canvas::srcAsCE(): CEW is notnull, but its ce is null! this function will wrongly assume, that the sender was the Canvas!";
		}
		return asCEW->ce;
	}
	else{
		return std::weak_ptr<CEB>();	
	}
}
/// sender egy CE vagy a Canvas lehet.
/// Visszaadjuk event Canvas-beli koordinátáit:
QPoint Canvas::pToCanv(QMouseEvent event){
	auto ce = srcAsCE();
	QPoint p = event.localPos().toPoint();
	if(auto ceO = ce.lock()){
		return p + ceO->getGeometry().topLeft();
	}
	return p;
}
void Canvas::mouseEventHelper(QMouseEvent *e){
	std::weak_ptr<CEB> ce;
	/// cPos lesz az esemény Canvas beli koordinátái:
	QPoint cPos = pToCanv(*e);
	m->canvas_mouseEvent(*e, srcAsCE(), cPos);
}
void Canvas::mousePresEventHelper(QMouseEvent *e){
	std::weak_ptr<CEB> ce;
	/// cPos lesz az esemény Canvas beli koordinátái:
	QPoint cPos = pToCanv(*e);
	m->canvas_mouseEvent(*e, srcAsCE(), cPos);
}
void Canvas::mousePressEvent(QMouseEvent *e){
	mousePresEventHelper(e);
}
///megjegyzés: előfordulhat, hogy az enter lenyomása után(keyPress event) először egy mouseMove event jön, így a mousePress nem hívódik meg előtte, hogy új gesture-t adjon
/// a keyPressEvent-ben létrehozott drawing-hoz, ezért nem létező gesture-höz akarunk pixel pontot hozzáadni. a Drawing addGesturePoint metódusa mostmár kezeli ezt:
/// csak akkor próbál meg ténylegesen pontot hozzáadni az általa tárolt legnagyobb indexű gesture-höz, ha van legalább egy gesture a 'gestures' tömbjében
void Canvas::mouseMoveEvent(QMouseEvent *e){
	mouseEventHelper(e);
}
void Canvas::mouseReleaseEvent(QMouseEvent *e){
	mouseEventHelper(e);
}


void Canvas::EntityWidgetmousePressEvent(QMouseEvent *e){
	mousePresEventHelper(e);
}
///megjegyzés: előfordulhat, hogy az enter lenyomása után(keyPress event) először egy mouseMove event jön, így a mousePress nem hívódik meg előtte, hogy új gesture-t adjon
/// a keyPressEvent-ben létrehozott drawing-hoz, ezért nem létező gesture-höz akarunk pixel pontot hozzáadni. a Drawing addGesturePoint metódusa mostmár kezeli ezt:
/// csak akkor próbál meg ténylegesen pontot hozzáadni az általa tárolt legnagyobb indexű gesture-höz, ha van legalább egy gesture a 'gestures' tömbjében
void Canvas::EntityWidgetmouseMoveEvent(QMouseEvent *e){
	mouseEventHelper(e);
}
void Canvas::EntityWidgetmouseReleaseEvent(QMouseEvent *e){
	mouseEventHelper(e);
}
void Canvas::EntityWidgetmouseDoubleClickEvent(QMouseEvent *e){
	mousePresEventHelper(e);
}








