#ifndef USERINPUTDIALOG_H
#define USERINPUTDIALOG_H

#include "model/model.h"
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class UserInputDialog;
}

class UserInputDialog : public QDialog{
	Q_OBJECT
	
	Ui::UserInputDialog *ui;
	std::shared_ptr<UserInputModel> uim;
	///
	///callBack-et akkor hívja majd meg az osztály, ha azz 'acceptButton' gombra kattintottak
	/// 
	std::function<void (std::shared_ptr<UserInputModel>) > callback;
	
public:
	enum STATE{RUNNING, ACCEPTED, DECLINED};
	STATE widgetState;// ez még nincs hasznalatban
public:
	UserInputDialog(std::shared_ptr<UserInputModel> _uim, std::function<void (std::shared_ptr<UserInputModel>)> _callback, QWidget* parent = nullptr);
	~UserInputDialog();
public slots:
	void accept() override;
	void reject() override;
};

class BindingLineEdit: public QLineEdit {
	Q_OBJECT
	
	//QDialog* dialog;
	QWidget* parent;
	UIM::Type* const boundVar;
protected slots:
	void mouseDoubleClickEvent(QMouseEvent * e) override;
public:
	BindingLineEdit(UIM::Type* _boundVar, bool isBoundVarSetup, QWidget* _parent = nullptr);
	~BindingLineEdit();
public slots:
	void updateBoundVariable();
};
#endif // USERINPUTDIALOG_H
