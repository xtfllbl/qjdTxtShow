#include "qjd_show_segy.h"
#include <QDebug>
#include "qjd.h"

/// 可以换成读文本文件的类
QJD_sgy_segy::QJD_sgy_segy(QObject *parent) :
        QObject(parent)
{
//    testFile.setFileName("test.out");
//    testFile.open(QFile::WriteOnly);
//    write.setDevice(&testFile);

    _dataList.clear();
    _ns=-1;
    _dt=-1;
    _traceNum=-1;
    isIBM=false;
}

void QJD_sgy_segy::cleanCache()
{
    _dataList.clear();
    _ns=-1;
    _dt=-1;
    _traceNum=-1;
    isIBM=false;

}
// 这是由外部调用的
void QJD_sgy_segy::setFileData(QString fileName)
{
    qDebug()<<fileName;
    sgyFile.setFileName(fileName);
    sgyFile.open(QFile::ReadOnly);  //只读，显示图像而已

    // 传回主界面显示数据就好
    read.setDevice(&sgyFile);
    /// ----------------- 目前格式为固定,需要更改为支持多种
    read.setFormat(QJDDataStream::IEEE);
    read.setByteOrder(QJDDataStream::LittleEndian);

    read3200();

    if(read400()==false)
        return;
    setData();

    sgyFile.close();
//    testFile.close();
}

void QJD_sgy_segy::read3200()
{
    bty.resize(3201);       //切勿忘记开空间，后果不堪设想
    sgyFile.seek(0);
    read.readRawData(bty.data(),3200);
    *(bty.data()+3200)='\0';
}

bool QJD_sgy_segy::read400()
{
    qDebug()<<"read400";
    sgyFile.seek(3200);
    read>>bh;
    // ibm=1;ieee=5
//    qDebug()<<"bh.format"<<bh.format;
    if(bh.format==1)
    {
        qDebug()<<"read set format to IBM";
        read.setFormat(QJDDataStream::IBM);
    }
//    qDebug()<<bh.hns<<bh.hdt<<bh.format;
    _ns=bh.hns;
    _dt=bh.hdt;

    qDebug()<<"----------------------------------"<<_ns<<_dt;
    if(_ns>20000 || _dt>20000)
    {
        qDebug()<<"abnormal";
        read.setByteOrder(QJDDataStream::BigEndian);
        isIBM=true;
        read400();
    }
    return true;
}

void QJD_sgy_segy::setData()
{
    int traceNum=(sgyFile.size()-3200)/(240+bh.hns*sizeof(float));  //取得道数
    _traceNum=traceNum;
//    qDebug()<<traceNum;  // 160

    sgyFile.seek(3200);
    read>>bh;
//    qDebug()<<"wahaha"<<bh.hns<<bh.hdt;
    _dataList.resize(traceNum);
    for(int i=0;i<traceNum;i++)
//        for(int i=0;i<1;i++)
    {
        read.readRawData((char *)&th,240);  // 显示的话不需要
        int saveNS;
        if(isIBM==true)
        {
            th.swap_header();
        }
        saveNS=th.ns;
        if(saveNS==0 ||saveNS>20000)  //做好多种预防措施
        {
            saveNS=bh.hns;
        }
//        qDebug()<<traceNum<<th.ns<<th.dt;
        float temp;

        for(int j=0;j<saveNS;j++)
        {
            read>>temp;
            _dataList[i].append(temp);  //其实开了N多vector,有点浪费
//            qDebug()<<_dataList;
//            qDebug()<<temp;
        }
//        qDebug()<<"~~~"<<_dataList.at(0).size();
//        qDebug()<<"saveNS::"<<saveNS;
    }
    qDebug()<<_dataList[1];
}

void QJD_sgy_segy::read240()
{

}

// 返回数据, 二维数组好一些
QVector<QVector<float> > QJD_sgy_segy::dataList()
{
    return _dataList;
}

int QJD_sgy_segy::ns()
{
    return _ns;
}
int QJD_sgy_segy::dt()
{
    return _dt;
}
int QJD_sgy_segy::traceNum()
{
    return _traceNum;
}
