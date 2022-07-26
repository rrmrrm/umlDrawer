#include "savelastdrawingwidget.h"
#include "ui_savelastdrawingwidget.h"

#include <QFileDialog>
#include <QMouseEvent>
DClickableButton::DClickableButton(QString title, QWidget* parent):
		QPushButton(title, parent)
{
	
}
  
DClickableButton::~DClickableButton(){
	
}

void DClickableButton::mousePressEvent(QMouseEvent *event){
	event->accept();
	if(event->button() == Qt::LeftButton){
		event->accept();
		emit leftClicked();
	}
	else if(event->button() == Qt::RightButton){
		event->accept();
		emit rightClicked();
	}
	else event->ignore();
}

SaveLastDrawingWidget::SaveLastDrawingWidget(
			std::weak_ptr<GenericFileUIM> dataSaveModel_, 
			QWidget *parent
		) :
	QWidget(parent),
	ui(new Ui::SaveLastDrawingWidget),
	dataSaveModel(dataSaveModel_)
{
	ui->setupUi(this);
	connect(ui->OpenDataFileButton, SIGNAL(clicked()), this, SLOT(accesAnotherDataFile()));
	if(auto dsmObs = dataSaveModel.lock()){
		for(auto file : dsmObs->getInputs()){
			addButton(file.first);
		}	
	}
}

SaveLastDrawingWidget::~SaveLastDrawingWidget()
{
	delete ui;
}

bool SaveLastDrawingWidget::accessFileAddButton(QString filePath){
	if(auto mObs = dataSaveModel.lock()){
		QString fNameReadable = mObs->addInput(filePath);
		if(!fNameReadable.isEmpty() && fNameReadable!=""){
			addButton(fNameReadable);
			return true;
		}
	}
	return false;
}
void SaveLastDrawingWidget::addButton(QString fNameReadable){
	if(!fNameReadable.isEmpty() && fNameReadable!=""){
		auto newButton = new DClickableButton(fNameReadable, this);
		saveButtons.push_back(newButton);
		connect(newButton, &DClickableButton::leftClicked, 
				this, &SaveLastDrawingWidget::fileButtonClicked
			);
		connect(newButton, &DClickableButton::rightClicked, 
				this, &SaveLastDrawingWidget::removeDataFileButton
			);
		ui->DataFileButtonsLayout->addWidget(newButton);
	}
}

void SaveLastDrawingWidget::fileButtonClicked(){
	if(QPushButton* src = dynamic_cast<QPushButton*>(sender())){
		if(!src->text().isEmpty() && src->text() != ""){
			if(auto mObs = dataSaveModel.lock()){
				if(mObs->saveLastDrawingToDataFile(src->text())){
					emit showMessage( 
						"Az utols= ábra ábra sikeresen mentve a " + src->text() + " fileba."
						);
					return;
				}
			}
		}
	}
	emit showMessage("Az utolsó ábra mentése sikertelen");
}
void SaveLastDrawingWidget::accesAnotherDataFile(){
	QString filePath = QFileDialog::getOpenFileName(this, "tanítóminta-file választása");
	if(filePath == "" || filePath.isEmpty()){
		emit showMessage("A file megnyitás sikertelen, mert üres a filenév!");
		return;
	}
	accessFileAddButton(filePath);
}
void SaveLastDrawingWidget::removeDataFileButton(){
	if(QPushButton* src = dynamic_cast<QPushButton*>(sender())){
		if(!src->text().isEmpty() && src->text() != ""){
			if(auto mObs = dataSaveModel.lock()){
				if(mObs->removeInput(src->text())){
					delete src;
					return;
				}
			}
		}
	}
	emit showMessage("A gomb törléae sikertelen");
}
