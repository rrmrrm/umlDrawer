#ifndef DIRDIM_HPP
#define DIRDIM_HPP
#include <QPoint>
#include <QRect>
enum Direction{LEFT, RIGHT, TOP, BOT};
enum Dimension{VERTICAL, HORIZONTAL};
inline Dimension dirToDim(Direction dir){
	switch(dir){
	case LEFT:{}
	case RIGHT:{
		return HORIZONTAL;
	}
	case TOP:{}
	case BOT:{
		return VERTICAL;
	}
	}
	return VERTICAL;
}
#endif // DIRDIM_HPP
