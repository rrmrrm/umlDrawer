#ifndef SAVELASTDRAWINGWIDGET_H
#define SAVELASTDRAWINGWIDGET_H

#include "model/userinputmodel.h"

#include <QWidget>
#include <QPushButton>

namespace Ui {
class SaveLastDrawingWidget;
}
class DClickableButton : public QPushButton{
	Q_OBJECT
public:
	
public:
	DClickableButton(QString title, QWidget* parent = nullptr);
	virtual ~DClickableButton();
public slots:
	void mousePressEvent(QMouseEvent *event) override;
signals:
	void leftClicked();
	void rightClicked();
};

class SaveLastDrawingWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::SaveLastDrawingWidget *ui;
	std::weak_ptr<GenericFileUIM> dataSaveModel;
	std::vector<DClickableButton*> saveButtons;
	
public:
	explicit SaveLastDrawingWidget(
			std::weak_ptr<GenericFileUIM> dataSaveModel_, 
			QWidget *parent = nullptr
		);
	~SaveLastDrawingWidget();
	
protected:
	bool accessFileAddButton(QString filePath);
	void addButton(QString fNameReadable);
signals: 
	void showMessage(QString);/// TODO: bekotni a widgetbe
private slots:
	void fileButtonClicked();
	void accesAnotherDataFile();
	void removeDataFileButton();
};

#endif // SAVELASTDRAWINGWIDGET_H
