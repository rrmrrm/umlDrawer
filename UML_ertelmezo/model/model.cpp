#include "model.h"
#include "network/network.h"
#include "CanvasElements/canvaselement.h"
#include "common.hpp"
#include "persistence/persistence.h"
#include "shape_drawing/drawing.hpp"
#include "model/plug.h"
#include "model/identityDir/connectiondata.h"
//#include "model/canvasevent.h"

#include <QDirIterator>
#include <QTimer>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <variant>
#include <string>
#include <cassert>
#include <iostream>
#define nameOf( val ) ( #val )
///TODO: a configfileban
///		megváltoztatni a networkfile nevét, amikor training-el uj networkot tanitunk(az uj nevere),
Model::Model():
		Identifiable(0),
		QObject(nullptr),
		saveLastDrawingModel(
			std::make_shared<GenericFileUIM>(
				this,
				"Utolsó elkészített ábra fileba mentése")),
		lineToShow(0,0,0,0),
		stateManager(ModelStateManager(this))
{;
	persistence = Persistence::GetSingleton();
	
	persistence->loadVars(
		std::vector<std::pair<QString, ptr_variants> >({
			std::make_pair<QString, ptr_variants>(nameOf(resHorizontal), ptr_variants(&resHorizontal)),
			std::make_pair<QString, ptr_variants>(nameOf(resVertical), ptr_variants(&resVertical)),
			std::make_pair<QString, ptr_variants>(nameOf(marginInPixels), ptr_variants(&marginInPixels)),
			std::make_pair<QString, ptr_variants>(nameOf(drawingBlurSD), ptr_variants(&drawingBlurSD)),
			std::make_pair<QString, ptr_variants>(nameOf(testNetwork), ptr_variants(&testNetwork)),
			std::make_pair<QString, ptr_variants>(nameOf(drawingsOutFileName), ptr_variants(&drawingsOutFileName)),
		})
	);
	dc = new NeuralClassifier(persistence, resHorizontal, resVertical);
	//dc = new SimpleClassifier;
	drawingFactory = new DrawingFactory(drawingBlurSD, marginInPixels);
	lastDrawing = drawingFactory->create(resHorizontal, resVertical);
	
	/// bizonyos időközönként mentjük a teljes modell állapotot. 
	QTimer *timer = new QTimer(this);
	timer->setInterval(10*1000);
	connect(timer, &QTimer::timeout, this, &Model::saveState);
	timer->start();
}
Model::~Model(){
	delete dc;
}

void Model::displayDrawingsAsMiniatures(){
	int i = 0;
	for(auto d = drawings.begin() ; d != drawings.end() ; ++d, ++i){
		emit addDotMatrixMiniature((*d)->getCells(), i);	
	}
}
std::weak_ptr<CEB> Model::getCEAt(QPoint cPos){
	for(const auto& ce : canvasElements){
		if(ce.get() && ce->getGeometry().contains(cPos)){
			return ce;
		}
	}
	return std::weak_ptr<CEB>();
}
void Model::saveState(){
	stateManager.saveState();
}
std::weak_ptr<GenericFileUIM> Model::getSaveLastDrawingModel(){
	return saveLastDrawingModel;
}
//todo: itt indexet is vissza kéne adni, vagy eltárolni inkább a sikeresen kijelolt Drawing indexét a drawings-ban:
bool Model::selectDrawing(int x,int y){
	bool found = false;
	uint i = 0;
	while(!found && i < drawings.size()){
		if(!drawings[i]){
			continue;
		}
		auto d = drawings[i];
		//megnézzük, hogy az (x,y) pont benn van e
		//a gesture-öket határoló téglalapban:
		found = isPointInRectangle(
					x,
					y,
					d->getMinX(), 
					d->getMinY(), 
					d->getMaxX() - d->getMinX(), 
					d->getMaxY() - d->getMinY()
				);
		++i;
	}
	if(found){
		--i;
		
	}
	return found;
}
const std::vector<Drawing*>& Model::getDrawings() const{
	return drawings;
}
Drawing const* Model::getLastDrawing() const {
	return lastDrawing;
}


///lastDrawing-ba új Drawing-ot készítünk:
void Model::initNewDrawing(){
	//Drawing oldLastDrawing = *lastDrawing;
	auto preState = lastDrawing;
	lastDrawing = drawingFactory->create(resHorizontal, resVertical);
	//ModelStateManager::addStateOp(new GenericModelStateOp<Drawing*>(preState, lastDrawing, &lastDrawing));
	//ModelStateManager::addStateOp(new DrawingStateOp_prepost(&lastDrawing, oldLastDrawing, *lastDrawing));
	/// lastDrawing-ot
}
bool Model::addGestureToActiveDrawing(Gesture g){
	if(lastDrawing == nullptr){
		return false;
	}
	auto pre = lastDrawing->getGestures();
	lastDrawing->addGesture(g);
	//ModelStateManager::addStateOp(
	//		new GenericModelStateOp<std::vector<Gesture>>(
	//				pre, 
	//				lastDrawing->getGestures(), 
	//				&lastDrawing->accessGestures()
	//		)
	//);
	emit updateCanvas(lastDrawing->getBoundingRect());
	return true;
}
void Model::addGesturePoint(int x, int y){
	lastDrawing->addGesturePoint(x,y);
	//QPoint range(lastDrawing->getSpreadRange()+1,
	//			 lastDrawing->getSpreadRange()+1
	//			);
	emit updateCanvas(
			QRect(
				QPoint(x-20, y-20),
				QPoint(x+20, y+20)
			)
		);
	//emit DrawingsUpdated();
}
//void Model::fullSave(){
//	std::string saveDir = "diagramTempSaves/";
//	std::string filePath = saveDir + "save1";
//	std::ofstream ofs(filePath);
//	saveCEs(ofs, ofs);
//	saveDrawings(ofs, QString(filePath.c_str()));
//}
//void Model::fullLoad(){
//	std::string saveDir = "diagramTempSaves/";
//	std::string filePath = saveDir + "save1";
//	std::ifstream ifs(filePath);
//	loadCEs(ifs, ifs);
//	loadDrawingsc(ifs, true, false);
//}
void Model::saveCEs(std::ostream& osElems, std::ostream& osAggrs){
	/// save contained objects:
	const std::string CEtoken = "CE";
	osElems << canvasElements.size() << std::endl;
	for(auto& it: canvasElements){
		osElems << CEtoken << std::endl;
		assert(it.get());
		if(it.get()){
			osElems << it->CEtype << std::endl;
			if(auto rel = std::dynamic_pointer_cast<UMLRelationship>(it)){
				osElems << rel->rtype << std::endl;
			}
			it->save(osElems);
		}
		else{
			osElems << CanvasElement_Type::ERROR << std::endl;
		}
	}
	osAggrs << canvasElements.size() << std::endl;
	for(auto& it: canvasElements){
		osAggrs << CEtoken << std::endl;
		assert(it.get());
		if(it.get()){
			it->saveAggregations(osAggrs);
		}
	}
	osAggrs << std::endl;
	osAggrs << "modellIntraconnections" << std::endl;
	saveContainer(osAggrs, ConnectionData::modellIntraconnections);
}
void Model::loadCEs(std::istream& isElems, std::istream& isAggrs){
	canvasElements.resize(0);
	emit removeCanvasElements();
	
	const std::string CEtoken = "CE";
	std::string checkstr = "";
	int csiz = 0; isElems>> csiz; assert(isElems);
	for(int i = 0 ; i < csiz ; ++i){
		isElems >> checkstr; assert(checkstr == CEtoken);
		CanvasElement_Type cet = ERROR;
		/// load CE type info into cet enum:
		int e=0; isElems >> e; cet=(CanvasElement_Type)e;
		assert(cet != CanvasElement_Type::ERROR);
		
		if(cet != CanvasElement_Type::ERROR){
			Relationship_Type rt = Relationship_Type::ERROR_RT;
			///if CE is a RelationShip, load RelationShips type into rt enum :
			if(cet == CanvasElement_Type::RELATIONSHIP){
				int r = 0; isElems >> r; rt = (Relationship_Type)r;
				assert(rt != Relationship_Type::ERROR_RT);
			}
			///create CE with the type according to cet, and rt enums:
			{
			auto newCE = createCanvasElement(cet, QRect(0,0,100,100), rt);
			///load CE:
			newCE->load(isElems);
			
			addCanvasElement(std::move(newCE));
			//csak 1 referenciát akarunk a tárolt objektumra,
			//a smart ptr itt megszűnik
			}
		}
		else{
			///loadCEs(std::istream& isElems, std::istream& isConns): cant load CE. invalid type read:
			assert(false);
			
		}
	}
	int aggrInfoNum; 
	isAggrs >> aggrInfoNum;
	///ellenőrzöm, hogy isElems és isAggrs ugyanannyie elem információját tartalmazza-e:
	///isAggrs-ben canvasElements.size() darab elem adatának kell lennie:
	assert(aggrInfoNum == (int)canvasElements.size());
	
	for(auto& it: canvasElements){
		isAggrs >> checkstr; assert(checkstr == CEtoken);
		assert(it.get());
		if(it.get()){
			it->loadAggregations(isAggrs);
		}
	}
	std::string connPartTok;
	isAggrs >> connPartTok;
	assert(connPartTok == "modellIntraconnections");
	loadContainer(isAggrs, ConnectionData::modellIntraconnections);
}
void Model::saveCEs(QString fileName){
	std::ofstream ceStream(fileName.toStdString());
	std::ofstream aggrStream(fileName.toStdString()+"_aggregations");
	saveCEs(ceStream, aggrStream);
	std::weak_ptr<int> s;
}
void Model::loadCEs(QString fileName){
	std::ifstream ceStream(fileName.toStdString());
	std::ifstream aggrStream(fileName.toStdString()+"_aggregations");
	loadCEs(ceStream, aggrStream);
}
/// 
/// \brief Model::save saves all drawings to the same file
/// \param fileName
void Model::saveDrawings(QString fileName){
	if(fileName == ""){
		fileName = QString::fromStdString(drawingsOutFileName);
	}
	std::ofstream fs;
	fs.open(fileName.toStdString(), std::ios_base::out);
	if(fs.fail()){
		emit showMessage( qstr("A ") + fileName + qstr(" file megnyitása sikertelen!") );
		return;
	}
	
	saveDrawings(fs, fileName);
}
void Model::saveDrawings(std::ostream& os, QString fName){
	for(uint i = 0u ; i < drawings.size(); ++i){
		assert(nullptr != drawings[i]);
		drawings[i]->print(os, true);
		os << std::endl;
		os << std::endl;
		if(os.fail()){
			emit showMessage( qstr("Az ábrák mentése sikertelen, de a ") + fName + qstr(" file-t sikerült megnyitni!") );
			return;
		}
	}
	emit showMessage( qstr("Az ábrák sikeresen mentésre kerültek a ") + fName + qstr("fileba") );
}
void Model::loadDrawingsc(QString file, bool discardCurrentDrawings, bool NNcreateCEs){
	if(file == "" || file.isEmpty()){
		//ha nem valasztottak ki egy file-t sem:
		emit showMessage("Sikertelen a betöltés, mert üres a filenév!");
		return;
	}
	std::fstream fs;
	fs.open(file.toStdString(), std::ios_base::in);
	if(fs.fail()){
		emit showMessage("a file megnyitása sikertelen!");
		return;
	}
	loadDrawingsc(fs, discardCurrentDrawings, NNcreateCEs);
}
void Model::loadDrawingsc(std::istream& is, bool discardCurrentDrawings, bool NNcreateCEs){
	auto prevDrawings = drawings;
	//ModelStateManager::addStateOp(new ModelStateOp_composite());
	if(discardCurrentDrawings){
		discardDrawings();
		discardCanvasElements();
	}
	Drawing* d = new Drawing();
	while(d->load(is, resHorizontal, resVertical, true, true)){
		if(
				//d->gb.standardDeviaton != drawingBlurSD
				//||
				d->marginInPixels != marginInPixels
					)
		{
			
			//if(d->standDeviat != drawingBlurSD){
			//	d->standDeviat = drawingBlurSD;
			//}
			if(d->marginInPixels != marginInPixels){
				d->marginInPixels = marginInPixels;
			}
			if(!d->isBoundsSetup_)
				d->updateBounds();
			d->updateCells(false);
		}
		drawings.push_back(d);
		if(testNetwork && NNcreateCEs)
			categorizeDrawingCreateCanvaElement(d);
		///last value of d(which is in the heap) is still accessible through the drawings vector,
		/// so this line shoudldn't cause memory leak(last one needs to be deleted though):
		d = drawingFactory->create(resHorizontal, resVertical);
	}
	///deleting d to avoid memory-leak:
	delete d;
	//ModelStateManager::addStateOp(new GenericModelStateOp<std::vector<Drawing*>>(
	//				   prevDrawings, drawings, &drawings));
	//
	//ModelStateManager::finishCompositeStateOp();
	
	emit updateCanvas();
	///képek megjelenítése kicsinyítve :
	displayDrawingsAsMiniatures();
	showMessage("az ábrák sikeresen betöltődtek");
}

void Model::undo(){
	stateManager.undo();
	//ModelStateManager::undo();
}
void Model::redo(){
	//ModelStateManager::redo();
	stateManager.redo();
}	
void Model::pushRecoginzeReplaceLastDrawing(){
	//ModelStateManager::addStateOp(new ModelStateOp_composite);
	pushLastDrawing();
	assert(getLastDrawing()->isCellsSetup());
	///tesztNetwork = igaz esetén:
	///betoltunk egy neuralis halot es eldontjuk, hogy melyik ábra-osztályra hasonlít a lastDrawing
	if(testNetwork)
	{
		categorizeDrawingCreateCanvaElement(lastDrawing);
	}
	///newDrawing-ba új Drawing-ot készítünk:
	initNewDrawing();
	//ModelStateManager::finishCompositeStateOp();
}
void Model::widget_enterPressed(){
	pushRecoginzeReplaceLastDrawing();
}
/// TODO: átméretezés
void Model::canvas_mouseEvent(QMouseEvent event,  std::weak_ptr<CEB> ce, QPoint cCoords){
	//auto element =  srcAsCE();
	cm.applyTransition(
		event,
		Target(cCoords, ce, 1)
	);
	//if(event.type == CanvasEvent::Type::EdgeClicked){
	//	resizizngCEEdge = event.getEdgeClicked();
	//}
	switch(CanvasManipulation::transRes.resultType){
	case Line:{
			addCanvasElement(std::make_shared<LineComponent>(
						QRect(0,0,0,0), 
						CanvasManipulation::transRes.t1, 
						CanvasManipulation::transRes.t2
						));
			//LineComponent* lcp = lineC.get();
			//std::move(lineC));
		break;
	};
	
	case GestureType:{
			addGestureToActiveDrawing(
						Gesture(
								CanvasManipulation::transRes.t1.getFreshXInCanvas(),
								CanvasManipulation::transRes.t1.getFreshYInCanvas()
						)
					);
		break;
	};
	case GesturePointType:{
		addGesturePoint(
					CanvasManipulation::transRes.t1.getFreshXInCanvas(),
					CanvasManipulation::transRes.t1.getFreshYInCanvas()
				);
		break;
	}
	case NOTHING:{
		break;
	}
	case ELEMENTPRESSED_RESULT:{
		break;
	}
	case ELEMENTSELECTED_RESULT:{
		if(auto so =  selectedCE.lock()){
			so->deselect();
			emit updateCanvas(so->geometry);
		}
		auto newSo = CanvasManipulation::transRes.t1.canvasElement.lock();
		assert(newSo);
		newSo->select();
		selectedCE = newSo;
		emit updateCanvas(newSo->geometry);
		break;
	}
	case ELEMENT_STARTRESIZING_RESULT:
	case ELEMENT_RESIZING_RESULT: {
		auto rt = cm.transRes.t1.canvasElement.lock();
		//ha kijelöltünk valamit, akkor azt fogjuk átméretezni:
		if(auto so = selectedCE.lock()){
			rt = so;
		}
		if(!rt)
			break;
		qDebug() << "transresX, transresY: " << cm.transRes.t1.getFreshXInCanvas()	
				<< cm.transRes.t1.getFreshYInCanvas() << "\n";				
		emit rt->resizeEdgeSig(
				cm.transRes.t1.getFreshXInCanvas()
				, cm.transRes.t1.getFreshYInCanvas()
				//, resizizngCEEdge
			);
		break;
	}
	case ELEMENT_STOPRESIZING_RESULT: {
		break;
	}
	case ELEMENT_DESELECTED_RESULT:{
		if(auto so =  selectedCE.lock()){
			so->deselect();
			emit updateCanvas(so->geometry);
		}
		break;
	}
	case ELEMENT_DBLPRESSED_RESULT:{
		auto to = CanvasManipulation::transRes.t1.canvasElement.lock();
		QPoint pressPoint = QPoint(
				CanvasManipulation::transRes.t1.x_inCanvasOriginal,
				CanvasManipulation::transRes.t1.y_inCanvasOriginal
		);
		assert(to);
		std::weak_ptr<Connector> pressedConn = to->mouseDoublePressed_My(CanvasManipulation::transRes.t1);
		auto pco = pressedConn.lock();
		if(!pco){
			break;
		}
		
		QRect area = QRect(100,100,200,200);
		auto ctype = pco->getConnectableCEType();
		///TODO: létrehozni kezelni a több fajta CanvasElement-et elfogadó
		///  connecotrokat és itt(is) számítani rájuk. valszeg máshogy kell ezt megoldani mint ahogy elkezdtem; lásd Connector-ban todo comment
		std::weak_ptr<CEB> movedCEO;
		{
		std::shared_ptr<CEB> newCE;
		if(ctype == RELATIONSHIP){
			newCE = createCanvasElement(ctype, area, Relationship_Type::AGGREGATION);
		}
		else{
			newCE = createCanvasElement(ctype, area);
		}
		/// először newCE-el meg kell hívni addCanvasElement-et, hogy newCE és a nézet réteg közötti connection-ök létrehozza.
		/// addCanvasElement ez törli newCE-t, viszont a visszatér egy weak_ptr-el amit továbbra is tudunk használni:
		movedCEO = addCanvasElement(std::move(newCE));
		/// newCE változó megszűnik, de át move-oltuk már
		}
		to->tryConnToThis(movedCEO, pressPoint, movedCEO.lock()->getMainPlug());
		//connectPlugToConnn(pco, newCE->getMainPlug());
		break;
	}
	case DRAG_UNSELECTED:{
		auto elementObs = ce.lock();
		if(!elementObs){//az esemény entityWidgetből származik-e
			break;
		}
		auto obs = cm.transRes.t1.canvasElement.lock();
		if(obs == nullptr)
			break;
		obs->dragUnselected(
					cm.transRes.t1.x_inCanvasOriginal, 
					cm.transRes.t1.y_inCanvasOriginal
				);
		
		// frissítjük a draggolt entityWidgetet:
		obs.get()->should_repaint();
		break;
	}
	case STOPDRAGGING:{
		auto elementObs = ce.lock();
		if(!elementObs){//az esemény entityWidgetből származik-e
			break;
		}
		auto obs = cm.transRes.t1.canvasElement.lock();
		if(obs == nullptr)
			break;
		//auto stoppedAt = getCEAt(event.cPos);
		obs.get()->stopDrag(cCoords);
		
		// frissítjük az összes entityWidgetet:
		for(const auto& it : canvasElements){
			if(it.get()){
				it->should_repaint();
			}
		}
		break;		
	}
	default:{
		assert(false);
		break;
	}
	}
	//delete event.mEvent;
}
std::weak_ptr<CEB> Model::addCanvasElement(CEContT&& ceb, int pos){
	if(pos != -1){
		assert(pos < (int)canvasElements.size());
		assert(canvasElements.at(pos) == nullptr);
	}
	emit canvasElementAdded(ceb->weak_from_this());
	modelConnect(
		ceb.get(), 
		SIGNAL(connectToCEAt(
				std::weak_ptr<CEB>, QPoint, std::weak_ptr<Plug>)
			   ), 
		this, 
		SLOT(CanvasElement_connectToElementAtCanvasCoords(
				std::weak_ptr<CEB>, QPoint, std::weak_ptr<Plug>)
			 )
	);
	/// innentől canvasElements menedzseli ceb éllettartamát:
	if(pos==-1){
		canvasElements.push_back(std::move(ceb));
		return canvasElements.back();
	}
	else{
		canvasElements.at(pos) = std::move(ceb);
		return canvasElements.at(pos);
	}
	
	//megjegyzés: az átláthatóság érdekében egy uml diagramnak nem kéne sok elemet tartalmaznia, ezért feltételezhetően nem is lesz sok. így nem lesz a program  memória igénye nagy
	//mi van ha minden m=dos\t's ut'n serialiy'lom ;s fileba mentem ay ;rintettt objektumokat? megoldható ilyen módon a undoredo-zás?
	
	//ModelStateManager::addStateOp(
	//	new GenericModelStateOp<std::vector<std::shared_ptr<CEB>>>
	//				(preState, canvasElements, &canvasElements));
}


int sysCommand(std::string command){
	//Checking if processor is available:
	if (!system(NULL)){
		return EXIT_FAILURE;
	}
	
	//Executing command;
	int i;
	i = system(command.c_str());
	return i;
}
//" " közé teszi a stringet,
// hogy egy szóközt tartalmazó elérési útvonal
// system call argumentumként való felhasználásakor ne legyen
// két külön paraméterként értelmezve
std::string Quotes(const std::string& string){
	return str("\"") + string + str("\"");
}
std::string Quotes(const char* cStr){
	return Quotes(std::string(cStr));
}
void Model::trainNNCallback(std::shared_ptr<TrainingUIM> trainingForm){
	std::string pyDirPath = "..\\";
	///TODO: a hardcoded dolgokat kiszedni
	if(trainingForm->doPopulateAndConvertBeforeTraining){
		produceDatasFromDir_phase2(trainingForm->sourceDir, trainingForm->populatedDir, trainingForm);
	}
	//4. trainer.py <inputSize> <expOutputSize> <epochs_num> <testDataRatio> <batchSize> [inputFileName = 'trainingData.csv' [<outputFileName ='trainedNetwork.txt'>]]
	sysCommand(
		(
			str("py ")
			+ pyDirPath
			+ str("trainer.py") + str(" ")
			+ trainingForm->inputSize.toStr()  + str(" ")
			+ trainingForm->expOutputSize.toStr() + str(" ")
			+ trainingForm->epochs_num.toStr()  + str(" ")
			+ trainingForm->testDataRatio.toStr()  + str(" ")
			+ trainingForm->batchSize.toStr()  + str(" ")
			+ Quotes(trainingForm->concatenatedCsvFileName.toStr()) + str(" ")
			+ Quotes(trainingForm->trainedNetworkName.toStr()) + str(" ")
			+ Quotes(trainingForm->ttfModelSaveDir.toStr()) + str(" ")
			+ Quotes(trainingForm->ttfModelLoadDir.toStr())
			+ trainingForm->ttfModelDoLoadAndRetrain.toStr()  + str(" ")
			+ str(" > trainLog\n")
			+ str(";pause")
		).c_str()
	);
	/// TODO: python progibol exportálást és a c++ beli importálást könnyebb legyen egymással szinkronizálni amikor az egyik megváltozik.
	/// egy megoldas lehet, hogy egy pzthon progira bizzuk az Drawing-ok felismerését: adunk neki egy pixel-mátrixot és válaszul küldené a háló eredményét(vector<double>)
	/// így sokal könyebb lenne bonyolult Neruális hálókat használni. pl olyat aminke az első rétegei konvolució+pooling, hogy elmossa az input ábrát
	std::cout << "Model::trainNNCallback(std::shared_ptr<TrainingUIM> trainingForm) called" << std::endl;
	//TODO: tensorflow trainert frissiteni "c:\Users\Miki\Documents\progi_peldak\pythonTensorflow\trainer.py" alapjan
}
void Model::CanvasElement_connectToElementAtCanvasCoords(std::weak_ptr<CEB> sourceOwner, QPoint pOnCanv, std::weak_ptr<Plug> srcPlug){
	auto srcObserved = sourceOwner.lock();
	assert(srcObserved != nullptr);
	for(const auto& ce : canvasElements){
		if(ce.get() && ce.get() != srcObserved.get()){
			std::weak_ptr<Connector> conn = ce.get()->tryConnToThis(srcObserved, pOnCanv, srcPlug);
			
			/// ha pOmCanv nem Connector fölött (vagy nem megfelelő Connector fölött) van,
			///  akkor lecsatlakoztatjuk srcPlug-ot az aktuális connectoráról:
			if(!conn.lock()){
				if(auto pO = srcPlug.lock()){
					pO->detach();
					/// TODO: itt kéne lecsatlakoztatni a CEW -et is,
					///  ha azt akarom hogy pontosan akkor legyen csatlakoztatva a CEW mint, amikor a CEB.
					/// most a CEW már a draggolás elején lecsatlakozik, míg a CEB csak a draggolás végén. a LineComponent-re ez nem vonatkozik
				}
			}
		}
	}
}
void Model::fwd_updateCanvas(QRect r){emit updateCanvas(r);}
void Model::fwd_updateCanvas(){emit updateCanvas();}
void Model::fwd_addDotMatrixMiniature(DotMatrix dm, int pos_canBeOverIndexed){
	emit addDotMatrixMiniature(dm, pos_canBeOverIndexed);
}
void Model::fwd_removeDotMatrixMiniature(int pos){
	emit removeDotMatrixMiniature(pos);
}

std::vector<std::weak_ptr<CEB>> getNeigh(std::weak_ptr<CEB> curr){
	std::vector<std::weak_ptr<CEB>> ret;
	for(auto& c : curr.lock()->getConnectors()){
		for(auto& p : c.lock()->connectedPlugs){
			CEB* n = p.lock()->getContainingCE();
			ret.push_back(n->weak_from_this());
		}
	}
	return ret;
}
template<class T>
struct weakEqual{
	std::weak_ptr<T> t;
	std::owner_less<std::weak_ptr<T>> less;
	weakEqual(std::weak_ptr<T> t_) :t(t_)
	{}
	bool operator()(std::weak_ptr<T> o) const{
		return !(less(t,o) || less(o,t));
	}
};

void h(std::vector<std::weak_ptr<CEB>>& explored, std::vector<std::weak_ptr<CEB>>& exploring, std::weak_ptr<CEB> curr){
	for(auto n : getNeigh(curr)){
		std::weak_ptr<CEB> a;
		std::weak_ptr<CEB> b;
		if(std::find_if(explored.begin(), explored.end(), weakEqual<CEB>(n)) == explored.end()){
			if(std::find_if(exploring.begin(), exploring.end(), weakEqual<CEB>(n)) == exploring.end()){
				exploring.push_back(n);
				h(explored, exploring, n);
			}
		}
	}
	exploring.erase(std::find_if(exploring.begin(), exploring.end(), weakEqual(curr)));
	explored.push_back(curr);
}
std::vector<std::weak_ptr<CEB>> Model::getConnectedRecursive(std::weak_ptr<CEB> s){
	std::vector<std::weak_ptr<CEB>> explored;
	std::vector<std::weak_ptr<CEB>> exploring;
	exploring.push_back(s);
	h(explored, exploring, s);
	return explored;
}
int Model::findPosOfCEB(CEB* c){
	assert(c);
	
	int id = c->getId();
	int pos = 0;
	for(pos = 0 ; pos < (int)canvasElements.size() ; ++pos){
		if(canvasElements[pos].get() && canvasElements[pos]->getId() == id){
			break;
		}
	}
	/// ha nem találtuk meg sCEO-t azonosító alapján a canvaselements-ben, akkor hibát dobunk:
	assert(pos < (int)canvasElements.size());
	return pos;
}
void Model::Wid_copySelected(){
	auto sCEO = selectedCE.lock();
	if(!sCEO){
		return;
	}
	/// copied-ba lemásoljuk a kijelölt CE-t:
	copied = sCEO.get()->modellCopy();
	qDebug("DEBUG: Model::Wid_copySelected(): TODO: implement CE copy mechanism");
}
void Model::Wid_pasteSelected(){
	auto sCEO = selectedCE.lock();
	if(!sCEO || !copied){
		return;
	}
	/// lemásoljuk a copied-ba entett CE-t:
	addCanvasElement(copied->modellCopy());
	qDebug("DEBUG: Model::Wid_pasteSelected(): TODO: implement CE paste mechanism");
}
void Model::Wid_undo(){
	undo();
}
void Model::Wid_redo(){
	redo();	
	///vmiert undoredozaskor 
	///		ha mozgatom a CEB-et delete undozasa utan, akkor redo+undo utan az uj helyen jelenik meg a CEB,
	///		pedig elméletileg a régi hely van elmentve a "pre" objektumban
}

void Model::deleteAt(int pos){
	
	///TODO: nem torlodik a tarolt objektum. valszeg lemásolom vhol magat a sharedÜptr-t és ez a baj.
	///Éddemes lehet sajat shared_ptr wrappert írni, ami nem másolható, 
	/// bár van ahol függvényparaméterként sharedÜptrt adok át érték szerint
	/// a move ctort engedem, akkor nem lesz baj a MzSharedPtr paraméterként és visszatérési értékkénti átadásával szerintem
	///		sCEO.reset();
	/// TODO: valamikor felszabadítani az átadott shared_ptr-t?
	
	///kiválasztott elem törlése:
	emit canvasElements.at(pos)->destroyView_CEW();
	canvasElements.at(pos).reset();
	

}
void Model::Wid_deleteSelected(){
	auto connected = getConnectedRecursive(selectedCE);
	for(auto& c : connected){
		assert(c.lock());
		int pos = findPosOfCEB(c.lock().get());
		assert(pos < (int)canvasElements.size());
		int id = canvasElements.at(pos)->getId();
		
		auto pre = canvasElements.at(pos)->modellCopy();
		
		/// TODO: kell egy összetett savedState törléskor, ami menti a törlendő objektum connectionjeit
		deleteAt(pos);
		
		///ha paraméter pack után van még argumentum,
		///  akkor követezteti ki a paraméter pack-et(non-deduced context):
		
		//ModelStateManager::addStateOp(
		//		new URDelCEBStateOp(
		//			urDelCEB,
		//			pos, id, pre->getId(), pre
		//		)
		//);
		//callFuncSaveState(this, &Model::deleteAt, pos);
	}
}
void Model::saveFullModel(){
	std::string saveDir = "diagramTempSaves/";
	std::string filePath = saveDir + "save1";
	std::ofstream ofs(filePath);
	saveCEs(ofs, ofs);
	saveDrawings(ofs, QString(filePath.c_str()));
}
void Model::loadFullModel(){
	std::string loadDir = "diagramTempSaves/";
	std::string filePath = loadDir + "save1";
	std::ifstream ifs(filePath);
	loadCEs(ifs, ifs);
	loadDrawingsc(ifs, true, false);
}

void Model::trainNN(){
	std::shared_ptr<UserInputModel> trainingForm(new TrainingUIM("Mintafileok, tanítás"));
	
	emit askForUserInput(trainingForm, [this](std::shared_ptr<UserInputModel> uim){
			auto tUIM = std::dynamic_pointer_cast<TrainingUIM>(uim);
			assert(tUIM);
			this->trainNNCallback(tUIM);
		});
		//Azt hiszem a python által egy könyvtár végigiterálása az abc sorrendjében halad,
		// ezért abc sorrendben kapják meg a mintaFileok az egység vektorokat - azaz az elvárt kimeneteket.
}
///TODO: ha léteznek a célkönyvtárban a fileok, akkor a másolás sikertelen, a célfájlok ürsek lesznek
void Model::produceDatasFromDir(QString sourceDir, QString destDir){
	std::shared_ptr<UserInputModel> uim(new DrawingPopulationUIM("minták sokszorosítása")); 
	
	emit askForUserInput(uim, [this, sourceDir, destDir](std::shared_ptr<UserInputModel> uim){
		auto casted = std::dynamic_pointer_cast<DrawingPopulationUIM>(uim);
		assert(casted);
		this->produceDatasFromDir_phase2(sourceDir, destDir, casted);
	});
}
///TODO: külön fileba kiírni, hogy mekkora drawing.cells-ből készültek a tanítási minták
//a könyvtárban csak .csv fileok, és a drawing mentések lehetnek
void Model::produceDatasFromDir_phase2(QString sourceDir, QString destDir, std::shared_ptr<DrawingPopulationUIM> uim){
	if(sourceDir == destDir){
		persistence->l->warn("WARNING: Model::produceDatasFromDir_phase2(source, dest): source equals dest dir. doing nothing. change dest dir");
		return;
	}
	int categNum = persistence->getDrawingCategoriesNumInDir(sourceDir);
	
	auto u = uim.get();
	///be lehet állitani, hogy a dir-iterátor subdirectory-kba is bele menjen(egy DirIterator flag-el), de ezt most nem használom
	persistence->startIterationInDir(sourceDir, "csv");
	Drawing* d = nullptr;
	std::ofstream populated(u->concatenatedCsvFileName.toStr());
	int categInd = 0;
	std::vector<double> expectedVals(categNum, 0);
	
	while(persistence->nextDrawingCategoryInDir() ){
		if(categInd > 0)
			expectedVals.at(categInd-1) = 0;
		expectedVals.at(categInd) = 1.0;
		std::ifstream is;
		std::ofstream os;
		
		QString destinationFileName = destDir + "/" + persistence->getActualFileName();
		os.open(destinationFileName.toStdString(), std::ios_base::out);
		///elkészítjük a módosított Drawing-okat az aktuális file alapján, és mentjük őket:
		int copyNum = u->populationSize  / std::max(persistence->getDrawingNumInCategory(resHorizontal, resVertical), 1);
		if(copyNum == 0)
			copyNum = 1;
		while(  ( d = persistence->nextDrawingInCategory(resHorizontal, resVertical) )  ){
			for (int i = 0; i < copyNum ; ++i) {
				///a drawing* klónozása:
				Drawing* dCopy = d->copyModifyDrawing(
						u->pointModificationRate, 
						u->minRangeRatio,
						u->maxRangeRatio,
						u->minDeviation,
						u->maxDeviation, 
						u->maxAmplitudeMultiplier, 
						u->minScaleX, 
						u->maxScaleX, 
						u->minScaleY,
						u->maxScaleY, 
						u->scaleChance
				);
				dCopy->print(os, true);
				dCopy->exportCellsAsLearningData(populated, expectedVals);
				delete dCopy;
			}
			delete d;	
		}
		is.close();
		os.close();
		++categInd;
	}
	emit showMessage("A '" + sourceDir + "' beli adatok sokszorosítása befejeződött");
	populated.close();
}
void Model::categorizeDrawingCreateCanvaElement(Drawing* d){
	//DrawingsInd lastInd = drawings.size()-1;
	if(!testNetwork)
		assert(false);
	auto CEType = dc->classify(d);
	showMessage(
		QString::fromStdString("a neurális háló szerint az ábra: ").append(CEType)
	);
	
	{
	std::shared_ptr<CEB> uml = nullptr;
	/// TODO: UmlCircle helyett interface requirement-és providing jelölő bevezetése
	switch (CEType) {
	case TEXTBOXT:{uml=std::make_shared<TextComponent>(d->getBoundingRect());break;}
	case CIRCLET:{uml=std::make_shared<UMLCircle>(d->getBoundingRect());break;}
	case COMPOSITIONT:{uml=std::make_shared<UMLCompositionSpecifier>(d->getBoundingRect());break;}
	case AGGREGATIONT:{uml=std::make_shared<UMLAggregationSpecifier>(d->getBoundingRect());break;}
	case UMLCLASST:{uml=std::make_shared<UMLClassComponent>(d->getBoundingRect());break;}
	case EXTENSIONT:{uml=std::make_shared<UMLExtensionSpecifier>(d->getBoundingRect());break;}
	default:{uml=nullptr;qDebug()<<"nem sikerült felismerni az ábrát\n"; break;}
	}
	assert(uml);
	addCanvasElement(std::move(uml));
	/// az uml változó megszűnik, de át move-oltuk már
	}
	//todo ujratanitani
}
void Model::pushLastDrawing(){
	///TODO: ellenőrzés
	auto preState = drawings;
	//std::function<void()> redo = [this](){
		lastDrawing->updateCells(false);//azért false, mert a Drawing szélsőértékeit minden olyan eljárás frissíti, ami módosítja a Drawing gesture-eit
		drawings.push_back(lastDrawing);
		int pos = drawings.size() - 1;
		emit addDotMatrixMiniature(drawings[pos]->getCells(), pos);
		emit updateCanvas(lastDrawing->getBoundingRect());
	//};
	//std::function<void()> undo = [this](){
	//	drawings.pop_back();
	//	int pos = drawings.size();
	//	emit removeDotMatrixMiniature(pos);
	//	emit updateCanvas(lastDrawing->getBoundingRect());
	//};
	//redo();
	//ModelStateManager::addStateOp(
	//	new URSigStateOp(
	//		//preState,
	//		//drawings, 
	//		//&drawings,
	//		urSignals, drawings[pos]->getCells(), pos
	//	)
	//);
	//ModelStateManager::addStateOp(new ModelStateOp_function(undo, redo));
}
/// minden olyan drawing-nak aminek az elmosása, vagy margója különbözik a modelben beállítottól, a model alapján beállítja azt,
///		és ezután update-eli a drawing-ot
void Model::discardDrawings(){
	for(uint i = 0 ; i < drawings.size() ; ++i){
		assert(nullptr != drawings[i]);
		auto temp = drawings[i];
		//először null-ra állítom a tömbelemet, és csak aztán szabdítom fell a mutatott memóriaterületet, 
		// hogy a tömböt használó nézetbeli objektumok ne hivatkozhassanak a felszabadított tömbelemre miközben itt felszabadítjuk azokat:
		drawings[i] = nullptr;
		delete temp;
	}
	drawings.resize(0);
	emit removeDrawingDotMatrixMiniatures();
	emit updateCanvas();
}
//drawingDotMaterices nem frissul betolteskor rogton. kijavitani
void Model::discardCanvasElements(){
	for(uint i = 0 ; i < canvasElements.size() ; ++i){
		if(nullptr == canvasElements[i])
			continue;
		canvasElements[i].reset();
		
		

	}
	canvasElements.resize(0);
	emit removeDrawingDotMatrixMiniatures();
	emit removeCanvasElements();
	emit updateCanvas();
}


	
