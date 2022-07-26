#ifndef DRAWING_FACTORY_H
#define DRAWING_FACTORY_H
#include <iostream>
#include <assert.h>
#include "drawing.hpp"
///TODO: 
class DrawingFactory{
	double standardDeviation;
	int marginInPixels;
public:
	virtual Drawing* create(double _resolutionHorizontal,  double _resolutionVertical) const;
	//virtual Drawing* createEmpty() const;
	DrawingFactory(double _standardDevitaion, uint marginInPixels);
	~DrawingFactory();
};

#endif // DRAWING_FACTORY_H
