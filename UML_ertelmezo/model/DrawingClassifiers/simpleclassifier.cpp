#include "simpleclassifier.h"
#include "shape_drawing/drawing.hpp"
#include "model/dirDim.hpp"

#include <QString>
#include <QPainter>

#include <vector>
#include <QPainterPath>

QImage smoothed(const QImage& imgOriginal, double sD){
	assert(sD != 0);
	assert(sD > 0.0000001);
	/// a kernel értékek összege 1 kell legyen
	int ksiz = 2*ceil(3*sD+1)+1;
	/// ksiz mindenképp páratlan lesz. ez jó, mert igy mindig az egyik pixel a kernel közepén kell legyen.
	std::vector<double> kernel(ksiz);
	/// szorzó, hogy a Convolúciós kernel nagyjából 1 összegű legyen:
	double normalizer = 1 / (sD * sqrt(2*3.14159265359));
	auto conv1D = Conv1D(ksiz, GaussBlur1D(sD, normalizer));
	int w = imgOriginal.width();
	int h = imgOriginal.height();
	
	QImage img_firstPass(QSize(w, h), QImage::Format_RGB32);
	img_firstPass.fill(Qt::black);
	QImage img_secondPass(QSize(w, h), QImage::Format_RGB32);
	img_secondPass.fill(Qt::black);
	
	{
	for(int row = 0 ; row < h ; ++row){
		ImageRowAccessorConst it_in(&imgOriginal, row);
		ImageRowAccessor it_1pass(&img_firstPass, row);
		
		conv1D(it_in, it_1pass, true);
		
		///teszt:
		std::vector<int> a;
		for(auto it : it_in){
			a.push_back(int(it));
		}
		std::vector<int> b;
		for(auto it : it_1pass){
			b.push_back(int(it));
		}
		int g = 0;
	}
	for(int col = 0 ; col < w ; ++col){
		ImageColumnAccessorConst it_1pass(&img_firstPass, col);
		ImageColumnAccessor it_2pass(&img_secondPass, col);
		
		conv1D(it_1pass, it_2pass, true);
		
		///teszt:
		std::vector<int> a;
		for(auto it : it_1pass){
			a.push_back(int(it));
		}
		std::vector<int> b;
		for(auto it : it_2pass){
			b.push_back(int(it));
		}
		int g = 0;
	}
	}
	return img_secondPass;
}
void drawRotated(QImage& img1, const QImage& toDraw, double deg){
	img1.fill(Qt::black);
	QPainter p(&img1);
	/// transforms-al fogjuk a képet transzformálni
	QTransform transforms;
	QTransform moveCenterTo0; moveCenterTo0.translate(-img1.width()/2.0, -img1.height()/2.0);
	transforms *= moveCenterTo0;
	
	QTransform rot; rot.rotate(deg); 
	transforms *= rot;
	
	QTransform moveBack; moveBack.translate(img1.width()/2.0, img1.height()/2.0);
	transforms *= moveBack;
	
	p.setTransform(transforms);
	/// kirajzoljuk a képet:
	p.drawImage(0,0, toDraw);
} 
SimpleClassifier::SimpleClassifier()
{
	double smoothing = imgSiz / 17; 
	auto format = QImage::Format_RGB32;
	{
	sLine = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sLine);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	p.drawLine(0,0, imgSiz,imgSiz);
	
	idealSamples.push_back(&sLine);
	}
	{
	sTextBox = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sTextBox);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	p.drawRect(0,0, imgSiz-1,imgSiz-1);
	idealSamples.push_back(&sTextBox);
	}
	{
	sUmlClass = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sUmlClass);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	p.drawRect(0,0, imgSiz-1,imgSiz-1);
	p.drawLine(0,imgSiz/2, imgSiz-1,imgSiz/2);
	idealSamples.push_back(&sUmlClass);
	}
	{
	sAggregation = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sAggregation);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	int halfSiz = imgSiz/2;
	p.translate(QPoint(halfSiz,halfSiz));
	p.drawLine(-halfSiz, 0, 0, -halfSiz);
	p.drawLine(0, -halfSiz, halfSiz, 0);
	p.drawLine(-halfSiz, 0, 0, halfSiz);
	p.drawLine(0, halfSiz, halfSiz, 0);
	idealSamples.push_back(&sAggregation);
	}
	{
	sCompozition = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sCompozition);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	int halfSiz = imgSiz/2;
	p.translate(QPoint(halfSiz,halfSiz));
	
	QPolygon compPoly;
	QPainterPath compPath;
	compPoly 
			<< QPoint(-halfSiz,0)
			<< QPoint(0,-halfSiz)
			<< QPoint(halfSiz,0)
			<< QPoint(0,halfSiz);
	compPath.addPolygon(compPoly);
	p.fillPath(compPath,QBrush(QColor(255,255,255), Qt::SolidPattern));
	idealSamples.push_back(&sCompozition);
	}
	
	{
	sExtensionUp = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sExtensionUp);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	int halfSiz = imgSiz/2;
	p.translate(QPoint(halfSiz,halfSiz));
	//rotPainter(p,TOP);
	p.drawLine(-halfSiz, 0, 0, -halfSiz);
	p.drawLine(0, -halfSiz, halfSiz, 0);
	// függőleges:
	p.drawLine(0, -halfSiz, 0, halfSiz);
	// vízszintes:
	p.drawLine(-halfSiz, 0, halfSiz, 0);
	idealSamples.push_back(&sExtensionUp);
	}
	{
	sExtensionRight = QImage(QSize(imgSiz, imgSiz), format);
	drawRotated(sExtensionRight, sExtensionUp, 90);
	idealSamples.push_back(&sExtensionRight);
	}
	{
	sExtensionDown = QImage(QSize(imgSiz, imgSiz), format);
	drawRotated(sExtensionDown, sExtensionUp, 180);
	idealSamples.push_back(&sExtensionDown);
	}
	{
	sExtensionLeft = QImage(QSize(imgSiz, imgSiz), format);
	drawRotated(sExtensionLeft, sExtensionUp, 270);
	idealSamples.push_back(&sExtensionLeft);
	}
	{
	sCircle = QImage(QSize(imgSiz, imgSiz), format);
	QPainter p(&sCircle);
	p.fillRect(0,0, imgSiz,imgSiz, QBrush(QColor(0,0,0)));
	p.setPen(QColor(255,255,255));
	int halfSiz = imgSiz/2;
	p.translate(QPoint(halfSiz,halfSiz));
	p.drawEllipse(-halfSiz, -halfSiz, imgSiz-1, imgSiz-1);
	idealSamples.push_back(&sCircle);
	}
	for(int i = 0 ; i < (int)idealSamples.size() ; ++i){
		idealSamples[i]->save(
				qstr("classSampleOrig") + QString::number(i) + qstr(".png"), "PNG"
			);
	}
	
	for(auto img : idealSamples){
		*img = smoothed(*img, smoothing);
	}
	for(int i = 0 ; i < (int)idealSamples.size() ; ++i){
		idealSamples[i]->save(
				qstr("classSampleAfterSmoothing") + QString::number(i) + qstr(".png"), "PNG"
			);
	}
	
	double minImgAvgLight = 255.0;
	for(auto& img : idealSamples){
		double act = getAvgLight(*img);
		if(minImgAvgLight > act)
			minImgAvgLight = act;
	}
	this->lightnessAvg = minImgAvgLight;
			
	for(auto& img : idealSamples){
		setAvgLight(*img, this->lightnessAvg);
	}
	
	for(int i = 0 ; i < (int)idealSamples.size() ; ++i){
		idealSamples[i]->save(
				qstr("classSampleAfterLightNormalizing") + QString::number(i) + qstr(".png"), "PNG"
			);
	}
}

double SimpleClassifier::sqrDiff(const QImage& i1, const QImage& i2) const{
	assert(i1.width() == i2.width());
	assert(i1.height() == i2.height());
	int w = i1.width();
	int h = i1.height();
	
	double ret = 0;
	for(int i = 0 ; i < w ; ++i){
		for(int j = 0 ; j < h ; ++j){
			int p1 = qGray(i1.pixel(i,j));
			int p2 = qGray(i2.pixel(i,j));
			ret += (p1 - p2)*(p1 - p2);
		}
	}
	return ret;
}
QImage rectified(Drawing* d, uint size){
	assert(d);
	double rw = d->getMaxX() - d->getMinX();
	double rh = d->getMaxY() - d->getMinY();
	QImage ret;
	if(rh > rw){
		ret = d->accessImage()
				.scaled(size*rh/rw, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
				.copy(QRect(0,0, size, size));
	}
	else{
		ret = d->accessImage()
				.scaled(size, size*rw/rh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
				.copy(QRect(0,0, size, size));
	}
	return ret;
}
double SimpleClassifier::getAvgLight(const QImage& img) const{
	
	double actLightSum = 0;
	for(int row = 0 ; row < img.height() ; ++row){
		ImageRowAccessorConst r(&img, row);	
		for(auto it : r){
			actLightSum += ((int)(it))/255.0;
		}
	}
	actLightSum /= (double)(img.width()*img.height());
	return actLightSum;
}
void SimpleClassifier::setAvgLight(QImage& img, double avg) const{
	
	double actLightAvg = getAvgLight(img);
	/// ha teljesen sötét a kép, akkor nincs értelme fényesíteni
	/// (és nagy számítási hibához is vezetne)
	if(actLightAvg < 0.0000001)
		return;
	/// img pixeleit annyival osztom le, hogy img átlagos fényessége lightnessAvg legyen:
	for(int row = 0 ; row < img.height() ; ++row){
		ImageRowAccessor r(&img, row);
		for(auto it : r){
			it = (QColor)(it) * (avg/actLightAvg);
		}
	}
}
CE_flatType SimpleClassifier::classify(Drawing* d){
	assert(d);
	assert(idealSamples.size() != 0);
	d->accessImage().save(qstr("originalDrawing.png"), "PNG");
	QImage norm = rectified(d, imgSiz);
	norm.save(qstr("rectifiedDrawing.png"), "PNG");
	setAvgLight(norm, lightnessAvg);
	norm.save(qstr("lightNormalizedDrawing.png"), "PNG");
	

	std::vector<double> metrics(idealSamples.size());
	int maxActivationIndex = 0;
	for(int i = 0 ; i < (int)idealSamples.size() ; ++i){
		metrics[i] = -1 * sqrDiff(norm, *idealSamples[i]);
	}
	double max = std::numeric_limits<double>::lowest();
	for(uint i = 0 ; i < metrics.size() ; ++i){
		if(metrics[i] > max){
			maxActivationIndex = i;
			max = metrics[i];
		}
	}
	switch(maxActivationIndex){
	case 0:
		return CE_flatType::LINET;
	case 1:
		return CE_flatType::TEXTBOXT;
	case 2:
		return CE_flatType::UMLCLASST;
	case 3:
		return CE_flatType::AGGREGATIONT;
	case 4:
		return CE_flatType::COMPOSITIONT;
	case 5:
	case 6:
	case 7:
	case 8:
		return CE_flatType::EXTENSIONT;
	case 9:
		return CE_flatType::CIRCLET;
	default: assert(false);
	}

	return (CE_flatType)0;
}

ImageIteratorInner::operator QColor(){
	assert(!parent->isEnd());
	return parent->accessImage()->pixelColor(parent->pixPos);
}
ImageIteratorInner::operator int(){
	assert(!parent->isEnd());
	return parent->accessImage()->pixelColor(parent->pixPos).value();
}
ImageIteratorInner& 
		ImageIteratorInner::operator=(QColor c)
{
	parent->accessImage()->setPixelColor(parent->getPos(), c);
	return *this;
}

ImageIteratorInner& 
		ImageIteratorInner::operator+=(QColor c)
{
	QPoint pos = parent->getPos();
	parent->accessImage()->setPixelColor(
				pos, 
				c + parent->accessImage()->pixelColor(pos)
			);
	return *this;
}

ImageIteratorInnerConst::operator QColor() const{
	assert(!parent->isEnd());
	
	return parent->getImage()->pixelColor(parent->pixPos);
}
ImageIteratorInnerConst::operator int() const{
	assert(!parent->isEnd());
	return parent->getImage()->pixelColor(parent->pixPos).value();
}
ImageIteratorInnerConst& 
		ImageIteratorInnerConst::operator=(QColor c)
{
	assert(false);
	return *this;
}

ImageIterator::ImageIterator(QImage* img_, QPoint pixPos_)
	: //isOnePastEnd(isGuard_)
	img(img_)
	, iii(this)
	, pixPos(pixPos_)
{}
bool ImageIterator::isEnd() const{
	return (pixPos.x() >= img->width() || pixPos.y() >= img->height());
}
bool ImageIterator::operator==(const ImageIterator& o) const{
	/// ugyanaban a  sorban vagy oszlopan kell iterálniuk, más különben az összehasonlításuk értelmetlen
	assert(pixPos.x() == o.getPos().x() || pixPos.y() == o.getPos().y());
	return pixPos == o.getPos();
}
bool ImageIterator::operator!=(const ImageIterator& o) const{
	return !(*this == o);
}
bool ImageIterator::operator==(const ImageIteratorConst& o) const{
	/// egy sorban vagy oszlopot kell iterálniuk, más különben az összehasonlításuk értelmetlen
	assert(pixPos.x() == o.getPos().x() || pixPos.y() == o.getPos().y());
	return pixPos == o.getPos();
}
bool ImageIterator::operator!=(const ImageIteratorConst& o) const{
	return !(*this == o);
}
ImageIteratorInner& ImageIterator::operator*(){
	assert(!isEnd());
	
	return iii;
}
QPoint ImageIterator::getPos() const{
	return pixPos;
}

ImageIteratorConst::ImageIteratorConst(const QImage* img_, QPoint pixPos_)
	: //isOnePastEnd(isGuard_)
	img(img_)
	, iii(this)
	, pixPos(pixPos_)
{}
ImageIteratorConst::~ImageIteratorConst()
{}
bool ImageIteratorConst::isEnd() const{
	return (pixPos.x() >= img->width() || pixPos.y() >= img->height());
}
bool ImageIteratorConst::operator==(const ImageIterator& o) const{
	/// egy sorban vagy oszlopot kell iterálniuk, más különben az összehasonlításuk értelmetlen
	assert(pixPos.x() == o.getPos().x() || pixPos.y() == o.getPos().y());
	return pixPos == o.getPos();
}
bool ImageIteratorConst::operator!=(const ImageIterator& o) const{
	return !(*this == o);
}
bool ImageIteratorConst::operator==(const ImageIteratorConst& o) const{
	/// egy sorban vagy oszlopot kell iterálniuk, más különben az összehasonlításuk értelmetlen
	assert(pixPos.x() == o.getPos().x() || pixPos.y() == o.getPos().y());
	return pixPos == o.getPos();
}
bool ImageIteratorConst::operator!=(const ImageIteratorConst& o) const{
	return !(*this == o);
}
const ImageIteratorInnerConst& ImageIteratorConst::operator*(){
	assert(!isEnd());/// TODO: itt pár léptetés után pixpos != iii.parent->pixpos, ez rossz mukodes
	
	return iii;
}
QPoint ImageIteratorConst::getPos() const{
	return pixPos;
}


ImageRowIterator::ImageRowIterator(QImage* img_, int rowInd_)
	: ImageIterator(img_, QPoint(0, rowInd_))
{}
ImageRowIterator::~ImageRowIterator(){
}
ImageIterator& ImageRowIterator::operator++(){
	pixPos.setX(pixPos.x()+1);
	return *this;
}

ImageColumnIterator::ImageColumnIterator(QImage* img_, int colInd_)
	: ImageIterator(img_, QPoint(colInd_, 0))
{}

ImageColumnIterator::~ImageColumnIterator(){
	
}
ImageIterator& ImageColumnIterator::operator++(){
	pixPos.setY(pixPos.y()+1);
	return *this;
}

ImageRowIteratorConst::ImageRowIteratorConst(const QImage* img_, int rowInd_)
	: ImageIteratorConst(img_, QPoint(0, rowInd_))
{}
ImageRowIteratorConst::~ImageRowIteratorConst(){
	
}
ImageIteratorConst& ImageRowIteratorConst::operator++(){
	pixPos.setX(pixPos.x()+1);
	return *this;
}

ImageColumnIteratorConst::ImageColumnIteratorConst(const QImage* img_, int colInd_)
	: ImageIteratorConst(img_, QPoint(colInd_, 0))
{}
ImageColumnIteratorConst::~ImageColumnIteratorConst(){
	
}
ImageIteratorConst& ImageColumnIteratorConst::operator++(){
	pixPos.setY(pixPos.y()+1);
	return *this;
}





ImageRowAccessor::ImageRowAccessor(QImage* img_, int rowInd_)
	: img(img_)
	, rowInd(rowInd_)
{
	assert(rowInd_ < img->height());
}
ImageRowIterator ImageRowAccessor::begin(){
	return ImageRowIterator(img, rowInd);
}
ImageRowIterator ImageRowAccessor::end(){
	auto ret = ImageRowIterator(img, rowInd);
	//az adott sor utolsó eleme után kell eggyel indexelnie ret-nek:
	ret.pixPos = QPoint(img->width(), rowInd);
	return ret;
}

ImageColumnAccessor::ImageColumnAccessor(QImage* img_, int colInd_)
	: img(img_)
	, colInd(colInd_)
{
	assert(colInd_ < img->width());
}
ImageColumnIterator ImageColumnAccessor::begin(){
	return ImageColumnIterator(img, colInd);
}
ImageColumnIterator ImageColumnAccessor::end(){
	auto ret = ImageColumnIterator(img, colInd);
	//az adott oszlop utolsó eleme után kell eggyel indexelnie ret-nek:
	ret.pixPos = QPoint(colInd, img->height());
	return ret;
}



ImageRowAccessorConst::ImageRowAccessorConst(const QImage* img_, int rowInd_)
	: img(img_)
	, rowInd(rowInd_)
{
	assert(rowInd_ < img->height());
}
ImageRowIteratorConst ImageRowAccessorConst::begin(){
	return ImageRowIteratorConst(img, rowInd);
}
ImageRowIteratorConst ImageRowAccessorConst::end(){
	auto ret = ImageRowIteratorConst(img, rowInd);
	//az adott sor utolsó eleme után kell eggyel indexelnie ret-nek:
	ret.pixPos = QPoint(img->width(), rowInd);
	return ret;
}
QColor ImageRowAccessorConst::operator[](int colInd) const{
	return img->pixelColor(colInd, rowInd);
}
unsigned int ImageRowAccessorConst::size() const{
	return img->width();
}

ImageColumnAccessorConst::ImageColumnAccessorConst(const QImage* img_, int colInd_)
	: img(img_)
	, colInd(colInd_)
{
	assert(colInd_ < img->width());
}
ImageColumnIteratorConst ImageColumnAccessorConst::begin(){
	return ImageColumnIteratorConst(img, colInd);
}
ImageColumnIteratorConst ImageColumnAccessorConst::end(){
	auto ret = ImageColumnIteratorConst(img, colInd);
	//az adott oszlop utolsó eleme után kell eggyel indexelnie ret-nek:
	ret.pixPos = QPoint(colInd, img->height());
	return ret;
}
QColor ImageColumnAccessorConst::operator[](int rowInd) const{
	return img->pixelColor(colInd, rowInd);
}
unsigned int ImageColumnAccessorConst::size() const{
	return img->height();
}
