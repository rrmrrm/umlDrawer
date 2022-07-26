#include "qrandomgenerator.h"

#include <random>
#include <limits.h>
QRandomGenerator::QRandomGenerator(){
    seed(0);
}
QRandomGenerator::QRandomGenerator(long s){
    seed(s);
}
void QRandomGenerator::seed(long s) const{
    srand(s);
}

int QRandomGenerator::generate() const{
    return rand();
}

///[0, 1) intervalum  beli double-t ad vissza
double QRandomGenerator::generateDouble() const{
    unsigned long cap = RAND_MAX;
    ///denom osztó 1-el nagyobb lesz cap-nál, hogy az eredmény a (-1,1) nyílt intervalumban legyen
    unsigned long denom = cap + 1;
    int r = rand();
    double d = r / (double)denom;
    ///most d (-1, 1) intervallumban van,
    if(d < 0.0){
        d *= -1.0;
    }
    ///mostmár d a [0, 1) intervalumban van.
    return d;
}
