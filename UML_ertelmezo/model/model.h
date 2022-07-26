#ifndef MODEL_H
#define MODEL_H

#include "persistence/persistence.h"
#include "idrawingcontainer.h"
#include "userinputmodel.h"
#include "common.hpp"
#include "shape_drawing/drawing.hpp"
#include "identityDir/identifiable.h"
#include "canvasmanipulation.h"
#include "DrawingClassifiers/neuralclassifier.h"
#include "DrawingClassifiers/simpleclassifier.h"
#include "shape_drawing/drawing.hpp"
#include "shape_drawing/drawing.hpp"
#include "shape_drawing/drawing_factory.h"
#include "modelstatemanager.h"
#include "model/CanvasElements/canvaselement.h"
#include "mysharedptr.h"
#include <QTimer>
#include <QObject>

#include <map>
#include <memory>
#include <functional>
class Connector;
class CanvasEvent;

/// TODO: qtConnection-ök mentése és betöltése
/// a ConnectionData osztály alkalmazásával.
/// A QtConnection tekinthető aggregációnak.
class Model : 
		public QObject,  
		public virtual Identifiable, 
		public virtual IDrawingContainer
{
	Q_OBJECT
	friend class Persistence;
	typedef CanvasManipulation::State State;
	//typedef std::shared_ptr<CEB> CEContT;
	typedef MySharedPtr<CEB> CEContT;
	
///fields:
private:
	int saveStateInd = 0;
	CanvasManipulation cm;
	
	Persistence* persistence;
	std::weak_ptr<CEB> selectedCE;
	//Direction resizizngCEEdge;
	std::shared_ptr<GenericFileUIM> saveLastDrawingModel;
	
	uint resHorizontal;
	uint resVertical;
	bool testNetwork = false;
	std::string drawingsOutFileName;
	
	int marginInPixels;
	double drawingBlurSD;
	QString saveLoadDir = "";
	///
	/// \brief isDragging
	/// a felhasználó épp húzza-e draggol-e épp az egérrel
	/// (azaz billentyűt lenyomva húzza-e épp az egeret)
	bool isDragging = false;
	///
	/// \brief isRightButtonDragging
	///
	bool isRightButtonDragging = false;
	CEB* lastDraggedCE = nullptr;
	std::shared_ptr<CEB> copied;
	IDrawingClassifier* dc;
public:
	/// canvasElements heterogén kollekció a rajzvásznon elhelyezkedő nézet-elemek modelljének(CEB-ek) tárolására.
	/// 
	/// Megjegyzés: Azért shared_ptr eket tárolok ptr-eke helyett a canvasElements -ben, hogy át tudjam adni az elemeit más osztályoknak anélkül,
	///  hogy a vektor reallokálná az átadott pointer által mutatott memória területet
	/// (reallokáció történhet amikor új elemet adunk a vektorhoz)
	/// a canvasElements elemeit weak_ptr formájában adom át.
	/// 
	/// Megjegyzés2: Azt is akarom, hogy a model bármikor törölhesse az elemeket anélkül, hogy ez problémát okozna a nézet rétegben.
	/// 
	/// Megjegyzés3: alternatív megoldás lehet std::list -ben tárolni az elemeket, és a nézetben ezen elemekre pointereket átadni.
	/// Így nincs reallokáció új elem hozzáadásakor. 
	/// Ezt nem próbáltam még ki, de lehet a baj:
	///  míg a weak_ptr lock() eljárása biztosítja hogy ne szabaduljon fel a nézetben épp használatban levő változó(persze csak a lock() hívás és a scope végének elérése között),
	/// egy sima pointer használatával ez nincs biztosítva. Tehát amennyiben a modell felszabadítja a tárolt canvasElementet amit a nézet épp használ egy eljárása közepén, akkor a program segfaultot dobhat.
	/// Ezen alternatíva használatához biztosítani kell még azt, hogy a modell nem töröl canvasElements -beli elemeket amikor a nézet egy eljárása épp használja azt.
	/// Lehet, hogy a qt signal slot hívása ezt eleve biztosítják és nem lesz ezzel probléma.
	std::vector<CEContT> canvasElements;
	Drawing* lastDrawing;
	DrawingFactory* drawingFactory;
	std::vector<Drawing*> drawings;
	size_t inputNum;
	QLine lineToShow;
	///TODO: a periódikus modellállapot mentés tönkreteszi a redo logikát
	/// (undo után automatikusan ráment az inaktív redozható mentett állapotra)
	
	/// stateManager a teljes model több állapotának elmentésére, elmentett állapotok között való lépkedésre való.
	/// (checkpointok és undo-redo végrehajtására)
	ModelStateManager stateManager;
///ctors, dtor, operators:
public:
	///Todo: a config fileba
	///		beletenni a networkFile nevét is,
	///		megváltoztatni a networkfile nevét, amikor training-el uj networkot tanitunk(az uj nevere),
	///		menteni a valtoztatasokat a config fileba
	Model();
	~Model();
	
///other methods:
private:
	void displayDrawingsAsMiniatures();
	std::weak_ptr<CEB> getCEAt(QPoint cPos);
	/// saveState : stateManager segítségével elmenti a modell állapotát
	void saveState();
public:
	std::weak_ptr<GenericFileUIM> getSaveLastDrawingModel();
	bool selectDrawing(int x,int y);
	const std::vector<Drawing*>&  getDrawings() const override;
	const Drawing* getLastDrawing() const;
	void initNewDrawing();
	bool addGestureToActiveDrawing(Gesture g);
	void addGesturePoint(int x, int y);
	
	//void fullSave();
	//void fullLoad();
	void saveCEs(std::ostream& osElems, std::ostream& osAggreg);
	void loadCEs(std::istream& isElems, std::istream& isAggreg);
	void saveCEs(QString fileName);
	void loadCEs(QString fileName);
	///
	/// \brief saveDrawings \n
	/// Menti a Drawing-okat a 'filename'-ben megadott fileból
	/// ha a fileName üres string (""), akkor a drawingsOutFilename helyre menti a drawingokat,  \n
	/// amúgy a megadott helyre.
	/// sikertelen filenyitás, vagy írás esetén showMessage(QString) signal-al jel jelzi azt a nézetnek
	/// \param fileName
	/// mentés helye
	void saveDrawings(QString fileName = "");
	void saveDrawings(std::ostream& os, QString fName = "");
	///
	///TODO: hibajelzés küldése akkor is,
	/// ha a filenyitás sikeres, de a drawing-ok beolvasása sikertelen:

	/// \brief loadDrawingsc
	/// Betölti a Drawing-okat a 'filename'-ben megadott fileból
	/// sikertelen filenyitás, vagy üres filename("") esetén
	///  showMessage(QString) signal-al jel jelzi ezt a nézetnek
	/// Küld hibajelző üzenetet a következő esetben!:
	///		Ha a filenyitás sikeres, de a drawing-ok beolvasása sikertelen
	/// \param file
	/// \param discardCurrentDrawings
	///
	void loadDrawingsc(QString filename, bool discardCurrentDrawings, bool NNcreateCEs = false);
	void loadDrawingsc(std::istream& is, bool discardCurrentDrawings, bool NNcreateCEs = false);

	void undo();
	void redo();
	void pushRecoginzeReplaceLastDrawing();
	void widget_enterPressed();
	/// helper for pToCanv:
	//CEB* srcAsCE();
	/// helper for canvas_mouseEvent:
	//QPoint pToCanv(QMouseEvent event);
	void canvas_mouseEvent(QMouseEvent, std::weak_ptr<CEB>, QPoint cCoords);
	///
	/// \brief addCanvasElement
	/// jelet küld a nézetnek, hogy az hozza létre a szükséges connection-öket a nézet és ceb között.
	/// ezután move-olja ceb-et a canvasElements-be. 
	/// Az eljárás meghívása után a ceb shared_ptr már üres,
	///  de az eljárás visszatér egy weak_ptr-el, amin keresztül továbbra is elérhető a CEB.
	/// \param ceb
	/// \param pos
	/// canvasElements hanyadik elemét kell lecsereélni.
	/// ha -1, akkor az utolsó elem utánra kerül.
	/// ha nem -1, akkor ellenőrzi, hogy a pos. helyen levő elem üres-e, aztán lecseréli ceb-re.
	/// \return 
	/// a canvasElements-be move-olt ceb weak_ptr-ként
	///	
	std::weak_ptr<CEB> addCanvasElement(CEContT&& ceb, int pos = -1);
	
	/// TODO: nehezen bővíthető ez az eljárás új fajta CE-k létrehozására
	/// megjegyzés: ha LineComponentet készítünk ezzel az eljárással,
	///  akkor az csak félkész lesz: a végpontjait még be kell állítani
	///  ezen eljárás meghívása után.
	template<class... Args>
	CEContT createCanvasElement(
			CanvasElement_Type ce_type, 
			QRect area,
			Relationship_Type rst=ERROR_RT,
			Args... args){
		switch (ce_type) {
		case ERROR :{
			assert(false);
			break;
		}
		case LINE :{
			return std::make_shared<LineComponent>(area, args...);
			///a LineComponent csak félkész lesz: a végpontjait még be kell állítani ezen eljárás meghívása után.
			break;
		}
		case TEXTBOX :{
			return std::make_shared<TextComponent>(area);
			break;
		}
		case UMLCLASS :{
			return std::make_shared<UMLClassComponent>(area);
			break;
		}
		case RELATIONSHIP :{
			switch(rst){
			case ERROR_RT:{
				assert(false);
				break;
			}
			case AGGREGATION :{
				return std::make_shared<UMLAggregationSpecifier>(area);
				break;
			}
			case COMPOSITION :{
				return std::make_shared<UMLCompositionSpecifier>(area);
				break;
			}
			case EXTENSION :{
				return std::make_shared<UMLExtensionSpecifier>(area);
				break;
			}
			case CIRCLE :{
				return std::make_shared<UMLCircle>(area);
				break;
			}
			default:{
				assert(false);
				break;
			}
			}
			break;
		}
		default:{
			assert(false);
			break;
		}
		}
		return std::make_shared<TextComponent>(area);//vissza kell valamit adni, hogy a compiler ne izéljen
	}
	
	
	//void reactToEvent(std::weak_ptr<CEB> origSource, CanvasEvent event);
	void trainNNCallback(std::shared_ptr<TrainingUIM> trainingForm);
	
	std::vector<std::weak_ptr<CEB>> getConnectedRecursive(std::weak_ptr<CEB> s);
	int findPosOfCEB(CEB* c);
	void Wid_copySelected();
	void Wid_pasteSelected();
	void Wid_undo();
	void Wid_redo();
	void deleteAt(int pos);
	void Wid_deleteSelected();
	void saveFullModel();
	void loadFullModel();
	
private slots:
	void CanvasElement_connectToElementAtCanvasCoords(std::weak_ptr<CEB> src, QPoint p, std::weak_ptr<Plug> srcPlug);
	
	void fwd_updateCanvas(QRect);
	void fwd_updateCanvas();
	void fwd_addDotMatrixMiniature(DotMatrix, int pos_canBeOverIndexed);
	void fwd_removeDotMatrixMiniature(int pos);
	
public slots:
	
	void trainNN();
	///
	///\brief produceDatasFromDir
	///
	///2 mappaválasztó dialog ugrik fel, amiben egy forrás és egy célkönyvtárat kell kiválasztani.
	///a forráskönyvtárban levő összes filet Drawingként értelmezve
	///ezek Gesture-adatai alapján több Drawing file-t készít a célkönyvtárban úgy, hogy a Gesture-ök pontjait kicsit eltolja véletlenszerűen
	///FONTOS:
	/// 1.: a forráskönyvtárban csak Drawing fileoknak szabad lennie,
	/// 2.: a Drawing file-ok Gesture datát is kell, hogy tartalmazzanak
	///megjegyzés: az  elkészített képekre az e-programban  használt margó és elmosás adatok vonatkoznak majd,
	///  tehát így ezeket a paramétereket is lehet módosítani
	void produceDatasFromDir(QString sourceDir, QString destDir);
	void produceDatasFromDir_phase2(QString sourceDir, QString destDir, std::shared_ptr<DrawingPopulationUIM>);	
	void categorizeDrawingCreateCanvaElement(Drawing*);
	//void mouseMoved(int x, int y);
	//void load_phase2(QString fileName, bool discardCurrentDrawings);
	///a pushAndTestLastDrawing használom, ahol a 'lastDrawing'-ot a 'drawings' vektorba helyezése után,
	/// de a lecserélése előtt még fel akarom használni a lastDrawing-ot:
	void pushLastDrawing();
	void discardDrawings();
	void discardCanvasElements();
signals:
	void removeCanvasElement(std::shared_ptr<CEB> ceb);
	void updateCanvas(QRect);
	void updateCanvas();
	void askUserForFileName(std::function<void (QString path)> callback);
	void askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)> callback);
	void showMessage(QString);
	void addDotMatrixMiniature(DotMatrix, int pos_canBeOverIndexed);
	void removeDotMatrixMiniature(int pos);
	void removeDrawingDotMatrixMiniatures();
	void canvasElementAdded(std::weak_ptr<CEB> ce);
	void removeCanvasElements();
};

#endif // MODEL_H
