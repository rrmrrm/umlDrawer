#ifndef MYWEAKPTR_IMPL_H
#define MYWEAKPTR_IMPL_H

#include "myweakptr.h"
#include "identityDir/identifiable.h"

#include "model/CanvasElements/canvaselementbase.h"

#include <QDebug>

#include <memory>
#include <type_traits>
#include <assert.h>

template<class T> 
Myweak_ptr<T>::Myweak_ptr() : wp()
{}
template<class T> 
Myweak_ptr<T>::Myweak_ptr(const std::shared_ptr<T>&  shared): wp(shared)
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	setAggregateId(shared);
}
template<class T> 
Myweak_ptr<T>::Myweak_ptr(const Myweak_ptr<T>& o) : wp(o.wp), aggregateId(o.getAggregateId())
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
}
template<class T> 
Myweak_ptr<T>::Myweak_ptr(Myweak_ptr<T>&& o ) : wp(std::move(o.wp)), aggregateId(o.getAggregateId())
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
}


template<class T> 
Myweak_ptr<T>& Myweak_ptr<T>::operator=(const Myweak_ptr<T>& o){
	wp = o.wp;
	aggregateId = o.getAggregateId();
	return *this;
}
template<typename T>
Myweak_ptr<T>& Myweak_ptr<T>::operator=(const std::shared_ptr<T>& shared){
	wp = shared;
	setAggregateId(shared);
	return *this;
}
template<typename T>
Myweak_ptr<T>& Myweak_ptr<T>::operator=(Myweak_ptr<T>&& o){
	wp = std::move(o);
	aggregateId = o.getAggregateId();
	return *this;
}

template<typename T>
Myweak_ptr<T>::Myweak_ptr(const std::weak_ptr<T>& stdweak) :wp(stdweak)
{}
template<typename T>
Myweak_ptr<T>::Myweak_ptr(std::weak_ptr<T>&& o) : wp(std::move(o))
{}
template<typename T>
Myweak_ptr<T>::operator std::weak_ptr<T>() const{
	return wp;
}
template<typename T>
bool Myweak_ptr<T>::operator==(std::weak_ptr<T> o) const{
	return wp.lock() == o.lock();
}
template<typename T>
bool Myweak_ptr<T>::operator!=(std::weak_ptr<T> o) const{
	return wp.lock() != o.lock();
}

template<typename T>
void Myweak_ptr<T>::setAggregateId(const std::shared_ptr<T>&  shared){
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	std::shared_ptr<Identifiable> sId = std::static_pointer_cast<Identifiable>(shared);
	if(sId){
		aggregateId = sId->getId();
	}
	else{
		aggregateId = Identifiable::INVALID_ID;
	}
}
template<typename T>
std::shared_ptr<T> Myweak_ptr<T>::lock() const{
	return wp.lock();
}
template<class T>
void Myweak_ptr<T>::reset(){
	wp.reset();
}
template<typename T>
IdType Myweak_ptr<T>::getAggregateId() const{
	return aggregateId;
}
template<typename T>
std::ostream& Myweak_ptr<T>::save(std::ostream& os) const{
	if(wp.lock()){
		os << "weak_nonnull" << std::endl;
		os << aggregateId << std::endl;
	}
	else{
		os << "weak_null" << std::endl;
	}
	return os;
}
/// egy  Myweakptr objektum (nevezzük 'B'-nek) 2 objektum közötti aggregációt  képvisel:
/// a 'B'-t tartalmazó 'A' objektum aggregálja a 'B' által mutatott 'C' objektumot.
/// 'B' aggregáció csak akkor tölthető be(akkor hozható létre) ha 'C' egy létező shared_ptr.
/// 
/// így lehet hogy több olyan objektumot kell egymás után betölteni, amik tartalmaznak Myweakptr-eket(tehát az általam épülő mentés-betöltés rendszer által felismerhető módon aggregálnak más objektumokat),
///  ha ezekbe az aggregációk kört alkotnak, akkor nem lehet semelyik objektumot se először teljesen betölteni,
///  viszont két fázisban be lehet tölteni az objektunokat:
///		1.: betöltjük az objektumokat, de az aggregációkat nem töltjük be. e mellett be kell tölteni minden aggregált objektumot is a második fázishoz. 
///		2.: az aggregált objektumokat töltjük be.(a Myewakptr-eket)
///TODO: implementálni ezt
template<typename T>
std::istream& Myweak_ptr<T>::load(std::istream& is, Identifiable* aggregateOwner){
	std::string validity;
	is >> validity;
	assert(is);
	
	if(validity == "weak_nonnull"){
		is >> aggregateId;
		assert(is);
		Identifiable* found = Identifiable::findById(aggregateId);
		if(!found){
			qDebug() << "WARNING: Myweak_ptr<T>::load(..): load failed: object with id doesnt exist. maybe it wasnt loaded yet\n";
			if(aggregateOwner){
				qDebug() << "\taggregateOwner's id: " << aggregateOwner->getId() << "/n";
			}
			qDebug() << "\taggregated object's id: " << getAggregateId() << "/n";
			return is;
		}
		T* foundAsT = dynamic_cast<T*>(found);
		if(!foundAsT){
			qDebug() << "WARNING: Myweak_ptr<T>::load(..): load failed: object with exists, BUT cannot be converted to T\n";
			if(aggregateOwner){
				qDebug() << "\taggregateOwner's id: " << aggregateOwner->getId() << "/n";
			}
			qDebug() << "\taggregated object's id: " << getAggregateId() << "/n";
			return is;
		}
		auto foundES = dynamic_cast<std::enable_shared_from_this<T>*>(foundAsT);
		if(!foundES){
			qDebug() << "WARNING: Myweak_ptr<T>::load(..): load failed: object with id exists, AND can be converted to T, BUT is not derived from std::enable_shared_from_this<T>!\n";
			if(aggregateOwner){
				qDebug() << "\taggregateOwner's id: " << aggregateOwner->getId() << "/n";
			}
			qDebug() << "\taggregated object's id: " << getAggregateId() << "/n";
			return is;
		}
		auto foundAsTShared = foundAsT->shared_from_this();
		wp = foundAsTShared;
	}
	else{
		wp.reset();
	}
	return is;
}
#endif // MYWEAKPTR_IMPL_H
