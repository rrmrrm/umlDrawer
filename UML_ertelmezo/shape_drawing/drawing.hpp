#ifndef DRAWING_HPP
#define DRAWING_HPP

#include "functions.hpp"
#include "shape_drawing/gesture.h"
#include "common.hpp"
#include "persistence/streamoperators.h"

#include <QObject>
#include <QTime>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QRgb>
#include <QString>//uint
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
    #include <QRandomGenerator>
#else
    #include <qrandomgenerator.h>
#endif

#include <iostream>
#include <vector>
#include <limits.h>
extern bool readAndSaveGestureData;
class Model;
class Drawing{
	
	friend Model;
protected:
	bool visible = true;
	std::vector<Gesture> gestures;
	///indexing of cells: cells[col][row]
	///az ábra képpont-cellái. a programban a képernyő pixeleiből ezen cellákba konvertáljuk az ábrákat,
	/// ennek megfelelően a továbbiakban: a pixel a képernyő egy képpontját, a cella pedig a már átkonvertált ábra egy pontját jelenti .
	std::vector< std::vector<double> > cells;
	QPixmap cellsPm;
	QImage cellsImg;
	///TODO: 0 elszórással hibás a program, kijavítani
	//double standDeviat = 1;
	
	//jelzi, hogy meghivtak-e mar ay update() fuggvenyt,
	//tehat a cellak be lettek-e allitva:
	bool isCellsSetupBool = false;
	/// number of cell columns in the Drawing:
	double resolutionHorizontal = 0;
	/// number of cells in each column in the Drawing:
	double resolutionVertical = 0;
	int marginInPixels = 0;
	///TODO: margókat nem használni. ha mégis kell, akkor skálázást használni helyette esetleg.
	int marginLeft = 0;
	int marginTop = 0;
	int marginRight = 0;
	int marginBottom = 0;
	int borderLeft = 0;
	int borderTop = 0;
	int borderRight = 0;
	int borderBottom = 0;
	int maxX = 0;
	int maxY = 0;
	int minX = 0;
	int minY = 0;
	bool isBoundsSetup_ = false;
	
	/// \brief drawPointAdditive
	/// increases (color-)value of a cell
	/// \param cellX
	/// \param cellY
	/// \param value
	///
	void drawPointAdditive(uint cellX, uint cellY, double value){
		cells[cellX][cellY] += value;
	}
public:
	Drawing(){}
	Drawing(double _standDeviat, int _marginInPixels):
		//standDeviat(_standDeviat),
		//gb(_standDeviat),
		marginInPixels(_marginInPixels)
	{}
	
	Drawing(double _standDeviat, int _marginInPixels, double _resolutionHorizontal,  double _resolutionVertical):
		//standDeviat(_standDeviat),
		//gb(_standDeviat),
		resolutionHorizontal(_resolutionHorizontal), 
		resolutionVertical(_resolutionVertical),
		marginInPixels(_marginInPixels)
	{
		
	}
	Drawing(const Drawing& d) :
		gestures(d.gestures),
		cells(d.cells),
		//standDeviat ( d.standDeviat),
		//gb ( d.gb),
		
		isCellsSetupBool(d.isCellsSetupBool),
		resolutionHorizontal(d.resolutionHorizontal),
		resolutionVertical(d.resolutionVertical),

		marginInPixels ( d.marginInPixels),
		marginLeft ( d.marginLeft),
		marginTop ( d.marginTop),
		marginRight ( d.marginRight),
		marginBottom ( d.marginBottom),
		borderLeft ( d.borderLeft),
		borderTop ( d.borderTop),
		borderRight ( d.borderRight),
		borderBottom ( d.borderBottom),
		maxX(d.maxX),
		maxY(d.maxY),
		minX(d.minX),
		minY(d.minY),
		isBoundsSetup_(d.isBoundsSetup_)
	{}
	virtual ~Drawing(){
	}
	
	///
	/// \brief createDrawingFromFile
	/// \param is
	/// \param useGestureDataToo
	/// \param newResHor
	///		ha különbözik a beolvasott resHorizontal értéktől, akkor a gesture-ökből újraelkészítjük a Cells-t  newResHor szélessgéggel
	/// \param newResVert
	///		ha különbözik a beolvasott resVertical értéktől, akkor a gesture-ökből újraelkészítjük a Cells-t  newResVert magassággal
	/// \param gestureDataIsInFile
	/// \return 
	///
	static Drawing* createDrawingFromFile(std::istream& is, int newResHor, int newResVert, bool useGestureDataToo, bool gestureDataIsInFile){
		if( !gestureDataIsInFile && useGestureDataToo){
			assert(!"ERROR: DrawingFactory::createDrawingFromFile(..): gestureDataIsInFile cannot be set to false if useGestureDataToo is set to true!");
		}
		Drawing* d = new Drawing();
		if(d->load(is, newResHor, newResVert, useGestureDataToo, gestureDataIsInFile)){
			return d;		
		}
		else{
			delete d;
			return nullptr;
		}
	
	}
	virtual int getDrawingLeft() const{
		assert(isBoundsSetup_);
		return marginLeft;
	}
	virtual int getDrawingRight() const{
		assert(isBoundsSetup_);
		return marginRight;
	}
	virtual int getDrawingTop() const{
		assert(isBoundsSetup_);
		return marginTop;
	}
	virtual int getDrawingBot() const{
		assert(isBoundsSetup_);
		return marginBottom;
	}
	
	virtual double getCellWidth() const{
		assert(isBoundsSetup_);
		assert(getCells().size() > 0);
		return (getDrawingRight() - getDrawingLeft()) /(double)(getCells().size());
	}
	const std::vector<std::vector<double>>& getCells() const{
		return cells;
	} 
	std::vector<std::vector<double>>& getCells() {
		return cells;
	} 
	QImage& accessImage() {
		assert(isCellsSetup());
		return cellsImg;
	} 
	virtual double getCellHeight() const{
		//ha nincsenek sorai, akkor is visszakell valamit adni:
		if(getCells().size() == 0){
			return 1.0;
		}
		
		return (getDrawingBot() - getDrawingTop()) /(double)(getCells()[0].size());
	}
	QRect getBoundingRect() const{
		assert(isBoundsSetup_);
		return QRect(QPoint(getDrawingLeft(), getDrawingTop()),
					 QPoint(getDrawingRight(), getDrawingBot()));
	}
	uint getResolutionVertical() const{
		return resolutionVertical;
	}
	double& getResolutionVertical(){
		return resolutionVertical;
	}
	uint getResolutionHorizontal() const{
		return resolutionHorizontal;
	} 
	double& getResolutionHorizontal(){
		return resolutionHorizontal;
	} 
	
	int getMinX() const{
		assert(isBoundsSetup_);
		return minX;
	}
	int getMaxX() const{
		assert(isBoundsSetup_);
		return maxX;
	}
	int getMinY() const{
		assert(isBoundsSetup_);
		return minY;
	}
	int getMaxY() const{
		assert(isBoundsSetup_);
		return maxY;
	}
	//int getSpreadRange() const{
	//	return 4*standDeviat;
	//}
	//double getStandDeviat() const{
	//	return standDeviat;
	//}
	//double& getStandDeviat() {
	//	return standDeviat;
	//}
	int getMarginInPixels() const{
		return marginInPixels;
	}
	int& getMarginInPixels(){
		return marginInPixels;
	}
	void setVisible(bool v){
		visible = v;	
	}
	bool isVisible() const{
		return visible;
	}
	bool isBoundsSetup() const{
		return isBoundsSetup_;
	}
	bool& isBoundsSetup(){
		return isBoundsSetup_;
	}
	bool isCellsSetup() const{
		return isCellsSetupBool;
	}
	//bool& isCellsSetup(){
	//	return isCellsSetupBool;
	//}
	const std::vector<Gesture>&  getGestures() const {
		return gestures;
	}
	std::vector<Gesture>&  accessGestures() {
		return gestures;
	}
	void addGesture(Gesture g){
		gestures.push_back(g);
		updateBounds(g);
		
	}
	void addGesturePoint(int x, int y){
		// csak akkor szabad új pontot hozzáadni a Drawinghoz, ha már van egy Gesture hozzáadva:
		assert(gestures.size() > 0);
		gestures[gestures.size()-1].addPoint(x,y);
		updateBounds(x,y);
	}
	
	// updateBounds_-ot hamisra, akkor nem frissítjük a min,max bounds margin szélsőértékeket.
	// viszont ekkor elvaultak lesznek ezek a szélsőértékek mindaddíg, amig meg nem hívjuk az UpdateBounds() eljárást.
	// tehát: ha valahonnan updateBounds_ =false-al hívjuk meg ezt az eljárást, akkor majd OTT meg kell hívni az updateBounds()-ot is.
	void setGesturePointAt(unsigned int ind, int x, int y, bool updateBounds_ = true){
		(void) updateBounds_;
		int flatIndTarget = ind;
		int flatIndCurr = 0;
		uint gInd = 0;
		///invariáns tul: flatIndCurr <= flatIndTarget
		while(gInd < gestures.size() && flatIndCurr <= flatIndTarget){
			///flatIndTarget-flatIndCurr itt pozitív
			uint diff = static_cast<uint>(flatIndTarget-flatIndCurr);
			if(diff < gestures[gInd].getPoints().size()){
				int localInd = flatIndTarget-flatIndCurr;
				/// itt localInd < gest[gInd].size(), ezért gest[gInd].getPoints()[localInd] már a keresett koordinátapárt jelzi.
				gestures[gInd].setPointAt(localInd, x, y);
				updateBounds(x,y);
				return;
			}
			else{
				flatIndCurr += gestures[gInd].getPoints().size();
			}
			++gInd;
		}
		assert(!"ERROR: void Drawing::setGesturePointAt(uint ind, int x, int y): _ind is out of bounds");
	}
	virtual Drawing* clone() const{
		return new Drawing(*this);
	}
	///
	/// \brief CopyModifyDrawing
	///a Canvas projectben a Widget-beli produceDatasFromDir függvény használja,
	/// hogy egy mappában levő drawing fileokból kicsit módosított és több Drawing-ot tartalmazó drawing datafileokat csináljon
	///a bool useGestureDataToo és bool gestureDataIsInFile paramétereket azért nem kell megadni ennek a metódusnak,
	/// mert feltételezzük, hogy van Gesture data a fileban,
	/// ugyanis a Gesture pontok alapján készítjük el a módosított Drawing-ot.
	/// \param drawing
	/// ezt a drawing-ot másolja le, és módosítja a metódus. nem lehet nullptr
	/// \param pointModificationRate
	/// a Gesture pontjai ilyen arányban lesznek módosítva(0- egyik pont se, 1- az összes pont módosul)
	/// \param range
	/// ekkora sugarú körben tolódhatnak el a módosított pontok
	/// 
	/// ugy hogy egyenkent a drawing gestur-einek minden pontja körül 'pointModificationRate' esellyel 
	/// történik modositas(nevezzünk egy módosításra kijelölt pontot "csúcsnak"),
	///  és a körülötte 'maxRangeRatio' sugaron belül lévő pontokkal együtt 
	/// normaleloszlas függvény 
	/// szerint veletlen(de az aktuális csúcsra tekintve állandó) iranyba eltolja a pontokat.
	/// Minden pontot sorraveszünk, és a a pontok 'pointModificationRate' eséllyel lesznek "csúcs"-nak választva.
	/// Ezután függőleges és vízszintes irányba skálázzuk a képet véletlenszerű mértékben.
	/// \param drawing
	/// \param pointModificationRate
	/// \param maxRangeRatio
	/// \param minDeviation
	/// \param maxDeviation
	/// \param maxAmplitudeMultiplier
	/// \param minScaleX
	/// \param maxScaleX
	/// \param minScaleY
	/// \param maxScaleY
	/// \param scaleChance
	/// \return 
	///
	Drawing* copyModifyDrawing(double pointModificationRate, double minRangeRatio,  double maxRangeRatio, double minDeviation, double maxDeviation, double maxAmplitudeMultiplier, double minScaleX, double maxScaleX, double minScaleY, double maxScaleY, double scaleChance ) const{
		//assert(drawing != nullptr);
		Drawing* d = clone();
		double boundingSize = std::max(getMaxX() - getMinX(), getMaxY() - getMinY());
		///görbe módosítások:
		QRandomGenerator r(QTime::currentTime().msec());
		
		for(int i = 0 ; i < (int)d->getGesturePointsNum() ; ++i){
			if( r.generateDouble() < pointModificationRate){
				double angle = r.generateDouble() * 2.0 * 3.14159265;
				int range = (
								minRangeRatio 
								+ r.generateDouble() * (maxRangeRatio - minRangeRatio)
							) 
							* d->getGesturePointsNum() / 2.0;
				///a (1.0-r.generateDouble()) kifejezés miatt a standDeviation (0,maxStandDeviation] intervalumban lesz,
				/// ez a zért kell, mert standDeviationnel osztani fog a program a GaussBlur1D-nek az f(..) függvényében.
				double standDeviation = minDeviation + (1.0-r.generateDouble()) * (maxDeviation-minDeviation);
				double amplitudeMult= r.generateDouble() * maxAmplitudeMultiplier * boundingSize;
				GaussBlur1D gB(standDeviation, amplitudeMult);
				for (int j = std::max(0, i-range) ; j < std::min(i+range, (int)d->getGesturePointsNum()-1) ; ++j) {
					double dist = gB.f(j-i);
					int xOriginal = d->gesturePointAt(j).first;
					int yOriginal = d->gesturePointAt(j).second;
					d->setGesturePointAt(j, xOriginal + qCos(angle) * dist, yOriginal + qSin(angle) * dist, false);
				}
			}
		}
		/// frissiteni kell a drawing szélső értékeit mielott tovabb modositanank, mert setGesturePointAt(...,..., false) elavulttá tezi a minmax bounds, margin szélsőértékeket, 
		///  és a cellák se azt fogják ábrázolni ami a gesture-ökben van, ezert szólni kell a drawing-nak, hogy újra készítse ell a cellatáblát - vagyis az ábrát - a gestureinek a pontjaiból:
		d->updateCells(true);
		///skálázás:
		
		if(r.generateDouble() < scaleChance ){
			double scaleX = minScaleX + r.generateDouble()*(maxScaleX-minScaleX);
			double scaleY = minScaleY + r.generateDouble()*(maxScaleY-minScaleY); 
			int leftBorder = d->getMinX();
			int upperBorder = d->getMinY();
			for (uint i = 0; i < d->getGesturePointsNum(); ++i) {
				int pX = d->gesturePointAt(i).first;
				int pY = d->gesturePointAt(i).second;
				int dX = pX - leftBorder;
				int dY = pY - upperBorder;
				d->setGesturePointAt(i, leftBorder + dX * scaleX, upperBorder + dY * scaleY);
			}
		}
		
		/// ld.: előző komment
		d->updateCells(true);
		return d;
	}
	
	
protected:
	//TODO:csak akkor stimmmelnek a meretaranyok, ha ugyanannyi sor es oszlop van
	// kijavitani, itt is es Drawing-ban is hogy ne egy valamekkora negyzetre kepezzuk le a pixeleket,
	// hanem egy -az oszlop-sor aranynak megfelelo- teglalapra
	
	///a gestures alapján beállítja a gestures-re vonatkozó szélsőértékeket(border..., margin...)
	void updateBounds(){
		if(gestures.size() == 0)
			return;
		minX = std::numeric_limits<int>().max();
		int cmp;
		for (uint i = 0; i < gestures.size(); ++i) {
			if(gestures[i].getMinX(&cmp) && cmp < minX){
				minX = cmp;
				isBoundsSetup_ = true;
			}
		}
		maxX = std::numeric_limits<int>().lowest();
		for (uint i = 0; i < gestures.size(); ++i) {
			if(gestures[i].getMaxX(&cmp) && cmp > maxX){
				maxX = cmp;
				isBoundsSetup_ = true;
			}
		}
		minY = std::numeric_limits<int>().max();
		for (uint i = 0; i < gestures.size(); ++i) {
			if(gestures[i].getMinY(&cmp) && cmp < minY){
				minY = cmp;
				isBoundsSetup_ = true;
			}
		}
		maxY = std::numeric_limits<int>().lowest();
		for (uint i = 0; i < gestures.size(); ++i) {
			if(gestures[i].getMaxY(&cmp) && cmp > maxY){
				maxY = cmp;
				isBoundsSetup_ = true;
			}
		}
		// ha semelyik gesture-ben se voltak beállítva a minmax X és Y értékek,
		//  akkor nincs értelme a többi korlát-értéket beállítani:
		if(!isBoundsSetup_){
			return;
		}
	
		borderLeft = minX;
		borderTop = minY;
		
		int innerWidth = maxX - borderLeft;
		int innerHeight = maxY - borderTop; 
		
		borderRight = borderLeft + innerWidth;
		borderBottom = borderTop + innerHeight;
		assert(maxX >= minX);
		assert(maxY >= minY);
		
		///margók:
		int margin = marginInPixels;
		marginLeft = borderLeft - margin;
		marginTop = borderTop - margin;
		marginRight = borderRight + margin;
		marginBottom = borderBottom + margin;
	}
	void updateBounds(int newX, int newY){
		if(isBoundsSetup_){
			maxX = maxX<newX ? newX : maxX;
			maxY = maxY<newY ? newY : maxY;
			minX = minX>newX ? newX : minX;
			minY = minY>newY ? newY : minY;
		}
		else{
			maxX = newX;
			maxY = newY;
			minX = newX;
			minY = newY;
		}
		borderLeft = minX;
		borderTop = minY;
		
		int innerWidth = maxX - borderLeft;
		int innerHeight = maxY - borderTop; 
		
		borderRight = borderLeft + innerWidth;
		borderBottom = borderTop + innerHeight;
		assert(maxX >= minX);
		assert(maxY >= minY);
		
		///margók:
		int margin = marginInPixels;
		marginLeft = borderLeft - margin;
		marginTop = borderTop - margin;
		marginRight = borderRight + margin;
		marginBottom = borderBottom + margin;
		isBoundsSetup_ = true;
	}
	void updateBounds(const Gesture& newG){
		int gMinX;
		int gMinY;
		int gMaxX;
		int gMaxY;
		//ha az ujGesture-ben nincs beállítva a min és max X és Y értékek, akkor nem kell semmit változtatni:
		if(!newG.getMaxX(&gMaxX) 
				|| !newG.getMaxY(&gMaxY) 
				|| !newG.getMinX(&gMinX) 
				|| !newG.getMinY(&gMinY) ){
			return;
		}
		// ha nincsenek beállítva a Drawing határai és szélsőértékei,
		//  akkor az új Gestrue-től egyszerűen átvesszük azokat:
		if(!isBoundsSetup_){
			maxX = gMaxX;
			maxY = gMaxY;
			minX = gMinX;
			minY = gMinY;
		}
		else{
			//amúgy össze kell hasonlítani az eddigi szálsőértékeket az újakkal:
			maxX = getMaxX() < gMaxX ? gMaxX : maxX;
			maxY = getMaxY() < gMaxY ? gMaxY : maxY;
			minX = getMinX() > gMinX ? gMinX : minX;
			minY = getMinY() > gMinY ? gMinY : minY;
		}
		borderLeft = minX;
		borderTop = minY;
		
		int innerWidth = maxX - borderLeft;
		int innerHeight = maxY - borderTop; 
		
		borderRight = borderLeft + innerWidth;
		borderBottom = borderTop + innerHeight;
		assert(maxX >= minX);
		assert(maxY >= minY);
		
		///margók:
		int margin = marginInPixels;
		marginLeft = borderLeft - margin;
		marginTop = borderTop - margin;
		marginRight = borderRight + margin;
		marginBottom = borderBottom + margin;
		isBoundsSetup_ = true;
	}
	
	//virtual uint calc_drawingCol_from_x(int pixelX) const{
	//	assert(0 != resolutionHorizontal);
	//	
	//	//csak akkor van ennek az eljárásnak értelme, ha a szélsőértékek be vannak már állítva:
	//	assert(isBoundsSetup_);
	//	if( marginRight > marginLeft && resolutionHorizontal > 1){
	//		double div = marginRight-marginLeft;
	//		if(marginBottom - marginTop > div)
	//			div = marginBottom - marginTop;
	//		double ret = (resolutionHorizontal-1) * (pixelX - marginLeft) / div + 0.5;
	//		return ret;//egeszre kerekites
	//	}
	//	else{
	//		///here marginRight == marginLeft OR resolutionHorizontal = 1
	//		if(marginRight == marginLeft && resolutionHorizontal >1 ){
	//			return 0;//nem kotelezo itt 0-t visszaadni, lehetne pl resolutionHorizontal/2 is
	//		}
	//		else{
	//			///here marginRight = marginLeft AND resolutionHorizontal = 1, OR
	//			/// OR  marginRight > marginLeft  AND resolutionHorizontal = 1,
	//			/// so resolutionHorizontal is 1 here
	//			return 0;
	//		}
	//	}
	//}
	//
	//virtual uint calc_drawingRow_from_y(int pixelY) const{
	//	assert(0 != resolutionVertical);
	//	//csak akkor van ennek az eljárásnak értelme, ha a szélsőértékek be vannak már állítva:
	//	assert(isBoundsSetup_);
	//	if(marginBottom > marginTop && resolutionVertical > 1){
	//		double div = marginRight-marginLeft;
	//		if(marginBottom - marginTop > div)
	//			div = marginBottom - marginTop;
	//		double ret = (resolutionVertical-1) * (pixelY - marginTop) / (double)(div) + 0.5;
	//		return ret;//egeszre kerekites
	//	}
	//	else{
	//		///here marginBottom == marginTop OR resolutionVertical = 1
	//		if(marginBottom == marginTop && resolutionVertical >1 ){
	//			return 0;//nem kotelezo itt 0-t visszaadni, lehetne pl resolutionVertical/2 is
	//		}
	//		else{
	//			///here marginBottom == marginTop AND resolutionVertical = 1, OR
	//			/// OR  marginBottom > marginTop  AND resolutionVertical = 1,
	//			/// so resolutionVertical is 1 here
	//			return 0;
	//		}
	//	}
	//	return 0;
	//}
	void clampCellValues(){
		for (uint col = 0; col < resolutionHorizontal; ++col) {
			for (uint row = 0; row < resolutionVertical; ++row) {
				if(cells[col][row] > 1.0){
					cells[col][row] = 1.0;
				}
			}
		}
	}
public:
	friend class DrawingFactory;
	
	
	unsigned int getGesturePointsNum() const{
		int s = 0;
		for (int i = 0; i < (int)gestures.size(); ++i) {
			s += gestures[i].getPoints().size();
		}
		return  s;
	}
	void drawPoint(uint cellCol, uint cellRow){
		cells[cellCol][cellRow] = 1.0;
	}
	const std::pair<int,int>& gesturePointAt(unsigned int ind ) const{
		int flatIndTarget = ind;
		int flatIndCurr = 0;
		uint gInd = 0;
		///invariáns tul: flatIndCurr <= flatIndTarget
		while(gInd < gestures.size() && flatIndCurr <= flatIndTarget){
			///flatIndTarget-flatIndCurr itt pozitív
			uint diff = static_cast<uint>(flatIndTarget-flatIndCurr);
			if( diff < gestures[gInd].getPoints().size()){
				int localInd = flatIndTarget-flatIndCurr;
				/// itt localInd < gest[gInd].size(), ezért gest[gInd].getPoints()[localInd] már a keresett koordinátapárt jelzi.
				return gestures[gInd].getPoints()[localInd];
			}
			else{
				flatIndCurr += gestures[gInd].getPoints().size();
			}
			++gInd;
		}
		assert(!"ERROR: _ind is out of bounds: std::pair<int,int>& Drawing::getPoint(uint _ind)");
		return gestures[gInd].getPoints()[0];//csak hogy a fordító ne problémázzon a 'return' mentes lefutási ág miatt
	}

	virtual void updateCells(bool updateBoundsToo = false){
		if(updateBoundsToo)
			updateBounds();
		//gb.standardDeviaton = standDeviat;
		
		isCellsSetupBool = true;
		assert(0 != resolutionHorizontal);
		assert(0 != resolutionVertical);
		/// készítünk egy QPixmap-ot:
		
		//cellsPm = QPixmap(resolutionHorizontal,resolutionVertical);
		cellsPm = QPixmap(	getDrawingRight()-getDrawingLeft()+1,
							 getDrawingBot()-getDrawingTop()+1);
		{
		QPainter p(&cellsPm);
		/// a cellsPm pixmapot kitöltjük fekete színnel:
		p.fillRect(
				0, 
				0, 
				getDrawingRight()-getDrawingLeft()+1,
				getDrawingBot()-getDrawingTop()+1, 
				QBrush(QColor(0,0,0))
		   );
		cells.resize(resolutionHorizontal);
		/// gesture-öket rárajzoljuk a cellsPm pixmap-re:
		p.setPen(QPen(QColor(255,255,255)));
		for (int i = 0; i < (int)getGestures().size(); ++i) {
			for (int j = 0; j < (int)getGestures()[i].getPoints().size()-1; ++j) {
				//int x0 = calc_drawingCol_from_x(getGestures()[i].getPoints()[j].first);
				//int y0 = calc_drawingRow_from_y(getGestures()[i].getPoints()[j].second);
				//int x1 = calc_drawingCol_from_x(getGestures()[i].getPoints()[j+1].first);
				//int y1 = calc_drawingRow_from_y(getGestures()[i].getPoints()[j+1].second);
				int x0 = getGestures()[i].getPoints()[j].first - getDrawingLeft();
				int y0 = getGestures()[i].getPoints()[j].second - getDrawingTop();
				int x1 = getGestures()[i].getPoints()[j+1].first - getDrawingLeft();
				int y1 = getGestures()[i].getPoints()[j+1].second - getDrawingTop();
				p.drawLine(x0,y0,x1,y1);
			}
		}
		//cellsPm.save("_cellsPm.png","PNG",100);
		}
		///többlépésben átméretezzük cellsPm-et:
		
		///reshor, vert = 30, 30 esetén nekünk 30x30as kép kell
		/// de a skálázott kép(scaledPm) lehet mondjuk 30x20-as lesz(úgy skálázzuk ugyanis, hogy megtartsa a méretarányokat)
		/// ezért csinálunk egy új 30x30-as pixmapet (newPm-et), és rárajzoljuk scaledPm-et:
		QPixmap scaledPm = cellsPm.scaled(resolutionHorizontal, resolutionVertical, Qt::KeepAspectRatio, Qt::SmoothTransformation);/////////////////////////////////////////////////
		//scaledPm.save("_cellsPm_scaled.png","PNG",100);
		
		cellsPm = scaledPm;
		
		/// a képet rámásoljuk egy megfelelő méretű képre:
		QPixmap newPixmap(resolutionHorizontal, resolutionVertical);
		QPainter p2(&newPixmap);
		p2.fillRect( QRect(0,0,resolutionHorizontal, resolutionVertical), QBrush(QColor(0,0,0)) );
		p2.drawPixmap(0,0, scaledPm);
		//newPixmap.save("_newPixmap.png","PNG",100);
		
		/// az elkészült newPixmap-ból feltöltjük a cells táblázatot:
		cellsImg = newPixmap.toImage();
		for (uint col = 0; col < cells.size(); ++col) {
			cells[col].resize(resolutionVertical);
			for (uint row = 0; row < cells[col].size(); ++row) {
				//QRgb a = cellsImg.pixel(col,row);
				//int b = qGray(a);
				//double c = b/255.0;
				cells[col][row] = qGray(cellsImg.pixel(col,row))/255.0;
			}
		}
		clampCellValues();
		
	}
	virtual std::ostream& print(std::ostream& os, bool printGestureDataToo) const{
		os << 0 << std::endl;
		os << marginInPixels << std::endl;
		os.precision(std::streamsize(5));
		write2DTable(os, cells);
		os << isBoundsSetup_ << std::endl;
		os << isCellsSetup() << std::endl;
		os << resolutionHorizontal << '\t' << '\t' << resolutionVertical << std:: endl;
		if (printGestureDataToo) {
			os << gestures;
		}
		os << std::endl;
		return os;
	}
	virtual std::ostream& exportCellsAsLearningData(std::ostream& os, std::vector<double>& expectedVals) const{
		bool first = true;
		for(int i = 0 ; i < resolutionVertical ; ++i){
			for(int j = 0 ; j < resolutionHorizontal ; ++j){
				if(first){
					first = false;
				}
				else{
					os << ',';
				}
				os << cells.at(j).at(i);
			}		
		}
		for(double d : expectedVals){
			os << ',' << d;
		}
		os << std::endl;
		return os;
	}
	
	//TODO model::load, model::populatedrawings tesztelni, hogy működik e a más felbontású Drawing-ok fileból beolvasása: jól frissíti a Cells-t az új felbontásnak megfelelően?
	
	///TODO: most QPixmap és double táblázat formájában(Cells) is tárolja a Drawing a pixeleket, ez redundáns
	
	///TODO: alaposabb kiiras a fileban: tobb adat kiirasa, és a beolvasott sorok elé kiírni, hogy mely változóhoz tartoznak, ezek alapján elleőrizni, hogy jó adatokat olvasunk-e be
	/// (pl azert, mert most a gestureDataIsInFile és useGestureDataToo változókról nem lehet tudni, hogy a fileban igazak, vagy hamisak
	/// (ha nem kompatibilis filebol olvasunk, azt a program nem észleli, eért ilyen esetben kiszámíthatatlan a működés)	
	///
	/// \brief Drawing::load
	/// \param is
	/// \param newResHor
	///		ha különbözik a beolvasott resHorizontal értéktől, akkor a gesture-ökből újraelkészítjük a Cells-t  newResHor szélessgéggel
	/// \param newResVert
	///		ha különbözik a beolvasott resVertical értéktől, akkor a gesture-ökből újraelkészítjük a Cells-t  newResVert magassággal
	/// \param readGestureDataToo
	/// if set to true, then we try to read gesture data too from the file
	/// if readGestureDataToo is set to true, BUT no gesture data is present in the file,
	///  the program fails on an assert
	/// \return 
	///
	virtual std::istream& load(std::istream& is, int newResHor, int newResVert, bool useGestureDataToo, bool gestureDataIsInFile){
		if( !gestureDataIsInFile && useGestureDataToo){
			assert(!"ERROR: Drawing::load(..): gestureDataIsInFile cannot be set to false if useGestureDataToo is set to true!");
		}
		bool isPixmapSetup = false;
		double unused;
		is >> unused;
		//gb = GaussBlur2D(standDeviat);
		is >> marginInPixels;
		//is >> unused;
		read2DTable(is, cells);
		is >> isBoundsSetup_;
		is >> isCellsSetupBool;
		is >> resolutionHorizontal >> resolutionVertical;
		if(!useGestureDataToo 
			&&(resolutionVertical != newResVert || resolutionHorizontal != newResHor)
				){
			assert(!"ERROR: Drawing::load(..): useGestureDataToo is set to false, but newResHor or nrewsVert doesnt match the dimensions in file! needs to read gestureData to update create Drawing with different dimensions than that in the file. set readGestureToo, or load from Drawing files that match the dimensions used in the programs config file");
		}
		if(!is.fail() && gestureDataIsInFile){
			if(is.peek() == EOF){
				assert(!"ERROR: Drawing::load(..): gestureDataIsInFile is set to true, but end of stream is reached before Gesture could be read!");
			}
			
			if(useGestureDataToo){
				is >> gestures;
				///gesture-ből beállítom a Drawing paramétereit:
				updateBounds();
				///ha át kell méretezni a Cells táblát, akkor azt itt teszem meg:
				if(resolutionVertical != newResVert || resolutionHorizontal != newResHor){
					resolutionVertical = newResVert;
					resolutionHorizontal = newResHor;
					updateCells(false);
					isPixmapSetup = true;
				}
			}
			else{
				///ekkor a fileban benne van a gesture-nek is az adatai, de nekünk nem kell:
				Gesture dummy;
				dummy.load(is);
			}
			if(is.fail()){
				assert(!"ERROR: Drawing::load(..): error during reading gesture data!");
			}
		}
		///ha valaminek a beolvasása nem sikerült de mégse dobott hibát a program, akkor itt nem próbáljuk meg használni a hibásan beállított értékeket:
		if(!is)
			return is;
		
		if(!isPixmapSetup){
			
			///TODO: a tesytelni ezt az ágat
			
			/// cellsPm beállítása a már elkészült cells tábla alapján:
			//cellsPm = QPixmap(getDrawingRight()-getDrawingLeft()+1,
			//		  getDrawingBot()-getDrawingTop()+1);
			cellsPm = QPixmap(resolutionHorizontal, resolutionVertical);
			QPainter painter(&cellsPm);
			
			painter.fillRect( QRect(0, 0, resolutionHorizontal, resolutionVertical), 
							  QBrush(QColor(0,0,0)) 
							);
			/// cellsImg megfelelő méretre állítása:
			cellsImg = cellsPm.toImage();
			//QPainter p(&cellsPm);
			//cells.resize(resolutionHorizontal);
			
			/// cellsImage beállítása a fent elkészített cells-ből:
			for (uint col = 0; col < cells.size(); ++col) {
				//cells[col].resize(resolutionVertical);
				for (uint row = 0; row < cells[col].size(); ++row) {
					/// a [0,1] inval beli double -> [0,255] interval beli int konverzió során kerekítési hiba lép fel.
					/// 0.5 hozzáadaásával a kerekítési hibakorlát 0.5 lesz
					
					/// ha sokszor konvertáljuk át más méretűre ugyanazt az ábrát,
					///  akkor ez a konverzió sokszor történik meg, így a hiba nagyobb is lehet.
					int l = 255.0 * cells[col][row] + 0.5;
					cellsImg.setPixel(col, row, qRgb(l,l,l));
					/// forDebug változóba visszakonvertálom a most beállított pixelt 0..255 közötti értékre QRgb-ből, hogy ellenőrízzem,
					///  hogy qGray(qRgb(l,l,l)) == l:
					double forDebug = qGray(cellsImg.pixel(col,row));
					if(std::abs(forDebug - l) > 0.5){
						assert(false);
					}
					/// qrgb.h alapján levezetm, hogy qGray(qRgb(l,l,l)) == l:
					/// qrgb.h alapján(az egyszerűség kedvéért a maszkolást figyelmen kívül hagyom itt):
					///		 
					///		qGray(qRgb(l,l,l)) = qGray(l*2^16 + l*2^8 + l) = qGray(l,l,l) = 32*l/32 = l
				}
			}
			/// cellsPm beállítása cellsImg-ből:
			QPixmap pm2(resolutionHorizontal, resolutionVertical);
			QPainter painter2(&pm2);
			painter2.fillRect( QRect(0, 0, resolutionHorizontal, resolutionVertical), 
							  QBrush(QColor(0,0,0)) 
							);
			pm2.fromImage(cellsImg);//todo vmiert fekete teljesen a kep 
			pm2.save("_loaded_cellsPm.png","PNG",100);
			painter.drawPixmap(0,0, pm2);
		}
		return is;
	}
};

#endif // DRAWING_HPP
