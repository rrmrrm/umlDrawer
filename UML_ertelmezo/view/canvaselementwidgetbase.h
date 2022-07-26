#ifndef CANVASELEMENT_H
#define CANVASELEMENT_H

#include "model/CanvasElements/canvaselementbase.h"
#include "model/target.h"
#include <QWidget>
#include <QLayout>

class ConnWidget;
class Connector;
class Plug;
class Canvas;

class CEW_Base : public QWidget{
	Q_OBJECT
	
	int permittedRepaintNum = 0;
public:
	const std::weak_ptr<CEB> ce;
	QRect rectToRepaint;
	
public:
	CEW_Base(std::weak_ptr<CEB> ce_, QWidget* parent);
	virtual ~CEW_Base();
	
protected:
	QRect toWCoords(QRect rInCanvas) const;
	/// Canvas-hoz relatív geometria beállítása:
	void setGeometryInC(QRect r);
public:
	/// Canvas-hoz relatív geometria lekérdezése:
	QRect getGeomInC();
	Canvas* accessCanvas();
	///ezzel a függvény döntjük el, hogy a smartUpdate(rect) hatására érkező paintEvent-ben újra kell-e rajzolni bármit is
	virtual bool shouldUsePaintEvent() const;
	virtual void paintBorder();
	void updateCEGeom();
	void updateConnsGeom();
	void ConnW_wantsToResize(ConnWidget* cw, QSize newSize);
	/// a CEW csatlakoztatása egy ConnWidget-hez:
	void C_connectedTo(std::weak_ptr<Plug> pl, CEW_Base* cew, ConnWidget* conn);
private slots:
	void decrPermRepaintNum();
	
public slots:
	/// frissíti a CE geometriáját és  meghívja a ConnWidgetek resizeEvent-jét:
	void resizeEvent(QResizeEvent *) override;
	/// frissíti a CE geometriáját és  meghívja a ConnWidgetek resizeEvent-jét:
	void moveEvent(QMoveEvent *event) override;
	
	void update();
	void repaint();
	void update(QRect r);
	void repaint(QRect r);
	
	void smartUpdate(QRect);
	void smartUpdate();
	virtual void CE_geometryChanged();
	//nem akarom, hogy a leszármazó osztályok a Qt paintEventjét felülírják és használják.
	// helyette a derivedPaintEvent-et kell felülírniuk
	void paintEvent(QPaintEvent* e) final;
	virtual void derivedPaintEvent(QPaintEvent* e);
	
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void CE_Drag(int cX, int cY);
	//virtual void CE_getPlugAtCCoords(int cX, int cY);
	void resizeEdge(int cX, int cY/*, Direction draggedEdge*/);
	void CE_destroyThis();
signals:
	void setGeometry_CEB(QRect r);
	//ha nem a megfelelő helyre kattintunk, akkor a CEW figyelmen kívül hagyja(ignore) a
	// mouseEvent-et, hogy a Canvas feldolgozhassa azt.
	void mousePressEventAccepted(QMouseEvent *event);
	void mouseMoveEventAccepted(QMouseEvent *event);
	void mouseReleaseEventAccepted(QMouseEvent *event);
	//void returnPlugAtCCoords_CE(std::weak_ptr<Plug>);
};


#endif // CANVASELEMENT_H
