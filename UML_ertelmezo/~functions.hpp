#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <assert.h>
#include <qmath.h>
template<class InType, class OutType, int paramNum>
class Func{
protected:
	Func(){};
public:
	virtual ~Func(){};
};

template<class InType, class OutType>
class Func<InType, OutType, 1>{
protected:
	Func(){};
public:
	virtual OutType f(InType x) const = 0;
	virtual ~Func(){};
};

template<class InType, class OutType>
class Func<InType, OutType, 2>{
protected:
	Func(){};
public:
	virtual OutType f(InType x1, InType x2) const = 0;
	virtual ~Func(){};
};
class GaussBlur1D: public Func<double,double, 1>{
	double standardDeviaton;
	double amplitudeMul;
public:
	GaussBlur1D(double _standardDeviation, double _amplitudeMultiplier = 1.0):
		standardDeviaton(_standardDeviation),
		amplitudeMul(_amplitudeMultiplier)
	{
		assert(_standardDeviation != 0.0);
	};
	double f(double dX) const{
		double sD = standardDeviaton;
		double ret = amplitudeMul * qExp( -(dX*dX) / (2*sD*sD) );
		return ret;
	}
	virtual ~GaussBlur1D(){};
};
class GaussBlur2D: public Func<double,double, 2>{
	double standardDeviaton;
public:
	GaussBlur2D(double _standardDeviation):
		standardDeviaton(_standardDeviation)
	{
		assert(_standardDeviation != 0.0);
	};
	double f(double dX, double dY) const{
		double PI = 3.141592653589793238462643383279502;
		double sD2 = standardDeviaton*standardDeviaton;
		double ret = 1.0 / (2*PI*sD2) * exp( -(dX*dX+dY*dY)/(2*sD2) );
		return ret;
	}
	virtual ~GaussBlur2D(){};
};

class ActivationFunc:public Func<double,double,1>{
protected:
	ActivationFunc(){};
public:
	//ActivationFunc(const ActivationFunc&);
	virtual double f(double I) const = 0;
	virtual ~ActivationFunc(){};
};

class Sigmoid:public ActivationFunc{
public:
	Sigmoid(){};
	virtual double f(double I) const{
		return 1.0 / ((long double)(1) + (long double)(exp(-I)) );
	}
	virtual ~Sigmoid(){};
};
class NetworkErrorFunction : public Func<std::vector<double>, double,2>{
protected:
	NetworkErrorFunction(){};
public:
	virtual double f(std::vector<double> o, std::vector<double> expected) const=0;
	virtual ~NetworkErrorFunction(){};
	
	
};
class SumOfSquaredErrors : public NetworkErrorFunction{
public:
	SumOfSquaredErrors(){};
	virtual double f(std::vector<double> o, std::vector<double> expected) const{
		assert(o.size() > 0);
		assert(o.size() == expected.size() );
		
		double ret = 0;
		for (unsigned int i = 0; i < o.size() ; ++i) {
			ret += (o[i] - expected[i])*(o[i] - expected[i]);
		}
		return ret/2;
	}
	virtual ~SumOfSquaredErrors(){};
};

#endif // FUNCTIONS_H
