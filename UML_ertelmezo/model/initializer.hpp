#ifndef INITIALIZABLE_H
#define INITIALIZABLE_H
#include <typeinfo>

#include <iostream>
class Initializer {
/// fields:
private:
protected:
	bool inited = false;
	
/// ctors, dtor, operators:
protected:
	///
	/// \brief Initializer
	/// If there are multiple derived class( each deriving from the another like a chain),
	///   then this class -with proper implementation of the base classes - ensures that in objects of any derived type:
	///     only the most derived type object(the actual object that contains the other objects as subobjects) 
	///     will call the init_overrideThis_callTheOtherOne method(indirectly, by calling the init_callable method)
	/// 
	/// All subobjects will call the init_callable(bool) method,
	/// and they have to pass a bool argument that tells the method whether its been called from the actual object(the "most derived" object),
	/// or not. if the argument is false, then the method doesn't do anything.
	/// If its true, he it calls the init_overrideThis_callTheOtherOne() method.
	/// 
	/// Proper implementation of the base classes:
	/// all class derived from this should look like InitializerDer
	/// (see InitializerDer class below)
	/// 
	/// \param isMostDerivedSubobject_
	///		tells the called subObject whether it is of the most derived type.(the actual type of the object.)
	///		leave isMostDerivedSubobject_ at default value when intitializing a new object(eg. Initializer* in = new Derived()),
	///		and set the param to false when calling baseclass ctor from derived ctor
	///		(eg. 
	///			InitializerDer(bool isMostDerivedSubobject_ = true)
	///			: Initializer(false)// call direct base class with false argument, so it will call init_callable with false(which in turn will do nothing)
	///			{
	///				init_callable(isMostDerivedSubobject_);
	///			}
	///		)
	Initializer(bool isMostDerivedSubobject_ = true)
	{
		(void)isMostDerivedSubobject_;
	}
	virtual ~Initializer()
	{
	}
	
/// other methods:
protected:
	void virtual init_overrideThis_callTheOtherOne() = 0;
	///
	/// \brief init_callable
	///		calls the init_overrideThis_callTheOtherOne method if
	///		the object is not initialized and isMostDerivedSubobject_ is true (indicating that the calling subobject 
	///		is the most derived subobject)
	/// \param isMostDerivedSubobject_
	/// 
	void init_callable(bool isMostDerivedSubobject_){
		if(!isMostDerivedSubobject_)
			return;
		if(inited)
			return;
		inited = true;
		init_overrideThis_callTheOtherOne();
	};	
	
};

///
/// \brief The InitializerDer class
/// example class
class InitializerDer : public Initializer{
public:
	int hasToBeInitedOnlyAfterMostDerivedIsConstructed;	
public:
	///
	/// \brief InitializerDer
	/// ctor is either called by a derived object in the first place 
	///  of its initializer list(argument should be false this case)
	/// or by initializating call like: InitializerDer o = new InitializerDer();,
	///  (or InitializerDer o;)
	/// \param isMostDerivedSubobject_
	///
	InitializerDer(bool isMostDerivedSubobject_ = true)
		/// call direct base class with false argument,
		/// here we are creating that (direct)base object as a subobject, 
		/// so the direct baseobject and its baseobject etc. are clearly not the objects with the most derived type)
		: Initializer(false)
	{
		init_callable(isMostDerivedSubobject_);
	}
	virtual ~InitializerDer(){}
	
public:
	virtual void init_overrideThis_callTheOtherOne() override{
		hasToBeInitedOnlyAfterMostDerivedIsConstructed = 11;
	}
};
#endif // INITIALIZABLE_H
