#ifndef STREAMOPERATORS_HPP
#define STREAMOPERATORS_HPP

#include <QString>
#include <QLine>
#include <QRect>

#include <iostream>
#include <vector>
#include <list>
#include <memory>

/// whitespace-t tartalmazó string írása és olvasása.
/// tartalmazhat sortöréseket is
template<class A = void>
std::ostream& writeWSpacedStr(std::ostream& os, const QString& content);
template<class A = void>
std::istream& readWSpacedStr(std::istream& is, QString& content);

/// saveContainer (loadContainer)
/// bármilyen típusú elemeket tartalmazó konténert tud menteni (betölteni)
template<class Elem, template<class...> class Ctr>
std::ostream& saveContainer(std::ostream& os, const Ctr<Elem>& v);
template<class Elem, template<class...> class Ctr>
std::istream& loadContainer(std::istream& is, Ctr<Elem>& v);

/// saveContainer tárolók szerializálására.
/// bármilyen típusú elemeket tartalmazó konténert tud menteni
/// az 'Elem::eSaveFun' (Elem tagfüggvénye) lesz meghívva os argumentummal. szóval eSaveFun az elem szerializáló tagfüggvénye kell legyen.
template<class Elem, template<class...> class Ctr, class ReturnType>
std::ostream& saveContainer(std::ostream& os, const Ctr<Elem*>& v, ReturnType (Elem::*eSaveFun)(std::ostream&));
/// ld.: azonos templattel rendelkező saveContainer-t.
/// az Elem osztálynak kell legyen üres konstruktora
template<class Elem, template<class...> class Ctr, class ReturnType>
std::istream& loadContainer(std::istream& is, const Ctr<Elem*>& v, ReturnType (Elem::*eLoadFun)(std::istream&));

/// saveContainer POINTER tárolók szerializálására.
/// bármilyen típusú elemeket tartalmazó konténert tud menteni
/// az 'Elem::eSaveFun' (Elem tagfüggvénye) lesz meghívva os argumentummal. szóval eSaveFun az elem szerializáló tagfüggvénye kell legyen.
template<class Elem, template<class...> class Ctr, class ReturnType>
std::ostream& saveContainer(std::ostream& os, const Ctr<Elem>& v, ReturnType (Elem::*eSaveFun)(std::ostream&));
/// ld.: azonos templattel rendelkező és paraméterezésű saveContainer-t.
/// az Elem osztálynak kell legyen üres konstruktora
template<class Elem, template<class...> class Ctr, class ReturnType>
std::istream& loadContainer(std::istream& is, const Ctr<Elem>& v, ReturnType (Elem::*eLoadFun)(std::istream&));

// /// a felhasználó adhatja meg, hogy az elemek mely függvényét akrja meghívni és mik a paraméterek(még meg se próbáltam compile-olni):
//template<class Elem, template<class...> class Ctr, class ReturnType, class ...ParameterList>
//std::ostream& saveContainer(std::ostream& os, const Ctr<Elem>& v, ReturnType (Elem::*arg)(ParameterList... prms));

/// std conténerek (de)szerializálásához.
/// az elemekre a stream szerializáló (deszerializáló) operátorát hívja meg az egyes elemekkel argumentumként:
template<class Elem, template<class...> class Ctr>
std::ostream& serializeContainer(std::ostream& os, const Ctr<Elem>& v);
template<class Elem, template<class...> class Ctr>
std::istream& deserializeContainer(std::istream& is, Ctr<Elem>& v);

/// 
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);
template<class T>
std::istream& operator>>(std::istream& is, std::vector<T>& v);
template<class T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& v);
template<class T>
std::istream& operator>>(std::istream& is, std::list<T>& v);

/// shared_ptr (de)szerializálásához:
template<class T>
std::ostream& serializeSharedPtr(std::ostream& os, const std::shared_ptr<T>& shared);
template<class T, class... ArgsT>
std::istream& deserializeSharedPtr(std::istream& is, std::shared_ptr<T>& shared, ArgsT... args);

/// shared_ptr mentéséhez/betöltéséhez:
template<class T>
std::ostream& saveSharedPtr(std::ostream& os, const std::shared_ptr<T>& shared);
template<class T, class... ArgsT>
std::istream& loadSharedPtr(std::istream& is, std::shared_ptr<T>& shared, ArgsT... args);

/// std conténer iterátor mentése és betöltése:
template<class Elem, template<class...> class Ctr>
std::ostream& saveIter(std::ostream& os, const Ctr<Elem>& container, const typename Ctr<Elem>::iterator& itSave);
template<class Elem, template<class...> class Ctr>
std::istream& load(std::istream& is, const Ctr<Elem>& container, typename Ctr<Elem>::iterator& itLoad);

///
/// \brief write2DTable
/// prints the dimensions first, then the data
/// T : type of the stored data
/// \param os destination stream
/// \param v vector of vectors of type T
/// \return returns os after writing to it
///
template<class T>
std::ostream& write2DTable(std::ostream& os, const std::vector< std::vector<T> >& v);

///
/// \brief read2DTable
/// reads the dimensions first, then the data
/// az adatot először v[i][0]-be olvassa, aztán v[i][2],...stb.(ahol i futóváltozó)
/// T : type of the stored data
/// \param is source stream
/// \param v vector of vectors of type T
/// \return returns is after reading from it
///
template<class T>
std::istream& read2DTable(std::istream& is, std::vector< std::vector<T> >& v);

/// egyeb:
inline std::ostream& operator<<(std::ostream& os, const QLine& ql){
	os << ql.x1() << std::endl;
	os << ql.y1() << std::endl;
	os << ql.x2() << std::endl;
	os << ql.y2() << std::endl;
	return os;
}
inline std::istream& operator>>(std::istream& is, QLine& ql){
	int x1,y1, x2,y2;
	is >> x1;
	is >> y1;
	is >> x2;
	is >> y2;
	ql = QLine(x1,y1, x2,y2);
	return is;
}
inline std::ostream& operator<<(std::ostream& os, const QPoint& ql){
	os << ql.x() << std::endl;
	os << ql.y() << std::endl;
	return os;
}
inline std::istream& operator>>(std::istream& is, QPoint& ql){
	int x,y;
	is >> x;
	is >> y;
	ql = QPoint(x, y);
	return is;
}
inline std::ostream& operator<<(std::ostream& os, const QRect& ql){
	os << ql.x() << std::endl;
	os << ql.y() << std::endl;
	os << ql.width() << std::endl;
	os << ql.height() << std::endl;
	return os;
}
inline std::istream& operator>>(std::istream& is, QRect& ql){
	int x1,y1, w,h;
	is >> x1;
	is >> y1;
	is >> w;
	is >> h;
	ql = QRect(x1, y1, w, h);
	return is;
}


#include "streamoperators_impl.h"

#endif // STREAMOPERATORS_HPP
