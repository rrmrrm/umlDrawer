#ifndef UMLRELATIONWIDGET_H
#define UMLRELATIONWIDGET_H
#include "view/canvaselementwidgetbase.h"

class UMLRelationship;
class UmlRelationWidget : public CEW_Base{
		Q_OBJECT
	
public:
	UmlRelationWidget(std::weak_ptr<UMLRelationship> ur_, QWidget* parent);
	virtual ~UmlRelationWidget(){}
private:
	const std::shared_ptr<UMLRelationship> getUr() const;
public slots:
	virtual void derivedPaintEvent(QPaintEvent* e) override;

	//void mouseMoveEvent(QMouseEvent *event) override;
	//void mouseReleaseEvent(QMouseEvent *event) override;
	//void mousePressEvent(QMouseEvent *event) override ;
};
#endif // UMLRELATIONWIDGET_H
