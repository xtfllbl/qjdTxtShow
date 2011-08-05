#ifndef QJD_SHOW_TEXT_H
#define QJD_SHOW_TEXT_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QVector>

class QJD_show_text : public QObject
{
    Q_OBJECT
public:
    explicit QJD_show_text(QObject *parent = 0);
    QVector<QVector<float> > dataList;

    int fileType;

private:
    QFile txtFile;
    QDataStream read;

    void setColNum(int);
    void setRowNum(int);
    void setColGap(int);
    void setRowGap(int);
    void setColMin(int);
    void setRowMin(int);

    int _setColNum;
    int _setRowNum;
    int _setColGap;
    int _setRowGap;
    int _setColMin;
    int _setRowMin;

signals:

public slots:
    void setFileData(QString fileName);
    void setData();

    int colNum();
    int rowNum();
    int colGap();
    int rowGap();
    int colMin();
    int rowMin();

};

#endif // QJD_SHOW_TEXT_H
