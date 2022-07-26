#include "network/network.h"

#include <iostream>
//#include <iomanip>
#include <vector>
#include <qmath.h>
#include <assert.h>
#include <functional> //std::ref
#include <QtGlobal>
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
    #include <QRandomGenerator>
#else
    #include <qrandomgenerator.h>
#endif
#include <QTime>

#include <chrono>



uint NeuralNetwork::getInputSize() const{
	assert(layers.size() > 0);
	assert(layers[0].size() > 0);
	return layers[0].size() - 1;//bias
}
uint NeuralNetwork::getOutputSize() const{
	assert(layers.size() > 0);
	assert(layers[layers.size()-1].size() > 0);
	return layers[layers.size()-1].size() - 1;//bias
}
///
/// \brief NeuralNetwork::addInterface
/// adds a weight matrix tha connects the last 2 layers
void NeuralNetwork::addInterface(){
	assert(layers.size() > 1);
	assert(weightMatrices.size() == layers.size()-1-1);

	uint left_nodes = layers[layers.size()-2].size();
	uint right_nodes = layers[layers.size()-1].size();

	WeightMx edges;
	for(uint i=0 ; i < left_nodes ; ++i){
		Layer edges_out;
		for(uint j = 0 ; j < right_nodes ; ++j){
			edges_out.push_back(0);
		}
		edges.push_back(edges_out);
	}
	weightMatrices.push_back(edges);
}




void NeuralNetwork::applyActivationFunc(Layer& layer){
	//we start from index of 1, because the first element of the Layer is not used:
	for(uint i = 1 ; i < layer.size() ; ++i){
		layer[i] = activationFunc->f(std::ref(layer[i]));
		//old: layer[i] = tanh(layer[i]);
	}
}

/// \brief NeuralNetwork::mul
/// (matrix * vector) multipliing considering that
/// the matrix's 0. vector contains the bias values, and
/// the first row of the matrx (mx[*][0]) is just a filler. we dont use its values,
///   instead the result vector's 0. element simply will be set to 1
/// mindezt azért, hogy a mul ismételt alkalmazásával lehessen kiszámolni a kimenetet
/// \param mx
/// \param b
/// \param result
///
void NeuralNetwork::mul(const WeightMx& mx, const Layer& b, Layer& result){
	assert(mx.size() > 0);
	assert(mx[0].size() > 0);
	assert(mx.size() == b.size());
	assert(mx[0].size() == result.size());
	//the 0. element of the result is constant 1(bias).
	result[0] = 1;
	for(size_t i = 1 ; i < result.size() ; ++i){
		result[i]=0;
	}
	//i: mx oszlop index
	for(size_t i = 0 ; i < mx.size() ; ++i){
		//the 0. element of the result is constant 1(bias).
		
		//j: mx sor index
		for(size_t j = 1  ; j < mx[i].size() ; ++j){
			if(0 == i){
				result[j] += mx[i][j] * 1;//bias
			}
			else{
				result[j] += mx[i][j] * b[i];
			}
		}
	}
}

NeuralNetwork::NeuralNetwork(long seed) :
	layers(),
	weightMatrices(),
	activationFunc(new Sigmoid){
		qr.seed(seed);
}

NeuralNetwork::NeuralNetwork(const std::vector<unsigned int>& LayerSizes,long seed) :
	//randomizationScale(0.005),
	layers(),
	weightMatrices(),
	activationFunc(new Sigmoid){
		qr.seed(seed);
		for(size_t i = 0 ; i < LayerSizes.size() ; ++i){
			addLayer(LayerSizes[i]);
		}
}
NeuralNetwork::~NeuralNetwork(){
	delete activationFunc;
}

///
/// \brief NeuralNetwork::setInput
///  copies all values from l to the first layer
/// (the first layer's i. element will be l[i-1],
///  because the 0. element is reserved for convinient calculation with bias)
/// 
/// \param l : Layer to copy into 
///
void NeuralNetwork::setInput(const Layer& l){
	assert(layers.size() > 0);
	assert(l.size() == layers[0].size()-1);
	layers[0][0] = 1;
	for(uint i = 0 ; i < l.size() ; ++i ){
		layers[0][i+1] = l[i];
	}
}

///
/// \brief NeuralNetwork::getOutput 
/// returns the output vector (without the first element)
/// \return vetctor of useful values
///
Layer NeuralNetwork::getOutput() const{
	assert(layers.size() > 0);
	assert( layers[layers.size()-1].size() > 0);
	
	Layer l(layers[layers.size()-1].size()-1);
	for(uint i = 1 ; i < layers[ layers.size()-1 ].size() ;++i ){
		l[i-1] = layers[ layers.size()-1 ][i];
	}
	return l;
}

///
/// \brief NeuralNetwork::addLayer
/// add layer to the end of the network(this will be the new output-layer)
/// \param s : size of new layer
/// (the size of actually created layer will be 1 larger,
///  because we reserve the 0. index for bias calculation)
///
void NeuralNetwork::addLayer(size_t s){
	layers.push_back(Layer(s+1));
	uint lli = layers.size()-1;
	layers[lli][0] = 1;
	if( layers.size() > 1){
		addInterface();
	}
}

///
/// \brief NeuralNetwork::addLayer
/// add layer to the end of the network(this will be the new output-layer)
/// \param l : layer to append to the network
///
void NeuralNetwork::addLayer(Layer& l){
	layers.push_back( Layer(l.size()+1) );
	uint lli = layers.size()-1;
	
	layers[lli][0] = 1;
	for(uint i = 0 ; i < l.size() ; ++i ){
		layers[0][i+1] = l[i];
	}
	
	if( layers.size() > 1){
		addInterface();
	}
}


//last Layer's first element is garbage
//(its there for convinient calculation with bias values)
void NeuralNetwork::calc(){
	assert(layers.size() > 1);
	auto layer1 = layers.begin();
	auto layer2 = layer1;++layer2;
	auto interface = weightMatrices.begin();

	while( layer2 != layers.end()){
		mul(*interface, *layer1, *layer2);
		applyActivationFunc(*layer2);
		++layer1; ++interface;
		++layer2;
	}
}


