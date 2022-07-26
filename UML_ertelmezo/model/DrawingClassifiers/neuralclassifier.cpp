#include "neuralclassifier.h"
#include "shape_drawing/drawing.hpp"
#include "network/network.h"
#include "persistence/persistence.h"

NeuralClassifier::NeuralClassifier(Persistence* persistence_, uint resHorizontal_, uint resVertical_)
	:
	persistence(persistence_)
	, resHorizontal(resHorizontal_)
	, resVertical(resVertical_)
{
	
}


CE_flatType NeuralClassifier::classify(Drawing* d){
	assert(d);
	NeuralNetwork* n = persistence->getNetwork(); 
	///ha a network input mérete nem igazodik a készített Drawing-okéhoz, akkor hibát jelzünk, és kiszállunk:
	if(n->getInputSize() != resHorizontal*resVertical){
		std::cerr << "ERROR: keyPressEvent(..): loaded network's input size is different from resolution of drawings created by this program'(resHoriz*resVertical):"
			 << n->getInputSize()
			 << ", versus " << resHorizontal << " x " << resVertical
			 << "call the widget with the apropriate DrawingReslution, or use a Network with proper input size "
			 <<  std::endl;
		std::cerr << std::flush;
		std::exit(1);
	}
	///ennél a lapításnál biztosan van elegánsabb megoldás arra, hogy a pzthon progi és a c++ progi ugyanazt az adathalmazt használhassa:
	///át kell konvertálni a Drawing 2 dimenziós tömbjét 1 dimenzióssá, hogy
	/// oda lehessen adni inputként a Neurális hálónak
	uint cols = d->getResolutionHorizontal();
	uint rows = d->getResolutionVertical();
	std::vector<double> cellsVecFlattened(
		d->getResolutionHorizontal() * 
		d->getResolutionVertical()
			);
	for (uint col = 0; col < cols; ++col) {
		for (uint row = 0; row < rows ; ++row) {
			cellsVecFlattened[col*cols + row] = d->getCells().at(col).at(row);
		}
	}
	n->setInput(cellsVecFlattened);
	n->calc();
	auto output = n->getOutput();
	assert(output.size() > 0);
	int maxActivationIndex = 0;
	double max = std::numeric_limits<double>::min();
	for(uint i = 0 ; i < output.size() ; ++i){
		if(output[i] > max){
			maxActivationIndex = i;
			max = output[i];
		}
	}
	return (CE_flatType)maxActivationIndex;
}
