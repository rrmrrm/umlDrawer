#ifndef CANVASMANIPULATION_H
#define CANVASMANIPULATION_H
#include "model/CanvasElements/canvaselementtypes.h"
#include "model/target.h"
//#include "model/canvasevent.h"

#include <QMouseEvent>
#include <QTime>
struct TransitionResult{
	CanvasManipulationResult resultType;
	Target t1;
	Target t2;
	TransitionResult(
			CanvasManipulationResult resultType,
			Target t1,
			Target t2
			);
};
struct CanvasManipulation
{
	enum State{
		INITIAL, BGPRESSED, ELEMENTPRESSED,  ELEMENTSELECTED, ELEMENT_DBLPRESSED, 
		LINEDRAWING, BGDRAGGING, ELEMENTDRAGGING, ELEMENTRESIZING
			  };
	State state = INITIAL;
	double mouseMoveSqrTreshold = 2;
	std::vector<Target> targets;
	Target lastpressTarget;
	int mousePressTimeGap = 0;
	QTime lastPressTime;
	// cm állapotátmeneteinek az erefménye lesz transRes -ben eltárolva.
	// azért tezem a static változóba, mert várhatóan egyszerre csak egy példány kell belőle, és nem akarok minden transition után új objektumot létrehozni
	static TransitionResult transRes;
	const double dPressMsec = 580;
public:
	CanvasManipulation();
	
	void applyTransition(
		QMouseEvent event, 
		Target target
		//CanvasEvent event
	);
};

#endif // CANVASMANIPULATION_H
