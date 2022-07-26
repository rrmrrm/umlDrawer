#ifndef GESTURE_H
#define GESTURE_H


#include <iostream>
#include <vector>

class Gesture{
	friend class DrawingFactory;
private:
	std::vector<std::pair<int,int> > points;
	int minX;
	int minY;
	int maxX;
	int maxY;
public:
	int startX;
	int startY;
	bool isMinMaxSetup = false;
	int endX;
	int endY;
	
public:
	///
	/// \brief Gesture
	/// initializes points as empty vector,
	/// and sets all members to 0
	/// 
	Gesture();
	Gesture(int _startX, int _startY);
	~Gesture();
	Gesture& operator=(const Gesture& g);
	
public:
	//ha isMinMaxSetup hamis, akkor ezek nem állítják be ret értékét, és hamisat adnak vissza:
	bool getMinX(int* ret) const;
	bool getMinY(int* ret) const;
	bool getMaxX(int* ret) const;
	bool getMaxY(int* ret) const;
	
	void updateMinMaxXY();
	void addPoint(int x, int y);
	const std::vector<std::pair<int,int> >& getPoints() const;
	void setPointAt(unsigned int, int, int);
	std::ostream& print(std::ostream& os) const;
	std::istream& load(std::istream& is);
	
};
std::ostream& operator<<(std::ostream& os, const Gesture& g);
std::istream& operator>>(std::istream& is, Gesture& g);
#endif // GESTURE_H
