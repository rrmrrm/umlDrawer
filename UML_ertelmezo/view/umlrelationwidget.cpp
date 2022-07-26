#include "umlrelationwidget.h"
#include "view/canvaselementwidgetbase.h"
#include "model/CanvasElements/canvaselement.h"

#include <QPainter>
#include <QPainterPath>

UmlRelationWidget::UmlRelationWidget(std::weak_ptr<UMLRelationship> ur_, QWidget* parent)
	:
	CEW_Base(ur_, parent)
		{
	auto urO = ur_.lock();
	assert(urO);
	setGeometryInC(urO->geometry);
}

const std::shared_ptr<UMLRelationship> UmlRelationWidget::getUr() const{
	auto ceObs = ce.lock();
	return std::dynamic_pointer_cast<UMLRelationship>(ceObs);
}

void rotPainter(QPainter& p, Direction dir){
	switch(dir){
	case TOP:break;
	case RIGHT:{
		p.rotate(90);
		break;
	}
	case BOT:{
		p.rotate(180);
		break;
	}
	case LEFT:{
		p.rotate(-90);
		break;
	}
	}
}
void UmlRelationWidget::derivedPaintEvent(QPaintEvent* ev){
	if(!shouldUsePaintEvent()){
		ev->ignore();
		return;
	}
	auto ceObs = ce.lock();
	if(!ceObs)
		return;
	QPainter p(this);
	
	int w = ceObs->width();
	int h = ceObs->height();
	int wh = w / 2;
	int hh = h / 2;
	QRect whole(0,0,w,h);
	QColor defCol(255,255,255);
	QBrush defBrush(defCol,Qt::SolidPattern);
	p.fillRect(whole, defBrush);
	//szabványos uml komponens kirajzolása, ha a drawing nem nullptr:
	auto ur = getUr();
	assert(ur != nullptr);
	//auto drawing = ce->dInd < (int)ce->drawings.size() ? ce->drawings[ce->dInd] : nullptr;
	QColor color(0,0,0);
	qreal lineWidth = 1.0;
	p.setPen(QPen(QBrush(color, Qt::SolidPattern), lineWidth));
	///TODO: az umlrelationship-ek irányát kezelni
	/// 
	///TODO: rosszul rajzolja a ki a nyilat és vmielyik rombuszt 
	switch(ur->getRtype()){
	case EXTENSION:{
		//háromszöget nyilat    ^
		//                    /_|_\  rajzolunk:
		p.translate(QPoint(wh,hh));
		//p.scale(w/7.0, h/7.0);
		rotPainter(p, ur->getDir());
		p.drawLine(-wh, 0, 0, -hh);
		p.drawLine(0, -hh, wh, 0);
		// függőleges:
		p.drawLine(0, -hh, 0, hh);
		// vízszintes:
		p.drawLine(-wh, 0, wh, 0);
		break;
	}
	case AGGREGATION:{
		//nyújtott rombuszt   /\   rajzolunk
		//                    \/
		p.translate(QPoint(wh,hh));
		//p.scale(w/7.0, h/7.0);
		//rotPainter(p, ur->getDir());
		
		p.drawLine(-wh, 0, 0, -hh);
		p.drawLine(0, -hh, wh, 0);
		p.drawLine(-wh, 0, 0, hh);
		p.drawLine(0, hh, wh, 0);
		break;
	}
	case CIRCLE:{
		p.translate(QPoint(wh,hh));
		p.drawEllipse(-wh, -hh, w, h);
		break;
	}
	case COMPOSITION:{
		//nyújtott teli  rombuszt   /\   rajzolunk
		//                          \/
		p.translate(QPoint(wh,hh));
		//p.scale(w/7.0, h/7.0);
		//rotPainter(p, ur->getDir());
		QPolygon compPoly;
		QPainterPath compPath;
		compPoly 
				<< QPoint(-wh,0)
				<< QPoint(0,-hh)
				<< QPoint(wh,0)
				<< QPoint(0,hh);
		//p.drawLine(-1, 0, 0, -1);
		//p.drawLine(0, -1, 1, 0);
		//p.drawLine(-1, 0, 0, 1);
		//p.drawLine(0, 1, 1, 0);
		compPath.addPolygon(compPoly);
		p.fillPath(compPath,QBrush(QColor(0,0,0), Qt::SolidPattern));
		break;
	}
	default:{
		assert(false);
		break;
	}
	};
	//auto trans = p.worldTransform();
}
