#ifndef CANVASELEMENTNTTYPES_H
#define CANVASELEMENTNTTYPES_H

enum CanvasElement_Type{
	ERROR,
	LINE,
	TEXTBOX,
	UMLCLASS,
	
	RELATIONSHIP
	//AGGREGATION,
	//COMPOSITION,
	//EXTENSION,
	//CIRCLE
};
enum Relationship_Type{
	ERROR_RT,
	AGGREGATION,
	COMPOSITION,
	EXTENSION,
	CIRCLE
};
enum CE_flatType{
	LINET,
	TEXTBOXT,
	UMLCLASST,
	
	AGGREGATIONT,
	COMPOSITIONT,
	EXTENSIONT,
	CIRCLET
};
enum CanvasManipulationResult {	
	DRAG_UNSELECTED, STOPDRAGGING, NOTHING, 
	ELEMENTSELECTED_RESULT, ELEMENT_DESELECTED_RESULT, ELEMENTPRESSED_RESULT, ELEMENT_DBLPRESSED_RESULT,
	ELEMENT_STARTRESIZING_RESULT, ELEMENT_RESIZING_RESULT, ELEMENT_STOPRESIZING_RESULT,
	Line,
	GestureType, GesturePointType
};
#endif // CANVASELEMENTNTTYPES_H
