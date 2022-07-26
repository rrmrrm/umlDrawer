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
	ui->setupUi(this);
	setWindowTitle(uim.get()->title);
	bool hasUIMdefaultValues;
	///TODO: nem hiszem hogy a nezetben van ennek a helye, de nagyon kenyelmes ide tenni:
	hasUIMdefaultValues = uim->readDataFromFile();
	
	
	//std::vector<BindingLineEdit*> inputFields;
	connect( ui->acceptButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( ui->rejectButton, SIGNAL(clicked()), this, SLOT(reject()) );
	
	for(std::map<QString, My::Type*>::iterator uim_it = uim.get()->inputs.begin(); uim_it != uim.get()->inputs.end() ; ++uim_it){
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
	///TODO: nem hiszem hogy a nezetben van ennek a helye, de nagyon kenyelmes ide tenni:
	uim->saveDataToFile();
	//nincs hasznalatban ez a signal, de azert kivaltjuk:
	emit done(QDialog::Accepted);
	
	callback(uim);
	delete this;
}
void UserInputDialog::reject(){
	//nincs hasznalatban ez a signal, de azert kivaltjuk:
	emit done(QDialog::Rejected);
	
	this->hide();
	delete this;
}
/*
template <typename Func1, typename Func2>
static inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 0, QMetaObject::Connection>::type
		connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
{
	return connect(sender, signal, sender, slot, Qt::DirectConnection);
}
*/

BindingLineEdit::BindingLineEdit(My::Type* _boundVar, bool isBoundVarSetup, QWidget* _parent): 
		QLineEdit(_parent),
		parent(_parent),
		boundVar(_boundVar)
{
	//dialog = nullptr;
	connect( this, SIGNAL(editingFinished()), this, SLOT(updateBoundVariable()));
	if( dynamic_cast<My::Integer*>(boundVar) ){
		int botVal = std::numeric_limits<int>::min();
		int topVal = std::numeric_limits<int>::max();
		setValidator( new QIntValidator(botVal, topVal, parent) );
	}
	if( dynamic_cast<My::Double*>(boundVar) ){
		double botVal = std::numeric_limits<double>::min();
		double topVal = std::numeric_limits<double>::max();
		int digitsNum = 8;
		setValidator( new QDoubleValidator(botVal, topVal, digitsNum, parent) );
	}
	if( dynamic_cast<My::FileName*>(boundVar) || dynamic_cast<My::DirName*>(boundVar) ){
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
	if( dynamic_cast<My::String*>(boundVar) ){
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
		if(QValidator::State::Acceptable == validator()->validate(defText, dummy)){
			setText(defText);
		}
		else{
			std::cerr << "WARNING: BindingLineEdit::BindingLineEdit(..): a field had invalid default value" << std::endl;
		}
	}
	
	
	setEnabled(true);
}
void BindingLineEdit::mouseDoubleClickEvent(QMouseEvent * e){
	if(dynamic_cast<My::FileName*>(boundVar)){
		QString fileName = QFileDialog::getOpenFileName(this, "file kiválasztása");
		if(!fileName.isEmpty() && fileName != ""){
			setText(fileName);
			updateBoundVariable();
		}
		//dialog->open();
	}
	if(dynamic_cast<My::DirName*>(boundVar)){
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
		if( auto casted = dynamic_cast<My::Integer*>(boundVar) ){
			casted->wrappedVal = this->text().toInt();
		}
		if( auto casted = dynamic_cast<My::Double*>(boundVar) ){
			casted->wrappedVal = this->text().toDouble();
		}
		if( auto casted = dynamic_cast<My::FileName*>(boundVar) ){
			casted->wrappedVal = this->text();
		}
		if( auto casted = dynamic_cast<My::DirName*>(boundVar) ){
			casted->wrappedVal = this->text();
		}
		if( auto casted = dynamic_cast<My::String*>(boundVar) ){
			casted->wrappedVal = this->text();
		}
	}
	else{
		
	}
}
