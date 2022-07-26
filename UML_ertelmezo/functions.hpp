#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDebug>
#include <QColor>

#include <iostream>
#include <vector>
#include <assert.h>
#include <numeric>
#include <qmath.h>
#include <functional>

///QColor matematikai operátorok:
inline QColor operator+(QColor c1, QColor c2){
	return QColor(
			std::min(255, (c1.red()%256) + (c2.red()%256)),
			std::min(255, (c1.green()%256) + (c2.green()%256)),
			std::min(255, (c1.blue()%256) + (c2.blue()%256))
		);
}
inline QColor operator*(double k, QColor c){
	///+0.5 a kerekítési hibák csökkentéséhez
	return QColor(
			std::min(255.0, 0.5 + k * (c.red()%256)),
			std::min(255.0, 0.5 + k * (c.green()%256)),
			std::min(255.0, 0.5 + k * (c.blue()%256))
		);
}
inline QColor operator*(QColor c, double k){
	return k*c;
}

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

class Conv1D{
	std::vector<double> kernel;
public:
	Conv1D(std::vector<double> kernel_)
		: kernel(kernel_)
	{};
	/// a kernel építésekor rendre .., -2, -1, 0, 1, 2, .. argumentumokkal meghívva a kernelBuildert adjuk meg a kernel értékeit úgy,
	///  hogy a középső elem kapja kernelBuilder(0) értéket
	Conv1D(uint kernelSiz, std::function<double(double)>& kernelBuilder)
		: kernel(kernelSiz)
	{
		// kernelSiz mindenképp páratlan kell legyen, hogy az egyik pixel a kernel közepén legyen.
		assert(kernelSiz%2 == 1);
		int midInd = kernelSiz/2;
		for(int i = 0; i < (int)kernelSiz ; ++i){
			int iShifted = i - midInd;
			kernel[i] = kernelBuilder(iShifted);
		}
	};
	/// ugyanaz mint az előző ctor, de ez rvalue reference-t fogad el
	Conv1D(uint kernelSiz, std::function<double(double)>&& kernelBuilder)
		: kernel(kernelSiz)
	{
		// kernelSiz mindenképp páratlan kell legyen, hogy az egyik pixel a kernel közepén legyen.
		assert(kernelSiz%2 == 1);
		int midInd = kernelSiz/2;
		for(int i = 0; i < (int)kernelSiz ; ++i){
			int iShifted = i - midInd;
			kernel[i] = kernelBuilder(iShifted);
		}
		
	};
	virtual ~Conv1D(){};
public:
	/// arr elemeinek összegzése [beg, end) intervallumon
	double sum(std::vector<double> arr, int beg, int end) const{
		beg = std::max(beg, 0);
		end = std::min(end, (int)arr.size());
		double s = 0;
		for(int i = beg; i < end; ++i){
			s += arr[i];
		}
		return s;
	}
	/// compensateEdgeLoss: 
	/// a következő problémára nyújt megoldást(ha true)
	/// amikor a konténer szélső elemeihez illesztjük a konvoluciós kernelt, akkor a kernel egyes elemei lelógnak.
	/// pl ha in egy csupa 1 értéket tartalmazó tömböt iterál, a kernel pedig {1/4, 1/2, 1/4},
	/// akkor elvárható, hogy out is csupa 1-es legyen. de {3/4, 1, 1, ..., 1, 3/4} lenne az eredmény kompenzáció nélkül.
	/// 
	template<class ContainerIn, class ContainerOut>
	void operator()(const ContainerIn& in, ContainerOut& out, bool compensateEdgeLoss) const{
		if(in.size() <= 0)
			return;
		if(kernel.size() % 2 != 1)
			return;
		
		int kerMidInd = kernel.size()/2;
		
		std::vector<double> compensation(in.size(), 0.0);
		
		/// kompenzáló tényezők kiszámítása 'in' minden eleméhez:
		int ci = 0;
		for(auto& ce : compensation){
			/// kernel[kernelMidInd] tartozik in[outI] elemhez.
			/// ez alapján kernelnek az első (kerMidInd-outI) eleme nem tartozik semmihez(feltéve, hogy ez a kifejezés pozitív)
			/// és az utolsó kerMidInd + outI - in.size() + 1 eleme sem (feltéve hogy ez a kifejezés pozitív)
			int ker_firstGoodI = kerMidInd-ci;
			int ker_firstBadInd = kernel.size() - (kerMidInd + ci - in.size() + 1);
			/// lehet, hogy ker_firstGoodI < 0 illetve lehet, hogy ker_firstBadI > kernel.size(),
			///  ezért leszűkítjük őket [0, kernel.size) intervallumra.
			int kernelStrtI = std::max(ker_firstGoodI, 0);
			int kernelEndI = std::min(ker_firstBadInd, (int)kernel.size());
			double s = sum(kernel, 0, kernel.size());
			double sPart = sum(kernel, ker_firstGoodI, ker_firstBadInd);
			
			double comp = s - sPart;
			/// a kernel 1 elemére jutó kompenzáció kiszámítása. 
			ce =  comp / (double)(kernelEndI-kernelStrtI);
			/// kernelEndI > kernelStrtI, belátása:
			/// kernelEndI > kernelStrtI <=>
			/// min(kernel.size - (kerMidInd + outI - in.size + 1), kernel.size) >
			///  max(kerMidInd-outI, 0) <=> 
			/// a && b, ahol
			/// a: kernel.size - (kerMidInd + outI - in.size + 1) > max(kerMidInd-outI, 0) ÉS
			/// b: kernel.size > max(kerMidInd-outI, 0).
			///		b igaz, mert  max(kerMidInd-outI, 0) < max(kernel.size-outI, 0) <= max(kernel.size, 0) <= kernel.size
			/// a is teljesül, mert a = aa ÉS ab, ahol
			///		aa: kernel.size - (kerMidInd + outI - in.size + 1) > kerMidInd-outI <=>
			///			kernel.size - ( - in.size + 1) > 0 <=>
			///			kernel.size + in.size - 1 > 0 <=> kernel.size + in.size > 1, ami teljesül, mert különben az eljárás elején visszatér a program
			///		ab: kernel.size - (kerMidInd + outI - in.size + 1) > 0 <=>
			///			kernel.size + in.size - 1 > kerMidInd + outI <=
			///			kernel.size > kerMidInd && in.size > outI  ami teljesül, mert kerMidInd és outI kernelnek illetve in konténernek az indexei
			
			++ci;
		}
		
		int outI = 0;
		for(auto& outIt : out){
			int ker_firstGoodI = kerMidInd-outI;
			int ker_firstBadInd = kernel.size() - (kerMidInd + outI - in.size() + 1);
			
			int kernelStrtI = std::max(ker_firstGoodI, 0);
			int kernelEndI = std::min(ker_firstBadInd, (int)kernel.size());
			
			for(int ki = kernelStrtI; ki < kernelEndI ; ++ki){
				double k = kernel[ki];
				int relativeInd = ki - kerMidInd;
				int runInd = outI + relativeInd;
				assert(runInd >= 0 && runInd < (int)in.size());
				
				if(compensateEdgeLoss){
					outIt += (compensation[runInd] + k) * in[runInd];
				}
				else
					outIt += k * in[runInd];
			}
			
			++outI;
		}
	}
	//virtual ArrayType f(ArrayType x) const = 0;
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
	void set(double am){
		assert(standardDeviaton != 0);
		amplitudeMul = am;
	}
	double f(double dX) const{
		double sD = standardDeviaton;
		double ret = amplitudeMul * qExp( -(dX*dX) / (2*sD*sD) );
		return ret;
	}
	double operator()(double dX) const{
		return f(dX);
	}
	virtual ~GaussBlur1D(){};
};
struct GaussBlur2D: public Func<double,double, 2>{
	double standardDeviaton = 1;
	GaussBlur2D(){}
	GaussBlur2D(double _standardDeviation):
		standardDeviaton(_standardDeviation)
	{
		//assert(_standardDeviation != 0.0);
	};
	double f(double dX, double dY) const{
		assert(standardDeviaton != 0.0);
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

#endif // FUNCTIONS_H
