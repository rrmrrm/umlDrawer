#ifndef WIDGET_PAINTER_INTERFACE_H
#define WIDGET_PAINTER_INTERFACE_H
#include "common.hpp"
#include "shape_drawing/drawing.hpp"
/*
class SpecInterface: private Base{
public:
	SpecInterface(const Base& b):
		Base(b)
	{}
	DotMatrix const* getLastDrawing() const {
			
	}
	const std::vector<Gesture>& getLastDrawingGestures() const {
		
	}
	const std::vector<DotMatrix>& getDrawings() const {
			
	}
};
class Base{
protected:
	int a;
};*/
class Model_painter_interface{
public:
	virtual DotMatrix const* getLastDrawing() const = 0;
	virtual const std::vector<Gesture>& getLastDrawingGestures() const = 0;
	virtual const std::vector<DotMatrix>& getDrawings() const = 0;
};
#endif // WIDGET_PAINTER_INTERFACE_H
