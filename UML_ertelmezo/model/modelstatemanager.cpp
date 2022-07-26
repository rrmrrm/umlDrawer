#include "modelstatemanager.h"
#include "model/model.h"


ModelStateManager::ModelStateManager(Model* m_)
	: m(m_),
	  allStateOpsNum(0),
	  activeStateOpsNum(0),
	  mStatesDir("savedModelStates")
{
	/// kezdeti állapot mentése. ez lesz a 0. :
	saveState();
}
QString ModelStateManager::getStateFileName(int ind){
	QDir d;
	/// ha nem létezik a könyvtár, létrehozzuk:
	if( !QDir(mStatesDir).exists() ){
		/// ha nem sikerült létrehozni a könyvtárat, akkor nem tudjuk menteni a modell állapotát, ezért hibát dobunk:
		assert(d.mkdir(mStatesDir));
	}
	
	d.cd(mStatesDir);
	return QString::number(ind);
}
QString ModelStateManager::getStateFilePath(int ind){
	QDir d;
	/// ha nem létezik a könyvtár, létrehozzuk:
	if( !QDir(mStatesDir).exists() ){
		/// ha nem sikerült létrehozni a könyvtárat, akkor nem tudjuk menteni a modell állapotát, ezért hibát dobunk:
		assert(d.mkdir(mStatesDir));
	}
	
	d.cd(mStatesDir);
	return d.filePath(getStateFileName(ind));
}
void ModelStateManager::undo(){
	assert(activeStateOpsNum >= 1 && activeStateOpsNum <= allStateOpsNum);

	if(allStateOpsNum == 1 || activeStateOpsNum == 1)
		return;
	m->loadCEs(getStateFilePath(activeStateOpsNum-2));
	activeStateOpsNum--;
}

void ModelStateManager::redo(){
	assert(activeStateOpsNum >= 1 && activeStateOpsNum <= allStateOpsNum);
	if(allStateOpsNum == 1 || activeStateOpsNum == allStateOpsNum)
		return;
	m->loadCEs(getStateFilePath(activeStateOpsNum-1));
	activeStateOpsNum++;
}
void ModelStateManager::saveState(){
	/// ha vannak aktív visszavonások(activeStateOpsNum != stateOperations.size()),
	/// akkor ez az új state azok helyére kerül:
	if(activeStateOpsNum < allStateOpsNum){
		//QDir d;
		//d.cd(mStatesDir);
		for(int i = activeStateOpsNum ; i < allStateOpsNum ; ++i){
			auto fpath = getStateFilePath(i);
			auto fpathAggr = getStateFilePath(i)+"_aggregations";
			/// ha nem sikerült törölni egy file-t azt jelezzük:
			if(!QFile::remove(fpath)){
				qDebug() << "WARNINIG: ModelStateManager::saveState(): failed to remove file '" + fpath + "'";
			}
			if(!QFile::remove(fpathAggr)){
				qDebug() << "WARNINIG: ModelStateManager::saveState(): failed to remove file '" + fpathAggr + "'";
			}
		}
		allStateOpsNum = activeStateOpsNum;
	}
	m->saveCEs(getStateFilePath(activeStateOpsNum));
	++allStateOpsNum;
	++activeStateOpsNum;
}


ModelStateManagerExperiment::ModelStateManagerExperiment()
	: activeStateOpsNum(0),
	  helper(&stateOperations)
{}
void ModelStateManagerExperiment::saveState(ModelStateOp* stateOp){
	/// ha vannak aktív visszavonások(activeStateOpsNum != stateOperations.size()),
	/// akkor ez az új state azok helyére kerül:
	if(activeStateOpsNum < (int)stateOperations.size()){
		/// TODO: valszeg itt kéne felszabaítani a FunctionalStateOp-okban tárolt shared_ptr-eket(bár lehet, hogy itt automatikusan fel is szabadulnak)
		stateOperations.resize(activeStateOpsNum);
	}
	if(helper.addOrInsertOp(stateOp)){
		/// Csak akkor növeljük a számlálót, ha közvetlenül a stateOperations-be került az új állapot-művelet.
		/// (amúgy egy - a stateOperations végén levő - CompositeStateOperator-ba tette a helper az új állapot-műveletet.)
		++activeStateOpsNum;
	}
}
void ModelStateManagerExperiment::undo(){
	assert(activeStateOpsNum >= 0 && activeStateOpsNum <= (int)stateOperations.size());

	if(stateOperations.size() == 0 || activeStateOpsNum == 0)
		return;
	stateOperations[activeStateOpsNum-1]->undo();
	activeStateOpsNum--;
}

void ModelStateManagerExperiment::redo(){
	assert(activeStateOpsNum >= 0 && activeStateOpsNum <= (int)stateOperations.size());
	if(stateOperations.size() == 0 || activeStateOpsNum == (int)stateOperations.size())
		return;
	///a sorban következő state-et redo-zzuk:
	stateOperations[activeStateOpsNum]->redo();
	activeStateOpsNum++;
}

void ModelStateManagerExperiment::finishCompositeStateOp(){
	helper.finishCompositeStateOp();
}

