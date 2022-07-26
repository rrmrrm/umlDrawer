#include "linewidget.h"
#include "view/connwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <qdebug.h>

LineWidget::LineWidget(std::weak_ptr<LineComponent> lc,
					   QWidget* parent
					   )
	: CEW_Base( lc, parent),
	  line(lc)
{
	auto lObs = line.lock(); 
	assert(lObs);
	connect(lObs.get(), SIGNAL(linesChanged_CEW()), this, SLOT(CE_LinesChanged()));
	cRelMarker1W = new ConnWidget(lObs->connRel1, QRect(0, 0,  30, 30), this);
	cRelMarker2W = new ConnWidget(lObs->connRel2, QRect(30, 30,  30, 30), this);
	cTexBox1W = new ConnWidget(lObs->connTB1, QRect(), this);
	cTexBox2W = new ConnWidget(lObs->connTB2, QRect(), this);
	adjustMaskAndGeometry();
}
LineWidget::~LineWidget(){
	
}

void LineWidget::CE_LinesChanged(){
	//arrangeConns();
	adjustMaskAndGeometry();
	smartUpdate();
}
void LineWidget::CE_geometryChanged(){
	assert(ce.lock());
	/// a vonalszegmensek és ConnectorWidgetek alapján frissítjük a LineWidget mask-ját és geometry-jét
	adjustMaskAndGeometry();
	//smartUpdate();
}

void LineWidget::adjustMaskAndGeometry(){
	auto lObs = line.lock();
	if(!lObs)
		return;
	if(lObs->lineSegments.empty())
		return;
	
	QRegion bounds;
	/// számolások Canvas coord rendszerében:
	int m = lObs->margin;
	auto  l = lObs->lineSegments.begin();
	while(l != lObs->lineSegments.end()){
		bounds += QRect(l->p1(), l->p2())
					.normalized()
					.adjusted(-m,-m, m,m);
		++l;
	}
	/// relationshipmarker-ek 2 connector-ának ]j helyei:
	QRect newRC1;
	QRect newRC2;
	int cThick = 20;
	int cLen = 40;
	{
	const auto& lb = lObs->lineSegments.begin();
	QPoint p1 = lb->getPart(LinePart::START);
	switch(lb->getDir()){
	case LEFT:{
		newRC1 = QRect(p1.x(), p1.y()-cThick/2 , -cLen, cThick);
		break;
	}
	case RIGHT:{
		newRC1 = QRect(p1.x(), p1.y()-cThick/2 , cLen, cThick);
		break;
	}
	case TOP:{
		newRC1 = QRect(p1.x()-cThick/2, p1.y() , cThick, -cLen);
		break;
	}
	case BOT:{
		newRC1 = QRect(p1.x()-cThick/2, p1.y() , cThick, cLen);
		break;
	}
	}
	newRC1 = newRC1.normalized();
	}
	
	{
	const auto& le = lObs->lineSegments.rbegin();
	QPoint p2 = le->getPart(LinePart::END);
	switch(le->getOtherDir()){//az irányt negáltam itt
	case LEFT:{
		newRC2 = QRect(p2.x(), p2.y()-cThick/2 , -cLen, cThick);
		break;
	}
	case RIGHT:{
		newRC2 = QRect(p2.x(), p2.y()-cThick/2 , cLen, cThick);
		break;
	}
	case TOP:{
		newRC2 = QRect(p2.x()-cThick/2, p2.y() , cThick, -cLen);
		break;
	}
	case BOT:{
		newRC2 = QRect(p2.x()-cThick/2, p2.y() , cThick, cLen);
		break;
	}
	}
	newRC2 = newRC2.normalized();
	}
	/// megakadályozom hogy a connectorok mérete 30x30 alá menjen:
	int cText1WW = cTexBox1W->width();
	int cText1WH = cTexBox1W->height();
	int cText2WW = cTexBox2W->width();
	int cText2WH = cTexBox2W->height();
	
	if( cText1WW < 30)
		cText1WW = 30;
	if( cText1WH < 30)
		cText1WH = 30;
	if( cText2WH < 30)
		cText2WH = 30;
	if( cText2WH < 30)
		cText2WH = 30;
	/// első vonalszegmens szövegdoboz-connectora:
	auto lf = lObs->lineSegments.front();
	/// utoló vonalszegmens szövegdoboz-connectora:
	auto lb = lObs->lineSegments.back();
	/// 'rat' arányban osztjuk el az első és utolsó vonalszegmenst(az utolsóban p1 és p2 felcserélve)
	float rat = 0.75;
	QPoint po1 =  rat*lf.p1() + (1-rat)*lf.p2();
	QPoint po2 =  rat*lb.p2() + (1-rat)*lb.p1();
	/// az osztópontok határozzák meg, hol legyenek a textconneotorok:
	QRect newRTextC1 = QRect(po1, QSize(cText1WW, cText1WH));
	QRect newRTextC2 = QRect(po2, QSize(cText2WW, cText2WH));

	/// bővíteni bounds-t hogy a connWidgetek új helyeit is tartalmazza:
	bounds += newRC1;
	bounds += newRC2;
	bounds += newRTextC1;
	bounds += newRTextC2;
	///geometry frissítése:
	setGeometry(bounds.boundingRect());
	
	/// átszámolás LineWidget coord rendszerébe:
	///
	newRC1 = toWCoords(newRC1);
	newRC2 = toWCoords(newRC2);
	newRTextC1 = toWCoords(newRTextC1);
	newRTextC2 = toWCoords(newRTextC2);
	bounds.translate(-x(),-y());
	setMask(bounds);
	
	///connectorok helyének frissítése:
	cRelMarker1W->setGeometry(newRC1);
	cRelMarker2W->setGeometry(newRC2);
	cTexBox1W->setGeometry(newRTextC1);
	cTexBox2W->setGeometry(newRTextC2);
	
	cRelMarker1W->repaint();
	cRelMarker2W->repaint();
	cTexBox1W->repaint();
	cTexBox2W->repaint();
}
void LineWidget::derivedPaintEvent(QPaintEvent* ev){
	if(!shouldUsePaintEvent()){
		ev->ignore();
		return;
	}
	auto ceObs = ce.lock();
	if(ceObs == nullptr)
		return;
	auto lObs = line.lock();
	if(!lObs)
		return;
	
	
	//cout << "Canvas::paintEvent(...)" << endl;
	QPainter p(this);
	//QColor color(0,0,50, 100);
	int lineWidth = 3;
	//QPen penForRect(QBrush(color, Qt::SolidPattern), lineWidth);
	//QPen penForTestGeometry(QBrush(QColor(200, 200, 0), Qt::SolidPattern), lineWidth);
	QColor lineColor(0,0,0);
	QPen penForLines(QBrush(lineColor, Qt::SolidPattern), lineWidth);
	QColor highlight(200,0,0);
	QPen penForConns(QBrush(highlight, Qt::SolidPattern), lineWidth);

	/// vonalszegmensek kirajzolása:
	/// áttérünk a Canvas koordinátarendszerébe, mert a vonalSzegmensek is abban a rendszerben vannak megadva:
	p.translate(-x(), -y());
	p.setPen(penForLines);
	for(auto& l : lObs->lineSegments){
		p.setPen(penForLines);
		p.drawLine(l.getLine());
		p.setPen(penForConns);
	}
}
