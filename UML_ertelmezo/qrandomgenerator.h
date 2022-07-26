#ifndef QRANDOMGENERATOR_H
#define QRANDOMGENERATOR_H


///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
class QRandomGenerator
{
public:
    QRandomGenerator();
    QRandomGenerator(long s);
    void seed(long s) const;
    int generate() const;
    double generateDouble() const;
};

#endif // QRANDOMGENERATOR_H
