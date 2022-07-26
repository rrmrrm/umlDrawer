#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "CanvasElements/canvaselementtypes.h"
#include "dirDim.hpp"
#include "identityDir/identifiable.h"
//#include "model/AggregationSaver/ihasaggregate.h"
#include "model/myweakptr.h"


#include <memory>
class CEB;
class Plug;

class Connector : 
		public QObject, 
		public virtual Identifiable, 
		//public virtual IHasAggregate, 
		public std::enable_shared_from_this<Connector>
{
	Q_OBJECT
private:
	/// TODO: connectableCEType enum használata helyett az enum helyett kaphatnának a Connectorok egy vagy több CEB leszármazottat, ami jelzi, hogy mely CEB eek csatlakozhatnak hozzá.
	/// amikor egy CEB csatlakozni akar, akkor a Connector összehasonlíja az ecélból tárolt CEB-jeivel, és ha a csatlakozni kívánó CEB leszármazik az egyik tárolt CEB-ből, akkor csatlakozhat.
	/// átgondolni ezt
	const CanvasElement_Type connectableCEType;
	// az area-nak az első pontjának eltolása TODO: nem biztos, hogy ez a comment még mindig igaz..:
	/// megjegyzés: a canvashoz képest relatívak a Connector koordinátái, nem a tartalmazó Canvaselementhez
	//QPoint translation;
	Direction dir = Direction::RIGHT;
	QRect geometry;
	//QRect area;
protected:
	//a movedAway-el biztosítom, hogy a move ctor hívása után a forrás objektumot senki se használja
	bool movedAway = false;
public:
	/// a felcsatlakoztatott CE-k eléréséhez:
	std::vector<Myweak_ptr<Plug>> connectedPlugs;
	CEB* const containingCE;
	
public:
	Connector(CEB* containingCE_, CanvasElement_Type ct);
	Connector(CEB* containingCE_, QRect geometry_, CanvasElement_Type ct);
	Connector(CEB* containingCE_, Direction dir_, CanvasElement_Type ct);
	virtual ~Connector();
	
protected:
	//void save_chainCallThis(std::ostream&);
	//void load_chainCallThis(std::istream&);
	//std::ostream& saveAggsChainCallThis(std::ostream& os);
	//std::istream& loadAggsChainCallThis(std::istream& is);

public:
	std::ostream& saveAggregations(std::ostream&);
	std::istream& loadAggregations(std::istream&);
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	QRect getGeometry() const;
	CanvasElement_Type getConnectableCEType() const;
	//QPoint getTranslation() const;
	//QPoint getCenter() const;
	Direction getDir() const;
	//QRect getArea() const;
	//QRect getTranslatedArea() const;
	bool containsPoint(QPoint p) const;
	//void setTranslation(QPoint);
	//void translate(QPoint);
	//void setArea(QRect);
	void setDir(Direction dir);
	//void setWidth(int w);
	//void tryExtend(Dimension dim);
	//void tryResize(QSize newSize);
public slots:
	void CW_setGeometry(QRect);
signals:
	///ez a signal a Connectort tartalmazó CanvasElementnek szól:
	void triingToResize(std::weak_ptr<Connector> weak_this, QSize newSize);
	
	/// ezek a felcsatlakozott Plug-oknak szólnak. 
	/// A connect meghívását a Plug osztály végzi el:
	void translationChanged(std::weak_ptr<Connector>, QPoint prevTr, QPoint newTr);
	void directionChanged(std::weak_ptr<Connector>, Direction prevDir, Direction newDir);
	void areaChanged(std::weak_ptr<Connector>, QRect newArea);
	
};

#endif // CONNECTOR_H
