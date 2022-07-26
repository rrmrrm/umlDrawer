#include "umlcomponentwidget.h"
#include "shape_drawing/drawing.hpp"
#include "shape_drawing/gesture.h"
#include "model/CanvasElements/canvaselement.h"

#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QRect>
#include <QPaintEvent>
#include <QBoxLayout>
#include <QFrame>
#include <qdebug.h>

///TODO: megoldani hogz a umlclassWidgetbeli textbox átméretezésekor átméeteződjön a widget is,
//// umlwidgetet .ui filebol a connwidget2 tipusu connectorokat felhasynalni
UMLClassWidget::UMLClassWidget(std::shared_ptr<UMLComponentBase> uc_, /*EntityWidgetInd thisInd_, const std::vector<CanvasElementBase*>& canvasElements_,*/ QWidget* parent)
	:	
		CanvasElementWidgetBase(uc_, parent),
		ucc(std::dynamic_pointer_cast<UMLClassComponent>(uc_))
		{
	assert(ucc.lock());
	auto ceObs = ce.lock();
	if(ceObs)
		setGeometry(ceObs->geometry);
	
	vbl = new QVBoxLayout(this);
	//setLayout(vbl);
	auto mObs = ucc.lock();
	assert(mObs);
	
	/// ConnWidget2-ket adok az UML-hez, amik összezsugorodnak amikor kivesszük belőlük a hozzácsatolt widgeteket
	auto cwt = new ConnWidget(mObs->getConnTitle());
	auto cwf = new ConnWidget(mObs->getConnFields());
	auto cwm = new ConnWidget(mObs->getConnMethods());
	
	auto cwLeft = new ConnWidget(mObs->getConnLeft());
	auto cwRight = new ConnWidget(mObs->getConnRight());
	auto cwTop = new ConnWidget(mObs->getConnTop());
	auto cwBottom = new ConnWidget(mObs->getConnBottom());
	
	vbl->addWidget(cwt, 1);
	vbl->addWidget(makeHSeparator());
	vbl->addWidget(cwf, 1);
	vbl->addWidget(makeHSeparator());
	vbl->addWidget(cwm, 1);
	
	cwt->setFixedHeight(titleHeight);
	//cwf->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	//cwm->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	//setLayout(vbl);
	qDebug() << "cwt->geometry(); \n";
	qDebug() << cwt->geometry() << "\n";
	qDebug() << cwf->geometry() << "\n";
	qDebug() << cwm->geometry() << "\n";
}
QFrame* UMLClassWidget::makeHSeparator(QWidget* parent){
	QFrame *line;
	line = new QFrame(parent);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	return line;
}

///TODO: az e->rect-et nem állítja ugy be a keretrendszer, ahogy én akarom. e->rect használata helyett valamit összebénáztam. a megoldásomat ellenőrizni
void UMLClassWidget::derivedPaintEvent(QPaintEvent* e){
	if(!shouldUsePaintEvent()){
		e->ignore();
		return;
	}
	auto ceObs = ce.lock();
	if(!ceObs)
		return;
	auto cu = std::static_pointer_cast<UMLClassComponent>(ceObs);
	QPainter p(this);

	int w = width();
	int h = height();
	QRect whole(0,0,w,h);
	QColor defCol(255,255,255);
	QBrush defBrush(defCol,Qt::SolidPattern);
	p.fillRect(whole, defBrush);
	
	//szabványos uml komponens kirajzolása, ha a drawing nem nullptr:
	assert(ceObs != nullptr);
	QColor color(0,0,0);
	int lineWidth = 3;
	p.setPen(QPen(QBrush(color, Qt::SolidPattern), lineWidth));
	QPoint p0 = QPoint(geometry().topLeft());
	//switch(ceObs->CEtype){
	////négyzet két vízszintes vonallal( az első a címelválasztó, a második a változókat választja el az eljárásoktól)
	//case UMLCLASS:{
	//	p.drawRect(QRect(0,0,w,h));
	//	int titleBot = cu->getConnTitle()
	//			.lock()->getTranslatedArea()
	//			.translated(-p0)// canvas coords -> elementCoords conversion
	//			//.boundingRect()
	//			.bottom();
	//	int fieldsBot = cu->getConnFields()
	//			.lock()->getTranslatedArea()
	//			.translated(-p0)// canvas coords -> elementCoords conversion
	//			//.boundingRect()
	//			.bottom();
	//	p.drawLine(0, titleBot, w, titleBot);
	//	p.drawLine(0, fieldsBot, w, fieldsBot);
	//			
	//	//int titleH = std::min(30, h/4);
	//	//p.drawLine(0,titleH,w,titleH);
	//	//p.drawLine(0,titleH+(h-titleH)/2,w,titleH+(h-titleH)/2);
	//	break;
	//}
	//default:{
	//	break;
	//}
	//};
	
	QColor connColor(200,20,20, 170);
	int connLineWidth = 1;
	p.setPen(QPen(QBrush(connColor, Qt::SolidPattern), connLineWidth));
	p.translate(-geometry().topLeft());
	for(auto conn : ceObs->getConnectors()){
		if(auto connObs = conn.lock()){
			p.drawRect(connObs->getTranslatedArea());
			//QPoint pPrev = connObs->getTranslatedArea().first();
			////TODO: UML line-connectorjait nem rajzolja ki jól
			//for(QPoint point : connObs->getTranslatedArea()){
			//	p.drawLine(pPrev, point);
			//	pPrev = point;
			//}
		}
	}
	p.translate(-geometry().topLeft());
	paintBorder(&p);
}
void UMLClassWidget::canvasElement_geometryChanged(){
	
}
