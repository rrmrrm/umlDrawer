#include "userinputdialog.h"
#include "ui_userinputdialog.h"

#include <QLineEdit>
#include <QValidator>
#include <QLabel>
#include <QFileDialog>

#include <vector>

UserInputDialog::UserInputDialog(std::shared_ptr<UserInputModel> _uim, std::function<void (std::shared_ptr<UserInputModel>)> _callback, QWidget* parent):
	QDialog(parent),
	ui(new Ui::UserInputDialog),
	uim(_uim),
	callback(_callback),
	widgetState(RUNNING)
{
	// aztán az utoljára lerajzolt ábra megfelelő fileba mentését is megcsinálni(elkezdtem). ehhez annyit, hogy ha csak párat mentek el közölük a filelokba, aztán
	//	az összeset elmentem, egyszerres utána, akkor lesznek duplikátumok, ezeket érdemes lehet szűrni 
	ui->setupUi(this);
	setWindowTitle(uim.get()->title);
	bool hasUIMdefaultValues;
	
	hasUIMdefaultValues = uim->hasUIMdefaultValues;
	uim.get();
	
	//std::vector<BindingLineEdit*> inputFields;
	connect( ui->acceptButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( ui->rejectButton, SIGNAL(clicked()), this, SLOT(reject()) );
	
	for(std::map<QString, UIM::Type*>::iterator uim_it = uim.get()->getInputs().begin(); uim_it != uim.get()->getInputs().end() ; ++uim_it){
		auto label = new QLabel(uim_it->first,this);
		auto lineEd = new BindingLineEdit(uim_it->second, hasUIMdefaultValues,  this);
		ui->fieldLayout->addWidget(label);
		ui->fieldLayout->addWidget(lineEd);
	}
	assert(layout());
	//setEnabled(true);
}

UserInputDialog::~UserInputDialog()
{
	disconnect( ui->acceptButton, SIGNAL(clicked()), this, SLOT(accept()) );
	disconnect( ui->rejectButton, SIGNAL(clicked()), this, SLOT(reject()) );
	
	delete ui;
}

void UserInputDialog::accept(){
	///TODO: erre már nincs itt szükség, ugye?:
	uim->saveDataToFile();
	//nincs hasznalatban ez a signal, de azert kivaltjuk:
	emit done(QDialog::Accepted);
	
	callback(uim);
	//delete this;
}
void UserInputDialog::reject(){
	//nincs hasznalatban ez a signal, de azert kivaltjuk:
	emit done(QDialog::Rejected);
	
	this->hide();
	//delete this;
}


BindingLineEdit::BindingLineEdit(UIM::Type* _boundVar, bool isBoundVarSetup, QWidget* _parent): 
		QLineEdit(_parent),
		parent(_parent),
		boundVar(_boundVar)
{
	//dialog = nullptr;
	connect( this, SIGNAL(editingFinished()), this, SLOT(updateBoundVariable()));
	if( dynamic_cast<UIM::Integer*>(boundVar) ){
		int botVal = std::numeric_limits<int>::min();
		int topVal = std::numeric_limits<int>::max();
		setValidator( new QIntValidator(botVal, topVal, parent) );
	}
	if( dynamic_cast<UIM::Double*>(boundVar) ){
		double botVal = std::numeric_limits<double>::min();
		double topVal = std::numeric_limits<double>::max();
		int digitsNum = 8;
		setValidator( new QDoubleValidator(botVal, topVal, digitsNum, parent) );
	}
	if( dynamic_cast<UIM::FileName*>(boundVar) || dynamic_cast<UIM::DirName*>(boundVar) ){
		//connect( dialog, SIGNAL(accept()), this, SLOT(fileDialogAccepted()) );
		//connect( dialog, SIGNAL(reject()), this, SLOT(fileDialogRejected()) );
		//dialog = new QDialog(this);
		setMouseTracking(true);
		//connect(this, SIGNAL(mouseDoubleClickEvent), this, SLOT())
		setValidator(
				new QRegularExpressionValidator(
					QRegularExpression(".{0,400}"),
					parent
				)
		);
	}
	if( dynamic_cast<UIM::String*>(boundVar) ){
		setValidator(
				new QRegularExpressionValidator(
					QRegularExpression(".{0,240}"),
					parent
				)
		);
	}
	///ha kapott a boundvar erteket(isBoundVarSetup), azt csak akkor
	/// allitjuk be, ha atmegy a validacion:
	int dummy = 0;
	QString defText = QString::fromStdString(boundVar->toStr());
	if(validator() && isBoundVarSetup){
		if(validator()->validate(defText, dummy) == QValidator::State::Acceptable){
			setText(defText);
		}
		else{
			std::cerr << "WARNING: BindingLineEdit::BindingLineEdit(..): a field had invalid default value" << std::endl;
		}
	}
	
	
	setEnabled(true);
}
void BindingLineEdit::mouseDoubleClickEvent(QMouseEvent * e){
	(void)e;
	if(dynamic_cast<UIM::FileName*>(boundVar)){
		QString fileName = QFileDialog::getOpenFileName(this, "file kiválasztása");
		if(!fileName.isEmpty() && fileName != ""){
			setText(fileName);
			updateBoundVariable();
		}
		//dialog->open();
	}
	if(dynamic_cast<UIM::DirName*>(boundVar)){
		QString dirName = QFileDialog::getExistingDirectory(this, "könyvtár kiválasztása");
		if(!dirName.isEmpty() && dirName != ""){
			setText(dirName);
			updateBoundVariable();
		}
		
	}
}
//fileDialogAccepted()
//fileDialogRejected()
BindingLineEdit::~BindingLineEdit(){
	disconnect( this, SIGNAL(editingFinished()), this, SLOT(updateBoundVariable()));
}

void BindingLineEdit::updateBoundVariable(){
	if(hasAcceptableInput()){
		if( auto casted = dynamic_cast<UIM::Integer*>(boundVar) ){
			casted->wrappedVal = this->text().toInt();
		}
		if( auto casted = dynamic_cast<UIM::Double*>(boundVar) ){
			casted->wrappedVal = this->text().toDouble();
		}
		if( auto casted = dynamic_cast<UIM::FileName*>(boundVar) ){
			casted->wrappedVal = this->text();
		}
		if( auto casted = dynamic_cast<UIM::DirName*>(boundVar) ){
			casted->wrappedVal = this->text();
		}
		if( auto casted = dynamic_cast<UIM::String*>(boundVar) ){
			casted->wrappedVal = this->text();
		}
	}
	else{
		
	}
}
