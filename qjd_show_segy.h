#ifndef QJD_SGY_SEGY_H
#define QJD_SGY_SEGY_H

#include <QObject>
#include <QFile>
#include "qjddatastream.h"
#include "qjdsegybh.h"
#include "qjdsegyth.h"
#include "qjdtraceheader.h"

class QJD_sgy_segy : public QObject
{
    Q_OBJECT
public:
    explicit QJD_sgy_segy(QObject *parent = 0);

signals:

public slots:

    void setFileData(QString);
    void read3200();
    bool read400();
    void setData();
    void read240();
    void cleanCache();
//    void readData();
    QVector<QVector<float> > dataList();
    int ns();
    int dt();
    int traceNum();

private:
    QFile sgyFile;
    QFile testFile;
    QJDDataStream read;
    QJDDataStream write;
    QByteArray bty;
    QJDSegyBH bh;
    QJDTraceHeader th;

    QVector<QVector<float> > _dataList;
    int _ns;
    int _dt;
    int _traceNum;

    bool isIBM;

};

#endif // QJD_SGY_SEGY_H
