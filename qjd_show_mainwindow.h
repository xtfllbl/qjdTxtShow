#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QPainter>
#include <QLinearGradient>
#include <QMouseEvent>
#include <stdio.h>
#include <QPointF>
#include <QDebug>
#include <QScrollArea>
#include <QLabel>
#include <QFile>
#include "qjd_show_widget2.h"
#include "qjd_show_text.h"
#include "qjd_show_help.h"
#include "qjd_show_segy.h"
#include <QProgressBar>
#include "qjdscrollbar.h"
#include "changecolorrange.h"
#include <QTimer>

namespace Ui
{
    class MainWindow;
}
class qjdMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    qjdMainWindow(QWidget *parent = 0);
    ~qjdMainWindow();
    Ui::MainWindow *ui;

private:
    qjd_sgy_widget2 *my;
    qjdHelp *help;
    QJD_sgy_segy *qjdsgy;
    QJD_show_text *qjdtxt;
    changeColorRange *changeRange;

//    QScrollBar *horBar;
//    QScrollBar *verBar;
//    QJDScrollBar *hBar;
//    QJDScrollBar *vBar;
    long COUNT;
    QTimer *paintTimer;

    QLabel *label1;
    QLabel *label2;
    QLabel *label3;
    QLabel *label4;
    QLabel *statusLabel1;
    QLabel *statusLabel2;
    QLabel *statusLabel3;
    QLabel *statusLabel4;
    QLabel *statusLabel5;
    QLabel *statusLabel6;
//    QLabel *perWidget;
    int perWidgetMax;
    QPoint point;

    int minW;
    int maxW;
    int minH;
    int maxH;

    int saveMinW;
    int saveMaxW;
    int saveMinH;
    int saveMaxH;

    bool corChange;

    float saveHeightRatio;
    float saveWidthRatio;

    bool isSgy;
    bool isTxt;

    bool isHorTooSmall;
    bool isVerTooSmall;

    int minHorNumber;
    int minVerNumber;
    float useHor;
    float useHorGap;
    float useVer;
    float useVerGap;
    int startHorPos;
    int startVerPos;

    int XADD;
    int YADD;
private slots:
    void on_actionChangeColorTable_triggered();
    void on_actionOpen_triggered();
    void on_actionHelp_triggered();
    void on_actionClose_triggered();
    void on_actionReset_triggered();

    void mouseMoveEvent ( QMouseEvent *);
    void mouseReleaseEvent ( QMouseEvent *);
    void mousePressEvent( QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void paintEvent(QPaintEvent *);
    void paintCor(QPainter *painter);
    void paintCorTxt(QPainter *painter);
    void paintColorTable(QPainter *painter);
    void resizeEvent (QResizeEvent * );

    void cleanCache();

    void setCor(QPoint,QPoint,QPoint,QPoint);

    void scrollBarChange(int);
    void setStatusBarText(QPoint);

    void on_actionChangeColorTableRange_triggered();

    void setColorRange(int min,int max);
    void resetAll();
};


#endif // MAINWINDOW_H
