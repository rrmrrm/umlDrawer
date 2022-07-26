#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include "canvaselementwidgetbase.h"
#include "model/CanvasElements/canvaselement.h"

#include <list>

class LineWidget :public CEW_Base
{
	Q_OBJECT
public:
	const std::weak_ptr<LineComponent> line;
	ConnWidget* cRelMarker1W;
	ConnWidget* cRelMarker2W;
	ConnWidget* cTexBox1W;
	ConnWidget* cTexBox2W;
public:
	
	LineWidget(std::weak_ptr<LineComponent> lc, QWidget* parent);
	virtual ~LineWidget();
public:
	/// a vonalszegmensek és ConnectorWidgetek alapján frissíti a LineWidget mask-ját és geometry-jét:
	void adjustMaskAndGeometry();
public slots:
	virtual void CE_LinesChanged();
	virtual void CE_geometryChanged() override;
	virtual void derivedPaintEvent(QPaintEvent* e) override;
};

#endif // LINEWIDGET_H
