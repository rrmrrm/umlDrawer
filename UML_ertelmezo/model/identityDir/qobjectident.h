#ifndef QOBJECTIDENT_H
#define QOBJECTIDENT_H

#include "identifiable.h"

#include <QObject>
#include <QDebug>

/// elméletileg lehet képes mzConnectionök et létrehozni QobjectIdentek között, de nem jó ötlet, mert azok nem törlődnek az objektum törlésekor, és nem menhetőek
///  arra a savableQObject való
class QObjectIdent: public QObject, public Identifiable
{
	Q_OBJECT
public:
	QObjectIdent(){}
	QObjectIdent(QObject* parent)
		:	QObject(parent)
	{
		
	}
	QObjectIdent(IdType forcedId, QObject* parent = nullptr)
		:	QObject(parent),Identifiable(forcedId)
	{
		
	}
	virtual ~QObjectIdent(){
		qDebug() << "QObjectIdent::~QObjectIdent() called\n";
	}
	
protected:
	//void save_chainCallThis(std::ostream&);
	//void load_chainCallThis(std::istream&);
public:
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
};

//template<class BaseQObjT>
//class QObjectIdent<BaseQObjT>: public QObject
//{
//	Q_OBJECT
//public:
//	QObjectIdent(){}
//	QObjectIdent(QObject* parent)
//		:	QObject(parent)
//	{
//		
//	}
//	QObjectIdent(IdType forcedId, QObject* parent = nullptr)
//		:	QObject(parent),Identifiable(forcedId)
//	{
//		
//	}
//	virtual ~QObjectIdent(){
//		qDebug() << "QObjectIdent::~QObjectIdent() called\n";
//	}
//};


#endif // QOBJECTIDENT_H
