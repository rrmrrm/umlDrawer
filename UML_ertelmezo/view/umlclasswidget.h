#ifndef UMLCLASSWIDGET_H
#define UMLCLASSWIDGET_H

#include "shape_drawing/drawing.hpp"
#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"

#include "ui_umlClass.h"

#include <QWidget>

class QVBoxLayout;
class UMLClassComponent;
class QFrame;
class Controller;

namespace Ui {
class umlClass;
}


class umlClassWidget : public CEW_Base
{
	Q_OBJECT
	
protected:
	int titleHeight = 50;
	std::weak_ptr<UMLClassComponent> ucc;
	//QVBoxLayout* vbl;
	
public:
	umlClassWidget(std::shared_ptr<UMLClassComponent> uc_, QWidget* parent);
	virtual ~umlClassWidget();
	
protected:
	QFrame* makeHSeparator(QWidget* parent = nullptr);
public slots:
	virtual void derivedPaintEvent(QPaintEvent* e) override;
	virtual void CE_geometryChanged() override;
private:
	Ui::umlClass *ui;
};

#endif // UMLCLASSWIDGET_H
