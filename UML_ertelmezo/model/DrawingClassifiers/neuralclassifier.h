#ifndef NEURALCLASSIFIER_H
#define NEURALCLASSIFIER_H

#include "idrawingclassifier.h"
#include <QString>

class Persistence;
class NeuralClassifier : public IDrawingClassifier
{
	Persistence* persistence;
	uint resHorizontal;
	uint resVertical;
public:
	NeuralClassifier(Persistence* persistance_, uint resHorizontal, uint resVertical);
	~NeuralClassifier(){}
public:
	CE_flatType classify(Drawing* d) override;
};

#endif // NEURALCLASSIFIER_H
