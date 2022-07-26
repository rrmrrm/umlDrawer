

bool readAndSaveGestureData = true;

#include "view/widget.h"
#include "persistence/streamoperators.h"

#include <QApplication>

#include <iostream>
#include <vector>
///TODO nem working directoriba tenni a fileokat, hanem itt beallitani egz konzvtart, es ahoy relativan elhelzeyni atyokat

///TODO: tanítás előtt adatokat feldolgozásaként: outlier ábrák megjelölése. előbb megtanulni, hogyan kell ezt
/// milyen tanítóminta elemzést érdemes még végrehajtani, és hogyan kell. elemzést valszeg pzthonban könnyű
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Widget* w = new Widget();
	QObject::connect(&app, SIGNAL(aboutToQuit()), w, SLOT(prepareForClosing()));
	w->show();
	
	return app.exec();
	
	return 0;
}
