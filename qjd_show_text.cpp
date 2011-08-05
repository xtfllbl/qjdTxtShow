#include "qjd_show_text.h"
#include <QDebug>
#include <QStringList>
#include <QDataStream>

QJD_show_text::QJD_show_text(QObject *parent) :
    QObject(parent)
{
//    qDebug()<<"handle txt";
    fileType=0;
}

void QJD_show_text::setFileData(QString fileName)
{
//    qDebug()<<fileName;
    txtFile.setFileName(fileName);
    txtFile.open(QFile::ReadOnly);
    read.setDevice(&txtFile);
    QString line1,line2,line3;
    line1=txtFile.readLine();
    line2=txtFile.readLine();
    line3=txtFile.readLine();
    fileType=line1.toInt();  //文件类型

    int colNum,minVelo,veloGap;
    int rowNum,timeGap;
    if(fileType==1)
    {
        qDebug()<<"FileType::Type 1";
        QStringList line2stringList,line3stringList;
        line2stringList= line2.split(QRegExp("\\s+"),QString::SkipEmptyParts);  //去除空格可去除换行
        line3stringList= line3.split(QRegExp("\\s+"),QString::SkipEmptyParts);
//        qDebug()<<line2stringList<<line3stringList;
        colNum=line2stringList.at(0).toInt();
        minVelo=line2stringList.at(1).toInt();
        veloGap=line2stringList.at(2).toInt();

        rowNum=line3stringList.at(0).toInt();
        timeGap=line3stringList.at(1).toInt();
        qDebug()<<"Col Num::"<<colNum<<"Min Velo::"<<minVelo<<"Velo Gap::"<<veloGap
               <<"Row Num::"<<rowNum<<"Time Gap::"<<timeGap;
    }

    if(fileType==2)
    {
        qDebug()<<"FileType::Type 2";
        QStringList line2stringList,line3stringList;
        line2stringList= line2.split(QRegExp("\\s+"),QString::SkipEmptyParts);  //去除空格可去除换行
        line3stringList= line3.split(QRegExp("\\s+"),QString::SkipEmptyParts);
//        qDebug()<<line2stringList<<line3stringList;
        colNum=line2stringList.at(0).toInt();
        minVelo=line2stringList.at(1).toInt();
        veloGap=line2stringList.at(2).toInt();

        rowNum=line3stringList.at(0).toInt();
        timeGap=line3stringList.at(1).toInt();
        qDebug()<<"Col Num::"<<colNum<<"Min Velo::"<<minVelo<<"Velo Gap::"<<veloGap
               <<"Row Num::"<<rowNum<<"Time Gap::"<<timeGap;
    }
    QString temp;
    QVector<QStringList> dataStringList;
    QStringList tempStringList;
    for(int i=0;i<rowNum;i++)
    {
        temp=txtFile.readLine();
//        qDebug()<<"test:"<<i<<temp;
        tempStringList= temp.split(QRegExp("\\s+"),QString::SkipEmptyParts);

        dataStringList.append( tempStringList );
    }

//    qDebug()<<"~~~use row number change to:"<<dataStringList.size();
    //    qDebug()<<dataStringList[0].at(0);
    dataList.clear();
    dataList.resize(colNum);
    for(int i=0;i<colNum;i++)
    {
        for(int j=0;j<dataStringList.size();j++)
        {
            float a=dataStringList[j].at(i).toFloat();
//            qDebug()<<"test::"<<colNum<<dataStringList.size()<<":::"<<i<<j<<a;
            dataList[i].append(a);
        }
    }
    qDebug()<<"test::out";

//    qDebug()<<"~~~"<<dataList.size()<<dataList[0].size();

    setColNum(colNum);
    setRowNum(rowNum);   /// 还是以不扔0为佳

    if(fileType==1)
    {
        setColGap(veloGap);
        setRowGap(timeGap);
        setColMin(minVelo);
        setRowMin(0);
    }
    if(fileType==2)
    {
        setColGap(veloGap);
        setRowGap(timeGap);
        setColMin(minVelo);
        setRowMin(0);
    }
    txtFile.close();
}

void QJD_show_text::setData()
{
//    qDebug()<<"txt setData";
}

void QJD_show_text::setColNum(int num)
{
    _setColNum=num;
}
void QJD_show_text::setRowNum(int num)
{
    _setRowNum=num;

}
void QJD_show_text::setColGap(int num)
{
    _setColGap=num;

}
void QJD_show_text::setRowGap(int num)
{
    _setRowGap=num;

}
void QJD_show_text::setColMin(int num)
{
    _setColMin=num;

}
void QJD_show_text::setRowMin(int num)
{
    _setRowMin=num;

}

int QJD_show_text::colNum()
{
    return _setColNum;
}
int QJD_show_text::rowNum()
{
    return _setRowNum;
}
int QJD_show_text::colGap()
{
    return _setColGap;
}
int QJD_show_text::rowGap()
{
    return _setRowGap;
}
int QJD_show_text::colMin()
{
    return _setColMin;
}
int QJD_show_text::rowMin()
{
    return _setRowMin;
}
