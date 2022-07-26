#include "textwidget.h"
#include "model/plug.h"
#include "connwidget.h"

#include <QKeyEvent>
#include <QPainterPath>
#include <QVBoxLayout>

MyTextEdit::MyTextEdit(int charSize_, int letterSpacing, QWidget* parent): 
	QTextEdit(parent),
	charSize(charSize_)
{
	setWordWrapMode(QTextOption::NoWrap);
	//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumHeight(fontPointSize());
	QFont font = QFont(
		fontFamily(),
		charSize, 
		-1, 
		false
	);
	font.setLetterSpacing(QFont::PercentageSpacing, letterSpacing);
	setCurrentFont(font);
	setFont(font);
	qDebug() <<fontPointSize();
	connect( this, SIGNAL( textChanged() ), this, SLOT( onTextChanged() ) );
	setText("a");
}
void MyTextEdit::onTextChanged(){
	QSize size = document()-> size().toSize();
	qDebug() <<fontPointSize();
	int textMargin = 10;
	if(size.height() < charSize + textMargin)
		setMaximumHeight(charSize + textMargin);
	else
		setMaximumHeight( size.height() + 3 );
}
void MyTextEdit::mousePressEvent(QMouseEvent *event){
	QTextEdit::mousePressEvent(event);
	if(parent()){
		if(auto pcew = dynamic_cast<CEW_Base*>(parent())){
			pcew->mousePressEvent(event);
		}
	}
}
void MyTextEdit::mouseMoveEvent(QMouseEvent *event){
	QTextEdit::mouseMoveEvent(event);
	if(parent()){
		if(auto pcew = dynamic_cast<CEW_Base*>(parent())){
			pcew->mouseMoveEvent(event);
		}
	}
}
void MyTextEdit::mouseReleaseEvent(QMouseEvent *event){
	QTextEdit::mouseReleaseEvent(event);
	if(parent()){
		if(auto pcew = dynamic_cast<CEW_Base*>(parent())){
			pcew->mouseReleaseEvent(event);
		}
	}
}

TextWidget::TextWidget(std::shared_ptr<TextComponent> tc_, QWidget* parent) :
		CEW_Base(tc_, parent),
		
		tc(tc_)
{
	assert(ce.lock());
	tc = std::static_pointer_cast<TextComponent>(ce.lock());
	auto tcObs = tc.lock();
	assert(tcObs);
	QVBoxLayout* lay  = new QVBoxLayout(this);
	lay->setMargin(3);
	textEdit = new MyTextEdit(tcObs->charSize, tcObs->letterSpacing);
	lay->addWidget(textEdit);
	//textEdit->setTabChangesFocus(true);
	
	setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
	setMouseTracking(true);
	
	setGeometryInC(tcObs->geometry);
	
	connect(textEdit, &QTextEdit::textChanged, 
			this, &TextWidget::textChanged);
	/// text beállítása a CE textje alapján:
	TextWidget::textChanged();
}

void TextWidget::keyPressEvent(QKeyEvent* e){
	if(e->key() == Qt::Key::Key_Escape){
		textEdit->clearFocus();
	}
}
QSize grownBy(QSize s, QMargins m){
	return {s.width() + m.left() + m.right(), s.height() + m.top() + m.bottom()}; 
}
void TextWidget::textChanged(){
	
	if(auto obs = tc.lock()){
		QString newt = textEdit->toPlainText();
		qDebug() << "TextWidget::textChanged(): old text:" << obs.get()->text << "\n";
		
		obs.get()->CEW_textChanged(newt);
	}
	/// ha egy connectorWidget-hez csatlakozik, akkor a szülő CEW-nek jelezzük, hogy méretezze át, hogy az új stzöveg beleférjen:
	QMargins extraMargin(10,10,10,10);
	if(auto pConn = dynamic_cast<ConnWidget*>(parent())){
		if(auto ppCEW = dynamic_cast<CEW_Base*>(pConn->parent())){
			/// setWordWrapMode(QTextOption::NoWrap);
			ppCEW->ConnW_wantsToResize( 
						pConn,
						grownBy(textEdit->document()->size().toSize(), extraMargin) 
					);
		}
	}
	qDebug() << "DEBUG: TextWidget::textChanged() called\n";
}
void TextWidget::derivedPaintEvent(QPaintEvent* e){
	QWidget::paintEvent(e);
	if(auto tcObs  = tc.lock()){
		//if(auto connectedC = tcObs->pl->getConnectedConn().lock()){
		//	
		//}
		//else{
		QPainter p(this);
		QPainterPath path;
		path.addEllipse(QRect(0, 0, width(), height()));
		p.fillPath(path, QBrush(QColor(100,0,0,120), Qt::SolidPattern));
		//}
		
	}
}
void TextWidget::mousePressEvent(QMouseEvent *event){
	qDebug() << "DEBUG: TextWidget::mousePressEvent(..) called\n";
	raise();//a képernyőn a többi widget elé helyezzük 
	CEW_Base::mousePressEvent(event);
}
void TextWidget::mouseMoveEvent(QMouseEvent *event){
	CEW_Base::mouseMoveEvent(event);
}
void TextWidget::mouseReleaseEvent(QMouseEvent *event){
	qDebug() << "DEBUG: TextWidget::mouseReleaseEvent(..) called\n";
	CEW_Base::mouseReleaseEvent(event);
}

