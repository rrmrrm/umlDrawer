#ifndef CONNWIDGET_H
#define CONNWIDGET_H

#include <QWidget>
#include <QLayout>

class Connector;
class Canvas;

class ConnWidget: public QWidget{
	Q_OBJECT
	std::weak_ptr<Connector> modelConn;
	const int minHeight = 10;
public:
	QVBoxLayout* l;
	
public:
	/// ha ezt a konstruktort használjuk, akkor külön be kell állítani a modelConn model-t a setModel(..) eljárással:
	ConnWidget(QWidget* parent = nullptr );
	
	ConnWidget(std::weak_ptr<Connector> modelConn_, QWidget* parent = nullptr );
	ConnWidget(std::weak_ptr<Connector>  modelConn_, QRect r, QWidget* parent = nullptr);
	
	virtual ~ConnWidget();
	
private:
	Canvas* accessCanvas();
public:
	void setGeometry(QRect);
	void setModel(std::weak_ptr<Connector> modelConn_);
	std::weak_ptr<Connector> getModel();
	virtual void resetSize();
	virtual void paintEvent(QPaintEvent* e) override;
	void updateConnGeom();
public slots:
	void resizeEvent(QResizeEvent*) override;
	void moveEvent(QMoveEvent *) override;
signals:
	void setGeometry_Conn(QRect rInCanv);
	
};
#endif // CONNWIDGET_H
