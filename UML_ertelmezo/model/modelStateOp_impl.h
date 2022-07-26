#ifndef MODELSTATEOP_IMPL_H
#define MODELSTATEOP_IMPL_H

#include "modelStateOp.h"
#include <vector>
inline ModelStateOp::ModelStateOp(){

}
inline ModelStateOp::~ModelStateOp(){

}
inline CompositeOpHelper::CompositeOpHelper(std::vector<ModelStateOp*>* ops_)
			:
		ops(ops_)
{
	
}

inline ModelStateOp_composite* CompositeOpHelper::accessLastAsComposite(){
	int siz = ops->size();
	/// ops[0][i] == *(ops+0)[i] == *(ops)[i]
	if(siz > 0){
		auto lastAsComposite = dynamic_cast<ModelStateOp_composite*>(ops[0][siz-1]);
		if(lastAsComposite){
			return lastAsComposite;
		}
	}
	return nullptr;
}
inline bool CompositeOpHelper::addOrInsertOp(ModelStateOp* newInnerOp){
	auto lastAsComp = accessLastAsComposite();
	
	if(lastAsComp && !lastAsComp->isFinished()){
		/// ha a legutolsó op egy építés alatt álló ModelStateOp_composite, akkor abba továbbítjuk newInerOp-ot
		lastAsComp->addOrInsertOp(newInnerOp);
		return false;
	}
	else{
		ops->push_back(newInnerOp);
		return true;
	}
}
inline void CompositeOpHelper::finishCompositeStateOp(){
	auto lastAsComp = accessLastAsComposite();
	assert(lastAsComp);
	lastAsComp->finish();
}
inline bool CompositeOpHelper::lastInnerAggregateFinished(){
	if(auto lastAsComp = accessLastAsComposite()){
		return lastAsComp->deepFinishedCheck();
	}
	return true;
}


template<class T>
inline GenericModelStateOp<T>::GenericModelStateOp(
		T preState,
		T postState,
		T* actual)
			:
		preState_(preState),
		postState_(postState),
		actual_(actual)
{
}
template<class T>
inline GenericModelStateOp<T>::~GenericModelStateOp(){
}
template<class T>
inline void GenericModelStateOp<T>::undo(){
	*actual_ = preState_;
}
template<class T>		
inline void GenericModelStateOp<T>::redo(){
	*actual_ = postState_;
}
template<class T>
inline test<T>::test(
		T preState)
			:
		preState_(preState)
{
}
template<class T>
inline void test<T>::undo(){
}
template<class T>		
void test<T>::redo(){
}


template<class Elem>
GenericModelStateOp<std::vector<Elem>>::GenericModelStateOp(
		std::vector<Elem> preState,
		std::vector<Elem> postState,
		std::vector<Elem>* actual)
			:
		preState_(preState),
		postState_(postState),
		actual_(actual)
{
	
}
template<class Elem>
inline void GenericModelStateOp<std::vector<Elem>>::undo(){
	*actual_ = preState_;
}
template<class Elem>
inline void GenericModelStateOp<std::vector<Elem>>::redo(){
	*actual_ = postState_;
}

template<class Elem>
GenericModelStateOp<std::list<Elem>>::GenericModelStateOp(
		std::list<Elem> preState,
		std::list<Elem> postState,
		std::list<Elem>* actual)
			:
		preState_(preState),
		postState_(postState),
		actual_(actual)
{
	
}
template<class Elem>
inline void GenericModelStateOp<std::list<Elem>>::undo(){
	*actual_ = preState_;
}
template<class Elem>
inline void GenericModelStateOp<std::list<Elem>>::redo(){
	*actual_ = postState_;
}

template<class Stored>
inline GenericModelStateOp<std::shared_ptr<Stored>>::GenericModelStateOp(
		std::shared_ptr<Stored> preState_,
		std::shared_ptr<Stored> postState_,
		std::shared_ptr<Stored>* actual_)
	:
		preState(preState_),
		postState(postState_),
		actual(actual_)
{

}
template<class Stored>
inline void GenericModelStateOp<std::shared_ptr<Stored>>::undo(){
	*actual = preState;
}
template<class Stored>
inline void GenericModelStateOp<std::shared_ptr<Stored>>::redo(){
	*actual = postState;
	
}

template<class OnHeap>
inline GenericModelStateOp<OnHeap*>::GenericModelStateOp(
		OnHeap* preState, 
		OnHeap* postState, 
		OnHeap** actual)
		:
	preState_(preState),
	postState_(postState),
	actual_(actual)
{

}

template<class OnHeap>
inline void GenericModelStateOp<OnHeap*>::undo(){
	*actual_ = preState_;
}

template<class OnHeap>
inline void GenericModelStateOp<OnHeap*>::redo(){
	*actual_ = postState_;
}


template<class... Args>
inline UndoRedoFunctional<Args...>::UndoRedoFunctional(
		std::function<void(Args...)> undoCall_,
		std::function<void(Args...)> redoCall_
	)
		:
	undoCall(undoCall_),
	redoCall(redoCall_)	
{
}
template<class... Args>
inline void UndoRedoFunctional<Args...>::undo(Args... args){
	/*emit*/ undoCall(args...);
	//undoCall(std::vector< std::vector<double> >(), 1);
}
template<class... Args>
inline void UndoRedoFunctional<Args...>::redo(Args... args){
	/*emit*/ redoCall(args...);
	//redoCall(std::vector< std::vector<double> >(), 1);
}

template<class... UndRedArgs>
inline FunctionalStateOp< UndRedArgs...>::FunctionalStateOp(
		//T preState,
		//T postState,
		//T* actual,
		UndoRedoFunctional<UndRedArgs...>& urt_,
		UndRedArgs... undoArgs,
		UndRedArgs... redoArgs) 
			: 
	urt(urt_)
{
	/// boundUndo-hoz és boundRedo-hoz hozzácsatolom az ezen konstruktornak átadott argumentumokat:
	boundUndo = std::bind(&UndoRedoFunctional<UndRedArgs...>::undo, std::ref(urt), undoArgs...);
	boundRedo = std::bind(&UndoRedoFunctional<UndRedArgs...>::redo, std::ref(urt), redoArgs...);
}
template<class... UndRedArgs>
inline FunctionalStateOp<UndRedArgs...>::FunctionalStateOp(
		//T preState,
		//T postState,
		//T* actual,
		UndoRedoFunctional<UndRedArgs...>& urt_,
		UndRedArgs... urArgs) 
			: 
	urt(urt_)
{
	/// boundUndo-hoz és boundRedo-hoz hozzácsatolom az ezen konstruktornak átadott argumentumokat:
	boundUndo = std::bind(&UndoRedoFunctional<UndRedArgs...>::undo, std::ref(urt), urArgs...);
	boundRedo = std::bind(&UndoRedoFunctional<UndRedArgs...>::redo, std::ref(urt), urArgs...);
}
template<class... UndRedArgs>
inline void FunctionalStateOp<UndRedArgs...>::undo(){
	//GenericModelStateOp::undo();
	boundUndo();
}
template<class... UndRedArgs>	
inline void FunctionalStateOp<UndRedArgs...>::redo(){
	//GenericModelStateOp::redo();
	boundRedo();
}

inline ModelStateOp_function::ModelStateOp_function(std::function<void()> undoOperation_, std::function<void()> redoOperation_)
			:
		undoOperation(undoOperation_),
		redoOperation(redoOperation_)
{
	
}
inline void ModelStateOp_function::undo(){
	undoOperation();
}
inline void ModelStateOp_function::redo(){
	redoOperation();
}


inline ModelStateOp_composite::ModelStateOp_composite()
	:
		stateOps(),
		helper(&stateOps)
{
}
inline void ModelStateOp_composite::undo(){
	assert(isFinished());//ha ez finished, akkor elméletileg a benne levő összetett stateOperátorok is, úgyhogy azokat nem, csak ezt a state-t kell ellenőrízni
	/// az undo-kat fordított sorrendben kell végrehajtani:
	for(int i = stateOps.size()-1 ; i >= 0; --i){
		stateOps[i]->undo();
	}
}
inline void ModelStateOp_composite::redo(){
	assert(isFinished());//ha ez finished, akkor a benne levő összetett stateOperátorok is elméletileg, úgyhogy azokat nem, csak ezt a state-t kell ellenőrízni
	for(int i = 0 ; i < (int)stateOps.size() ; ++i){
		stateOps[i]->redo();
	}
}
inline void ModelStateOp_composite::addOrInsertOp(ModelStateOp* newInnerOp){
	helper.addOrInsertOp(newInnerOp);
}
inline void ModelStateOp_composite::finish(){
	assert(helper.lastInnerAggregateFinished());
	built = true;
}
inline bool ModelStateOp_composite::isFinished() const{
	return built;
}
inline bool ModelStateOp_composite::deepFinishedCheck(){
	return helper.lastInnerAggregateFinished() && isFinished();
}

inline ModelStateOp_prepost::ModelStateOp_prepost(){
	
}

inline DrawingStateOp_prepost::DrawingStateOp_prepost(Drawing** variable_,
											   const Drawing& preState_, 
											   const Drawing& postState_)
			:
	variable(variable_),
	preState(new Drawing(preState_)),
	postState(new Drawing(postState_))
{
}
inline void DrawingStateOp_prepost::undo(){
	delete *variable;
	**variable = *preState;
}
inline void DrawingStateOp_prepost::redo(){
	*variable = new Drawing(*postState);
}


#endif // MODELSTATEOP_IMPL_H
