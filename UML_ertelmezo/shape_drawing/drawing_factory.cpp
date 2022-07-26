#include "shape_drawing/drawing_factory.h"
#include "shape_drawing/drawing.hpp"
#include "functions.hpp"
#include "drawing.hpp"
#include "drawing.hpp"

#include <QTime>
#include <qmath.h>
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
    #include <QRandomGenerator>
#else
    #include <qrandomgenerator.h>
#endif


Drawing* DrawingFactory::create(double _resolutionHorizontal,  double _resolutionVertical) const{
	return new Drawing(standardDeviation, marginInPixels, _resolutionHorizontal, _resolutionVertical);
}
DrawingFactory::DrawingFactory(double _standardDevitaion, uint _marginInPixels):
	standardDeviation(_standardDevitaion),
	marginInPixels(_marginInPixels)
{}
DrawingFactory::~DrawingFactory(){
	
}
