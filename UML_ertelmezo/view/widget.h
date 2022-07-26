#ifndef WIDGET_H
#define WIDGET_H

#include "view/canvas.h"
#include "model/model.h"
#include "userinputdialog.h"

#include <QWidget>
#include <QTimer>
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
	#include <QRandomGenerator>
#else
	#include <qrandomgenerator.h>
#endif
#include <QFileDialog>
#include <QValidator>
#include <QInputDialog>
#include <QKeyEvent>
#include <QPixmap>

#include "savelastdrawingwidget.h"
#include "ui_widget.h"

#include <QLabel>


#include <memory>
#include <functional>
namespace Ui {
class Widget;
}

class Controller;
class QToolBar;

class Widget : 
		public QWidget
		//public virtual IHasAggregate
{
	Q_OBJECT
	
	Model* m;
	//uint resHorizontal;
	//uint resVertical;
	size_t wid;//size of Canvas widget
	///lastDrawing-ot átadjuk a Canvas widget-nek, hogy kirajzolja at a vászonra
	//std::shared_ptr<Drawing*> lastDrawing;
	
	Canvas* canvas;
	int dWidth;
	QLabel* imageLabel;
	QPixmap* drawingsPixmap;
	QString saveLoadDir = "";
	std::vector<DotMatrix> drawingDotMatrices;
	DotMatrix* lastDrawing;
	QToolBar* toolBar;
	
	SaveLastDrawingWidget* saveLDW;
public:
	explicit Widget(QWidget *parent = nullptr);
	~Widget();
private:
	void addActions();
private slots:
	void copySelected();
	void pasteSelected();
public slots:
	void askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)> callback);

	///
	/// \brief addDotMatrixMiniature ha pos_canBeOverIndexed túlindexelné a 'drawings' vectort,
	///  akkor 'drawing' hozzáadasa előtt kibővíti a vector-t:
	/// \param drawing
	/// \param pos_canBeOverIndexed
	///
	void addDotMatrixMiniature(DotMatrix drawing, int pos_canBeOverIndexed);
	void removeDotMatrixMiniature(int pos_canBeOverIndexed);
	void updateDrawingDotMatrixMiniatures();
	///megjeleníti a képernyő tetején a 'msg'-ben megadott szöveget, 
	/// majd pár másodperc múlva eltünteti a megjelenített szöveget
	void showMessage(QString msg);
	///feldob egy QMessageBox-ot msg tartalmát megjelenítve:
	void messagePopup(std::string msg);
	void keyPressEvent(QKeyEvent* e) override;
	void showNeuralNetworkResult(std::vector<double> result);
	void prepareForClosing();
private slots:
	void M_save(std::ostream&);
	void M_load(std::istream&);
	//std::ostream& saveAggregations(std::ostream&);
	//std::istream& loadAggregations(std::istream&);
	///
	///\brief produceDatasFromDir
	///
	///egy 2 mappaválasztó dialog ugrik fel, amiben egy forrás és egy célkönyvtárat kell kiválasztani.
	///a forráskönyvtárban levő összes filet Drawingként értelmezve
	///ezek Gesture-adatai alapján több Drawing file-t készít a célkönyvtárban úgy, hogy a Gesture-ök pontjait kicsit eltolja véletlenszerűen
	///FONTOS:
	/// 1.: a forráskönyvtárban csak Drawing fileoknak szabad lennie,
	/// 2.: a Drawing filek Gesture adatokat is kell, hogy tartalmazzanak
	///megjegyzés: az  elkészített képekre az e-programban  használt margó és elmosás adatok vonatkoznak majd,
	///  tehát így ezeket a paramétereket is lehet módosítani
	void produceDatasFromDir();
	void trainNN();
	void saveCEs();
	void loadCEs();
	void saveDrawings();
	void loadDrawingsCreateCanvasElements();
	void removeDrawingDotMatrixMiniatures();
	//eltünteti az ui->messageLabel-ben levö üzenetet:
	void resetLabelState();
	void undo();
	void redo();
	void deleteSelected();
	void saveDiagram();
	void loadDiagram();
private:
	Ui::Widget *ui;
};

#endif // WIDGET_H
