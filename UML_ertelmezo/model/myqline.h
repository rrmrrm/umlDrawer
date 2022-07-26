#ifndef MYQLINE_H
#define MYQLINE_H


#include "connector.h"
#include "dirDim.hpp"
#include "persistence/streamoperators.h"

#include <qline.h>

enum LinePart{START, MIDDLE, END};

class MyQLine
{
private:
	QLine line;
	Dimension dim;
public:
	//std::shared_ptr<Connector> connText;
	
public:
	MyQLine();
	MyQLine(QPoint bounds_p1, QPoint bounds_p2, Dimension dim);
	///
	/// \brief MyQLine
	/// \param d
	/// dimC1 és dimC2 koordinátákhoz tartozó dimenzió
	/// \param dimC1
	/// \param dimC2
	/// \param otherC
	///	másik dimenzióhoz tartozó koordináta
	/// 
	MyQLine(Dimension d, int dimC1, int dimC2, int otherC);
	MyQLine(MyQLine&& other);
	MyQLine(const MyQLine& other);/// a connectorok shallow-copzyódnak!
	MyQLine& operator=(const MyQLine& other);/// a connectorok shallow-copzyódnak!
	void fromBounds(QPoint bounds_p1, QPoint bounds_p2, Dimension dim);
	
public:
	QPoint p1() const;
	QPoint p2() const;
	int x1() const;
	int y1() const;
	int x2() const;
	int y2() const;
	int dx() const;
	int dy() const;
	const QLine& getLine() const;
	void setLine(QLine l);
	void translate(const QPoint &p);
	void translate(int dx, int dy);
	bool validate() const;
	QPoint center() const;
	Direction getDir() const;
	Dimension getDim() const;
	void setDim(Dimension d);
	Dimension getOtherDim(Dimension d) const;
	Dimension getOtherDim() const;
	Direction getOtherDir() const;
	Direction getOtherDir(Direction dir) const;
	QPoint getPart(LinePart part) const;
	QPoint zeroComponent(QPoint p, Dimension d);
	void translatePart(LinePart lp, QPoint translation);
	void moveTransversal(QPoint newPlace, bool doValidation=true);
	void movePartAlongLine(const QPoint& newPlace, LinePart lp, bool doValidation=true);
};

inline std::ostream& operator<<(std::ostream& os, const MyQLine& ql){
	os << ql.getLine() << std::endl;
	os << ql.getDim() << std::endl;
	//saveSharedPtr(os, ql.connRel);//os << ql.connRel << std::endl;
	//saveSharedPtr(os, ql.connText);//os << ql.connText << std::endl;
	return os;
}
inline std::istream& operator>>(std::istream& is, MyQLine& ql){
	QLine newL; is >> newL; assert(is); ql.setLine(newL);
	int d = 0; is >> d; ql.setDim((Dimension)d);
	//loadSharedPtr(is, ql.connRel, CanvasElement_Type::RELATIONSHIP);
	//loadSharedPtr(is, ql.connText, CanvasElement_Type::TEXTBOX);
	return is;
}
#endif // MYQLINE_H
