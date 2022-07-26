#ifndef MODELSTATE_H
#define MODELSTATE_H
#include "shape_drawing/drawing.hpp"

#include <QObject>

#include <memory>
#include <functional>
class ModelStateOp
{
public:
	
protected:
	ModelStateOp();
	virtual ~ModelStateOp();
public:
	virtual void undo() = 0;
	virtual void redo() = 0;
};

template<class T>
class GenericModelStateOp : public ModelStateOp
{
protected:
	T preState_;
	T postState_;
	T* actual_;
	
public:
	GenericModelStateOp(
			T preState,
			T postState,
			T* actual);
	~GenericModelStateOp();
public:
	virtual void undo() override;
	virtual void redo() override;
};
template<class T> GenericModelStateOp(T, T, T*) 
	-> GenericModelStateOp<T>;

template<class T>
class ArrStateOp : public ModelStateOp
{
protected:
	T preState_;
	T postState_;
	T* actual_;
	
public:
	ArrStateOp(
			T preState,
			T postState,
			T* actual);
	~ArrStateOp();
public:
	virtual void undo() override;
	virtual void redo() override;
};

template<class T>
class test : public ModelStateOp
{
protected:
	T preState_;
	
public:
	test(T preState);
	virtual void undo() override;
	virtual void redo() override;
};
template<class T> test(T) 
	-> test<T>;


template<class Elem>
class GenericModelStateOp<std::vector<Elem>> : public ModelStateOp
{
protected:
	std::vector<Elem> preState_;
	std::vector<Elem> postState_;
	std::vector<Elem>* actual_;
	
public:
	GenericModelStateOp(
			std::vector<Elem> preState,
			std::vector<Elem> postState,
			std::vector<Elem>* actual);
	
public:
	virtual void undo() override;
	virtual void redo() override;
};
template<class Elem> GenericModelStateOp(
		std::vector<Elem>,
		std::vector<Elem>,
		std::vector<Elem>*
	) 
	-> GenericModelStateOp<std::vector<Elem>>;


template<class Elem>
class GenericModelStateOp<std::list<Elem>> : public ModelStateOp
{
protected:
	std::list<Elem> preState_;
	std::list<Elem> postState_;
	std::list<Elem>* actual_;
	
public:
	GenericModelStateOp(
			std::list<Elem> preState,
			std::list<Elem> postState,
			std::list<Elem>* actual);
	
public:
	virtual void undo() override;
	virtual void redo() override;
};
template<class Elem> GenericModelStateOp(
		std::list<Elem>,
		std::list<Elem>,
		std::list<Elem>*
	) 
	-> GenericModelStateOp<std::list<Elem>>;

template<class Stored>
class GenericModelStateOp<std::shared_ptr<Stored>> : public ModelStateOp
{
protected:
	std::shared_ptr<Stored> preState;
	std::shared_ptr<Stored> postState;
	std::shared_ptr<Stored>* actual;
	
protected:
	GenericModelStateOp(
			std::shared_ptr<Stored> preState_,
			std::shared_ptr<Stored> postState_,
			std::shared_ptr<Stored>* actual_);
	
public:
	virtual void undo() override;
	virtual void redo() override;
};

template<class OnHeap>
class GenericModelStateOp<OnHeap*> : public ModelStateOp
{
protected:
	OnHeap* preState_;
	OnHeap* postState_;
	OnHeap** actual_;
public:
	GenericModelStateOp(OnHeap* preState, OnHeap* postState, OnHeap** actual);
	
public:
	virtual void undo() override;
	virtual void redo() override;
};
template<class OnHeap> GenericModelStateOp(
		OnHeap*,
		OnHeap*,
		OnHeap**
	) 
	-> GenericModelStateOp<OnHeap*>;

///
/// \brief The UndoRedoFunctional class
/// az argumentumok értékét a FunctionalStateOp fogja megadni.
/// amikor a FunctionalStateOp -ot létrehozzuk 
/// és megadunk neki egy (akár előre létrehozott) UndoRedoFunctionalt,
///  akkor rögzülnek az argumentumok értékei. 
/// innentől a FunctionalStateOp tárolja az argumentumok értékeit,
///  és ezen rögzített argumentumokkal hívja meg az undo-t és redo-t mikor kell.
template<class... Args> 
class UndoRedoFunctional{
private:
	std::function<void(Args...)> undoCall;
	std::function<void(Args...)> redoCall;
	
public:
	UndoRedoFunctional(
			std::function<void(Args...)> undoCall_,
			std::function<void(Args...)> redoCall_
			);
public:	
	void undo(Args... args);
	void redo(Args... args);
};

///
/// \brief The FunctionalStateOp class
template<class... UndRedArgs>
class FunctionalStateOp : public ModelStateOp{
private:
	UndoRedoFunctional<UndRedArgs...>& urt;
	std::function<void()> boundUndo;
	std::function<void()> boundRedo;
public:
	///
	/// \brief FunctionalStateOp
	/// az urt_ argumentumainak értékét a undoArgs és redoArgs adja meg.
	/// Amikor FunctionalStateOp -ot létrehozzuk 
	/// és megadunk neki egy (akár előre létrehozott) UndoRedoFunctionalt,
	///  akkor rögzülnek az argumentumok értékei. 
	/// innentől FunctionalStateOp tárolja az argumentumok értékeit,
	///  és az undo és redo meghívásával ezen rögzített argumentumokkal meghívja az urt_.undo-t és urt_.redo-t.
	/// 
	/// \param urt_
	/// \param undoArgs
	/// \param redoArgs
	///
	FunctionalStateOp(
		//T preState,
		//T postState,
		//T* actual,
		UndoRedoFunctional<UndRedArgs...>& urt_,
		UndRedArgs... undoArgs,
		UndRedArgs... redoArgs);
	FunctionalStateOp(
		//T preState,
		//T postState,
		//T* actual,
		UndoRedoFunctional<UndRedArgs...>& urt_,
		UndRedArgs... urArgs);
public:
	virtual void undo() override;
	virtual void redo() override;
};

class ModelStateOp_function : public virtual ModelStateOp
{
protected:
	//enum OpType{
	//	pushRecoginzeReplaceLastDrawing
	//};
	//const OpType opType;
	std::function<void()> undoOperation;
	std::function<void()> redoOperation;
	
public:
	ModelStateOp_function(std::function<void()> undoOperation_, 
						  std::function<void()> redoOperation_);
	
public:
	virtual void undo();
	virtual void redo();
};
class ModelStateOp_composite;
class CompositeOpHelper{
private:
	std::vector<ModelStateOp*>* const ops;
public:
	CompositeOpHelper(std::vector<ModelStateOp*>* ops_);
	
private:
	ModelStateOp_composite* accessLastAsComposite();
public:
	///
	/// \brief addOrInsertOp
	/// \return igazat ad vissza ha az új állapotművelet közvetlenül a felhasználó osztály/objektum állapot-művelet vektorához lett hozzáadva,
	/// és hamisat, ha az állapot-művelet-vektor végén levő összetett állapot-műveletbe lett továbbítva.
	///
	bool addOrInsertOp(ModelStateOp*);
	void finishCompositeStateOp();
	bool lastInnerAggregateFinished();
	
};
class ModelStateOp_composite : public ModelStateOp
{
private:
	bool built = false;
protected:
	std::vector<ModelStateOp*> stateOps;
	CompositeOpHelper helper;
	
public:
	ModelStateOp_composite();
	
public:
	virtual void undo() override;
	virtual void redo() override;
	void addOrInsertOp(ModelStateOp* newInnerOp);
	void finish();
	bool isFinished() const;
	bool deepFinishedCheck();
};


class ModelStateOp_prepost : public ModelStateOp
{
protected:
	ModelStateOp_prepost();
	
};
class DrawingStateOp_prepost : public ModelStateOp_prepost
{
	Drawing** variable;
	Drawing* preState;
	Drawing* postState;
public:
	DrawingStateOp_prepost(Drawing** variable_, const Drawing& preState_, const Drawing& postState_);
	
public:
	virtual void undo() override;
	virtual void redo() override;
};


#include "modelStateOp_impl.h"
#endif // MODELSTATE_H
