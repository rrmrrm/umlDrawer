#ifndef MYSHAREDPTR_H
#define MYSHAREDPTR_H

#include <memory>
#include <iostream>
#include <assert.h>
///
/// \brief The MySharedPtr class
/// MySharedPtr olyan shared_ptr, aminek csak egy referenciája lehet (a lockolt weak ptr-eket leszámítva)
/// ezt úgy érem, el, hogy a másoló konstruktort törlöm.
///
/// 1 tároló smartpointer ami a tárolt obj élettartamát megszabja, és tetszőleges számú rámutató smartpointer(ezek viszont nem befolyásolhatják "lényegesen" az élettartamot)
/// (lényegesen: pl a weakÜptr::lock() ebben az értelemben nem lényegesen hosszabbítja a tárolt objektum élettartamát)
//a saját shared_ptr típusom copizánának a tiltása

template<class T>
class MySharedPtr : public std::shared_ptr<T>
{
private:				   
	template<class TDer>
	MySharedPtr(const MySharedPtr<TDer>& o){
	   /// a teamplatelt =delete deklarácó tiltja meg a templatelt copy ctor használatát valamiért.
	   ///Ugzhogz priváttá tettem a copzctort, és emellett itt egy assert(false):
	   assert(false);
	}					
public:
	MySharedPtr() :std::shared_ptr<T>()
	{}
	
	template<class TDer>
	MySharedPtr() :std::shared_ptr<TDer>()
	{}

	MySharedPtr(const MySharedPtr<T>& o) =delete;
	
	template<class TDer>
	MySharedPtr(MySharedPtr<TDer>&& o) :std::shared_ptr<TDer>(std::move(o))
	{}
	template<class TDer>
	MySharedPtr(std::shared_ptr<TDer>&& o) :std::shared_ptr<T>(std::move(o))
	{}
	
	virtual ~MySharedPtr()
	{}
	
	template<class TDer>
	MySharedPtr<T>& operator=(const MySharedPtr<TDer>& o){
		///egyszerre csak egy menedzselő smartPointer lehet
		assert(o == nullptr);
		std::shared_ptr<TDer>::operator=(o);
		return *this;
	}
	template<class TDer>
	MySharedPtr<T>& operator=(const std::shared_ptr<TDer>& o){
		assert(o == nullptr);
		std::shared_ptr<TDer>::operator=(o);
		return *this;
	}
	template<class TDer>
	MySharedPtr<T>& operator=(MySharedPtr<TDer>&& o){
		std::shared_ptr<TDer>::operator=(std::move(o));
		/// jelezni kell shared_ptr -nek, hogy átvegye o menedzselését.(move nélkül másolná)
		assert(o == nullptr);
		return *this;
	}
	template<class TDer>
	MySharedPtr<T>& operator=(std::shared_ptr<TDer>&& o){
		/// jelezni kell shared_ptr -nek, hogy átvegye o menedzselését.(move nélkül másolná)
		std::shared_ptr<TDer>::operator=(std::move(o));
		assert(o == nullptr);
		return *this;
	}
	
	operator std::shared_ptr<T>(){
		return *static_cast<std::shared_ptr<T>*>(this);
	}
	
	/*
	constexpr shared_ptr() noexcept : __shared_ptr<_Tp>() { }

	shared_ptr(const shared_ptr&) noexcept = default;


	template<typename _Yp, typename = _Constructible<_Yp*>>
  explicit
  shared_ptr(_Yp* __p) : __shared_ptr<_Tp>(__p) { }


	template<typename _Yp, typename _Deleter,
		 typename = _Constructible<_Yp*, _Deleter>>
  shared_ptr(_Yp* __p, _Deleter __d)
	  : __shared_ptr<_Tp>(__p, std::move(__d)) { }


	template<typename _Deleter>
  shared_ptr(nullptr_t __p, _Deleter __d)
	  : __shared_ptr<_Tp>(__p, std::move(__d)) { }


	template<typename _Yp, typename _Deleter, typename _Alloc,
		 typename = _Constructible<_Yp*, _Deleter, _Alloc>>
  shared_ptr(_Yp* __p, _Deleter __d, _Alloc __a)
  : __shared_ptr<_Tp>(__p, std::move(__d), std::move(__a)) { }


	template<typename _Deleter, typename _Alloc>
  shared_ptr(nullptr_t __p, _Deleter __d, _Alloc __a)
  : __shared_ptr<_Tp>(__p, std::move(__d), std::move(__a)) { }

	template<typename _Yp>
  shared_ptr(const shared_ptr<_Yp>& __r, element_type* __p) noexcept
  : __shared_ptr<_Tp>(__r, __p) { }

	template<typename _Yp,
		 typename = _Constructible<const shared_ptr<_Yp>&>>
  shared_ptr(const shared_ptr<_Yp>& __r) noexcept
	  : __shared_ptr<_Tp>(__r) { }

	shared_ptr(shared_ptr&& __r) noexcept
	: __shared_ptr<_Tp>(std::move(__r)) { }

	template<typename _Yp, typename = _Constructible<shared_ptr<_Yp>>>
  shared_ptr(shared_ptr<_Yp>&& __r) noexcept
  : __shared_ptr<_Tp>(std::move(__r)) { }

	template<typename _Yp, typename = _Constructible<const weak_ptr<_Yp>&>>
  explicit shared_ptr(const weak_ptr<_Yp>& __r)
  : __shared_ptr<_Tp>(__r) { }

	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 2399. shared_ptr's constructor from unique_ptr should be constrained
	template<typename _Yp, typename _Del,
		 typename = _Constructible<unique_ptr<_Yp, _Del>>>
  shared_ptr(unique_ptr<_Yp, _Del>&& __r)
  : __shared_ptr<_Tp>(std::move(__r)) { }

	constexpr shared_ptr(nullptr_t) noexcept : shared_ptr() { }
	*/
};

#endif // MYSHAREDPTR_H
