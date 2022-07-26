#ifndef PLUG_H
#define PLUG_H

#include "model/dirDim.hpp"
class Connector;
//#include "model/AggregationSaver/ihasaggregate.h"
#include "model/myweakptr.h"

#include <QObject>
#include <QRect>
class CEB;
#include <memory>

class Plug : 
		public QObject, 
		//public virtual IHasAggregate,
		public virtual Identifiable, 
		public std::enable_shared_from_this<Plug>
{
	Q_OBJECT
protected:
	Myweak_ptr<Connector> connectedConn;
	//uint id_inCE;
	CEB * const containingCE;
public:
	Plug();
	Plug(CEB*);
	//Plug(CEB*, uint id_);
	virtual ~Plug();
	
protected:
	//void save_chainCallThis(std::ostream&);
	//void load_chainCallThis(std::istream&);
	//std::ostream& saveAggsChainCallThis(std::ostream&);
	//std::istream& loadAggsFinalizeChainCallThis(std::istream&);
public:
	std::ostream& saveAggregations(std::ostream&);
	std::istream& loadAggregations(std::istream&);
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	//void setId(uint id_);
	//uint getId();
	QRect getGeometry() const;
	std::weak_ptr<Connector> getConnectedConn() const;
	void attach(Connector& conn);
	//detach(): connectedConn felé megszakítom a signal-slot kapcsolatokat
	void detach();
	CEB* getContainingCE();
public slots:
	void connectorTranslationChanged(std::weak_ptr<Connector>src, QPoint prevTr, QPoint newTr);
	void connectorDirectionChanged(std::weak_ptr<Connector>src, Direction prevDir, Direction newDir);
	void connectorAreaChanged(std::weak_ptr<Connector>, QRect newArea);
signals:
	void translationChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QPoint prevTr, QPoint newTr);
	void directionChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, Direction prevDir, Direction newDir);
	void areaChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QRect newArea);
};
///nem használom, de később talán szükséág lesz erre:
///// a LineComponent 2 UMLComponent Connectorán belül egy-egy kijelölt ponthoz csatlakozik.
///// a kijelölt pontok számontartására való a PlugGeom
//class PlugGeom : public Plug{
//	Q_OBJECT
//public:
//	/// relToConn a csatlakozott Connector-hoz relatív eltolása a plug-nak
//	QPoint relToConn;
//	/// a Plug lehet, hogy nincs csatlakoztatva egy Connectorhoz, de ekkor is van helye:
//	/// ott lesz, ahol legutoljára csatlakoztatva volt egyhez.
//	QPoint lastPoint;
//	Direction attachedSide = RIGHT;
//public:
//	PlugGeom();
//	PlugGeom(CEB*);
//	PlugGeom(CEB*, uint id_);
//	virtual ~PlugGeom();
//	
//	void setAttachedSide(Direction);
//	/// a PlugGeom - csatlakozott connectorhoz - relatív eltolása
//	void setRelToConn(QPoint);
//	/// visszaadja az utolsó helyet ahol a PlugGeom csatlakoztatva volt egy Connectorhoz:
//	QPoint getLastPoint();
//	bool isAttachedToConn() const;
//	/// frissíti a PlugGeom helyét a csatlakozott Connector helye alapján és visszaadja ezt a helyet:
//	QPoint updateAttP();
//};

#endif // PLUG_H
