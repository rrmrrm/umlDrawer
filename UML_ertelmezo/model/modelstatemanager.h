#ifndef MODELSTATEMANAGER_H
#define MODELSTATEMANAGER_H

#include "modelStateOp.h"

class Model;

class ModelStateManager
{
private:
	Model* m;
	int allStateOpsNum;
	int activeStateOpsNum;
	QString mStatesDir;
public:
	ModelStateManager(Model* m_);
	
private:
	QString getStateFileName(int ind);
	QString getStateFilePath(int ind);
public:
	void undo();
	void redo();
	void saveState();
};
class ModelStateManagerExperiment
{
private:
	int activeStateOpsNum;
	///
	/// \brief stateOperations
	///		modellen végrehajtott műveletek sorozata. az undo-redo funkciókhoz kell.
	std::vector<ModelStateOp*> stateOperations;
	///
	/// \brief helper
	///		összetett stateOperator-ok kezelésére
	CompositeOpHelper helper;
public:
	ModelStateManagerExperiment();
	
public:
	void undo();
	void redo();
	void saveState(ModelStateOp* state);
	void finishCompositeStateOp();
};


#endif // MODELSTATEMANAGER_H
