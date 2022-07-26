#include <assert.h>
#include "shape_drawing/gesture.h"



Gesture::Gesture():
	points(),
	minX(0),
	minY(0),
	maxX(0),
	maxY(0),
	startX(0),
	startY(0),
	isMinMaxSetup(false),
	endX(0),
	endY(0)
{
	
}

Gesture::Gesture(int _startX, int _startY):
		points(),
		minX(_startX),
		minY(_startY),
		maxX(_startX),
		maxY(_startY),
		startX(_startX),
		startY(_startY),
		isMinMaxSetup(true),
		endX(_startX),
		endY(_startY)
			{
	addPoint(_startX, _startY);
}
Gesture::~Gesture(){}
Gesture& Gesture::operator=(const Gesture& g){
	if(&g == this){
		return *this;
	}
	points = g.points;
	startX	= g.startX;
	startY	= g.startY;
	minX	= g.minX;
	minY	= g.minY;
	maxX	= g.maxX;
	maxY	= g.maxY;
	isMinMaxSetup = g.isMinMaxSetup;
	endX	= g.endX;
	endY	= g.endY;
	return *this;
}
bool Gesture::getMinX(int* ret) const{
	assert(ret != nullptr);
	if(isMinMaxSetup){
		*ret = minX;
		return true;
	}
	return false;
}
bool Gesture::getMinY(int* ret) const{
	assert(ret != nullptr);
	if(isMinMaxSetup){
		*ret = minY;
		return true;
	}
	return false;
	
}
bool Gesture::getMaxX(int* ret) const{
	assert(ret != nullptr);
	if(isMinMaxSetup){
		*ret = maxX;
		return true;
	}
	return false;
	
}
bool Gesture::getMaxY(int* ret) const{
	assert(ret != nullptr);
	if(isMinMaxSetup){
		*ret = maxY;
		return true;
	}
	return false;
	
}
void Gesture::addPoint(int x, int y){
	if(getPoints().size() > 0){
		if(isMinMaxSetup){
			if(x < minX){
				minX = x;
			}
			if(y < minY){
				minY = y;
			}
			if(x > maxX){
				maxX = x;
			}
			if(y > maxY){
				maxY = y;
			}
		}
		else{
			minX = x;
			maxX = x;
			minY = y;
			maxY = y;
		}
		isMinMaxSetup = true;
	}
	else{
		
	}
	endX = x;
	endY = y;
	points.push_back(std::pair<int,int>(x,y));
}
const std::vector<std::pair<int,int> >& Gesture::getPoints() const{
	return points;
}

void Gesture::setPointAt(unsigned int ind , int x, int y){
	assert(ind < points.size());
	points[ind].first = x;
	points[ind].second = y;
	
	assert(isMinMaxSetup);
	if(x < minX){
		minX = x;
	}
	if(y < minY){
		minY = y;
	}
	if(x > maxX){
		maxX = x;
	}
	if(y > maxY){
		maxY = y;
	}
}
std::ostream& Gesture::print(std::ostream& os) const{
	os << startX << '\t' << startY << std::endl;
	os << minX << '\t' << minY << std::endl;
	os << maxX << '\t' << maxY << std::endl;
	os << isMinMaxSetup << std::endl;
	os << endX << '\t' << endY << std::endl;
	os << points.size() << std::endl;
	for (unsigned int i = 0; i < points.size(); ++i) {
		os << points[i].first << '\t' << points[i].second << std::endl;
	}
	os << std::endl;
	if(os.fail()){
		std::cerr << "ERROR: Gesture::print(..): failed to write all data!" << std::endl;
	}
	return os;
}
///TODO: a hibakiiradst valszeg ki kiell szedni majd ha a Drawing-tól külön is
/// fogom használni a Gesture::load() függvényt, mert ha a file csak gesture-öket tartalmaz, akkor a fail annyit jelent, hogy a file végére értünk
std::istream& Gesture::load(std::istream& is){
	is >> startX;
	is >> startY;
	is >> minX;
	is >> minY;
	is >> maxX;
	is >> maxY;
	is >> isMinMaxSetup;
	is >> endX;
	is >> endY;
	unsigned int pointsNum;
	is >> pointsNum;
	points.resize(pointsNum);
	for (unsigned int i = 0; i < points.size(); ++i) {
		is >> points[i].first;
		is >> points[i].second;
	}
	if(is.fail()){
		std::cerr << "ERROR: Gesture::load(..): failed to load all data!" << std::endl;
	}
	return is;
}

std::ostream& operator<<(std::ostream& os, const Gesture& g){
	g.print(os);
	return os;
}
std::istream& operator>>(std::istream& is, Gesture& g){
	g.load(is);
	return is;
}
