#ifndef SAVELASTDRAWINGWIDGET_H
#define SAVELASTDRAWINGWIDGET_H

#include <QWidget>

namespace Ui {
class SaveLastDrawingWidget;
}

class SaveLastDrawingWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit SaveLastDrawingWidget(QWidget *parent = nullptr);
	~SaveLastDrawingWidget();
	
private:
	Ui::SaveLastDrawingWidget *ui;
};

#endif // SAVELASTDRAWINGWIDGET_H
