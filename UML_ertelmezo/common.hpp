#ifndef COMMON_H
#define COMMON_H
#include <QRect>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <QString>


typedef std::vector< std::vector<double> > DotMatrix;

inline bool isPointInRectangle(std::pair<int,int> p, QRect r){
	if(p.first <= r.x())
		return false;
	if(p.second <= r.y())
		return false;
	if(p.first > r.x()+r.width())
		return false;
	if(p.second > r.y()+r.height())
		return false;
	return true;
}
inline bool isPointInRectangle(int pX, int pY, QRect r){
	return isPointInRectangle( std::pair<int,int>(pX,pY), r );
}
inline bool isPointInRectangle(int pX, int pY, int rTop, int rLeft, int rW, int rH){
	return isPointInRectangle( std::pair<int,int>(pX,pY), QRect(rTop, rLeft, rW, rH) );
}

template<class Constrained>
struct Iteratable{
	static void constraints(Constrained c){auto it = c.begin() ; it != c.end() ; ++it;}

	Iteratable(){
		void(*unused)(Constrained)=constraints;
		(void)unused;
	}
};
template<class T>
inline std::string str(T iteratable){
	Iteratable<T> i;
	std::string ret;
	for (auto it = iteratable.begin(); it != iteratable.end() ; ++it) {
		ret += std::to_string(*it)+", ";
	}
	return ret;
}
template<>
inline std::string str<const char*>(const char* iteratable){
	std::string ret(iteratable);
	return ret;
}
		
template<class T>
inline QString qstr(T iteratable){
	Iteratable<T> i;
	QString ret;
	for (auto it = iteratable.begin(); it != iteratable.end() ; ++it) {
		ret += std::to_string(*it)+", ";
	}
	return ret;
}
template<>
inline QString qstr<const char*>(const char* iteratable){
	QString ret(iteratable);
	return ret;
}


///
/// \brief vFind
///  segédfüggvény ami megallapitja, hogy egy string eleme-e egy string-vektornak
///   a main fileokban lesz erre szükség a program indítási argumentumok beolvasásához
/// \param v
/// \param str
/// \return 
///
inline bool vFind(const std::vector<std::string>& v,const std::string& str){
	return find( v.begin(), v.end(), str) != v.end();
}

///TODO: frissiteni a helpet:
/// 
/// \brief printHelp
///  kiírja a program  argumentumainak helyes használatának módját a standard kimenetre:
inline void printHelp(){
	std::cout << "help menu not implemented yet" << std::endl;
}

#endif // COMMON_H
