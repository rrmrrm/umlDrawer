#ifndef IDRAWINGCONTAINER_H
#define IDRAWINGCONTAINER_H
#include <iostream>
#include <vector>
class Drawing;
class IDrawingContainer{
protected:
	IDrawingContainer(){}
public:
	virtual const std::vector<Drawing*>&  getDrawings() const = 0;
};
#endif // IDRAWINGCONTAINER_H
