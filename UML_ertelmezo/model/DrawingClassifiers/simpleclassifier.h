#ifndef SIMPLECLASSIFIER_H
#define SIMPLECLASSIFIER_H

#include "idrawingclassifier.h"

#include <QPixmap>

#include <iterator>
class SimpleClassifier : public IDrawingClassifier
{
private:
	std::vector<QImage*> idealSamples;
	QImage sLine;
	QImage sTextBox;
	QImage sUmlClass;
	QImage sAggregation;
	QImage sCompozition;
	QImage sExtensionUp;
	QImage sExtensionDown;
	QImage sExtensionLeft;
	QImage sExtensionRight;
	QImage sCircle;
	/// két kép összehasonlítása előtt az átlagos fényességüket azonos szintre hozom.
	/// ez az azonos szint lesz lightnessSum.
	/// 
	int imgSiz = 40;
	double lightnessAvg;
public:
	SimpleClassifier();
	~SimpleClassifier(){}
public:
	/// négyzetes eltérések pixelenkénti összege
	double sqrDiff(const QImage& i1, const QImage& i2) const;
	double getAvgLight(const QImage& img) const;
	void setAvgLight(QImage& img, double avg) const;
	CE_flatType classify(Drawing* d) override;
};


class ImageIterator;
class ImageIteratorConst;

class ImageIteratorInner{
	friend ImageIterator;
private:
	ImageIterator* parent;
public:
	ImageIteratorInner(ImageIterator* parent_) : parent(parent_)
	{}
	operator QColor();
	explicit operator int();
public:
	ImageIteratorInner& operator=(QColor c);
	ImageIteratorInner& operator+=(QColor c);
};

class ImageIteratorInnerConst{
	friend ImageIteratorConst;
private:
	ImageIteratorConst* parent;
public:
	ImageIteratorInnerConst(ImageIteratorConst* parent_) : parent(parent_)
	{}
	operator QColor() const;
	operator int() const;
public:
	ImageIteratorInnerConst& operator=(QColor c);
};

class ImageIterator{
	friend ImageIteratorInner;
protected:
	QImage* img;
	ImageIteratorInner iii;
	QPoint pixPos;
	
public:
	ImageIterator(QImage* img_, QPoint pixPos_);
	
private:
	QImage* accessImage(){
		return img;
	}
protected:
public:
	virtual ImageIterator& operator++() = 0;
	
	/// elmondja, hogy túliteráltunk-e
	bool isEnd() const;
	bool operator==(const ImageIterator&) const;
	bool operator!=(const ImageIterator&) const;
	bool operator==(const ImageIteratorConst&) const;
	bool operator!=(const ImageIteratorConst&) const;
	ImageIteratorInner& operator*();
	QPoint getPos() const;
public:
	using iterator_category = std::input_iterator_tag;
};

class ImageIteratorConst{
	friend ImageIteratorInnerConst;
protected:
	const QImage* img;
	const ImageIteratorInnerConst iii;
	QPoint pixPos;
	
public:
	/// ha isGuard igaz akkor az objektumot nem szabad másra használni, mint a ==
	ImageIteratorConst(const QImage* img_, QPoint pixPos_);
	virtual ~ImageIteratorConst();
private:
	const QImage* getImage() const{
		return img;
	}
protected:
public:
	virtual ImageIteratorConst& operator++() = 0;
	
	/// elmondja, hogy túliteráltunk-e
	bool isEnd() const;
	bool operator==(const ImageIterator&) const;
	bool operator!=(const ImageIterator&) const;
	bool operator==(const ImageIteratorConst&) const;
	bool operator!=(const ImageIteratorConst&) const;
	const ImageIteratorInnerConst& operator*();
	QPoint getPos() const;
public:
	using iterator_category = std::input_iterator_tag;
};

class ImageRowIterator : public ImageIterator{
	friend class ImageRowAccessor;
	
private:
	ImageRowIterator(QImage* img_, int rowInd_);
public:
	virtual ~ImageRowIterator();
public:
	virtual ImageIterator& operator++();
};
class ImageColumnIterator : public ImageIterator{
	friend class ImageColumnAccessor;
	
private:
	ImageColumnIterator(QImage* img_, int colInd_);
public:
	virtual ~ImageColumnIterator();
	
public:
	virtual ImageIterator& operator++();
};

///konstans iterátorok:
class ImageRowIteratorConst : public ImageIteratorConst{
	friend class ImageRowAccessorConst;
	
private:
	ImageRowIteratorConst(const QImage* img_, int rowInd_);
public:
	virtual ~ImageRowIteratorConst();
	
public:
	virtual ImageIteratorConst& operator++();
};
class ImageColumnIteratorConst : public ImageIteratorConst{
	friend class ImageColumnAccessorConst;
	
private:
	ImageColumnIteratorConst(const QImage* img_, int colInd_);
public:
	virtual ~ImageColumnIteratorConst();
public:
	virtual ImageIteratorConst& operator++();
};
template<>
struct std::iterator_traits<ImageRowIterator>{
	struct iterator_category{
		operator std::input_iterator_tag(){return std::input_iterator_tag();}
	};
};
template<>
struct std::iterator_traits<ImageColumnIterator>{
	struct iterator_category{
		operator std::input_iterator_tag(){return std::input_iterator_tag();}
	};
};
template<>
struct std::iterator_traits<ImageRowIteratorConst>{
	struct iterator_category{
		operator std::input_iterator_tag(){return std::input_iterator_tag();}
	};
};
template<>
struct std::iterator_traits<ImageColumnIteratorConst>{
	struct iterator_category{
		operator std::input_iterator_tag(){return std::input_iterator_tag();}
	};
};

/// bázisosztály ImageAccessor-oknak
class ImageAccessor{
protected:
	ImageAccessor(){};
public:
};

class ImageRowAccessor : public ImageAccessor{
	QImage* img;
	int rowInd;
public:
	ImageRowAccessor(QImage* img_, int rowInd);
public:
	ImageRowIterator begin();
	ImageRowIterator end();
};
class ImageColumnAccessor : public ImageAccessor{
	QImage* img;
	int colInd;
public:
	ImageColumnAccessor(QImage* img_, int colInd_);
public:
	ImageColumnIterator begin();
	ImageColumnIterator end();
};

/// konstans QImage iterálásához wrapperek:
class ImageRowAccessorConst : public ImageAccessor{
	const QImage* img;
	int rowInd;
public:
	ImageRowAccessorConst(const QImage* img_, int rowInd);
public:
	ImageRowIteratorConst begin();
	ImageRowIteratorConst end();
	QColor operator[](int colInd) const;
	unsigned int size() const;
};
class ImageColumnAccessorConst : public ImageAccessor{
	const QImage* img;
	int colInd;
public:
	ImageColumnAccessorConst(const QImage* img_, int colInd_);
public:
	ImageColumnIteratorConst begin();
	ImageColumnIteratorConst end();
	QColor operator[](int rowInd) const;
	unsigned int size() const;
};
#endif // SIMPLECLASSIFIER_H
