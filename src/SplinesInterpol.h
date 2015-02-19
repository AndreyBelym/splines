#ifndef SPLINESINTERPOL_H
#define SPLINESINTERPOL_H
#include <QList>
#include <QAbstractTableModel>

typedef struct {
       double A,B,C,F;
} ThreeDiagMatrix;

typedef struct {
    double A,B,C,D;
} CubicSpline;
typedef struct Point{
    double X,Y;

} Point;
bool operator <(const Point &a,const Point &other);
class FuncTable:public QList<Point>,public QAbstractTableModel{

    int rowCount ( const QModelIndex & parent) const ;
    int columnCount ( const QModelIndex & parent ) const;
    QVariant data ( const QModelIndex & index, int role ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role );
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
public:
    void clearModel();

};

void test();
CubicSpline* getCubicSpline(const FuncTable &F, int count);
#endif // SPLINESINTERPOL_H
