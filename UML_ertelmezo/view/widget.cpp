#include "view/widget.h"
#include "network/network.h"
#include "savelastdrawingwidget.h"
#include "userinputdialog.h"
#include "common.hpp"
#include "model/CanvasElements/canvaselement.h"
#include "ui_widget.h"


#include <QObject>
#include <QString>
#include <QSpinBox>
#include <QDirIterator>
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
	#include <QRandomGenerator>
#else
	#include <qrandomgenerator.h>
#endif
#include <QMouseEvent>
#include <QFileDialog>
#include <QScrollArea>
#include <QPixmap>
#include <QMessageBox>
#include <QInputDialog>
#include <QTime>
#include <QAction>
#include <QAction>
#include <QToolBar>

#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>
#include <memory>


Widget::Widget(QWidget *parent) :
	QWidget(parent),
	m(new Model),
    wid(800),
	
	canvas(
		new Canvas(
			1000, 
			m,
			this
		)
	),
	dWidth(100),
	imageLabel(new QLabel),
	drawingsPixmap(new QPixmap(QSize(dWidth,20000))),
	ui(new Ui::Widget)
{
	ui->setupUi(this);
	ui->painterLayout->addWidget(canvas);
	setFixedHeight(1000);

	
	///TODO: felszabaditani ezeket:
	drawingsPixmap->fill(Qt::black);
	///draw on pixmap
	QPainter drawingMiniaturesPainter(drawingsPixmap);
	drawingMiniaturesPainter.setPen(QColor(255,20,10));

	imageLabel->setPixmap(*drawingsPixmap);
	ui->imagesArea->setBackgroundRole(QPalette::Dark);
	ui->imagesArea->setWidget(imageLabel);
	
	
	saveLDW = new SaveLastDrawingWidget(m->getSaveLastDrawingModel(), this);
	ui->saveLastDrawingLayout->addWidget(saveLDW);		
	
	connect( ui->saveCEs, SIGNAL(clicked()), this, SLOT(saveCEs()));
	connect( ui->loadCEs, SIGNAL(clicked()), this, SLOT(loadCEs()));
	connect( ui->saveDrawings, SIGNAL(clicked()), this, SLOT(saveDrawings()));
	connect( ui->loadDrawingsCreateCanvasElements, SIGNAL(clicked()), this, SLOT(loadDrawingsCreateCanvasElements()));
	connect( ui->populateDataFromDir, SIGNAL(clicked()), this, SLOT(produceDatasFromDir()));
	connect( ui->trainButton, SIGNAL(clicked()), m, SLOT(trainNN()));
	
	connect( m, SIGNAL(updateCanvas(QRect)), canvas, SLOT(Update(QRect)));
	connect( m, SIGNAL(updateCanvas()), canvas, SLOT(Update()));
	connect(
		m,
		SIGNAL(askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)>)),
		this,
		SLOT(askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)>))
	);
	connect(m, SIGNAL(showMessage(QString)),this, SLOT(showMessage(QString)));
	connect(
		m, 
		SIGNAL(addDotMatrixMiniature(DotMatrix, int)), 
		this, 
		SLOT(addDotMatrixMiniature(DotMatrix, int))
	);
	connect(
		m, 
		SIGNAL(removeDotMatrixMiniature(int)), 
		this, 
		SLOT(removeDotMatrixMiniature(int))
	);
	connect(
		m, 
		SIGNAL(removeDrawingDotMatrixMiniatures()), 
		this, 
		SLOT(removeDrawingDotMatrixMiniatures())
	);
	connect(
		m, 
		SIGNAL(canvasElementAdded(
				   std::weak_ptr<CEB>
			   )
		), 
		canvas, 
		SLOT(Model_CanvasElementAdded(
				std::weak_ptr<CEB>
			)
		)
	);
	connect(
		m,
		SIGNAL(removeCanvasElements()),
		canvas,
		SLOT(removeCanvasElementWidgets())
	);
	/// TODO: undo, redo action-t nem érzékeli a program
	QAction* undoAction = new QAction(this);
	undoAction->setShortcut(QKeySequence::Undo);
	connect(undoAction, &QAction::triggered, this, &Widget::undo);
	QAction* redoAction = new QAction(this);
	redoAction->setShortcut(QKeySequence::Redo);
	connect(redoAction, &QAction::triggered, this, &Widget::redo);
	
	connect(ui->undo, SIGNAL(clicked()), this, SLOT(undo()));
	connect(ui->redo, SIGNAL(clicked()), this, SLOT(redo()));
	setEnabled(true);
	
	toolBar = new QToolBar(tr("&tools"), this);
	addActions();
	ui->toolBarLayout->addWidget(toolBar);
	/*
	std::weak_ptr<CEB> cl1w;
	std::weak_ptr<CEB> cl2w;
	{
	auto cl1 = m->createCanvasElement(
		CanvasElement_Type::UMLCLASS,
		QRect(0,500,100,500)
	);
	auto cl2 = m->createCanvasElement(
		CanvasElement_Type::UMLCLASS,
		QRect(500,500,500,800)
	);
	cl1w = cl1;
	cl2w = cl2;
	m->addCanvasElement(std::move(cl1));
	
	
	
	m->addCanvasElement(std::move(cl2));
	/// cl1 és cl2 itt felszabadul, de már move-oltuk azokat:
	}
	
	/// a két umlClass (cl1 és cl2) közé húzok egy törtvonalat:
	{
	auto l = m->createCanvasElement(
		CanvasElement_Type::LINE,
		QRect(400,400,100,100),
		ERROR_RT,
		Target(98, 100, cl1w),
		Target(1, 200, cl2w)
	);
	m->addCanvasElement(std::move(l));
	/// l itt felszabadul, de már move-oltuk:
	}
	m->addCanvasElement(
		m->createCanvasElement(
			CanvasElement_Type::TEXTBOX,
			QRect(100,0,100,100)
	));
	
	m->addCanvasElement(
		m->createCanvasElement(
			CanvasElement_Type::RELATIONSHIP,
			QRect(0,300,100,100),
			Relationship_Type::CIRCLE
	));
	m->addCanvasElement(
		m->createCanvasElement(
			CanvasElement_Type::RELATIONSHIP,
			QRect(100,300,100,100),
			Relationship_Type::EXTENSION
	));
	m->addCanvasElement(
		m->createCanvasElement(
			CanvasElement_Type::RELATIONSHIP,
			QRect(200,300,100,100),
			Relationship_Type::AGGREGATION
	));
	*/
	m->addCanvasElement(
		m->createCanvasElement(
			CanvasElement_Type::TEXTBOX,
			QRect(100,0,100,100)
	));
	
	
}
Widget::~Widget()
{
	//delete canvas;
	//delete ui;
}

void Widget::addActions(){
	QAction *copyAct = new QAction(tr("&copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy selected element"));
	connect(copyAct, &QAction::triggered, this, &Widget::copySelected);
	toolBar->addAction(copyAct);
	
	QAction *pasteAct = new QAction(tr("&paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("&Paste copied element"));
	connect(pasteAct, &QAction::triggered, this, &Widget::pasteSelected);
	toolBar->addAction(pasteAct);
	
	
	QAction *undoAct = new QAction(tr("&undo"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("&undo modification"));
	connect(undoAct, &QAction::triggered, this, &Widget::undo);
	toolBar->addAction(undoAct);
	
	QAction *redoAct = new QAction(tr("&redo"), this);
	redoAct->setShortcuts(QKeySequence::Redo);
	redoAct->setStatusTip(tr("&redo undone modification"));
	connect(redoAct, &QAction::triggered, this, &Widget::redo);
	toolBar->addAction(redoAct);
	
	
	QAction *delAct = new QAction(tr("&delete"), this);
	delAct->setShortcuts(QKeySequence::Delete);
	delAct->setStatusTip(tr("&delete selected element"));
	connect(delAct, &QAction::triggered, this, &Widget::deleteSelected);
	toolBar->addAction(delAct);
	
	
	QAction *saveAct = new QAction(tr("&save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("&save diagram"));
	connect(saveAct, &QAction::triggered, this, &Widget::saveDiagram);
	toolBar->addAction(saveAct);
	
	QAction *loadAct = new QAction(tr("&load"), this);
	loadAct->setShortcuts(QKeySequence::Save);
	loadAct->setStatusTip(tr("&load diagram"));
	connect(loadAct, &QAction::triggered, this, &Widget::loadDiagram);
	toolBar->addAction(loadAct);
}

void Widget::copySelected(){
	m->Wid_copySelected();
}
void Widget::pasteSelected(){
	m->Wid_pasteSelected();
}
void Widget::undo(){
	m->Wid_undo();
}
void Widget::redo(){
	m->Wid_redo();
}
void Widget::deleteSelected(){
	m->Wid_deleteSelected();
}
void Widget::saveDiagram(){
	m->saveFullModel();
}
void Widget::loadDiagram(){
	m->loadFullModel();
}

void Widget::askForUserInput(std::shared_ptr<UserInputModel> uim, std::function<void (std::shared_ptr<UserInputModel>)> callback){
	
	QWidget* parent = this;
	
	UserInputDialog dialog(uim, callback, parent);
	dialog.exec();
}
void Widget::addDotMatrixMiniature(DotMatrix drawing, int pos_canBeOverIndexed){
	assert(pos_canBeOverIndexed >= 0);
	if(pos_canBeOverIndexed >= (int)drawingDotMatrices.size()){
		drawingDotMatrices.resize(pos_canBeOverIndexed + 1);
	}
	drawingDotMatrices[pos_canBeOverIndexed] = drawing;
	updateDrawingDotMatrixMiniatures();
}
void Widget::removeDotMatrixMiniature(int pos_canBeOverIndexed){
	assert(pos_canBeOverIndexed >= 0);
	if(pos_canBeOverIndexed >= (int)drawingDotMatrices.size()){
		return;
	}
	drawingDotMatrices.erase(drawingDotMatrices.begin()+pos_canBeOverIndexed);
	updateDrawingDotMatrixMiniatures();
}
void Widget::updateDrawingDotMatrixMiniatures(){
	///képek megjelenítése kicsinyítve :
	///TODO: felszabaditani ezeket:
	drawingsPixmap->fill(Qt::black);
	///draw on pixmap
	QPainter drawingMiniaturesPainter(drawingsPixmap);
	drawingMiniaturesPainter.setPen(QColor(255,20,10));
	double translation = 0;
	/// TODO: visszavonás után valahogy rossz indexel adódnak hozzá az új dotmatrixokminiatűrök
	///  itt emiatt egy üres dotmatrix nem létező a második axisának méretét próbáljuk lekérdezni(segfault):
	for(uint cnt = 0 ; cnt < drawingDotMatrices.size() ; ++cnt){
		double scale = drawingDotMatrices[cnt].size() > drawingDotMatrices[cnt][0].size() ?
					dWidth / (double)drawingDotMatrices[cnt].size() :
					dWidth / (double)drawingDotMatrices[cnt].at(0).size();
		if(translation > drawingsPixmap->height()){
			std::cerr << "WARNING: Widget::Widget(..): miniature drawings exceed the maximal size! some won't appear" << std::endl;
			break;
		}
		drawingMiniaturesPainter.drawText(QPointF(0,translation),QString(std::to_string(cnt+1).c_str()));
		for (uint i = 0; i < drawingDotMatrices[cnt].size(); ++i) {//horizontal direction
			for (uint j = 0; j < drawingDotMatrices[cnt][0].size(); ++j) {//vertical direction
				int x0 = (int)(i)*scale;
				int y0 = translation + (int)(j)*scale;
				int w = scale;
				int h = scale;
				double val = drawingDotMatrices[cnt][i][j];
				QColor col(val*255, val*255, val*255);
				QBrush brush(col);
				drawingMiniaturesPainter.fillRect(x0,y0, w,h, brush);
			}
		}
		translation += drawingDotMatrices[cnt][0].size()*scale;
	}
	imageLabel->setPixmap(*drawingsPixmap);
	ui->imagesArea->setBackgroundRole(QPalette::Dark);
	ui->imagesArea->setWidget(imageLabel);
}
void Widget::resetLabelState(){
	auto l = ui->messageLabel;
	l->setStyleSheet("color: red; background-color: transparent");
	l->setEnabled(false);
	l->setText("");
}
void Widget::showMessage(QString msg){
	auto l = ui->messageLabel;
	l->setEnabled(true);
	l->setStyleSheet("color: red; background-color: black");
	l->setText(msg);
	//egy idő után eltüntetjük az üzenetet a resetLabelState() eljárással: 
	QTimer t;
	int showMessageInterval = 10000;
	t.singleShot(
		showMessageInterval, 
		[this]{
			resetLabelState();
		}
	);
}
void Widget::keyPressEvent(QKeyEvent* e){
	if(e->key() == Qt::Key::Key_Return){
		m->widget_enterPressed();
	}
	else{
		return QWidget::keyPressEvent(e);
	}
}
void Widget::showNeuralNetworkResult(std::vector<double> result){
	std::cout 
			<< "a neurális háló ezt az eredményt adta: "
			<< std::endl
			<< result;
}
void Widget::prepareForClosing(){
	//az alapértelmezett helyre mentjük a drawing-okat:
	m->saveDrawings();
}
void Widget::messagePopup(std::string msg){
	QMessageBox qmb;
	qmb.setText(QString::fromStdString(msg));
	qmb.exec();
}

void Widget::M_save(std::ostream& os){
	canvas->save(os);
//	canvas->saveAggregations(os);
}
void Widget::M_load(std::istream& is){
	canvas->load(is);
//	canvas->loadAggregations(is);
	
}
//std::ostream& Widget::saveAggregations(std::ostream& os){
//	canvas->saveAggregations(os);
//	
//}
//std::istream& Widget::loadAggregations(std::istream& is){
//	canvas->loadAggregations(is);
//	
//}
void Widget::produceDatasFromDir(){
	std::cout << "produceDatasFromDir()" << std::endl;
	QString sourceDir = QFileDialog::getExistingDirectory(this, tr("Choose Source directory"), "");
	std::cout << "sourceDir: " << sourceDir.toStdString() << std::endl;
	QString destDir = QFileDialog::getExistingDirectory(this, tr("Choose Destination directory"), "");
	std::cout << "destinationDir: " << destDir.toStdString() << std::endl;
	
	m->produceDatasFromDir(sourceDir, destDir);
}
void Widget::trainNN(){
	m->trainNN();
}
void Widget::saveCEs(){
	QString file = QFileDialog::getSaveFileName(this, "rajzok mentése");
	if(file != "" || !file.isEmpty()){
		m->saveCEs(file);
	}
	else{
		showMessage("Sikertelen a mentés, mert üres a filenév!");
	}
}
void Widget::loadCEs(){
	QString file = QFileDialog::getOpenFileName(this, "rajzok betöltése");
	if(file != "" || !file.isEmpty()){
		m->loadCEs(file);
	}
	else{
		showMessage("Sikertelen a betöltés, mert üres a filenév!");
	}
}
void Widget::saveDrawings(){
	QString file = QFileDialog::getSaveFileName(this, "rajzok mentése");
	if(file != "" || !file.isEmpty()){
		m->saveDrawings(file);
	}
	else{
		showMessage("Sikertelen a mentés, mert üres a filenév!");
	}
}
void Widget::loadDrawingsCreateCanvasElements(){
	QString file = QFileDialog::getOpenFileName(this, "rajzok betöltése");
	if(file != "" || !file.isEmpty()){
		m->loadDrawingsc(file, true);
	}
	else{
		showMessage("Sikertelen a betöltés, mert üres a filenév!");
	}
}
void Widget::removeDrawingDotMatrixMiniatures(){
	drawingDotMatrices.resize(0);
}
