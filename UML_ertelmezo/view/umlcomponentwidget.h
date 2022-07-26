#ifndef RECOGNISEDENTITYWIDGET_H
#define RECOGNISEDENTITYWIDGET_H

#include "shape_drawing/drawing.hpp"
#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"
#include <QWidget>

class QVBoxLayout;
class UMLClassComponent;
class QFrame;

class UMLClassWidget : public CanvasElementWidgetBase{
	Q_OBJECT
protected:
	int titleHeight = 50;
	std::weak_ptr<UMLClassComponent> ucc;
	QVBoxLayout* vbl;
	
public:
	UMLClassWidget(std::shared_ptr<UMLComponentBase> uc_, QWidget* parent);
	virtual ~UMLClassWidget(){}
	
protected:
	QFrame* makeHSeparator(QWidget* parent = nullptr);
public slots:
	virtual void derivedPaintEvent(QPaintEvent* e) override;
	virtual void canvasElement_geometryChanged() override;

	//void mouseMoveEvent(QMouseEvent *event) override;
	//void mouseReleaseEvent(QMouseEvent *event) override;
	//void mousePressEvent(QMouseEvent *event) override ;
};

#endif // RECOGNISEDENTITYWIDGET_H
