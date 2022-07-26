#ifndef NETWORK_H
#define NETWORK_H

#include "functions.hpp"

#include <vector>

#include <ostream>

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
    #include <QRandomGenerator>
#else
    #include <qrandomgenerator.h>
#endif

typedef std::vector<double> Layer;
typedef std::vector<Layer> Layers;
typedef std::vector< std::vector<double> > WeightMx;

//a programban a háló egy súlymátrixának az indexe az általa összekötött két réteg közül a baloldaliéval egyezik meg
class NeuralNetwork {
protected:
	Layers layers;
	///weightMatrices[i] is the matrix storing
	/// edge weights between Layers[i] and Layers[i+1]
	///an interface's elements are column-vectors which sizes' equal to the next layers size
	///indexing of the 'weightMatrices' vector: weightMatrices[leftLayer][col=leftPerceptron_ind][row=rightPerceptron_ind]
	/// the inputs of layer_k can be calculated the following way: layer_k := weightMatrices[k-1] multiplied by layer_(k-1) (matrix*vector multiplication)
	std::vector<WeightMx> weightMatrices;
	
	void addInterface();
	void applyActivationFunc(Layer& layer);
	QRandomGenerator qr= QRandomGenerator();
public:
	friend class Persistence;
	const ActivationFunc* activationFunc;
	void mul(const WeightMx& mx, const Layer& b, Layer& result);

	///
	/// \brief NeuralNetwork
	/// \param seed
	/// seed alalpján készítjük el a randomGenerátort, 
	/// ami a randomizeInterfaces függvényben
	/// véletlenszerűen állítjuk be a háló súlyait
	///
	NeuralNetwork(long seed = 322);
	NeuralNetwork(const std::vector<unsigned int>& LayerSizes, long seed = 322);
	virtual ~NeuralNetwork();
	uint getInputSize() const;
	uint getOutputSize() const;
	//void setInput(Layer& Layer);
	void setInput(const Layer& Layer);
	Layer getOutput() const;
	
	//size: the size of the layer.
	virtual void addLayer(size_t size);
	virtual void addLayer(Layer& Layer);

	void calc();
};
#endif // NETWORK_H
