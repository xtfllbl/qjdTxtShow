#include "qjd_show_mainwindow.h"
#include "ui_qjd_show_mainwindow.h"
#include <QFileDialog>
#include <QRubberBand>
#include <QtGui>
#include <QGridLayout>
#include <stdlib.h>
#include <stdio.h>
#define Num 100

qjdMainWindow::qjdMainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    XADD=5;
    YADD=15;
    COUNT=0;
    my = new qjd_sgy_widget2();  //换成widget2
    qjdsgy=new QJD_sgy_segy();
    qjdtxt=new QJD_show_text();
    changeRange=new changeColorRange();

    connect(changeRange,SIGNAL(rangeChanged(int,int)),this,SLOT(setColorRange(int,int)));

    my->rectWidth=0;
    my->rectHeight=0;

    saveHeightRatio=-1;
    saveWidthRatio=-1;
    useHor=80;
    useVer=80;

    isSgy=false;
    isTxt=false;

    isHorTooSmall=false;
    isVerTooSmall=false;

    ui->setupUi(this);
    ui->mainToolBar->setMovable(false);

    my->rubberBandIsShown = false;
    my->mouseRightZoom=false;

    corChange=false;
    label1 = new QLabel();
    label2 = new QLabel();
    label3 = new QLabel();
    label4 = new QLabel();
    statusLabel1=new QLabel();
    statusLabel2=new QLabel();
    statusLabel3=new QLabel();
    statusLabel4=new QLabel();
    statusLabel5=new QLabel();
    statusLabel6=new QLabel();

    statusLabel1->setMinimumWidth(35);
    statusLabel2->setMinimumWidth(50);
    statusLabel3->setMinimumWidth(35);
    statusLabel4->setMinimumWidth(50);
    statusLabel5->setMinimumWidth(35);
    statusLabel6->setMinimumWidth(50);

    statusBar()->addWidget(statusLabel1);
    statusBar()->addWidget(statusLabel2);
    statusBar()->addWidget(statusLabel3);
    statusBar()->addWidget(statusLabel4);
    statusBar()->addWidget(statusLabel5);
    statusBar()->addWidget(statusLabel6);

    label1->setFixedHeight(0); //空出上面 25
    label2->setFixedWidth(80);  //左面 80  |口
    label3->setFixedWidth(80);  //右面 80  口|
    label4->setFixedHeight(40); //下面 40

    QGridLayout *layout = new QGridLayout( );
    layout->addWidget(label1,0,0);
    layout->addWidget(label2,1,0);
    layout->addWidget(my,1,1);  //layout 中加入
    layout->addWidget(label3,1,2);
    //    layout->addWidget(verBar,1,3);
    layout->addWidget(label4,2,0);
    //    layout->addWidget(horBar,3,1);

    ui->mainw->setLayout(layout);  //设置layout
    setCentralWidget(ui->mainw);  //设置显示

    my->resize(640,613);

    //    scrollarea->setMouseTracking(true);
    my->setMouseTracking(true);
    setMouseTracking(true);
    ui->mainw->setMouseTracking(true);

    help = new qjdHelp();

    int width=QApplication::desktop()->width();         //获取屏幕的分辨率
    int height=QApplication::desktop()->height();
    this->move((width/2)-250,(height/2)-300);

    paintTimer=new QTimer(this);
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(update()));
    paintTimer->start(500);

    /// ---------------设置样式表----------------///
    QFile file(":/styles/stylesheet.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString s = in.readAll();
    this->setStyleSheet(s);
}

qjdMainWindow::~qjdMainWindow()
{
    delete ui;  //不置可否
}

void qjdMainWindow::mousePressEvent( QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton) //左键，无动作
    {
        my->rubberBandIsShown=false;
    }
    if(event->button()==Qt::RightButton)  // 右键，控制缩放
    {
        my->rubberBandIsShown=true;
        my->mouseRightZoom=true;
        setCursor(Qt::CrossCursor);
        QPoint point =  my->mapFromGlobal(event->globalPos());
        my->rubberBandRect.setTopLeft(point);
        my->rubberBandRect.setBottomRight(point);
        my->updateRubberBandRegion();
    }
}

void qjdMainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        resetAll();
    }
}

void qjdMainWindow::mouseMoveEvent ( QMouseEvent *event )
{
    QPoint point =  my->mapFromGlobal(event->globalPos());
//    qDebug()<<point<<my->width()<<my->height();
    if (my->rubberBandIsShown)      //画框框
    {
        my->updateRubberBandRegion();
        if(point.x() <1){                               //只能确保不落在mywidget的外面
            point.setX(1);
        }
        else if(point.x() > my->width() - 1){
            point.setX(my->width() - 1);
        }
        if(point.y() <1){
            point.setY(1);
        }
        else if(point.y() > my->height() - 1){
            point.setY(my->height() - 1);
        }
        my->rubberBandRect.setBottomRight(point);
        my->updateRubberBandRegion();
    }
    // 需要限制point(0,0)~(width,height)
    if(point.x()>=0 && point.x()<my->width() && point.y()>=0 && point.y()<my->height())
    {
        //        qDebug()<<"mouseMoveEvent::"<<point;
        setStatusBarText(point);
    }
}

void qjdMainWindow::mouseReleaseEvent ( QMouseEvent *event )
{
    if ((event->button() == Qt::RightButton) && my->rubberBandIsShown )  //鼠标右键缩放
    {
        my->rubberBandIsShown = false;
        my->updateRubberBandRegion();

        unsetCursor();  //不论条件，一律重置

        QRect rect = my->rubberBandRect.normalized();

        my->rectWidth=rect.width();
        my->rectHeight=rect.height();

        //        qDebug()<<"zuobaio:"<<rect.topLeft()<<rect.topRight()<<rect.bottomLeft()<<rect.bottomRight();
        // 差值坐标内的那些到主界面就可以了
        //        qDebug()<<"changkuan"<<rect.width()<<rect.height();
        if(rect.width()<10 || rect.height()<10 )
        {
            qDebug()<<"Warning::Too small to zoom in.";
            return;
        }

        /// x=topright-topleft  ;  y= bottomleft-topleft
        // 差值区域
        int left=rect.topLeft().x();
        int right=rect.topRight().x();
        int up=rect.topLeft().y();
        int down=rect.bottomLeft().y();

        if(my->isSet==true)
        {            
            /// 设置需要显示的区域
            my->setArea(left,right,up,down);

            /// 设置坐标轴
            setCor(rect.topLeft(),rect.topRight(),rect.bottomLeft(),rect.bottomRight());
            /// ---------------------------------------------------------------------------------------------------- ///
            my->refreshPixmap();
        }
    }
}

// 无法写到my当中去，写入便无法控制scrollbar 这个很好,哈哈.英文标点,
void qjdMainWindow::wheelEvent(QWheelEvent */*event*/)
{
}

void qjdMainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);       //引起错误
    paintCor(&painter);     // 坐标轴
    paintCorTxt(&painter );
    paintColorTable(&painter );
}

void qjdMainWindow::setCor(QPoint tl, QPoint tr, QPoint bl, QPoint br)
{
    /// 目前做法只是在第一层的基础上，再次放大后不可用
    qDebug()<<"setCor::"<<my->isSet;
    if(my->isSet==false)
    {
        qDebug()<<"setCor::None data has been set. Can not set cor!";
        return;
    }
    if(tl.x()<0)
        tl.setX(0);
    if(tr.x()<0)
        tr.setX(0);
    if(bl.x()<0)
        bl.setX(0);
    if(tl.x()<0)
        br.setX(0);

    if(tl.y()<0)
        tl.setY(0);
    if(tr.y()<0)
        tr.setY(0);
    if(bl.y()<0)
        bl.setY(0);
    if(tl.y()<0)
        br.setY(0);

    if(tl.x()>my->width())
        tl.setX(my->width());
    if(tr.x()>my->width())
        tr.setX(my->width());
    if(bl.x()>my->width())
        bl.setX(my->width());
    if(br.x()>my->width())
        br.setX(my->width());

    if(tl.y()>my->height())
        tl.setY(my->height());
    if(tr.y()>my->height())
        tr.setY(my->height());
    if(bl.y()>my->height())
        bl.setY(my->height());
    if(br.y()>my->height())
        br.setY(my->height());

    int x1=saveMinW+ ( (saveMaxW-saveMinW) * tl.x() / my->width() );
    int x2=saveMinW+ ( (saveMaxW-saveMinW) * tr.x() / my->width() );
    int y1=saveMinH+ ( (saveMaxH-saveMinH) * tl.y() / my->height() );
    int y2=saveMinH+ ( (saveMaxH-saveMinH) * bl.y() / my->height() );

    //    qDebug()<<saveMinW<<tl.x()<< ( (saveMaxW-saveMinW) * tl.x() / my->width() )<<x1<<"wahahahahahha";
    //    qDebug()<<saveMinW<<tr.x()<< ( (saveMaxW-saveMinW) * tr.x() / my->width() )<<x2<<"wahahahahahha";

    minW=x1;
    maxW=x2;
    minH=y1;
    maxH=y2;

    saveMinW=x1;
    saveMaxW=x2;
    saveMinH=y1;
    saveMaxH=y2;
    /// 这些需要保存，再放大就是从这里取值
    //    qDebug()<<"setCor::zuo biao::"<<x1<<x2<<y1<<y2;   //这个计算出来的就是对应坐标

    corChange=true;
    /// x   x1~x2
    /// y   y1~y2

}


/// 未放大时确保显示80pix为倍数周围的整数
/// 放大后见机行事
void qjdMainWindow::paintCor(QPainter *painter)
{
    /// -------------------没有放大----------------
    if( isTxt==true && corChange==false)
    {
        // 横轴
        float horGap=(qjdtxt->colMin()+qjdtxt->colNum()*qjdtxt->colGap())*1. / my->width();
        // 纵轴
        float verGap=(qjdtxt->rowMin()+qjdtxt->rowNum()*qjdtxt->rowGap())*1. / my->height();
        //        qDebug()<<"paintCor::offsetGap::"<<offsetGap;
        //        qDebug()<<"paintCor::timeGap::"<<timeGap;

        //        qDebug()<<qjdtxt->colMin()<<qjdtxt->rowMin();
        // 如何计算出最近的50的或100的倍数
        int a1=(int)(80*horGap);
        int b1=(int)(80*verGap);
        int a2=(int)( (qjdtxt->colMin()+80)*horGap );
        int b2=(int)( (qjdtxt->rowMin()+80)*verGap );
        // 使用25作为间隔
        float c1=a2/25;
        float c2=a1%25;
        float d1=b2/25;
        float d2=b1%25;
        if(c2<=12.5)
        {
            c1=c1;
        }
        if(c2>12.5)
        {
            c1=c1+1;
        }
        minHorNumber=(int)(c1*25);

        if(d2<=12.5)
        {
            d1=d1;
        }
        if(d2>12.5)
        {
            d1=d1+1;
        }
        minVerNumber=(int)(d1*25);
        useHor=minHorNumber*1./horGap;
        useVer=minVerNumber*1./verGap;
        //        qDebug()<<"Hor::minHor::"<<minHorNumber<<useHor<<"Ver::minVer::"<<minHorNumber<<useVer;
    }

    /// ------------------放大之后-----------------
    if( isTxt==true && corChange==true)
    {
        /// 无法在此控制不放大,直接显示空轴
        if(maxW-minW<25)
        {
            isHorTooSmall=true;
        }
        if(maxH-minH<25)
        {
            isVerTooSmall=true;
        }
        /// ----- 横轴------ ///
        if(maxW-minW>=25)
        {
            /// 1. 找到最小值附近的整数值
            float horGap= (maxW-minW) *1. / my->width();
            int a1=minW;
            float c1=ceil(a1*1./25); // 使用25作为间隔, 这里只许取大,不许取小
            minHorNumber=(int)(c1*25);

            /// 2. 定位整数值所在的坐标,以此为基来画
            startHorPos=(int)( (minHorNumber-minW)/horGap );
            int widRange=maxW-minW;
            float widTime=widRange*1./25;
            // 如何计算最佳值,目前间隔偏大
            if( widTime<1 )
            {
                // do nothing
                //                useHorGap=5;
            }
            if( widTime>=1 && widTime <8 )
            {
                useHorGap=25;
            }
            if( widTime>=8 && widTime <16 )
            {
                useHorGap=50;
            }
            if(widTime>=16 && widTime<32)
            {
                useHorGap=100;
            }
            if(widTime>=32 && widTime<64)
            {
                useHorGap=200;
            }
            if(widTime>=64 && widTime<96)
            {
                useHorGap=300;
            }
            if(widTime>=96 && widTime<128)
            {
                useHorGap=400;
            }
            if(widTime>=128 && widTime<160)
            {
                useHorGap=500;
            }
            if(widTime>= 160)
            {
                useHorGap=1000;
            }
            /// 3. 保存有效数据
            useHor=useHorGap*1./horGap;
            //            qDebug()<<"Width From:"<<minW<<"to"<<maxW<<"widRange"<<widRange<<"widTime"<<widTime;
            //            qDebug()<<"startHorPos"<<startHorPos<<"HorGap"<<horGap
            //                   <<"minHorNumber"<<minHorNumber<<"useHor"<<useHor;
        }

        /// ----- 纵轴------ ///
        if(maxH-minH>=25)
        {
            float verGap=(maxH-minH)*1. / my->height();

            int b1=minH;
            float d1=ceil(b1*1./25);
            minVerNumber=(int)(d1*25);
            startVerPos=(int)( (minVerNumber-minH)/verGap );

            int heiRange=maxH-minH;
            float heiTime=heiRange*1./25;
            // 如何计算最佳值
            if( heiTime<1 )
            {
                // do nothing
                useVerGap=5;
            }
            if( heiTime>=1 && heiTime <8 )
            {
                useVerGap=25;
            }
            if( heiTime>=8 && heiTime <16 )
            {
                useVerGap=50;
            }
            if(heiTime>=16 && heiTime<32)
            {
                useVerGap=100;
            }
            if(heiTime>=32 && heiTime<64)
            {
                useVerGap=200;
            }
            if(heiTime>=64 && heiTime<96)
            {
                useVerGap=300;
            }
            if(heiTime>=96 && heiTime<128)
            {
                useVerGap=400;
            }
            if(heiTime>=128 && heiTime<160)
            {
                useVerGap=500;
            }
            if(heiTime>=160 && heiTime<500)
            {
                useVerGap=1000;
            }
            if(heiTime>=160 && heiTime>500)
            {
                useVerGap=2000;
            }

            useVer=useVerGap*1./verGap;
        }
        //        qDebug()<<"Hor::minHor::"<<minHorNumber<<useHor<<"Ver::minVer::"<<minHorNumber<<useVer;
    }
    /// ---------------------------------
    int moreX=20;
    int moreY=10;
    if(point.isNull())      //保证这个值不变
    {
        point=my->mapTo(centralWidget(),my->zeroPoint);
    }
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setPen(QPen(Qt::black,2,Qt::SolidLine,Qt::RoundCap));

    /// ----------------------------------------------------------------------------------*/
    ///                           画坐标轴                                                            */
    /// ----------------------------------------------------------------------------------*/

    /// ----------------------------------横轴--------------------------------------
    painter->drawLine(point.x(),
                      my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height(),
                      my->width()+point.x(),
                      my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height());

    /// 放大前
    if( isTxt==true && corChange==false )
    {
        for(int x=0;x<=(my->width()/useHor);x++)
        {
            painter->drawLine((int)(useHor*x+point.x()),
                              my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height(),
                              (int)(useHor*x+point.x()),
                              my->height()+XADD+ui->mainToolBar->height()+moreX-6+ui->menuBar->height());
        }
        for(int x=0;x<(my->width()/(useHor/5));x++)
        {
            painter->drawLine((int)((useHor/5)*x+point.x()),
                              my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height(),
                              (int)((useHor/5)*x+point.x()),
                              my->height()+XADD+ui->mainToolBar->height()+moreX-3+ui->menuBar->height());
        }
    }
    /// 放大后
    if( isTxt==true && corChange==true && isHorTooSmall==false)
    {
        for(int x=0;x<=(my->width()/useHor);x++)
        {
            if(startHorPos+useHor*x+point.x() <= (my->width()+point.x()) )
            {
                painter->drawLine((int)(startHorPos+useHor*x+point.x()),
                                  my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height(),
                                  (int)(startHorPos+useHor*x+point.x()),
                                  my->height()+XADD+ui->mainToolBar->height()+moreX-6+ui->menuBar->height());
            }
        }
        for(int x=0;x<(my->width()/(useHor/5));x++)
        {
            if(startHorPos+(useHor/5)*x+point.x() <= (my->width()+point.x()) )
            {
                painter->drawLine((int)(startHorPos+(useHor/5)*x+point.x()),
                                  my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height(),
                                  (int)(startHorPos+(useHor/5)*x+point.x()),
                                  my->height()+XADD+ui->mainToolBar->height()+moreX-3+ui->menuBar->height());
            }
        }
    }


    /// ------------------------------------纵轴--------------------------------------
    painter->drawLine(my->width()+moreY+point.x(),
                      YADD+ui->mainToolBar->height()+ui->menuBar->height(),
                      my->width()+moreY+point.x(),
                      my->height()+YADD+ui->mainToolBar->height()+ui->menuBar->height());
    /// 放大前
    if( isTxt==true && corChange==false)
    {
        for(int y=0;y<=(my->height()/useVer);y++)
        {
            painter->drawLine(my->width()+moreY+point.x(),
                              (int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height() ),
                              my->width()+moreY+point.x()-6,
                              (int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height()) );
        }
        for(int y=0;y<(my->height()/(useVer/5));y++)
        {
            painter->drawLine(my->width()+moreY+point.x(),
                              (int)( YADD+ui->mainToolBar->height()+y*(useVer/5)+ui->menuBar->height() ),
                              my->width()+moreY+point.x()-3,
                              (int)( YADD+ui->mainToolBar->height()+y*(useVer/5)+ui->menuBar->height()) );
        }
    }
    /// 放大后
    if( isTxt==true && corChange==true && isVerTooSmall==false)
    {
        for(int y=0;y<=(my->height()/useVer);y++)
        {
            if((int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height()) +startVerPos<=
                    my->height()+YADD+ui->mainToolBar->height()+ui->menuBar->height())
            {
                painter->drawLine(my->width()+moreY+point.x(),
                                  (int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height()+startVerPos ),
                                  my->width()+moreY+point.x()-6,
                                  (int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height())+startVerPos );
            }
        }
        for(int y=0;y<(my->height()/(useVer/5));y++)
        {
            if((int)( YADD+ui->mainToolBar->height()+y*(useVer/5)+ui->menuBar->height() +startVerPos) <=
                    my->height()+YADD+ui->mainToolBar->height()+ui->menuBar->height())
            {
                painter->drawLine(my->width()+moreY+point.x(),
                                  (int)( YADD+ui->mainToolBar->height()+y*(useVer/5)+ui->menuBar->height()+startVerPos ),
                                  my->width()+moreY+point.x()-3,
                                  (int)( YADD+ui->mainToolBar->height()+y*(useVer/5)+ui->menuBar->height())+startVerPos );
            }
        }
    }


    if(isSgy==true)
    {
        /// time
        painter->drawText(my->width()+moreY+point.x()-4,
                          YADD+ui->mainToolBar->height()+ui->menuBar->height()-7,
                          "Time (ms)");

        /// Trace
        painter->drawText(my->width()+point.x(),
                          my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height()+20,
                          "Trace Num");
    }
//    qDebug()<<isTxt<<qjdtxt->fileType;
    if(isTxt==true && qjdtxt->fileType==1)
    {
        painter->save();
        painter->rotate(90);
        /// time
        // x是y,y是x,y是负方向
        painter->drawText(YADD+ui->mainToolBar->height()+ui->menuBar->height()+my->height()/2 -30,
                          -(  my->width()+moreY+point.x()+50   ),
                          "Time (ms)");
        painter->restore();

        /// velo
        painter->drawText(my->width()/2 +point.x() -30,
                          my->height()+ui->mainToolBar->height()+moreX+ui->menuBar->height()+3*YADD,
                          "Offset (m)");

    }
    if(isTxt==true && qjdtxt->fileType==2)
    {
        /// time
        painter->drawText(my->width()+moreY+point.x()-4,
                          YADD+ui->mainToolBar->height()+ui->menuBar->height()-7,
                          "Time (ms)");


        /// Velo
        painter->drawText(my->width()+point.x()+5,
                          my->height()+XADD+ui->mainToolBar->height()+moreX+ui->menuBar->height()+5,
                          "Unknow");
    }       

}

// 改变思路,依据坐标轴的变化,仅显示整数值
void qjdMainWindow::paintCorTxt(QPainter *painter)
{
    int moreX=20;
    int moreY=10;
    /// ----------------------------------------------------------------------------------*/
    ///                           写坐标值                                                          */
    /// ----------------------------------------------------------------------------------*/
    /// 未放大
    if( (isSgy==true || isTxt==true) && corChange==false)
    {
        for(int x=0;x<=(my->width()/useHor);x++)        //set Axis X text
        {
            QString string;
            if(isSgy==true)
            {
                string=QString::number((qjdsgy->traceNum()*useHor*x/my->width()));
            }
            if(isTxt==true)
            {
                if(x==0)
                {
                    string=QString::number(qjdtxt->colMin());
                }
                if(x>0)
                {
                    string=QString::number(minHorNumber*x);
                }
            }
            painter->drawText((int)(useHor*x+point.x()-10),
                              my->height()+XADD+ui->mainToolBar->height()+2*moreX+ui->menuBar->height(),
                              string);
        }

        int time=qjdsgy->ns()*qjdsgy->dt()/1000;
        for(int y=0;y<=my->height()/useVer;y++)
        {
            QString string;
            if(isSgy==true)
            {
                int number=(int)( time*(my->height()-useVer*y)/my->height() );
                string=QString::number(number);
            }
            if(isTxt==true)
            {
                if(y==0)
                {
                    string=QString::number(qjdtxt->rowMin());
                }
                if(y>0)
                {
                    string=QString::number(minVerNumber*y);
                }
            }
            painter->drawText(my->width()+moreY+point.x()+10,
                              (int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height()+5 ),
                              string);
        }
    }

    /// --------放大后的坐标---------------------- 需要重新计算----------------------------------------------------------------------
    if( (isSgy==true || isTxt==true) && corChange==true)
    {
        if(isHorTooSmall==false)
        {
            // 这个没法妥协,需要想办法显示到当中去,25之内的就不显示了
            for(int x=0;x<=(my->width()/useHor);x++)        //set Axis X text
            {
                if(startHorPos+useHor*x+point.x() <= (my->width()+point.x()) )
                {
                    QString string;
                    if(x==0)
                    {
                        string=QString::number(minHorNumber);
                    }
                    if(x>0)
                    {
                        string=QString::number(minHorNumber+useHorGap*x);  /// 不应当是这样计算的
                    }
                    painter->drawText((int)( startHorPos+useHor*x+point.x()-10 ),
                                      my->height()+XADD+ui->mainToolBar->height()+2*moreX+ui->menuBar->height(),
                                      string);
                }
            }
        }

        if( isVerTooSmall==false)
        {
            //        int time=qjdsgy->ns()*qjdsgy->dt()/1000;
            for(int y=0;y<=my->height()/useVer;y++)
            {
                if((int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height())+startVerPos <=
                        my->height()+YADD+ui->mainToolBar->height()+ui->menuBar->height())
                {
                    QString string;
                    if(y==0)
                    {
                        string=QString::number(minVerNumber);
                    }
                    if(y>0)
                    {
                        string=QString::number(minVerNumber+useVerGap*y);
                    }
                    painter->drawText(my->width()+moreY+point.x()+10,
                                      (int)( YADD+ui->mainToolBar->height()+y*useVer+ui->menuBar->height()+5+startVerPos ),
                                      string);
                }
            }
        }
    }
}

/// ----------------------------------------------------------------------------------*/
///                           画色表                                                               */
/// ----------------------------------------------------------------------------------*/
void qjdMainWindow::paintColorTable(QPainter *painter)
{

    QImage imageTable(30,my->height(),QImage::Format_ARGB32);
    int colorValue;
    my->setColorTable();                    //设置眼色表
    imageTable.setColorTable(my->colorTable);    //将颜色表附上

    for(int j=0;j<my->height();j++)          //给每个像素上色
    {
        for(int i=0;i<30;i++)
        {
            colorValue=255*j/my->height();
            imageTable.setPixel(i,j,my->colorTable[colorValue]);
        }
    }
    //色表
    painter->drawImage(60,YADD+ui->mainToolBar->height()+ui->menuBar->height(),imageTable);

    //包围
    painter->drawRect(60,YADD+ui->mainToolBar->height()+ui->menuBar->height(),30,my->height());
    //刻度
    for(int i=0;i<9;i++)
    {
        painter->drawLine(60,YADD+ui->mainToolBar->height()+ui->menuBar->height()+my->height()/9*i,
                          70,YADD+ui->mainToolBar->height()+ui->menuBar->height()+my->height()/9*i);
    }

    // 色表边上的数值
    //! 大于1234567的数值将不显示???
    if(my->isSet==true)
    {
        float gap=my->useMax-my->useMin;  //最大，最小
        int showMax=my->useMax;
        int yushu=showMax%10;
        if(yushu<=4)
        {
            showMax=showMax-yushu;
        }
        if(yushu>4)
        {
            showMax=showMax-yushu+10;
        }

        for(int i=0;i<9;i++)
        {
            int showValue=my->useMin+gap*1.*i/9;  /// 计算一下 离整数近
            yushu=showValue%10;
            if(yushu<=4)
            {
                showValue=showValue-yushu;
            }
            if(yushu>4)
            {
                showValue=showValue-yushu+10;
            }
            QString text = QString::number(showValue);

            // qDebug()<<"shu chu de text:"<<text;  // 输出没问题,问题出在显示上
            /// 最大显示12345678
            /// x,y,width,height
            painter->drawText(0,
                              ui->mainToolBar->height()+ui->menuBar->height()+my->height()/9*i+XADD,
                              58,50,Qt::AlignRight,
                              text);
        }
        QString text=QString::number(showMax);
        painter->drawText(0,
                          ui->mainToolBar->height()+ui->menuBar->height()+my->height()+XADD,
                          58,50,Qt::AlignRight,
                          text);
    }
}

void qjdMainWindow::resizeEvent (QResizeEvent * )
{
    //    qDebug()<<"resize:"<<my->width()<<my->height();
    // 此处放开,不受限制
    my->resize(my->width(),my->height());
    my->refreshPixmap();
}

void qjdMainWindow::cleanCache()
{
    qjdsgy->cleanCache();
    my->cleanCache();
    isSgy=false;
    isTxt=false;
    my->mouseRightZoom=false;
    my->isZoomIn=false;

    corChange=false;
}

void qjdMainWindow::on_actionOpen_triggered()
{
    QString openFileName=QFileDialog::getOpenFileName(this,tr("Open Files"),"/home/xtf/Source/SEGY/sgyshow",
                                                      tr("text files(*.txt *.text);;SEGY files(*.segy *.Segy *.SEGY *.sgy *.Sgy *.SGY)"));
    if(openFileName=="")
    {
        QMessageBox::warning(this,"Choose a file","You need to choose a segy file to continue.");
        return;
    }
    // 这里可以做一些事先的清理工作
    cleanCache();

    if(openFileName.right(3)=="txt" || openFileName.right(3)=="text")
    {
        qDebug()<<"FileType::This is a txt file.";
        isTxt=true;
        qjdtxt->setFileData(openFileName);
        my->setData(qjdtxt->dataList);

        saveMinW=qjdtxt->colMin();
        saveMaxW=qjdtxt->colMin()+qjdtxt->colGap()*(qjdtxt->colNum()-1);
        saveMinH=qjdtxt->rowMin();
        saveMaxH=qjdtxt->rowMin()+qjdtxt->rowGap()*(qjdtxt->rowNum()-1);

    }
    else
    {
        qDebug()<<"FileType::This is a segy file.";
        isSgy=true;
        qjdsgy->setFileData(openFileName);
        my->setData(qjdsgy->dataList());
        ///    qDebug()<<qjdsgy->dataList().size();  //输出数据准备显示

        saveMinW=0;
        saveMaxW=qjdsgy->traceNum();
        saveMinH=0;
        saveMaxH=qjdsgy->ns() * qjdsgy->dt() /1000;
    }
}

void qjdMainWindow::on_actionReset_triggered()
{
    resetAll();
}

void qjdMainWindow::on_actionClose_triggered()
{
    exit(0);
}


void qjdMainWindow::on_actionHelp_triggered()
{
    help->show();
}


void qjdMainWindow::on_actionChangeColorTable_triggered()
{
    qDebug()<<"on_actionChangeColorTable_triggered";
    if(my->zero==1)
    {
        my->one=1;
        my->zero=0;

        my->refreshPixmap();
        return;
    }
    if(my->one==1)
    {
        my->zero=1;
        my->one=0;
        my->refreshPixmap();

        return;
    }
}


void qjdMainWindow::scrollBarChange(int /*value*/)
{
    // 本质一样,所以不需要分横竖
    //    qDebug()<<"scrollbar change::"<<value;
}

void qjdMainWindow::setStatusBarText(QPoint point)
{
    if(isTxt==true)
    {
        statusLabel1->setText("Offset (m)::");
        QString offset=QString::number((qjdtxt->colMin()+(qjdtxt->colNum()-1)*qjdtxt->colGap()*point.x()/my->width()));
        statusLabel2->setText(offset);

        statusLabel3->setText("Time (ms)::");
        QString time=QString::number((qjdtxt->rowMin()+(qjdtxt->rowNum()-1)*qjdtxt->rowGap() ) * (point.y())/my->height());
        statusLabel4->setText(time);

        /// 关键是值怎么算
        statusLabel5->setText("Velo (m/s)::");
        float xxx=my->boxXY[point.x()][point.y()];
        QString a=QString::number(xxx*my->unit + my->useMin);  // 这个是从0开始
        statusLabel6->setText(a);
    }
}

void qjdMainWindow::on_actionChangeColorTableRange_triggered()
{
    changeRange->ui->spinMin->setValue((int)my->useMin);
    changeRange->ui->spinMax->setValue((int)my->useMax);
    changeRange->show();
}

void qjdMainWindow::setColorRange(int min, int max)
{
    qDebug()<<"setColorRange";

    cleanCache();

    // -----------------------------------------------------------------------------------------------------------//
    isTxt=true;
    my->setColorRange(min,max);
    my->setData(qjdtxt->dataList);

    saveMinW=qjdtxt->colMin();
    saveMaxW=qjdtxt->colMin()+qjdtxt->colGap()*(qjdtxt->colNum()-1);
    saveMinH=qjdtxt->rowMin();
    saveMaxH=qjdtxt->rowMin()+qjdtxt->rowGap()*(qjdtxt->rowNum()-1);

}

void qjdMainWindow::resetAll()
{

    my->mouseRightZoom=false;
    my->isZoomIn=false;

    corChange=false;

    //    qDebug()<<"on_actionReset_triggered"<<"isTXt::"<<isTxt<<"issgy::"<<isSgy;
    if(isTxt==true)
    {
        saveMinW=qjdtxt->colMin();
        saveMaxW=qjdtxt->colMin()+qjdtxt->colGap()*(qjdtxt->colNum()-1);
        saveMinH=qjdtxt->rowMin();
        saveMaxH=qjdtxt->rowMin()+qjdtxt->rowGap()*(qjdtxt->rowNum()-1);
    }
    if(isSgy==true)
    {
        saveMinW=0;
        saveMaxW=qjdsgy->traceNum();
        saveMinH=0;
        saveMaxH=qjdsgy->ns() * qjdsgy->dt() /1000;
    }
    useHor=80;
    useVer=80;

    isHorTooSmall=false;
    isVerTooSmall=false;

    my->refreshPixmap();
}
