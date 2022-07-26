#ifndef CANVASELEMENTWIDGETCREATOR_H
#define CANVASELEMENTWIDGETCREATOR_H

#include "canvaselementwidgetbase.h"
#include "linewidget.h"
//#include "umlcomponentwidget.h"
#include "umlclasswidget.h"
#include "umlrelationwidget.h"
#include "textwidget.h"

CEW_Base* createDerived(std::weak_ptr<CEB> ceb, QWidget* parent){
	auto obsCEB = ceb.lock();
	if(!obsCEB)
		return nullptr;
	switch(obsCEB->CEtype){
	case ERROR:{
		assert(!"ERROR: createDerived(..): trying to create a canvas element from CanvasManipulationResult::ERROR");
		return nullptr;
		break;
	}
	case LINE:{
		auto casted = std::dynamic_pointer_cast<LineComponent>(obsCEB);
		return new LineWidget(casted, parent);
		break;
	}
	case UMLCLASS:{
		std::shared_ptr<UMLClassComponent> casted = std::dynamic_pointer_cast<UMLClassComponent>(obsCEB);
		return new umlClassWidget(casted, parent);
		break;
	}
	case TEXTBOX:{
		std::shared_ptr<TextComponent> casted = std::dynamic_pointer_cast<TextComponent>(obsCEB);
		return new TextWidget(casted, parent);
		break;
	}
	case RELATIONSHIP:{
		auto obsRel = std::static_pointer_cast<UMLRelationship>(obsCEB);
		switch(obsRel->getRtype()){
			case EXTENSION:{
				std::shared_ptr<UMLExtensionSpecifier> casted = std::dynamic_pointer_cast<UMLExtensionSpecifier>(obsCEB);
				return new UmlRelationWidget(casted, parent);
				break;
			}
			case AGGREGATION:{
				std::shared_ptr<UMLAggregationSpecifier> casted = std::dynamic_pointer_cast<UMLAggregationSpecifier>(obsCEB);
				return new UmlRelationWidget(casted, parent);
				break;
			}
			case COMPOSITION:{
				std::shared_ptr<UMLCompositionSpecifier> casted = std::dynamic_pointer_cast<UMLCompositionSpecifier>(obsCEB);
				return new UmlRelationWidget(casted, parent);
				break;
			}
			case CIRCLE:{
				std::shared_ptr<UMLCircle> casted = std::dynamic_pointer_cast<UMLCircle>(obsCEB);
				return new UmlRelationWidget(casted, parent);
				break;
			}
			case ERROR_RT:{
				assert(false);
				break;
			}
			default:{
				assert(false);
				break;
			}
		}
		break;
	}
	default:{
		assert(!"ERROR: createDerived(..): kezeletlen CEB::elementType");
		break;		
	}
	}
	return nullptr;
}

#endif // CANVASELEMENTWIDGETCREATOR_H
