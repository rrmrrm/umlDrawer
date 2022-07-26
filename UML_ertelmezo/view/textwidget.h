#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"
#include <QWidget>
#include <QTextEdit>
class MyTextEdit: public QTextEdit{
	Q_OBJECT
	int charSize;
public:
	MyTextEdit(int charSize_, int letterSpacing, QWidget* parent = nullptr);
	//const int minHeight = 20;
private slots:
	void onTextChanged();
public slots:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};

class TextWidget : public CEW_Base{
	Q_OBJECT
	MyTextEdit* textEdit;
	std::weak_ptr<TextComponent> tc;
public:
	TextWidget(std::shared_ptr<TextComponent> tc_, QWidget* parent);
	virtual ~TextWidget(){}
	
private slots:
	void textChanged();
public slots:
	void keyPressEvent(QKeyEvent* e) override;
	virtual void derivedPaintEvent(QPaintEvent* e) override;
	
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};
#endif // TEXTWIDGET_H
