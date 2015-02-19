#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <QPrinter>
#include "SplinesInterpol.h"
#include <muParser.h>
#include <string>
const QString filetypes("Portable Network Graphics  (*.png *.PNG);;"\
                        "Windows Bitmap (*.bmp *.BMP);;"\
                        "Joint Photographic Experts Group   (*.jpg *.JPG *.jpeg *.JPEG)");
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:

    void on_action_triggered();
    void on_action_2_triggered();
    void on_action_3_triggered();
    void on_action_4_triggered();

    void on_toolButton_3_clicked();

    void on_action_5_triggered();

    void on_toolButton_2_clicked();

    void on_pushButton_clicked();

    void on_toolButton_4_clicked();

    void on_action_6_triggered();

    void on_toolButton_clicked();

private:
    Ui::MainWindow *ui;

    QPrinter *printer;

    QwtPlotGrid grid;
    QwtLegend *legend;
    QwtPlotPanner *pan;
    QwtPlotMagnifier *magn;
    QwtPlotZoomer *zoom;
    QwtPlotCurve curv;
    QwtPlotCurve demo;
    QwtPlotCurve source;
    QwtSymbol *symbol1;

    FuncTable F;
    CubicSpline *S;
    double *X,*Y,*X1,*Y1,*demoX,*demoY;
    void ClearArrays();
    void ClearModel();
    void GenerateModel();
    void DrawPlot();
    void DrawDemo();
    mu::Parser parser;
    double parsX;
    void ReadData(QString fname);
};

#endif // MAINWINDOW_H
