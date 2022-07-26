#include "myqline.h"

#include <qdebug.h>

MyQLine::MyQLine() : line(0,0,0,0), dim(HORIZONTAL)
	//connRel(std::make_shared<Connector>(CanvasElement_Type::RELATIONSHIP)),
	//connText(std::make_shared<Connector>(CanvasElement_Type::TEXTBOX))
{
	
	assert(validate());
}

MyQLine::MyQLine(QPoint bounds_p1, QPoint bounds_p2, Dimension dim_)
		: dim(dim_)
		  //connRel(std::make_shared<Connector>(CanvasElement_Type::RELATIONSHIP)),
		  //connText(std::make_shared<Connector>(CanvasElement_Type::TEXTBOX))
{
	line.setP1(bounds_p1);
	switch(dim){
	case HORIZONTAL:{
		line.setP2(QPoint(bounds_p2.x(),bounds_p1.y()));
		break;
	}
	case VERTICAL:{
		line.setP2(QPoint(bounds_p1.x(),bounds_p2.y()));
		break;
	}
	}
	//connRel->setTranslation(bounds_p1);
	//connRel->setArea(  QRect( QRect(QPoint(0,0), bounds_p2 - bounds_p1) )  );
	//connText->setTranslation(bounds_p1);
	//connText->setArea(  QRect( QRect(QPoint(0,0), bounds_p2 - bounds_p1) )  );
	assert(validate());	
}
MyQLine::MyQLine(Dimension d, int dimC1, int dimC2, int otherC)
{
	dim = d;
	switch (d) {
	case HORIZONTAL:{
		line = QLine(dimC1, otherC, dimC2, otherC);
		break;
	}
	case VERTICAL:{
		line = QLine(otherC, dimC1, otherC, dimC2);
		break;
	}
	}
}
MyQLine::MyQLine(MyQLine&& other)
	:
	line(std::move(other.line)),
	dim(std::move(other.dim))
	//connRel(std::move(other.connRel)),
	//connText(std::move(other.connText))
		{
}
MyQLine::MyQLine(const MyQLine& other)
	:
	line(other.line),
	dim(other.dim)
	//connRel(other.connRel),
	//connText(other.connText)
		{
}
MyQLine& MyQLine::operator=(const MyQLine& other){
	line = other.line;
	dim = other.dim;
	//connRel = other.connRel;
	//connText = other.connText;
	return *this;
}



void MyQLine::fromBounds(QPoint bounds_p1, QPoint bounds_p2, Dimension dim_){
	dim = dim_;
	line.setP1(bounds_p1);
	switch(dim){
	case HORIZONTAL:{
		line.setP2(QPoint(bounds_p2.x(),bounds_p1.y()));
		break;
	}
	case VERTICAL:{
		line.setP2(QPoint(bounds_p1.x(),bounds_p2.y()));
		break;
	}
	}
	//connRel->setTranslation(bounds_p1);
	//connRel->setArea(  QRect( QRect(QPoint(0,0), bounds_p2 - bounds_p1) )  );
	//connText->setTranslation(bounds_p1);
	//connText->setArea(  QRect( QRect(QPoint(0,0), bounds_p2 - bounds_p1) )  );
	assert(validate());	
}

QPoint MyQLine::p1() const{
	assert(validate());	
	return line.p1();
}
QPoint MyQLine::p2() const{
	assert(validate());	
	return line.p2();
}

int MyQLine::x1() const{
	assert(validate());	
	return line.x1();
}
int MyQLine::y1() const{
	assert(validate());	
	return line.y1();}

int MyQLine::x2() const{
	assert(validate());	
	return line.x2();
}
int MyQLine::y2() const{
	assert(validate());	
	return line.y2();
}

int MyQLine::dx() const{
	assert(validate());	
	return line.dx();
}
int MyQLine::dy() const{
	assert(validate());	
	return line.dy();
}

const QLine& MyQLine::getLine() const{
	assert(validate());	
	return line;
}
void MyQLine::setLine(QLine l){
	line = l;
}
void MyQLine::translate(const QPoint &p){
	assert(validate());	
	line.translate(p);
	//connRel->setTranslation(connRel->getTranslation()+p);
	//connText->setTranslation(connText->getTranslation()+p);
}
void MyQLine::translate(int dx, int dy){
	assert(validate());	
	line.translate(dx,dy);
	//connRel->setTranslation(connRel->getTranslation() + QPoint(dx, dy));
	//connText->setTranslation(connText->getTranslation() + QPoint(dx, dy));
}
bool MyQLine::validate() const{
	bool p = false;
	switch(dim){
	case HORIZONTAL:{
		p = line.y1()==line.y2()	
		&& (getDir()==RIGHT || getDir()==LEFT);
		break;
	}
	case VERTICAL:{
		p = (line.x1() == line.x2())
		&& (getDir()==BOT || getDir()==TOP);
		break;
	}
	}
	if(!p){
		qDebug() << "MyQLine state invalid" << '\n';
	}
	return p;
}
QPoint MyQLine::center() const {
	assert(validate());	
	return line.center();
}
Direction MyQLine::getOtherDir() const{
	switch (dim) {
	case HORIZONTAL:{
		if(line.x1() < line.x2())
			return LEFT;
		else
			return RIGHT;
		break;
	}
	case VERTICAL:{
		if(line.y1() < line.y2())
			return TOP;
		else
			return BOT;
		break;
	}
	}
	return RIGHT;
}
Direction MyQLine::getOtherDir(Direction dir) const{
	switch (dir) {
	case LEFT:{
		return RIGHT;
	}
	case RIGHT:{
		return LEFT;
	}
	case TOP:{
		return BOT;
	}
	case BOT:{
		return TOP;
	}
	}
	return RIGHT;
}
Direction MyQLine::getDir() const{
	switch (dim) {
	case HORIZONTAL:{
		if(line.x1() < line.x2())
			return RIGHT;
		else
			return LEFT;
		break;
	}
	case VERTICAL:{
		if(line.y1() < line.y2())
			return BOT;
		else
			return TOP;
		break;
	}
	}
	return RIGHT;
}

Dimension MyQLine::getDim() const{
	assert(validate());
	return dim;
}
void MyQLine::setDim(Dimension d){
	dim = d;
}
Dimension MyQLine::getOtherDim(Dimension dim) const{
	assert(validate());	
	switch (dim) {
	case HORIZONTAL:{
		return VERTICAL;
		break;
	}
	case VERTICAL:{
		return HORIZONTAL;
		break;
	}
	}
	return VERTICAL;
}
Dimension MyQLine::getOtherDim() const{
	assert(validate());	
	return getOtherDim(dim);
}
QPoint MyQLine::getPart(LinePart part) const{
	switch(part){
	case START:{
		return line.p1();
	}
	case END:{
		return line.p2();
	}
	default:{
		assert(false);
	}
	}
	return line.p1();
}
QPoint MyQLine::zeroComponent(QPoint p, Dimension d){
	assert(validate());
	switch (d) {
	case HORIZONTAL:{
		p.setX(0);
		break;
	}
	case VERTICAL:{
		p.setY(0);
		break;
	}
	}
	return p;
}
void MyQLine::translatePart(LinePart lp, QPoint translation){
	assert(validate());	
	switch(lp){
	case START:{
		line.setP1(translation + line.p1());
		break;
	}
	case END:{
		line.setP2(translation + line.p2());
		break;
	}
	default:{
		assert(false);
		break;
	}
	}
	assert(validate());	
}
void MyQLine::moveTransversal(QPoint newPlace, bool doValidation){
	if(doValidation)
		assert(validate());	
	QPoint trans = zeroComponent(newPlace-line.p1(), dim);
	line.translate(trans);
	//connRel->setTranslation(connRel->getTranslation() + trans);
	//connText->setTranslation(connText->getTranslation() + trans);
	if(doValidation)
		assert(validate());	
}
void MyQLine::movePartAlongLine(const QPoint& newPlace, LinePart lp, bool doValidation){
	if(doValidation)
		assert(validate());	
	switch(lp){
	case START:{
		QPoint trans( zeroComponent(newPlace - line.p1(), getOtherDim(dim)) );
		translatePart(START, trans);
		break;
	}
	case END:{
		QPoint trans( zeroComponent(newPlace - line.p2(), getOtherDim(dim)) );
		translatePart(END, trans);
		break;
	}
	default:{
		assert(false);
		break;
	}
	}
	if(doValidation)
		assert(validate());		
}
