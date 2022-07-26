#ifndef TARGET_H
#define TARGET_H

#include "CanvasElements/canvaselementbase.h"

#include <QMouseEvent>
///TODO: megváltoztattam Target-et, hogy legyen oly konstruktora, amiben Canvashoz relatíve adjuk meg akoordinátákat. tesztelni

///TODO: eldönteni, hogy kell-emég ez az osztály CanvasEvent mellett. a kettő feladaata kb uzgyanaz. 

///A Canvas beli elemekre kattintás-esemény  paramétereinek összefogására kell a Target osztály
struct Target//: public virtual IHasAggregate
{
public:
	int x_inTarget = 0;
	int y_inTarget = 0;
	int x_inCanvasOriginal = 0;
	int y_inCanvasOriginal = 0;
	Myweak_ptr<CEB> canvasElement;
	
public:
	
	///ctor, amiben a koordinátákat a Canvashoz relatíve várjuk attól függetlenül,
	///  hogy canvasElement_ nullptr-e.
	/// (cXcY_relToCanvasMarker) értéke nem számít,
	///  csak meg kell adni, ha ezt a konstruktort akarjuk meghívni:
	Target(int cX, int cY,
			Myweak_ptr<CEB> canvasElement_,
			int cXcY_relToCanvasMarker
			)
	{
		(void)cXcY_relToCanvasMarker;
		canvasElement = canvasElement_;
		
		if(auto ceO = canvasElement.lock()){
			x_inTarget = cX - ceO->x();
			y_inTarget = cY - ceO->y();
			x_inCanvasOriginal = cX;
			y_inCanvasOriginal = cY;
		} 
		else{
			x_inTarget = cX;
			y_inTarget = cY;
			x_inCanvasOriginal = cX;
			y_inCanvasOriginal = cY;
		}
	}
	/// az mint az előző, csak a koordinátát QPoint-ben veszi át
	Target(QPoint cCoords, Myweak_ptr<CEB> canvasElement_, int cXcY_relToCanvasMarker)
		:	Target(cCoords.x(), cCoords.y(), canvasElement_, cXcY_relToCanvasMarker)
	{}
	
	/// ctor, amiben a koordinátákat a canvasElement_-hez relatíven várjuk. 
	/// canvasElement_ lehet nullptr is. 
	/// ebben az esetben viszont a Canvas-hoz relatívak a koordináták
	Target(int x_inTarget_, int y_inTarget_, Myweak_ptr<CEB> canvasElement_)
		:	
			x_inTarget(x_inTarget_),
			y_inTarget(y_inTarget_),
			canvasElement(canvasElement_)
	{
		if(auto observedCebOwner = canvasElement.lock()){
			x_inCanvasOriginal = x_inTarget_ + observedCebOwner.get()->geometry.x();
			y_inCanvasOriginal = y_inTarget_ + observedCebOwner.get()->geometry.y();
		} 
		else{
			x_inCanvasOriginal = x_inTarget_;
			y_inCanvasOriginal = y_inTarget_;
		}
	}
	/// az mint az előző, csak a koordinátát QPoint-ben veszi át
	Target(QPoint cCoords, Myweak_ptr<CEB> canvasElement_)
		:	Target(cCoords.x(), cCoords.y(), canvasElement_)
	{}
	Target()
		:	
		  Target(0,0, Myweak_ptr<CEB>())
	{}
	
public:
	//std::ostream& saveAggregations(std::ostream& os) override{
	//	saveAggsChainCallThis(os);
	//	return os;
	//}
	//std::istream& loadAggregations(std::istream& is) override{
	//	loadAggsFinalizeChainCallThis(is);
	//	return is;
	//}
	std::ostream& saveAggregations(std::ostream& os){
		canvasElement.save(os);
		return os;
	}
	std::istream& loadAggregations(std::istream& is){
		canvasElement.load(is);
		return is;
	}
	//ha a Target-elt EntityWidgetet elmozgatják, akkor a getFreshXInCanvas más értéket fog adni.
	// mert az EntityWidget megváltozott koordinátájához egyszerűen hozzáadja originalLocalX-et
	// a Target konstruálásakor rögzített EWidgeten belüli koordinátát.
	int getFreshXInCanvas() const{
		if(auto observedCebOwner = canvasElement.lock()){
			assert(observedCebOwner.get()!= nullptr);
			return observedCebOwner.get()->geometry.x() + x_inTarget;
		}
		else{
			return x_inTarget;
		}
	}
	//getFreshYInCanvas-ra is vonatkoznak a getXYInCanvas kommentjei
	int getFreshYInCanvas() const{
		if(auto observedCebOwner = canvasElement.lock()){
			assert(observedCebOwner.get() != nullptr);
			return observedCebOwner.get()->geometry.y() + y_inTarget;
		}
		else{
			return y_inTarget;
		}
	}
	QPoint getPoint_FreshInCanvas() const {
		return QPoint(getFreshXInCanvas(),getFreshYInCanvas());
	}
	QPoint getCPoint() const {
		return QPoint(x_inCanvasOriginal, y_inCanvasOriginal);
	}
	QPoint getTPoint() const {
		return QPoint(x_inTarget, y_inTarget);
	}
	
	///
	/// \brief isInCe
	/// ha canvasElement üres, akkor azt mondjuk,
	///  hogy a Canvas-ra vonatkozik a Target, amúgy pedig egy CE-re:
	/// \return 
	///
	bool isInCE(){
		return canvasElement.lock() != nullptr;
	}
	std::ostream& save(std::ostream& os){
		os << x_inTarget << std::endl;
		os << y_inTarget << std::endl;
		os << x_inCanvasOriginal << std::endl;
		os << y_inCanvasOriginal << std::endl;
		return os;
	};
	std::istream& load(std::istream& is){
		is >> x_inTarget;
		is >> y_inTarget;
		is >> x_inCanvasOriginal;
		is >> y_inCanvasOriginal;
		return is;
	};
};
#endif // TARGET_H
