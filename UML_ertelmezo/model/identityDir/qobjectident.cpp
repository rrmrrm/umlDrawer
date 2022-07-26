#include "qobjectident.h"

//#include <QObject>
//#include <QDebug>



std::ostream& QObjectIdent::save(std::ostream& os){
	Identifiable::save(os);
	return os;
}

std::istream& QObjectIdent::load(std::istream& is){
	Identifiable::load(is);
	return is;
}

//void QObjectIdent::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void QObjectIdent::load(std::istream& is){
//	load_chainCallThis(is);
//}

