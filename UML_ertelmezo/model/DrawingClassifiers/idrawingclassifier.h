#ifndef IDRAWINGCLASSIFIER_H
#define IDRAWINGCLASSIFIER_H
#include "model/CanvasElements/canvaselementtypes.h"
class Drawing;
class IDrawingClassifier
{
public:
	IDrawingClassifier();
	virtual ~IDrawingClassifier();
public:
	virtual CE_flatType classify(Drawing* d) = 0;
};

#endif // IDRAWINGCLASSIFIER_H
