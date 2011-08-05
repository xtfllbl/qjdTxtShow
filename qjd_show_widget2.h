#ifndef QJD_SGY_WIDGET2_H
#define QJD_SGY_WIDGET2_H

#include <QtGui/QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>
#include <QMouseEvent>
#include <stdio.h>
#include <QPointF>
#include <QDebug>
#include <QScrollArea>
#include <QLinearGradient>


class qjd_sgy_widget2 : public QWidget
{
    Q_OBJECT
public:
    qjd_sgy_widget2(QWidget *parent = 0);
    //~MyWidget();

    qreal rectWidth;
    qreal rectHeight;
    qreal drawWid;
    qreal drawHei;
    bool rubberBandIsShown;
    bool mouseRightZoom;
    bool zoomInAvailable;

    QRect rubberBandRect;

    float unit;  //回算需要
    QVector<QVector<int> > boxXY;
    QVector<QVector<int> > saveBoxXY;  /// 保存时机要掌握好
    QVector<QVector<int> > showColorData;

    QVector<QRgb> colorTable;

    bool zero;
    bool one;

    QPoint zeroPoint;

    bool isSet;
    bool isZoomIn;

    float useMax;
    float useMin;

    bool isSetRange;


public slots:
    void setData(QVector<QVector<float> >);
    void refreshPixmap();
    void updateRubberBandRegion();

    void setColorTable();
    void cleanCache();

    void setArea(int left,int right,int up,int down);

    void setColorRange(int min,int max);


private:

    QPixmap pixmap;


    QVector<QVector<float> > boxAB;
    QVector<QVector<float> > boxAY;
    qreal unitX;
    qreal unitY;
    float pixelValue;
    QVector<float> maxData;
    QVector<float> minData;

    QVector<QVector<float> > colorData;

    void setInterpolation(QVector<QVector<float> >);
    void setZoomInterpolation(QVector<QVector<int> >);

protected:
    void paintEvent(QPaintEvent *);
    void paintData(QPainter *painter);

signals:
    void sigPBarMaxSet(int max);
    void sigPBarValueSet(int value);
private slots:
    void resizeEvent ( QResizeEvent * );


};

#endif // QJD_SGY_WIDGET2_H
