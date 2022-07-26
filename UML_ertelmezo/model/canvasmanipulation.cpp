#include "canvasmanipulation.h"
//#include "model/canvasevent.h"

#include <QMouseEvent>
#include <qdebug.h>

TransitionResult CanvasManipulation::transRes = TransitionResult(
													NOTHING,
													Target(0, 0, std::shared_ptr<CEB>()),
													Target(0, 0, std::shared_ptr<CEB>())
												);

TransitionResult::TransitionResult(
		CanvasManipulationResult resultType_,
		Target t1_,
		Target t2_)
	:
	resultType(resultType_),
	t1(t1_),
	t2(t2_)
{
	
}
CanvasManipulation::CanvasManipulation():
	lastPressTime(QTime::currentTime())
{	
}
double distSqr(int x1, int y1, int x2, int y2){
	return (double)(x1-x2)*(x1-x2) + (double)(y1-y2)*(y1-y2);
}
void CanvasManipulation::applyTransition(
			//CanvasEvent::Type transition, 
			QMouseEvent event,
			Target target
		){
	QMouseEvent::Type transition = event.type();
	//Target target(event.cPos.x(), event.cPos.y(), event.getSrc(), 1);
	
	//NOTHING azt jelenti, hogy nem kell semmilyen EntityWidgetet létrehoznia CanvasManipulationt használó objektumnak
	transRes.resultType = NOTHING;
	auto ceObs = target.canvasElement.lock();
	
	// duplakattintás érzékeléséhez időmérés:
	if(transition == QMouseEvent::MouseButtonPress){
		mousePressTimeGap = lastPressTime.msecsTo(QTime::currentTime());
		lastPressTime = QTime::currentTime();
	}
	switch(state){
	case INITIAL:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonDblClick:
		case QMouseEvent::MouseButtonPress:{
			targets.push_back(target);
			if(targets.back().canvasElement.lock() != nullptr && targets.back().x_inTarget > 500){
				qDebug() << "SAR";				
			}
			if(!ceObs){
				//targets.push_back(target);
				transRes.t1 = target;
				transRes.resultType = GestureType;
				state = BGPRESSED;
				qDebug() << "BGPRESSED";
			}
			else{
				transRes.t1 = target;
				transRes.resultType = ELEMENTPRESSED_RESULT;
				state = ELEMENTPRESSED;
				qDebug() << "ELEMENTPRESSED";
			}
			break;
		};
		case QMouseEvent::MouseMove:{
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			if(!targets.empty()) targets.pop_back();
			state = INITIAL;
			qDebug() << "INITIAL";
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	case BGPRESSED:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			if(!targets.empty()) targets.pop_back();
			targets.push_back(target);
			if(targets.back().canvasElement.lock() != nullptr && targets.back().x_inTarget > 500){
				qDebug() << "SAR";				
			}
			break;
		};
		case QMouseEvent::MouseMove:{
			transRes.resultType = GesturePointType;
			transRes.t1 = target;
			state = BGDRAGGING;
			qDebug() << "BGDRAGGING";
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			if(!targets.empty()) targets.pop_back();
			state = INITIAL;
			qDebug() << "INITIAL";
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	case ELEMENTPRESSED:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			if(!ceObs){
				if(!targets.empty()) targets.pop_back();
				targets.push_back(target);
				if(targets.back().canvasElement.lock() != nullptr && targets.back().x_inTarget > 500){
					qDebug() << "SAR";				
				}
				state = BGPRESSED;
				qDebug() << "BGPRESSED";
			}
			break;
		};
		case QMouseEvent::MouseMove:{
			if(
					distSqr(
						targets.back().getFreshXInCanvas(),
						targets.back().getFreshYInCanvas(),
						target.getFreshXInCanvas(),
						target.getFreshYInCanvas()
						)
					>= mouseMoveSqrTreshold
			){
				// itt nem veszünk fel újabb targetet. majd az ELEMENTDRAGGING-ban foglakozunk csak az új targetekkel.
				// az INITIAL állapotból való idemenetkor már felvettük a targets-be az entityWidget-re kattintás helyét
				state = ELEMENTDRAGGING;
				qDebug() << "ELEMENTDRAGGING";
			}
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			//nincs pop. majd ay elementSELECTED-ben lesz
			if(event.button() == Qt::RightButton){
				state = LINEDRAWING;
				qDebug() << "LINEDRAWING";
			}
			else{
				state = ELEMENTSELECTED;
				transRes.t1 = targets.back();
				transRes.resultType = ELEMENTSELECTED_RESULT;
				qDebug() << "ELEMENTSELECTED";
			}
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	case ELEMENTSELECTED:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:{
		//	if( event.button() == Qt::LeftButton
		//			&& target.canvasElement.lock()
		//			//&& lastpressTarget.canvasElement.lock() == target.canvasElement.lock()
		//	){
		//		qDebug() << "ELEMENTRESIZING";
		//		state = ELEMENTRESIZING;
		//		///TODO: ellenőrizni ezt a pár sort, mert megváltoztattam:
		//		transRes.resultType = ELEMENT_STARTRESIZING_RESULT;
		//		transRes.t1.canvasElement = target.canvasElement;
		//		transRes.t1.x_inTarget = target.x_inTarget;
		//		transRes.t1.y_inTarget = target.y_inTarget;
		//	}
		//	break;
		//}
		case QMouseEvent::MouseButtonPress:{
			if( event.button() == Qt::LeftButton && target.isInCE()){
				/// ha balegérgombot nyomtak egy kijelölt CE-re
				/// akkor két dolog történhet(átméretezés, vagy duplakattintás):
				QPoint ceP2 = target.canvasElement.lock()
						->getGeometry()
						.bottomRight();
				QPoint detectionSiz(10,10);
				QRect detectResize(ceP2 - detectionSiz, ceP2);
				if(detectResize.contains(target.getCPoint())){
					/// ha egy CE szélére kattintottak, akkor elkezdjük az átméretezést:
					qDebug() << "ELEMENTRESIZING";
					state = ELEMENTRESIZING;
					/// TODO: ellenőrizni ezt a pár sort, mert megváltoztattam:
					transRes.resultType = ELEMENT_STARTRESIZING_RESULT;
					transRes.t1 = target;
					//transRes.t1.canvasElement = target.canvasElement;
					//transRes.t1.x_inTarget = target.x_inTarget;
					//transRes.t1.y_inTarget = target.y_inTarget;
				}
				else if(lastpressTarget.canvasElement.lock() == target.canvasElement.lock()
						&& distSqr(
								lastpressTarget.getFreshXInCanvas(),
								lastpressTarget.getFreshYInCanvas(),
								target.getFreshXInCanvas(),
								target.getFreshYInCanvas()
							)
							<= mouseMoveSqrTreshold
						&& mousePressTimeGap < dPressMsec
				){
					/// amúgy, az előző kattintáshoz - helyben és időben - közel kattintottak, akkor jelezzük a duplakattintást:
					state = ELEMENT_DBLPRESSED;
					qDebug() << "ELEMENT_DBLPRESSED";
					transRes.resultType = ELEMENT_DBLPRESSED_RESULT;
					transRes.t1 = target;
				}
			}
			else{
				/// ha nem balgombbal kattintottak, vagy a Canvasra, akkor megszüntetjük a kijelölést:
				
				//state = ELEMENT_DESELECTED;
				transRes.resultType = ELEMENT_DESELECTED_RESULT;
				//transRes.t1 = target;
				//if(!targets.empty()) targets.pop_back();
				state = INITIAL;
				qDebug() << "ELEMENT_DESELECTED, STATE is INITAL now";
				
			}
			break;
		};
		case QMouseEvent::MouseButtonDblClick:{
			if( event.button() == Qt::LeftButton
					&& target.canvasElement.lock()
					&& lastpressTarget.canvasElement.lock() == target.canvasElement.lock()
					&& distSqr(
						lastpressTarget.getFreshXInCanvas(),
						lastpressTarget.getFreshYInCanvas(),
						target.getFreshXInCanvas(),
						target.getFreshYInCanvas()
					)
					<= mouseMoveSqrTreshold
			){
				state = ELEMENT_DBLPRESSED;
				qDebug() << "ELEMENT_DBLPRESSED";
				transRes.resultType = ELEMENT_DBLPRESSED_RESULT;
				transRes.t1 = target;
			}
			else{
				//state = ELEMENT_DESELECTED;
				transRes.resultType = ELEMENT_DESELECTED_RESULT;
				//transRes.t1 = target;
				if(!targets.empty()) targets.pop_back();
				state = INITIAL;
				qDebug() << "ELEMENT_DESELECTED, STATE is INITAL now";
				
			}
			break;
		};
		case QMouseEvent::MouseMove:{
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			//state = INITIAL;
			//qDebug() << "and STATE is INITAL now";
			//if(!targets.empty()) targets.pop_back();
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	case ELEMENTRESIZING:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			break;
		};
		case QMouseEvent::MouseMove:{
			if(target.canvasElement != targets.back().canvasElement){
				qDebug() << "deb";
			}
			
			transRes.resultType = ELEMENT_RESIZING_RESULT;
			//qDebug() << "ELEMENT_RESIZING_RESULT\n";
			
			transRes.t1.canvasElement = targets.back().canvasElement;
			transRes.t1.x_inTarget = target.x_inTarget;
			transRes.t1.y_inTarget = target.y_inTarget;
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			transRes.resultType = ELEMENT_STOPRESIZING_RESULT;
			//transRes.t1 = target;
			state = ELEMENTSELECTED;
			qDebug() << "result is STOPRESIZING";
			qDebug() << "STATE is ELEMENTSELECTED";
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	}
	case ELEMENT_DBLPRESSED:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			break;
		};
		case QMouseEvent::MouseMove:{
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			state = ELEMENTSELECTED;
			qDebug() << "STATE is ELEMENTSELECTED";
			//if(!targets.empty()) targets.pop_back();
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	}
	case LINEDRAWING:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			break;
		};
		case QMouseEvent::MouseMove:{
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			if(event.button() == Qt::RightButton){
				//a vonal akkor érvényes, ha CanvasElementWidget-en végződik:
				//if(!ceObs){
				//	//nem érvényes a vonal, az INITIAL stateben push-olt targetet kidobjuk
				//}
				//else{
				// az INITIAL-ben push-olt, és itt elkészített target alapján kell a Canvas osztálynak létrehoznia egy új vonalat.
				// ezt úgy jelezzük felé, hogy CanvasElemetTypes::Line-al térünk vissza.
				transRes.resultType = Line;
				qDebug() << "resulttype is Line";
				transRes.t1 = targets.back();
				transRes.t2 = target;
				//}
			}
			state = INITIAL;
			qDebug() << "and STATE is INITIAL now";
			if(!targets.empty()) targets.pop_back();
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	case BGDRAGGING:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			break;
		};
		case QMouseEvent::MouseMove:{
			transRes.resultType = GesturePointType;
			transRes.t1 = target;
			state = BGDRAGGING;
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			if(!targets.empty()) targets.pop_back();
			state = INITIAL;
			qDebug() << "INITIAL";
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	case ELEMENTDRAGGING:{
		switch(transition){
		//case QMouseEvent::EdgeClicked:
		case QMouseEvent::MouseButtonPress:{
			break;
		};
		case QMouseEvent::MouseMove:{
			state = ELEMENTDRAGGING;
			transRes.t1 = target;
			//nem kell új elemet létrehozni, de valamit csinálnia kell Cacanvasnak transRes tartalma alapján
			// ezt így jelzem a Canvasnak:
			transRes.resultType = DRAG_UNSELECTED;
			break;
		};
		case QMouseEvent::MouseButtonRelease:{
			transRes.t1 = target;
			if(!targets.empty()) targets.pop_back();//az INITIAL State-ben felvett targetet elhagyjuk a targets-ből:
			state = INITIAL;
			qDebug() << "INITIAL";
			qDebug() << "and resulttype is STOPDRAGGING";///TODO: ezutan vhol warningot dob: couldnt linker symbol find vtable for CEB value
			transRes.resultType = STOPDRAGGING;
			break;
		};
		default:{
			assert(false);
			break;
		}
		}
		break;
	};
	}
	// duplakattintás érzékeléséhez az új target mentése:
	if(transition == QMouseEvent::MouseButtonPress){
		lastpressTarget = target;
	}
}
