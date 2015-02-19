#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SplinesInterpol.h"
#include <cmath>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegExp>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QProcess>
#include <qwt_plot_renderer.h>
#include <QImage>
#include <QPicture>
#include <QAbstractTextDocumentLayout>
#include <QUrl>
QString reports_head("");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    printer=new QPrinter();
    printer->setOutputFormat(QPrinter::PdfFormat);
    //printer->setResolution(300);

    ui->qwtPlot->setAutoFillBackground( true );
    QPalette p = ui->qwtPlot->palette();
    p.setColor(QPalette::Window, QColor(Qt::white));
    ui->qwtPlot->setPalette(p);

    ui->qwtPlot->canvas()->setPaintAttribute(QwtPlotCanvas::Opaque);

    QwtLegend *legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
    ui->qwtPlot->insertLegend(legend, QwtPlot::BottomLegend);

    QRect r=ui->dockWidget_2->geometry();
    r.setWidth(155);
    ui->dockWidget_2->setGeometry(r);

    magn=new QwtPlotMagnifier(ui->qwtPlot->canvas());
    pan=new QwtPlotPanner(ui->qwtPlot->canvas());
    //zoom=new QwtPlotZoomer(ui->qwtPlot->canvas());

    curv.setTitle("Сплайн-функция");
    //curv.setLegendAttribute(QwtPlotCurve::LegendShowLine);
    curv.setPen(QPen(QColor("Blue")));
    curv.attach(ui->qwtPlot);

    demo.setPen(QPen(QColor("Green")));
    //demo.setLegendAttribute(QwtPlotCurve::LegendShowLine);
    //demo.attach(ui->qwtPlot);

    symbol1=new QwtSymbol();
    symbol1->setStyle(QwtSymbol::Ellipse);
    symbol1->setPen(QColor(Qt::black));
    symbol1->setSize(4);

    source.setTitle("Опорные точки");
    source.setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
    source.setPen(QPen(QColor("Red")));
    source.attach(ui->qwtPlot);
    source.setStyle(QwtPlotCurve::NoCurve);
    source.setSymbol(symbol1);

    ui->qwtPlot->canvas()->setLineWidth(0);

    grid.attach(ui->qwtPlot);

    X=NULL;Y=NULL;X1=NULL;Y1=NULL;
    demoX=NULL;demoY=NULL;
    S=NULL;
    //F=new FuncTable();
    ui->tableView->setModel(&F);

    parser.DefineVar("x",&parsX);
}

void MainWindow::ReadData(QString fname){
    QFile file(fname);
    if (!file.exists()||!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    ClearModel();
    QTextStream in(&file);
    while (!in.atEnd()) {
        Point t;
        in>>t.X>>t.Y;
        F<<t;
    }
    qSort(F);
    file.close();
}

MainWindow::~MainWindow()
{
    delete magn;
    delete pan;
    //delete zoom;
    delete ui;
    delete symbol1;
    ClearArrays();
    ClearModel();
}

void MainWindow::ClearModel(){
    F.clearModel();
}

void MainWindow::ClearArrays(){
    curv.detach();
    demo.detach();
    source.detach();
    curv.setSamples(NULL,NULL,0);
    if (X!=NULL){
        delete X;
        X=NULL;
    }

    if (Y!=NULL){
        delete Y;
        Y=NULL;
    }

    demo.setSamples(NULL,NULL,0);
    if (demoX!=NULL){
        delete demoX;
        demoX=NULL;
    }

    if (demoY!=NULL){
        delete demoY;
        demoY=NULL;
    }

    source.setSamples(NULL,NULL,0);
    if (X1!=NULL){
        delete X1;
        X1=NULL;
    }

    if (Y1!=NULL){
        delete Y1;
        Y1=NULL;
    }

    if (S!=NULL){
        delete S;
        S=NULL;
    }
    ui->qwtPlot->replot();
}

void MainWindow::on_action_triggered(){
    //QPixmap pix=QPixmap::grabWidget(ui->qwtPlot);

    QString filename=QFileDialog::getSaveFileName(this,"Сохранить график...",QString(),filetypes);
    if(filename!=""){
        QImage *img=new QImage(3.5*ui->qwtPlot->canvas()->width(),
                               3.5*ui->qwtPlot->canvas()->height(),
                               QImage::Format_RGB32);
        img->setDotsPerMeterX(3.5*ui->qwtPlot->canvas()->logicalDpiX()*100/2.54);
        img->setDotsPerMeterY(3.5*ui->qwtPlot->canvas()->logicalDpiY()*100/2.54);
        img->fill(Qt::white);
        QwtPlotRenderer render;
        render.renderTo(ui->qwtPlot,*img);
        img->save(filename);
        delete img;
        //pix.save(filename);
    }
}

void MainWindow::DrawPlot(){
    int n=F.size();
    double dx=(F.last().X-F.first().X)/ui->qwtPlot->canvas()->width(); int i=0;
    qDebug()<<dx;

    S=getCubicSpline(F,n);
    X=new double[int((F.last().X-F.first().X)/dx)+1];
    Y=new double[int((F.last().X-F.first().X)/dx)+1];
    X1=new double[n];
    Y1=new double[n];

    i=0;
    X[0]=F.at(0).X;Y[0]=F.at(0).Y;
    for(int len=1;len<=int((F.last().X-F.first().X)/dx);++len){
        X[len]=X[len-1]+dx;
        Y[len]=S[i].A+(S[i].B+(S[i].C+S[i].D*(X[len]-F.at(i).X))*(X[len]-F.at(i).X))*(X[len]-F.at(i).X);
        //qDebug()<<Y[len];
        if(i<n-1&&X[len]>=F.at(i+1).X){
            i++;
        }
    }

    curv.setRawSamples(X,Y,(F.last().X-F.first().X)/dx+1);
    for (int i=0;i<n;i++){
        X1[i]=F.at(i).X;
        Y1[i]=F.at(i).Y;
    }
    source.setRawSamples(X1,Y1,n);
    curv.attach(ui->qwtPlot);
    source.attach(ui->qwtPlot);
    ui->qwtPlot->setAxisAutoScale(QwtPlot::xBottom);
    ui->qwtPlot->setAxisAutoScale(QwtPlot::yLeft);
    ui->qwtPlot->updateAxes();
    ui->qwtPlot->replot();
}

void MainWindow::GenerateModel(){
    QString filename=QFileDialog::getOpenFileName(this,"Открытие файла...");
    if(filename=="")
        return;
    ReadData(filename);
}

void MainWindow::DrawDemo(){
    double dx=(F.last().X-F.first().X)/ui->qwtPlot->canvas()->width();
    int size=int((F.last().X-F.first().X)/dx)+1;
    demoX=new double[size];
    demoY=new double[size];

    parsX=F.at(0).X;
    demoX[0]=F.at(0).X;demoY[0]=F.at(0).Y;
    for(int i=1;i<size;++i){
        parsX+=dx;
        try{
            demoX[i]=parsX;
            demoY[i]=parser.Eval();
        }catch (mu::ParserError &e){
            --i;
        }
    }

    demo.setTitle(ui->lineEdit->text());
    demo.setRawSamples(demoX,demoY,size);
    demo.attach(ui->qwtPlot);

}

void MainWindow::on_action_2_triggered(){

    QString filename=QFileDialog::getOpenFileName(this,"Открытие файла...");
    if(filename=="")
        return;
    ReadData(filename);

    ClearArrays();
    DrawPlot();

};

void MainWindow::on_action_3_triggered(){
    bool draw_demo=false;
    if(demoX!=NULL&&demoY!=NULL)
        draw_demo=true;
    ClearArrays();
    //qSort(F);
    if(draw_demo)
        DrawDemo();
    DrawPlot();
}

void MainWindow::on_action_4_triggered(){
    close();
}

void MainWindow::on_toolButton_3_clicked()
{
    ui->action_3->activate(QAction::Trigger);
}

void MainWindow::on_action_5_triggered()
{
    QString name=QFileDialog::getSaveFileName(this,"Сохранение отчета","","Файлы PDF (*.pdf)");
    if (name!=""){
        printer->setOutputFileName(name);
        //printer->setPageMargins(0,10,0,10,QPrinter::Millimeter);

        QTextDocument doc;
        doc.documentLayout()->setPaintDevice(printer);
        doc.setDocumentMargin(0);
        doc.setPageSize(printer->pageRect().size());

        QTextCursor main_curs(&doc),curs;
        QTextTable *table;
        QTextTableFormat tform;
        QTextCharFormat cform;
        QTextBlockFormat bform;
        QTextImageFormat iform;

       //qDebug()<<printer.resolution();

        bform.setAlignment(Qt::AlignHCenter);
        cform.setFontCapitalization(QFont::AllUppercase);
        cform.setFontWeight(QFont::Bold);
        cform.setFontFamily("Arial");
        cform.setFontPointSize(20);
        main_curs.insertBlock(bform,cform);
        main_curs.insertText("входные данные\n\n");

        main_curs.movePosition(QTextCursor::End);

        tform.setAlignment(Qt::AlignHCenter);
        tform.setCellSpacing(0);
        tform.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        table=main_curs.insertTable(F.size()+1,3);
        table->setFormat(tform);

        curs=table->cellAt(0,1).firstCursorPosition();
        bform.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        curs.setBlockFormat(bform);
        curs.insertText(QString("X"));

        curs=table->cellAt(0,2).firstCursorPosition();
        bform.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        curs.setBlockFormat(bform);
        curs.insertText(QString("Y"));

        for(int i=0;i<F.size();++i){
            QString msg;


            curs=table->cellAt(i+1,0).firstCursorPosition();
            bform.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            curs.setBlockFormat(bform);
            curs.insertText(msg.sprintf("%d",i));

            curs=table->cellAt(i+1,1).firstCursorPosition();
            bform.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            curs.setBlockFormat(bform);
            curs.insertText(msg.sprintf("%f",F[i].X));

            curs=table->cellAt(i+1,2).firstCursorPosition();
            bform.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            curs.setBlockFormat(bform);
            curs.insertText(msg.sprintf("%f",F[i].Y));

        }
        main_curs.movePosition(QTextCursor::End);

        bform.setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        bform.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
        cform.setFontCapitalization(QFont::AllUppercase);
        cform.setFontWeight(QFont::Bold);
        cform.setFontFamily("Arial");
        cform.setFontPointSize(20);
        main_curs.insertBlock(bform,cform);
        main_curs.insertText("График результата");

        main_curs.movePosition(QTextCursor::End);

        bform.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
        bform.setAlignment(Qt::AlignLeft);
        QImage *img=new QImage(3.5*doc.pageSize().width(),
                               0.8*3.5*doc.pageSize().height(),
                               QImage::Format_RGB32);
        img->setDotsPerMeterX(printer->logicalDpiX()*350/2.54);
        img->setDotsPerMeterY(printer->logicalDpiY()*350/2.54);
        qDebug()<<img->logicalDpiX()<<img->physicalDpiX();
        img->fill(Qt::white);
        QwtPlotRenderer rend;
        rend.renderTo(ui->qwtPlot,*img);
        doc.addResource(QTextDocument::ImageResource,QUrl("abracadabra"),QVariant(*img));

        iform.setWidth(doc.pageSize().width());
        iform.setHeight(0.8*doc.pageSize().height());
        iform.setName("abracadabra");
        main_curs.insertBlock(bform);
        main_curs.insertImage(iform);

        doc.print(printer);
        delete img;

    }
}

void MainWindow::on_toolButton_2_clicked()
{
    F.insertRow(F.size());
    qDebug()<<F.size();
}


void MainWindow::on_pushButton_clicked()
{
    parser.SetExpr(ui->lineEdit->text().toAscii().constData());
    int n=ui->spinBox->value();
    double  a=ui->doubleSpinBox->value(),
            b=ui->doubleSpinBox_2->value(),
            h=(b-a)/(n-1);
    ClearArrays();
    ClearModel();

    Point t;
    try{
        parsX=a;t.X=a;t.Y=parser.Eval();
    } catch (mu::Parser::exception_type &e){
        QMessageBox msg;
        msg.setText(e.GetMsg().c_str());
        msg.exec();
        return;
    }

    F<<t;
    for(int i=1;i<n;i++){
        parsX+=h;
        try{
            t.X=parsX;t.Y=parser.Eval();
            F<<t;
        } catch (...){
            --i;
        }
    };

    DrawDemo();
    DrawPlot();

}

void MainWindow::on_toolButton_4_clicked()
{
    ClearArrays();
    ClearModel();
}

void MainWindow::on_action_6_triggered()
{
    QProcess *process = new QProcess;
    QStringList args;
    args << QLatin1String("-collectionFile")
        << QLatin1String("C:/Users/Wolf/Documents/NumMat/NumMat-1.0.qhc")
        << QLatin1String("-enableRemoteControl");
    process->start(QLatin1String("assistant"), args);
}

void MainWindow::on_toolButton_clicked()
{
    QModelIndexList list=ui->tableView->selectionModel()->selectedRows();
    for(int i=list.size()-1;i>=0;--i){
        F.removeRow(list[i].row());
    }
}
