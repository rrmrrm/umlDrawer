#include "savelastdrawingwidget.h"
#include "ui_savelastdrawingwidget.h"

SaveLastDrawingWidget::SaveLastDrawingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SaveLastDrawingWidget)
{
	ui->setupUi(this);
}

SaveLastDrawingWidget::~SaveLastDrawingWidget()
{
	delete ui;
}
