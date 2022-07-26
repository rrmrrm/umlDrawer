#ifndef MYWEAKPTR_H
#define MYWEAKPTR_H

#include "identityDir/identifiable.h"

#include <iostream>
#include <memory>
/// TODO: néha az aggregateId inicializálatlan marad

template<class T> //T should be derived from Identifiable
class Myweak_ptr
{
private:
	std::weak_ptr<T> wp;
	int aggregateId;
	
public:
	//myweak_ptr();
	Myweak_ptr();
	Myweak_ptr(const std::shared_ptr<T>&  shared);
	//constexpr myweak_ptr() noexcept = default;
	Myweak_ptr(const Myweak_ptr<T>&);
	//Myweak_ptr(const Myweak_ptr<T>& __r);
	Myweak_ptr(Myweak_ptr<T>&& __r);
	Myweak_ptr<T>& operator=(const Myweak_ptr<T>& __r);
	//operator=(const weak_ptr<_Yp>& __r) noexcept
	Myweak_ptr<T>& operator=(const std::shared_ptr<T>& __r);
	Myweak_ptr<T>& operator=(Myweak_ptr<T>&& __r);
	
	Myweak_ptr(const std::weak_ptr<T>&);
	//Myweak_ptr(const Myweak_ptr<T>& __r);
	Myweak_ptr(std::weak_ptr<T>&& __r);
	//operator std::weak_ptr<T>();
	operator std::weak_ptr<T>() const;
	bool operator==(std::weak_ptr<T> o) const;
	bool operator!=(std::weak_ptr<T> o) const;
private:
	void setAggregateId(const std::shared_ptr<T>&  shared);
public:
	std::shared_ptr<T> lock() const;
	void reset();
	IdType getAggregateId() const;
	virtual std::ostream& save(std::ostream& os) const;
	virtual std::istream& load(std::istream& is, Identifiable* aggregateOwner = nullptr);
};

#include "myweakptr_impl.h"

#endif // MYWEAKPTR_H
