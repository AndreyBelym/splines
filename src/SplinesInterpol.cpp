#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "SplinesInterpol.h"
#include <QDebug>

double *TDMA(ThreeDiagMatrix *params,int count)
{
    int i;
    int n=count-1;
    double *P=new double[count],*Q=new double[count];

    //P[0]=params[0].C/params[0].B; Q[0]= - params[0].F/params[0].B;
    P[0]=0;Q[0]=0;
    for (i=1;i<=n;i++){
        double R=params[i-1].B-params[i-1].A*P[i-1];
        if(fabs(R)<0.000000001)
            qDebug()<<"Error!";
        P[i]=params[i-1].C / R;
        Q[i]= (- params[i-1].F + params[i-1].A*Q[i-1]) / R;
    }

    double *X=new double[count];

    X[n]= (- params[n].F + params[n].A*Q[n]) / (params[n].B-params[n].A*P[n]);
    for (i=n-1;i>=0;i--){
        X[i]=P[i+1]*X[i+1]+Q[i+1];
    }

    delete P; delete Q;
    return X;
}
CubicSpline* getCubicSpline(const FuncTable &F, int count){
    ThreeDiagMatrix *sys=new ThreeDiagMatrix[count];
    double *M;
    int n=count-1,i;
    for(i=1;i<n;i++){
        double h=F[i].X-F[i-1].X, h_1=F[i+1].X-F[i].X;
        double df=F[i].Y-F[i-1].Y, df_1=F[i+1].Y-F[i].Y;

        sys[i].A=h/6;
        sys[i].B=-(h+h_1)/3;
        sys[i].C=h_1/6;
        sys[i].F=df_1/h_1-df/h;

        //std::cout<<"m1:"<<M_PI/sys[i].A<<" x2:"<<M_PI/sys[i].B<<" x3:"<<M_PI/sys[i].C<<" x4:"<<sys[i].F-
        //                            6*(F[3].Y-2*F[2].Y+F[1].Y)/M_PI<<"\n";
    }

    sys[0].A=0; sys[0].B=-1; sys[0].C=0; sys[0].F=0;
    sys[n].A=0; sys[n].B=-1; sys[n].C=0; sys[n].F=0;
    M=TDMA(sys,count);

    CubicSpline *S=new CubicSpline[n];
    for (i=0;i<n;i++){
        double h_1=F[i+1].X-F[i].X;
        double df_1=F[i+1].Y-F[i].Y;
        double dm_1=M[i+1]-M[i];

        S[i].A=F[i].Y;
        S[i].B=df_1/h_1-h_1*M[i]/2-h_1*dm_1/6;
        S[i].C=M[i]/2;
        S[i].D=dm_1/6/h_1;
    }
    delete M; delete sys;
    return S;
}
double calcSpline(CubicSpline *S,FuncTable* F,int count){
    for(int i=0;i<count;i++){

    }
}

void test_TDMA(){
    ThreeDiagMatrix a[]={{0,-1,2,5},{2,1,1,3},{1,1,1,3},{1,-1,0,7}};
    double *x=TDMA(a,4);
    //std::cout<<"x1:"<<x[0]<<" x2:"<<x[1]<<" x3:"<<x[2]<<" x4:"<<x[3]<<"\n";
    delete x;

}

void test(){

}

int FuncTable::rowCount ( const QModelIndex & parent = QModelIndex() ) const {
    return this->size();
}

int FuncTable::columnCount ( const QModelIndex & parent = QModelIndex() ) const{
    return 2;
}

QVariant FuncTable::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const{
    switch(role){
        case Qt::DisplayRole:
        case Qt::EditRole:
            if(index.row()<this->size()){
                if(index.column()==0)
                    return QVariant(this->at(index.row()).X);
                else if (index.column()==1);
                    return QVariant(this->at(index.row()).Y);
            } else
                return QVariant(QVariant::Invalid);
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignRight|Qt::AlignVCenter);
        default:
            return QVariant(QVariant::Invalid);
    }


}

QVariant FuncTable::headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const{
    switch(role){
        case Qt::DisplayRole:
        case Qt::EditRole:
            if(orientation==Qt::Vertical){
                return QVariant(section);
            } else if (orientation==Qt::Horizontal){
                if (section==0)
                    return QVariant("X");
                else if(section==1)
                    return QVariant("Y");
                else
                    return QVariant(QVariant::Invalid);
            } else
                return QVariant(QVariant::Invalid);
        default:
            return QVariant(QVariant::Invalid);
    }


}

Qt::ItemFlags FuncTable::flags ( const QModelIndex & index ) const{
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
}

bool FuncTable::setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole ){
    switch(role){
        case Qt::EditRole:
            if(index.row()<this->size()){
                if(index.column()==0){
                    (*this)[index.row()].X=value.toDouble();
                    emit this->dataChanged(index,index);
                    qSort(*this);
                    return true;
                }
                else if (index.column()==1){
                    (*this)[index.row()].Y=value.toDouble();
                    emit this->dataChanged(index,index);
                    //qSort(*this);
                    return true;
                }
                return false;
            } else
                return false;
        default:
            return false;
    }
}

bool FuncTable::insertRows ( int row, int count, const QModelIndex & parent ){
    if (count>0&&row<=this->size()){
        this->beginInsertRows(parent,row,row+count-1);
        while(count){
            Point t={0,0};
            *this<<t;
            --count;
        }
        endInsertRows();
        return true;
    } else return false;
}

bool FuncTable::removeRows ( int row, int count, const QModelIndex & parent ){
    if (count>0&&row<=this->size()){
        this->beginRemoveRows(parent,row,row+count-1);
        while(count){
            this->removeAt(row);
            --count;
        }
        endRemoveRows();
        return true;
    } else return false;
}

bool operator <(const Point &a, const Point &other){
    return (a.X)<(other.X);
}


void FuncTable::clearModel(){
    this->clear();
    this->reset();
}

