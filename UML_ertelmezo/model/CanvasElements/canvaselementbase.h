#ifndef CANVASELEMENTBASE_H
#define CANVASELEMENTBASE_H

#include "canvaselementtypes.h"
#include "shape_drawing/drawing.hpp"
#include "model/dirDim.hpp"
#include "model/modelstatemanager.h"
#include "model/identityDir/identifiable.h"
//#include "model/AggregationSaver/ihasaggregate.h"
#include "model/myweakptr.h"

#include <QDebug>
#include <QMouseEvent>

#include <memory>
#include <vector>

class Plug;
class Connector;
class Target;
template<class AggregatedT>
//using AggregationT = std::weak_ptr<AggregatedT>;
using AggregationT = Myweak_ptr<AggregatedT>;
/// TODO: weak helyett MyWeak kellene és id szerinti state mentés és betöltés, mert a mutatott objektumok megszűnhetnek
class CanvasElementStateOp : public ModelStateOp{
protected:
	std::weak_ptr<Plug> plug;
	std::weak_ptr<Connector> prevConnectedConn;
	std::weak_ptr<Connector> newConnectedConn;
	
public:
	CanvasElementStateOp(std::weak_ptr<Plug> plug_,
						 std::weak_ptr<Connector> prevConnectedConn_,
						 std::weak_ptr<Connector> newConnectedConn_);
	
public:
	virtual void undo();
	virtual void redo();
};
class CEB: 
		public QObject,
		//public virtual IHasAggregate,
		public virtual Identifiable, 
		public std::enable_shared_from_this<CEB>
{
	Q_OBJECT
protected:
	bool isSelected_ = false;
	///TODO: lehetne itt shared_ptr /eket tartolni,
	///  es a lesytramyaottbanc typedeffel hivatkoyni ay egzes elemekre,
	///  hisy ugzse valtoynak a connector-ok es plug-ok
	std::vector<Myweak_ptr<Connector> > connectors;
public:
	const CanvasElement_Type CEtype;
	std::vector<Myweak_ptr<Plug> > plugs;
	QRect geometry;
	int dragStartX = 0;
	int dragStartY = 0;
	QPoint lastDragPoint;
	bool isDragging = false;
protected:
	//static std::shared_ptr<CEB> create(CanvasElement_Type CEtype);
	CEB(CanvasElement_Type type_, QRect geometry_);
	virtual ~CEB();
	
protected:
	void moveTopLeft(int cX, int cY);
	void forwardPlugSig(Plug* pl);
	//std::ostream& saveAggregations(std::ostream&);
	//std::istream& loadAggregations(std::istream&);
	//std::ostream& save(std::ostream&);
	//std::istream& load(std::istream&);
	virtual void dragUnselectedOverrideThis(int canvas_x, int canvas_y);
	virtual void stopDragOverrideThis(QPoint cPos);
public:
	virtual std::ostream& saveAggregations(std::ostream&);
	virtual std::istream& loadAggregations(std::istream&);
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	//void connectPlugToConnn(std::weak_ptr<Connector> targetConn,  std::weak_ptr<Plug> plDragged);
	/// a Canvas coordinátarendszerében értendő a visszaadott QRect:
	QRect getGeometry();
	//std::weak_ptr<Plug> getPlugAt(QPoint p);
	std::weak_ptr<Connector> getConnAt(QPoint p);
	//std::weak_ptr<Plug> getPlugAt(int cX, int cY);
	std::weak_ptr<Connector> getConnAt(int cX, int cY);
	virtual std::weak_ptr<Plug> getMainPlug() const = 0;
	virtual std::vector<AggregationT<Connector>> getConnectors();
	virtual bool containsPoint(QPoint p) const;
	/*virtual*/ std::weak_ptr<Connector> getConnFirstAtCanvasCoords(QPoint);
	virtual std::vector<AggregationT<Connector>> getConnsAllAtCanvasCoords(QPoint);
	virtual int x() const ;
	virtual int y() const ;
	virtual int width() const;
	virtual int height() const;
	//virtual void setGeometrySizeCentered(QSize newSize);
	void dragUnselected(int canvas_x, int canvas_y);
	void select();
	void deselect();
	bool isSelected();
	virtual void dragSelected(int canvas_x, int canvas_y);
	void stopDrag(QPoint cPos);
	
	virtual std::weak_ptr<Connector> tryConnToThis(std::weak_ptr<CEB> srcOwner, QPoint pOnCanv, std::weak_ptr<Plug> plug);
	virtual void forceConnToConnector(
			std::weak_ptr<CEB> srcOwner, 
			std::weak_ptr<Connector> target,
			std::weak_ptr<Plug> plug);
	virtual void connectionAllowed(std::weak_ptr<CEB> src, 
								   std::weak_ptr<Connector> connSrc, 
								   QPoint p, 
								   std::weak_ptr<Plug> plug);
	std::weak_ptr<Connector> mouseDoublePressed_My(Target t);
	
	///
	/// \brief modellCopy
	/// Alábbiakban feltételezem, hogy a leszármaztatási hierarchiában nincs többszörös öröklés. ekkor tehát a hierarchia egy irányítatlan fa.
	/// 
	///	új objektum lértrehozása ami egyezik az aktuális (leszármazott) objektum valódi típusával. az új objektum pár mezőjének másolása this-ből
	///	a lényeg, hogy heterogén tárolóban tárolom majd ezen típus leszármazottait shared_ptr formájában
	///	 és kellett egy eljárás, ami képes heterogén elem létrehozására és másolására. 
	///	A másolást a copyFrom eljárás segítségével végzem
	///
	/// virtual, bizonyos leszármazott osztályokban felül kell írni(részletek az eljárás törzsében)
	/// részletek: commentek a függvény törzsben.
	/// \param fromDerived
	///		default értéken kell hagyni amikor másik objektum hívja meg modellCopy-t.
	///		viszont ahol felül defiiáljuk ezt az eljárást ott megh kell híni a közvetlen ősön 
	///		ezt az eljárást, méghozzá az ott létrehozott fromDerived -al argumentumként.
	/// \return visszatérés: az új objektum ami egyezik this valódi típusával és bizonyos mezői is át vannak másolva this-ből.
	///
	virtual std::shared_ptr<CEB> modellCopy(std::shared_ptr<CEB> fromDerived = nullptr) const;
	
	/// a másolás csak részleges: a QObject rész nem másolható,
	///  és Identifiable lényege hogy minden Identifiable objektumnak különböző id-je lesz,
	///  tehát az id-t sem másolódik.
	virtual void copyFrom(const CEB* o);
signals:
	void should_repaint();
	void geometryChanged_CEW();
	//void geometryChanged();
	
	void drag_CEW(int cX, int cY);
	void resizeEdgeSig(int cX, int cY/*, Direction draggedEdge*/);
	void connectPlugToConn_C(std::weak_ptr<Connector> targetConn, std::weak_ptr<Plug> plDragged);
	void connectToCEAt(
			std::weak_ptr<CEB> src,
			QPoint pC,
			std::weak_ptr<Plug> pl
	);
	void ConnectedTo_C(
			std::weak_ptr<CEB> ceS,
			std::weak_ptr<Plug> pl,
			std::weak_ptr<CEB> ceT, 
			std::weak_ptr<Connector> conn
	);
	/// jel a CE nézet-párjának, hogy törlődjön:
	void destroyView_CEW();
public slots:
	/// r-t a Canvas coordrendszerében várjuk
	void CEW_setGeometry(QRect r);
	virtual void connectorTransformed(std::weak_ptr<Connector> src, QSize newSize);
	
	virtual void connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr);
	virtual void connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction prevDir, Direction newDir);
	virtual void connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea);

};
#endif // CANVASELEMENTBASE_H
