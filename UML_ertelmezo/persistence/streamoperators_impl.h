#ifndef STREAMOPERATORS_IMPL_HPP
#define STREAMOPERATORS_IMPL_HPP
#include "streamoperators.h"
#include <QString>

#include <iostream>
#include <vector>
#include <list>
template<class A>
std::ostream& writeWSpacedStr(std::ostream& os, const QString& content){
	std::string multiLineStrTok = "FULLLINE";
	os << multiLineStrTok << std::endl;
	/// a stream kiolvasásához tudnunk kell, hogy sorvége jelre végződik-e.
	os << content.endsWith('\n') << std::endl;
	
	/// content hány sorból áll?
	/// "" 1 soros, "blabla" 1 soros, "\N" , 
	/// "\Nblabla" 2 soros; 
	/// Viszont "blabla\N"-t csak egy sorosnak definiálom, mert különben a beolvasó függvény 2. std::getline hívása a \N utáni dolgokat beolvasná.
	/// 
	/// Szóval a string végi \N nem növeli a sorok számát
	if(content.endsWith('\n'))
		os << content.count('\n')+1-1 << std::endl;
	else
		os << content.count('\n')+1 << std::endl;
	os << content.toStdString();
	/// *kiegészítő sorvége jel.
	/// lejjebb hivatkozok rá
	os << std::endl; 
	
	return os;
}
/// Definíció
///  ~content-nek hívom a stream azon részét,
///   amit writeWSpacedStr(stream, string) hívással beleírtunk(ahol 'is' és stream
///   ugyanazt a memóriaterületet(file-t) használja, és egy string tetszőleges string).
/// Definíció
///  szóbeolvasás: is >> string; ahol string std::string típusú.
/// 
/// std::getLine működése: https://en.cppreference.com/w/cpp/string/basic_string/getline
/// 
/// readWSpacedStr feladata a ~content streamrészt kiolvasni, és pontosan ezt beleírni content-be.
/// a programban minden beolvasó eljárás szóbeolvasással kezdődik(ami megeszi az összes whitespace-t amit a szó előtt talál),
///  ezért nem baj ha a streamben kiolvasatlanul marad a *kiegészítő sorvége jel.
template<class A>
std::istream& readWSpacedStr(std::istream& is, QString& content){
	std::string fullLineStrTok = "FULLLINE";
	std::string checkStr;
	std::string lineEnd;
	
	is >> checkStr; 
	assert(checkStr == fullLineStrTok);
	
	bool endsWithLineEnd = false;
	is >> endsWithLineEnd;
	
	int lineNum = 0;
	is >> lineNum;
	
	std::getline(is, lineEnd);
	
	content = "";
	std::string actLine = "";
	for (int i = 0; i < lineNum; ++i) {
		actLine = "";
		
		std::getline(is, actLine); 
		actLine += '\n';
		content += actLine.c_str();
	}
	/// 
	/// Ha \N-re végződik ~content, akkor kiolvastuk, a content helyesen tartalmazni fogja azt, és a streamben marad a *kiegészítő sorvége jel.
	/// Viszont ha nem \N-re végződik, akkor kiolvastuk *kiegészítő sorvége jelet és (helytelenül) beletettük a content-be.
	if(!endsWithLineEnd){
		content.chop(1);
	}
	return is;
}
template<class Elem, template<class...> class Ctr>
std::ostream& saveContainer(std::ostream& os, const Ctr<Elem>& v){
	os << v.size() << std::endl;
	if(os.fail()){
		return os;
	}
	int j = 0;
	for(const auto& it : v){
		it.save(os);
		//az adat kiírása után teszünk egy tabulátort:
		if(j < (int)v.size()-1)
			os << "\t";
		++j;
	}
	os << std::endl;
	return os;
}
template<class Elem, template<class...> class Ctr>
std::istream& loadContainer(std::istream& is, Ctr<Elem>& v){
	int vecSize;
	is >> vecSize;
	if(is.fail()){
		return is;
	}
	Ctr<Elem>().swap(v);
	for(int j = 0 ; j < vecSize ; ++j){
		Elem e;
		e.load(is);
		v.push_back(e);
	}
	return is;
}

/// figyleni kell arra, hogy eSaveFun constans függvény legyen 
/// (mint szerializáló függvény, amúgy is constnak kellene lennie)
template<class Elem, template<class...> class Ctr, class ReturnType>
std::ostream& saveContainer(std::ostream& os, const Ctr<Elem>& v, ReturnType (Elem::*eSaveFun)(std::ostream&) const){
	os << v.size() << std::endl;
	if(os.fail()){
		return os;
	}
	int j = 0;
	for(const auto& it : v){
		
		///NSMF meghívása:
		/// ReturnType value = (object.*eSaveFun)(Params); 
		/// a 'object.*eSaveFun' rész bezárójelezése fontos,
		/// nélküle it(.*eSaveFun(os)) ként zárójeleződne a kifejezés
		(it.*eSaveFun)(os);
		
		//az adat kiírása után teszünk egy tabulátort:
		if(j < (int)v.size()-1)
			os << "\t";
		++j;
	}
	os << std::endl;
	return os;
}
template<class Elem, template<class...> class Ctr, class ReturnType>
std::istream& loadContainer(std::istream& is, Ctr<Elem>& v, ReturnType (Elem::*eLoadFun)(std::istream&)){
	int vecSize;
	is >> vecSize;
	if(is.fail()){
		return is;
	}
	Ctr<Elem>().swap(v);
	for(int j = 0 ; j < vecSize ; ++j){
		Elem e;
		///NSMF meghívása:
		/// ReturnType value = (object.*eLoadFun)(Params); 
		/// a 'object.*eLoadFun' rész bezárójelezése fontos,
		/// nélküle it(.*eLoadFun(os)) ként zárójeleződne a kifejezés
		(e.*eLoadFun)(is);
		v.push_back(e);
	}
	return is;
}

/// figyleni kell arra, hogy eSaveFun constans függvény legyen 
/// (mint szerializáló függvény, amúgy is constnak kellene lennie)
template<class Elem, template<class...> class Ctr, class ReturnType>
std::ostream& saveContainer(std::ostream& os, const Ctr<Elem*>& v, ReturnType (Elem::*eSaveFun)(std::ostream&) const){
	os << v.size() << std::endl;
	if(os.fail()){
		return os;
	}
	int j = 0;
	for(const auto& it : v){
		assert(it);
		///NSMF meghívása:
		/// ReturnType value = (object.*eSaveFun)(Params); 
		/// a 'object.*eSaveFun' rész bezárójelezése fontos,
		/// nélküle it(.*eSaveFun(os)) ként zárójeleződne a kifejezés
		(it->*eSaveFun)(os);
		
		//az adat kiírása után teszünk egy tabulátort:
		if(j < (int)v.size()-1)
			os << "\t";
		++j;
	}
	os << std::endl;
	return os;
}
template<class Elem, template<class...> class Ctr, class ReturnType>
std::istream& loadContainer(std::istream& is, Ctr<Elem*>& v, ReturnType (Elem::*eLoadFun)(std::istream&)){
	int vecSize;
	is >> vecSize;
	if(is.fail()){
		return is;
	}
	Ctr<Elem>().swap(v);
	for(int j = 0 ; j < vecSize ; ++j){
		Elem e;
		///NSMF meghívása:
		/// ReturnType value = (object.*eLoadFun)(Params); 
		/// a 'object.*eLoadFun' rész bezárójelezése fontos,
		/// nélküle it(.*eLoadFun(os)) ként zárójeleződne a kifejezés
		(e.*eLoadFun)(is);
		v.push_back(e);
	}
	return is;
}


template<class Elem, template<class...> class Ctr>
std::ostream& serializeContainer(std::ostream& os, const Ctr<Elem>& v){
	os << v.size() << std::endl;
	if(os.fail()){
		return os;
	}
	int j = 0;
	for(const auto& it : v){
		os << it;
		//az adat kiírása után teszünk egy tabulátort:
		if(j < (int)v.size()-1)
			os << "\t";
		++j;
	}
	os << std::endl;
	return os;
}
template<class Elem, template<class...> class Ctr>
std::istream& deserializeContainer(std::istream& is, Ctr<Elem>& v){
	int vecSize;
	is >> vecSize;
	if(is.fail()){
		return is;
	}
	Ctr<Elem>().swap(v);
	for(int j = 0 ; j < vecSize ; ++j){
		Elem e;
		is >> e;
		v.push_back(e);
	}
	return is;
}
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
	return serializeContainer<T, std::vector>(os, v);
}
template<class T>
std::istream& operator>>(std::istream& is, std::vector<T>& v){
	return deserializeContainer<T, std::vector>(is, v);
}
template<class T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& v){
	return serializeContainer<T, std::list>(os, v);
}
template<class T>
std::istream& operator>>(std::istream& is, std::list<T>& v){
	return deserializeContainer<T, std::list>(is, v);
}

template<class T>
std::ostream& serializeSharedPtr(std::ostream& os, const std::shared_ptr<T>& shared){
	if(shared.get()){
		os << "shared_nonnull" << std::endl;
		os << *shared << std::endl;
	}
	else{
		os << "shared_null" << std::endl;
	}
	return os;
}
template<class T, class... ArgsT>
std::istream& deserializeSharedPtr(std::istream& is, std::shared_ptr<T>& shared, ArgsT... args){
	std::string validity;
	is >> validity;
	assert(is);
	if(validity == "shared_nonnull"){
		// ha shared null(üres) volt, akkor most elkészítjük:
		if(shared == nullptr){
			shared = std::make_shared<T>(args...);
		}
		is >> *shared;
	}
	else{
		shared = nullptr;
	}
	return is;
};
template<class T>
std::ostream& saveSharedPtr(std::ostream& os, const std::shared_ptr<T>& shared){
	if(shared.get()){
		os << "shared_nonnull" << std::endl;
		shared->save(os);
		os << std::endl;
	}
	else{
		os << "shared_null" << std::endl;
	}
	return os;
}
template<class T, class... ArgsT>
std::istream& loadSharedPtr(std::istream& is, std::shared_ptr<T>& shared, ArgsT... args){
	std::string validity;
	is >> validity;
	assert(is);
	if(validity == "shared_nonnull"){
		/// shared-et töröljük, újralétrehozzuk és betöltjk:
		shared.reset();
		// // ha shared null(üres) volt, akkor most elkészítjük:
		// if(shared == nullptr){
		shared = std::make_shared<T>(args...);
		// }
		shared->load(is);
	}
	else{
		shared = nullptr;
	}
	return is;
};
/// std conténer iterátor mentése és betöltése:
template<class Elem, template<class...> class Ctr>
std::ostream& saveIter(std::ostream& os, const Ctr<Elem>& c, 
					   const typename Ctr<Elem>::iterator& itSave)
{
	int i = 0;
	auto it = c.begin();
	for( ; it != itSave && it != c.end() ; ++it){
		++i;
	}
	assert(it == itSave);//az iterátor nem c-beli iterátor
	os << "Iter" << std::endl;
	os << i << std::endl;
	return os;
}


template<class Elem, template<class...> class Ctr>
std::istream& loadIter(std::istream& is, Ctr<Elem>& c, 
					   typename Ctr<Elem>::iterator& itLoad)
{
	int iGoal = 0; 
	std::string checkStr = ""; is >> checkStr; assert(checkStr=="Iter");
	is >> iGoal;
	//itLoad = c.begin(); why cant i use operator= here?
	itLoad = c.begin();
	int i = 0;
	for( ; i < iGoal && itLoad != c.end() ; ++itLoad){
		++i;
	}
	/// a beolvasott index kfutott a c tárolóból. lehet, hogy a tároló megszűnt. 
	/// vagy még nem töltődött be, mert véletlenül a rá vonatkozó iterátort előbb töltöm be, mint magát a tárolót:
	assert(i == iGoal);
	return is;
}

//TODO: a következő eljárásban(loadWeakPtr-ben) az aggregált objektum betöltésének problémája jelenik meg: weak_ptr-el hivatkoznánk egy shared_ptr-ként tárolt objektumra, amit nem érünk el.
//template<class T, class... ArgsT>
//std::ostream& saveWeakPtr(std::ostream& os, const std::weak_ptr<T>& weak, ArgsT... args){
//	if(auto obs = weak.lock()){
//		os << "weak_nonnull" << std::endl;
//		os << *obs << std::endl;
//	}
//	else{
//		os << "weak_null" << std::endl;
//	}
//	return os;
//}
//TODO: a loadWeakPtr-ben az aggregált objektum betöltésének problémája jelenik meg: weak_ptr-el hivatkoznánk egy shared_ptr-ként tárolt objektumra, amit nem érünk el.
//template<class T>
//std::istream& loadWeakPtr(std::istream& is, std::weak_ptr<T>& weak){
//	std::string validity;
//	is >> validity;
//	assert(is);
//	if(validity == "weak_nonnull"){
//		// ha shared null(üres) volt, akkor most elkészítjük:
//		auto obs = weak.lock();
//		assert(obs);
//	}
//	else{
//		weak.reset();
//	}
//	return is;
//};

template<class Elem, template<class...> class Ctr>
std::ostream& save(std::ostream& os, const Ctr<Elem>& container, const typename Ctr<Elem>::iterator& itSave){
	int ind = 0;
	Ctr<Elem> it = container.begin();
	for( ; it != itSave && it != container.end() ; ++it){
		++ind;
	}
	if(it == container.end()){
		os << -1 << std::endl;
	}
	else{
		os << ind << std::endl;
	}
	return os;
}
template<class Elem, template<class...> class Ctr>
std::istream& load(std::istream& is, const Ctr<Elem>& container, typename Ctr<Elem>::iterator& itLoad){
	int indGoal = 0;
	is >> indGoal;
	if(!is){
		return is;
	}
	if(indGoal == -1){
		itLoad = container.end();
		return is;
	}
	itLoad = container.begin();
	int itInd = 0;
	for( ; itLoad != container.end() && itInd != indGoal; ++itInd){
		++itLoad;
	}
	return is;
}
template<class T>
std::ostream& write2DTable(std::ostream& os, const std::vector< std::vector<T> >& v){
	os << v.size(); os << '\t';
	uint v_secondDim;
	if(v.size() > 0){
		os << v.at(0).size() << std::endl;
		v_secondDim = v.at(0).size();
	}
	else{
		///üres táblázat esetén "0	0"-t írunk ki
		os << 0 << std::endl;
		v_secondDim = 0;
	}
	for(uint i = 0 ; i < v.size() ; ++i){
		if(v.at(i).size() != v_secondDim){
			assert(!"ERROR: write2DTable(..): v has inconsistent size along the second dimension");
		}
	}
	for (uint i = 0; i < v_secondDim; ++i) {
		for (uint j = 0; j < v.size(); ++j) {
			os << v.at(j).at(i) << '\t';
		}
		os << std::endl;
	}
	os << std::endl;
	return os;
}

template<class T>
std::istream& read2DTable(std::istream& is, std::vector< std::vector<T> >& v){
	uint size1;
	uint size2;
	is >> size1;
	is >> size2;
	if(is.fail()){
		return is;
	}
		
	v.resize(size1);
	for (uint i = 0; i < size1; ++i) {
		v.at(i).resize(size2);
	}
	for (uint i = 0; i < size2; ++i) {
		for (uint j = 0; j < size1; ++j) {
			is >> v.at(j).at(i);
		}
	}
	if(is.fail()){
		std::cerr << "ERROR: operator>>(istream& is, vector<vector<T>>& v): failed while writing to v!" << std::endl;
		return is;
	}
	return is;
}

#endif // STREAMOPERATORS_IMPL_HPP
