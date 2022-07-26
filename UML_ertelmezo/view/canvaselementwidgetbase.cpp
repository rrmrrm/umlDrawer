#include "canvaselementwidgetbase.h"
#include "linewidget.h"
#include "view/canvas.h"
#include "view/connwidget.h"
#include <model/plug.h>

#include <QMouseEvent>
#include <QEventLoop>
#include <qdebug.h>

CEW_Base::CEW_Base(
			std::weak_ptr<CEB> ce_,
			QWidget* parent
		)
	:	
		QWidget(parent),
		ce(ce_)
{
}
CEW_Base::~CEW_Base(){
	
}

QRect CEW_Base::toWCoords(QRect rInCanvas) const{
	return rInCanvas.translated(-x(),-y());
}
QRect CEW_Base::getGeomInC(){
	QPoint p0 = mapTo(accessCanvas(), QPoint(0, 0));
	return QRect(p0, rect().size());
}
void CEW_Base::setGeometryInC(QRect rInC){
	/// a setGeometry utasítással fogom beállítani a CEW geometriáját.
	/// az új geometria a Canvas-hoz relatív rInC. Nekem CEW szülőjéhez relatíven kell.
	/// CEW szülője lehet egy másik CEW, vagy maga a Canvas
	
	/// rInC téglalap bal-felső pontja a CEW-hez képest:
	QPoint p0InCEW = mapFrom(accessCanvas(), rInC.topLeft());
	/// rInC balfelső pontja a CEW szülőjéhez képest(a szülője lehet a Canvas, vagy egy másik CEW is):
	QPoint p0InP = mapToParent(p0InCEW);
	QRect rInP = QRect(p0InP, rInC.size());
	setGeometry(rInP);
}
Canvas* CEW_Base::accessCanvas(){
	QObject* p = parent();
	while(p != nullptr){
		if(auto casted = dynamic_cast<Canvas*>(p))
			return casted;
		else
			p = p->parent();
	}
	assert(false);
	return nullptr;
}
bool CEW_Base::shouldUsePaintEvent() const{
	if(mask().isEmpty()){
		if(!rectToRepaint.intersects(geometry())){
			return false;
		}	
	}
	else{
		//geometry és rectToRepaint a szülő widget koord rendszerében van megadva, 
		//mask() viszont a LineWidget koord rendszerében van megadva
		QRect rectToRepaint_local = rectToRepaint.translated(-geometry().topLeft());
		if(!QRegion(rectToRepaint_local).intersects(mask())){
			return false;
		}
	}
	return true;
}
void CEW_Base::paintBorder(){
	QPainter p(this);
	auto co = ce.lock();
	if(!co)
		return;
	//auto savedPen = p->pen();
	
	int thickness = 3;
	QColor selectHightlightC = QColor(0, 248, 0, 200);
	p.setPen(QPen(selectHightlightC, 3));
	if(co->isSelected()){
		p.drawRect(QRect(0,0,width()-thickness,height()-thickness));
	}
	
	//p->setPen(savedPen);
}
void CEW_Base::updateCEGeom(){
	updateConnsGeom();
	emit setGeometry_CEB(getGeomInC());
}
void CEW_Base::updateConnsGeom(){
	for(auto connW : findChildren<ConnWidget*>()){
		connW->updateConnGeom();
	}
}
void CEW_Base::ConnW_wantsToResize(ConnWidget* cw, QSize newSize){
	assert(cw);
	/// az alapműködés az hogy egyszerűen végrehajtjuk a kért átméretezést,
	/// cw ennek hatására beállítja a modelljének(Connector) is az új méretet...
	cw->resize(newSize);
	/// ...és frissítjük a nézetet(CEW):
	CE_geometryChanged();
}
void CEW_Base::C_connectedTo(std::weak_ptr<Plug>, CEW_Base*, ConnWidget* cw){
	/// ha nem a Canvas volt a parentje, akkor hiba történt
	assert(parent() == accessCanvas());

	setParent(cw);
	assert(cw->layout());
	cw->layout()->addWidget(this);
	setVisible(true);

	/// előre hozzuk a cew-t:
	raise();
	repaint();
	smartUpdate();
}

void CEW_Base::decrPermRepaintNum(){
	//qDebug() << "yay!";
	if(permittedRepaintNum > 0){
		--permittedRepaintNum;
	}		
	else{
		//qDebug() << "CEW_Base::decrPermRepaintNum(): permittedRepaintNum already 0!";
	}
}

void CEW_Base::resizeEvent(QResizeEvent* ){
	for(auto connW : findChildren<ConnWidget*>()){
		connW->resizeEvent(nullptr);
	}
	emit setGeometry_CEB(getGeomInC());
	//qDebug() << "CEW_Base::resizeEvent called\n";
}
void CEW_Base::moveEvent(QMoveEvent *){
	for(auto connW : findChildren<ConnWidget*>()){
		connW->resizeEvent(nullptr);
	}
	emit setGeometry_CEB(getGeomInC());

}

void CEW_Base::update(){
	QWidget::update();
}
void CEW_Base::repaint(){
	QWidget::repaint();
}
void CEW_Base::update(QRect r){
	QWidget::update(r);
}
void CEW_Base::repaint(QRect r){
	QWidget::repaint(r);
}

void CEW_Base::smartUpdate(QRect r){
	++permittedRepaintNum;
	rectToRepaint = r;
	repaint();
}
void CEW_Base::smartUpdate(){
	++permittedRepaintNum;
	rectToRepaint = geometry();
	repaint();
}
void CEW_Base::CE_geometryChanged(){
	if(auto ceObs = ce.lock()){
		setGeometryInC(ceObs->geometry);
		smartUpdate();
	}
}

void CEW_Base::paintEvent(QPaintEvent* e){
	//if(permittedRepaintNum > 0){
		derivedPaintEvent(e);
		paintBorder();
		decrPermRepaintNum();
	//}
}
void CEW_Base::derivedPaintEvent(QPaintEvent* e){
	(void)e;
}

void CEW_Base::mousePressEvent(QMouseEvent *event){
	//if(isOnGeometry(event)){
		event->accept();
		emit mousePressEventAccepted(event);
	//}
	//else{
	//	//itt hagyjuk, hogy a Canvas reagáljon a MouseEventre:
	//	event->ignore();
	//}
}
void CEW_Base::mouseMoveEvent(QMouseEvent *event){
	event->accept();
	emit mouseMoveEventAccepted(event);
}
void CEW_Base::mouseReleaseEvent(QMouseEvent *event){
	emit mouseReleaseEventAccepted(event);
}
void CEW_Base::CE_Drag(int cX, int cY){
	//auto cew = accessCanvas();
	auto cso = size();
	/// ha a cew egy másik cew connW-jében volt, akkor most kivesszük,
	/// és közvetlenül a Canvasba tesszük vissza:
	if(parent() != accessCanvas()){
		auto cebp = dynamic_cast<ConnWidget*>(parent());
		///itt szolni kell a connwidgetnek vagy parentjének, hogy kivettem a widgete belőle
		cebp->resetSize();
		assert(cebp);
		auto pLay = cebp->layout();
		assert(pLay);
		pLay->removeWidget(this);
		
		//assert(_layout);
		setParent(accessCanvas());
		setVisible(true);
		setGeometry(cX, cY, cso.width(), cso.height());
	}
	else{
		move(cX, cY);
		emit setGeometry_CEB(QRect(cX, cY, cso.width(), cso.height()));
	}
	/// előre hozzuk a cew-t:
	raise();
	repaint();
	smartUpdate();
}
void CEW_Base::resizeEdge(int cX, int cY/*, Direction draggedEdge*/){
	QPoint local = mapFrom(accessCanvas(), QPoint(cX, cY));
	qDebug() << "resizeEdgeSig: geometry() preResize:" << geometry() << "\n";
	
	resize(local.x(), local.y());
	//switch(draggedEdge){
	//case LEFT:{
	//	break;
	//}
	//case RIGHT:{
	//	resize(local.x(), height());
	//	break;
	//}
	//case TOP:{
	//	break;
	//}
	//case BOT:{
	//	resize(width(), local.y());
	//	break;
	//}
	//}
	emit repaint();
	qDebug() << "geometry() after resize:" << geometry() << "\n";
	///TODO: hogz méretezem majd át a tartalmazó widgetet?
}
void CEW_Base::CE_destroyThis(){
	delete this;
}
