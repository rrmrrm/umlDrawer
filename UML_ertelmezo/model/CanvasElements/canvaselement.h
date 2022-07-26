#ifndef UMLCOMPONENT_H
#define UMLCOMPONENT_H

#include "shape_drawing/drawing.hpp"
#include "canvaselementtypes.h"
#include "canvaselementbase.h"
#include "model/target.h"
#include "model/myqline.h"

class LineComponent : 
		public CEB
		//public virtual IHasAggregate
{
	Q_OBJECT
private:
	//a vonalszegmensek mozgatásához szükséges változók:
	std::list<MyQLine>::iterator draggingLineIt;
	//MyQLine* draggedLineInitialState;
	LinePart draggedLinePart = MIDDLE;
public:
	int margin = 14;
	// törtvonalak vektora:
	// a koordináták a Canvas rendszerében lesznek megadva, nem a LineWidget-ében
	std::list<MyQLine> lineSegments;
	Target start;
	Target end;
	std::shared_ptr<Plug> pl1;
	std::shared_ptr<Plug> pl2;
	
	std::shared_ptr<Connector> connRel1;
	std::shared_ptr<Connector> connRel2;
	std::shared_ptr<Connector> connTB1;
	std::shared_ptr<Connector> connTB2;
	
public:
	LineComponent(QRect geometry_);
	LineComponent(
			QRect geometry_, 
			Target start_, 
			Target end_
		);
	virtual ~LineComponent();
	
private:
	void addLineSegment(MyQLine&& l);
	void addLineSegmentToFront(MyQLine&& l);
protected:
	//std::ostream& save_chainCallThis(std::ostream& os);
	//std::istream& load_chainCallThis(std::istream& is);
	//std::ostream& saveAggsChainCallThis(std::ostream&);
	//std::istream& loadAggsFinalizeChainCallThis(std::istream&);
public:
	virtual std::weak_ptr<Plug> getMainPlug() const override;
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	virtual std::ostream& saveAggregations(std::ostream&) override;
	virtual std::istream& loadAggregations(std::istream&) override;
	
	//virtual std::vector<AggregationT<Connector>> getConnectors() override;
	//Direction getAttachedSide(const Target& target) const;
	
	//megadja, hogy p0-hoz képest p1 (strict=true esetén szigrúan) dir irányban van-e
	// (strict: ha strict igaz és egyezik a dir irány menti tengely menti koordinátájuk, akkor hamisat ad vissza)
	bool isInDirection(QPoint p0, Direction dir, QPoint p1, bool strict) const;
	
	bool isWidgetInWay(const CEB* w,
					   Direction dir,
					   QPoint p,
					   QPoint bound,
					   int margin,//a widgetet ennyivel szélesebbnek tekintjük az ütközés detektálásakor
					   QPoint* OUTcollosionPoint = nullptr);//widgetnek ütközés pontja(a marginnal eltolva). akkor állítjuk csak be ha a visszatérési érték igaz(tehát egy widget útban van)
	
	void moveLines(const std::list<MyQLine>::iterator& lineIt, const QPoint& newPlace, LinePart lp);
	//extendInDirIfcsak akkor mozgatja el a pontot, ha extensionBound pont lineEndToUpdate-hez képes a dir által jelölt félsíkban van
	void extendInDirIf(QPoint& INOUTlineEnd, Direction dir, QPoint extensionBound);
	//QPoint moveInDim(QPoint INOUTlineEnd, Dimension dim, QPoint extensionGoal);
	
	// rotate90ToPoint a két lehetséges merőleges irány közül arra forgatja lineDir, amelyik p0 ból olyan azon félsik felé  mutat, ami tartalmazza p1-et:
	void rotate90ToPoint(Direction& line1Dir, QPoint p0, QPoint p1);
	//csak a dir irányú koordinátát tölti fel w marginnal kitolt dir által kiválasztott oldalának koordinátájával. a másik koordinátát 0-ra állítja:
	QPoint getWidgetAttPoint(Direction dir, const CEB* w, int margin) const;
	QPoint getWidgetAttPoint(const Target& t) const;
	QPoint adjustCoordInDimension(QPoint p, Dimension dim, QPoint newCoord);
	void attachToElement(const std::list<MyQLine>::iterator& l, LinePart part, std::weak_ptr<Connector> conn, const Target& t);
	void reconnect();
	void recreateLineSegments();
	void adjustMaskAndGeometry();
	void mergeLinesWhenNeeded();
	virtual void dragUnselectedOverrideThis(int canvas_x, int canvas_y) override;
	virtual void dragSelected(int canvas_x, int canvas_y) override;
	//virtual std::weak_ptr<Connector> tryConnToThis(std::weak_ptr<CEB> w, QPoint pOnCanv, std::weak_ptr<Plug> plug) override;
	virtual void stopDragOverrideThis(QPoint cPos) override;
	// ha OUTlinedivisionRatio nem nullptr és valamelyik vonal közelében van (cx,cy), 
	//  akkor visszaadjuk, hogy a vonal hányadánál osztja (cx,cy) (ha c= line.start, akkor 0-t lesz OUTlinedivisionRatio)
	// OUTlinedivisionRatio kívül is lehet a [0,1] intervalumon, mert elég ha (cx,cy) margin távolságra van a szakasztól ahoz, hogy elfogadjuk.
	std::list<MyQLine>::iterator getLineItAtCanvasCoords(int cx, int cy, double* OUTlineDivisionRatio);
	//virtual std::weak_ptr<Connector> getConnFirstAtCanvasCoords(QPoint) override;
	void connectionAllowed(std::weak_ptr<CEB> srcOwner, std::weak_ptr<Connector> connSrc, QPoint p, std::weak_ptr<Plug> plug) override;
	virtual void copyFrom(const CEB* o) override;
signals:
	void linesChanged_CEW();
	//void createTBox_CEW(const MyQLine&);
private slots:
	/// a LineComponent a ctorában használja a tryConnToThis virtuális felülírt függvényt. ezt úgy lehet, hogy a ctro-ból QTimer-el késleltetve hívom meg az init-et(ezáltal az örököklt virtuális függvények már felülíródnak mire az init végrehajtódik)
	void init();
public slots:
	virtual void connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr) override;
	virtual void connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction prevDir, Direction newDir) override;
	virtual void connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea) override;
};

class TextComponent : public CEB{
	Q_OBJECT
public:
	QString text;
	std::shared_ptr<Plug> pl;
	double letterSpacing = 120;
	double charSize = 20;
	
public:
	TextComponent(QRect geometry_);
	virtual ~TextComponent();
	
public:
	virtual std::weak_ptr<Plug> getMainPlug() const override;
	void CEW_textChanged(QString newt);
	void connectionAllowed(std::weak_ptr<CEB> ceT, 
						   std::weak_ptr<Connector> conn, 
						   QPoint p, 
						   std::weak_ptr<Plug> plug) override;
	virtual void copyFrom(const CEB* o) override;
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	virtual std::ostream& saveAggregations(std::ostream&) override;
	virtual std::istream& loadAggregations(std::istream&) override;
};

///class UMLComponentBase :public CEB
///{
///protected:
///	std::shared_ptr<Plug> pl;
///	UMLComponentBase(
///				CanvasElement_Type type_,
///				QRect geometry_ );
///public:
///	virtual ~UMLComponentBase(){};
///	
///public:
///	virtual std::weak_ptr<Plug> getMainPlug() const override;
///	virtual std::shared_ptr<CEB> modellCopy(std::shared_ptr<CEB> fromDerived = nullptr) const override;
///	virtual void copyFrom(const CEB* o) override;
///};


class UMLClassComponent : public CEB{
	//geometry középpontjához van megadva ezen connector-ok területe:
	std::shared_ptr<Connector> connTextTitle;
	std::shared_ptr<Connector> connTextFields;
	std::shared_ptr<Connector> connTextMethods;
	int textHeight = 17;
	std::shared_ptr<Connector> connLeft;
	std::shared_ptr<Connector> connRight;
	std::shared_ptr<Connector> connTop;
	std::shared_ptr<Connector> connBottom;
public:
	UMLClassComponent(QRect geometry_
					  );
	virtual ~UMLClassComponent(){}
	
protected:	
	std::shared_ptr<Plug> pl;
	//std::ostream& saveAggsChainCallThis(std::ostream&);
	//std::istream& loadAggsFinalizeChainCallThis(std::istream&);
	//std::ostream& save_chainCallThis(std::ostream&);
	//std::istream& load_chainCallThis(std::istream&);
public:
	virtual std::weak_ptr<Plug> getMainPlug() const override;
	std::ostream& saveAggregations(std::ostream&) override;
	std::istream& loadAggregations(std::istream&) override;
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	std::weak_ptr<Connector> getConnTitle() const;
	std::weak_ptr<Connector> getConnFields() const;
	std::weak_ptr<Connector> getConnMethods() const;
	
	std::weak_ptr<Connector> getConnLeft() const;
	std::weak_ptr<Connector> getConnRight() const;
	std::weak_ptr<Connector> getConnTop() const;
	std::weak_ptr<Connector> getConnBottom() const;
	
	virtual void copyFrom(const CEB* o) override;
	//virtual std::weak_ptr<Connector> tryConnToThis(std::weak_ptr<CEB> srcOwner, QPoint pOnCanv, std::weak_ptr<Plug> plug) override;
	
};
class UMLRelationship :public CEB  {
	Q_OBJECT
private:
	Direction dir;
	int length = 30;
	int thickness = 15;
protected:
	std::shared_ptr<Plug> pl;
public:
	const Relationship_Type rtype;
	
public:
	UMLRelationship(Relationship_Type type_, QRect geometry_);
	virtual ~UMLRelationship(){}
	 
public:
	virtual std::weak_ptr<Plug> getMainPlug() const override;
	Relationship_Type getRtype() const;
	Direction getDir() const;
	//void adjustGeometryToDir();
	virtual std::shared_ptr<CEB> modellCopy(std::shared_ptr<CEB> fromDerived = nullptr) const override;
	virtual void copyFrom(const CEB* o) override;
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	virtual std::ostream& saveAggregations(std::ostream&) override;
	virtual std::istream& loadAggregations(std::istream&) override;
public slots:
	void connectionAllowed(std::weak_ptr<CEB> connectorOwner, std::weak_ptr<Connector> connSrc, QPoint p, std::weak_ptr<Plug> plug) override;
	virtual void connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint prevTr, QPoint newTr) override;
	virtual void connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction prevDir, Direction newDir) override;
	virtual void connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea) override;
};

class UMLAggregationSpecifier : public UMLRelationship{
	Q_OBJECT
private:
public:
	UMLAggregationSpecifier(QRect geometry_
							) 
		:
		  UMLRelationship(
			  Relationship_Type::AGGREGATION,
			  geometry_
						   ){}
	virtual ~UMLAggregationSpecifier(){}
};

class UMLCompositionSpecifier : public UMLRelationship{
	Q_OBJECT
private:
public:
	UMLCompositionSpecifier(QRect geometry_
							) 
		:
		  UMLRelationship(
			  Relationship_Type::COMPOSITION,
			  geometry_
						   ){}
	virtual ~UMLCompositionSpecifier(){}
};

class UMLExtensionSpecifier : public UMLRelationship{
	Q_OBJECT
private:
public:
	
		
	UMLExtensionSpecifier(QRect geometry_
						  ) 
		:
		  UMLRelationship(
			  Relationship_Type::EXTENSION,
			  geometry_
						   ){}
	virtual ~UMLExtensionSpecifier(){}
};

class UMLCircle : public UMLRelationship{
	Q_OBJECT
private:
public:
	UMLCircle(QRect geometry_
			  ) 
		:
		  UMLRelationship(
			  Relationship_Type::CIRCLE,
			  geometry_
						   ){}
	virtual ~UMLCircle(){}
};

#endif // UMLCOMPONENT_H
