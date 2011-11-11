#include "qjd_show_widget2.h"
#include <QFileDialog>
#include <QRubberBand>
#include <QtGui>
#include <math.h>
#define PAI 3.1415926
#define ZENYI 1

// 再次读取segy，以及处理segy
/// todo: 边上添加scrollbar

qjd_sgy_widget2::qjd_sgy_widget2(QWidget *)
{
    setPalette(Qt::white);

    // 色表
    zero=0;
    one =1;

    useMax=-1;
    useMin=-1;

    zeroPoint.setX(0);
    zeroPoint.setY(0);

    isSet=false;
    isZoomIn=false;
    isSetRange=false;
}

void qjd_sgy_widget2::cleanCache()
{
    boxXY.clear();
    boxAB.clear();
    boxAY.clear();
    colorData.clear();
    showColorData.clear();

    useMax=-1;
    useMin=-1;
}

void qjd_sgy_widget2::resizeEvent (QResizeEvent * )
{}

void qjd_sgy_widget2::setData(QVector<QVector<float> > data)
{
//    qDebug()<<"setData"<<data.size()<<data[0].size();  // This is what we need

    float tempMaxData=-100.0;
    float tempMinData=10000.0;
    maxData.clear();
    minData.clear();
    /// ------------------图像增益----------------- ///
//    for(int i=0;i<data.size();i++)
//    {
//        for(int j=0;j<data[0].size();j++)
//        {
//            data[i][j]=data[i][j]*ZENYI;
//        }
//    }
    /// ----------------------------------------------- ///
    if(isSetRange==false)
    {
        for(int i=0;i<data.size();i++)
        {
            for(int j=0;j<data[0].size();j++)
            {
                if(data[i][j]>tempMaxData)        //判断文件中的最大最小值
                {
                    tempMaxData=data[i][j];
                    maxData.append(data[i][j]);
                }
                else if(data[i][j]<tempMinData && data[i][j]!=0)
                {
                    tempMinData=data[i][j];
                    minData.append(data[i][j]);
                }
            }
        }
        qDebug()<<minData;

        //! 筛选最大最小, 此项目前被放弃使用,无法掌握尺度
        useMax=maxData[maxData.size()-1];
        useMin=minData[minData.size()-1];
    }
    /// ///////////////////////////////////
    /// 强制设置最大最小
//    if(useMax>5000)
//    {
//        useMax=5000;
//    }
//    useMin=2000;
    /// //////////////////////////////////
    qDebug()<<"ColorMaxValue:"<<useMax<<"ColorMinValue:"<<useMin;
    colorData.resize(data.size());
    int trueColor;
    float interval=useMax-useMin;
    unit=interval/255;            // 0.0078

    // 转换到0~255区间内
    for(int i=0;i<data.size();i++)
    {
        for(int j=0;j<data[0].size();j++)
        {
            if(data[i][j]>useMax)
                data[i][j]=useMax;
            if(data[i][j]<useMin)
                data[i][j]=useMin;

            data[i][j]=data[i][j]+(-useMin);  // (0~max+min)

            trueColor=(int)ceil(data[i][j]/unit);
            if(trueColor>255 || trueColor<0)
            {
                qDebug()<<"Error:color over range::"<<trueColor;
            }
            colorData[i].append(trueColor);   //保存了原始的颜色数据,需要截取一部分
        }
    }

    isSet=true;

    refreshPixmap();
}

void qjd_sgy_widget2::setArea(int left, int right, int up, int down)
{
    showColorData.clear();
    if(isSet==false)
    {
        qDebug()<<"setArea::None data has been set. Can not zoom in!";
        return;
    }
    if(isSet==true)
    {
        isZoomIn=true;
    }
    qDebug()<<"setArea";
    if(left<0)
    {
        left=0;
    }
    if(left>width())
    {
        left=width();
    }
    if(right<0)
    {
        right=0;
    }
    if(right>width())
    {
        right=width();
    }
    if(up<0)
    {
        up=0;
    }
    if(up>height())
    {
        up=height();
    }
    if(down<0)
    {
        down=0;
    }
    if(down>height())
    {
        down=height();
    }
//    int showWidth=right-left+1;
//    int showHeight=down-up+1;
    showColorData.resize(right-left);
//    qDebug()<<"need range H:"<<left<<right;
//    qDebug()<<"need range V:"<<up<<down;
    for(int i=left; i<right;i++)
    {
        for(int j=up;j<down;j++)
        {
            showColorData[i-left].append(boxXY[i][j]);   // 这里的i要单独从0开始
        }
    }

    /// 保存boxXY-->saveBoxXY
    saveBoxXY.clear();
    saveBoxXY.resize(boxXY.size());            //初始化XY数组
    for(int i=0;i<saveBoxXY.size();i++)
    {
        for(int j=0;j<boxXY[0].size();j++)
        {
            saveBoxXY[i].append(boxXY[i][j]);   /// 保存以备移动之需
        }
    }
//    qDebug()<<"~~~~~"<<showWidth<<showHeight<<showColorData.size()<<showColorData[0].size();
}


/// 这里的差值很成问题，不是整数差值，需要将小数对应到0-255的范围之中
// 标准差值
void qjd_sgy_widget2::setInterpolation(QVector<QVector<float> > data)
{
//    qDebug()<<"setInterpolation";
    boxAY.clear();
    boxAY.resize(data.size());            //初始化AY数组
    for(int j=0;j<data.size();j++)
        boxAY[j].resize(height());

    boxXY.clear();
    boxXY.resize(width());            //初始化XY数组
    for(int j=0;j<width();j++)
        boxXY[j].resize(height());


    /// 这差值完全不行阿，当中有空隙
    /*-------------------------------双线插值显示覆盖次数-------------------------------------*/
    // 原来的是网格，现在的好像不是
    // 使用此差值候数值全部为0
    unitX=(data.size()-1)*1./(width()-1);
    unitY=(data[0].size()-1)*1./(height()-1);

    //    emit sigPBarMaxSet(data.size()-1);
    int xyz;
//    qDebug()<<"n1="<<data.size()<<height()-1;
    for(int a=0;a<data.size();a++)                    //纵向插值,[a][b]-->[a][height()]
    {
        xyz=0;
        for(int y=0;y<height()-1;y++)
        {
            if(y*unitY<xyz+ceil(unitY) && y*unitY>=xyz)
            {
            }
            else
                xyz=xyz+(int)(ceil(unitY));

            pixelValue=(data[a][xyz]+(y*unitY-(int)(y*unitY))*(data[a][xyz+1]-data[a][xyz]));

            /// 这里没有问题阿。。。
            if(pixelValue>255 || pixelValue<0)
            {
                //                qDebug()<<"question value:"<<pixelValue;
            }

            boxAY[a][y]=pixelValue;
        }

        //        emit sigPBarValueSet(a);
    }


    int ABC;                                  //横向插值,[a][height()]-->[width()][height()]
//    qDebug()<<"n1b:"<<height();
    for(int Y=0;Y<height();Y++)
    {
        ABC=0;
        for(int X=0;X<width()-1;X++)
        {
            if(X*unitX<ABC+ceil(unitX) && X*unitX>=ABC)
            {
                // do nothing
            }
            else
                ABC=ABC+(int)(ceil(unitX));

            pixelValue=(boxAY[ABC][Y]+(X*unitX-ABC)*(boxAY[ABC+1][Y]-boxAY[ABC][Y]));

            //一堆问题数值,不知如何算出
            if(pixelValue>255 || pixelValue<0)
            {
                //                qDebug()<<"question value:"<<pixelValue<<"="<<boxAY[ABC][Y]<<"+"<<(X*unitX-ABC)<<"*"<<(boxAY[ABC+1][Y]-boxAY[ABC][Y]);
            }
            boxXY[X][Y]=(int)pixelValue;
        }
    }

    // 然后就是显示boxXY,如何显示呢？
    qDebug()<<"Show Size::"<<boxXY.size()<<boxXY[0].size();
}

/// 放大差值
void qjd_sgy_widget2::setZoomInterpolation(QVector<QVector<int> > data)
{
//    qDebug()<<"setZoomInterpolation";
    /// 每次差值都预先清空
    boxAY.clear();
    boxAY.resize(data.size());            //初始化AY数组
    for(int j=0;j<data.size();j++)
        boxAY[j].resize(height());

    boxXY.clear();
    boxXY.resize(width());            //初始化XY数组
    for(int j=0;j<width();j++)
        boxXY[j].resize(height());


    /*-------------------------------双线插值显示覆盖次数-------------------------------------*/
    // 原来的是网格，现在的好像不是
    // 使用此差值候数值全部为0
    unitX=(data.size()-1)*1./(width()-1);
    unitY=(data[0].size()-1)*1./(height()-1);

    //    emit sigPBarMaxSet(data.size()-1);
    int xyz;
//    qDebug()<<"n1="<<data.size()<<height()-1;
    for(int a=0;a<data.size();a++)                    //纵向插值,[a][b]-->[a][height()]
    {
        xyz=0;
        for(int y=0;y<height()-1;y++)
        {
            if(y*unitY<xyz+ceil(unitY) && y*unitY>=xyz)
            {
            }
            else
                xyz=xyz+(int)(ceil(unitY));

            pixelValue=(data[a][xyz]+(y*unitY-(int)(y*unitY))*(data[a][xyz+1]-data[a][xyz]));

            /// 这里没有问题阿。。。
            if(pixelValue>255 || pixelValue<0)
            {
                //                qDebug()<<"question value:"<<pixelValue;
            }

            boxAY[a][y]=pixelValue;
        }

        //        emit sigPBarValueSet(a);
    }


    int ABC;                                  //横向插值,[a][height()]-->[width()][height()]
//    qDebug()<<"n1b:"<<height();
    for(int Y=0;Y<height();Y++)
    {
        ABC=0;
        for(int X=0;X<width()-1;X++)
        {
            if(X*unitX<ABC+ceil(unitX) && X*unitX>=ABC)
            {
                // do nothing
            }
            else
                ABC=ABC+(int)(ceil(unitX));

            pixelValue=(boxAY[ABC][Y]+(X*unitX-ABC)*(boxAY[ABC+1][Y]-boxAY[ABC][Y]));

            //一堆问题数值,不知如何算出
            if(pixelValue>255 || pixelValue<0)
            {
                //                qDebug()<<"question value:"<<pixelValue<<"="<<boxAY[ABC][Y]<<"+"<<(X*unitX-ABC)<<"*"<<(boxAY[ABC+1][Y]-boxAY[ABC][Y]);
            }
            boxXY[X][Y]=(int)pixelValue;
        }
    }

    // 然后就是显示boxXY,如何显示呢？
    qDebug()<<"Show Size::"<<boxXY.size()<<boxXY[0].size();
}

void qjd_sgy_widget2::paintData(QPainter *painter)
{
    qDebug()<<"----------Paint Refreash-------- \n";
    QImage image(width(),height(),QImage::Format_ARGB32);
    int colorValue;
    setColorTable();                    //设置眼色表
    image.setColorTable(colorTable);    //将颜色表附上

    // 将小数们分配到各自的颜色去
    /// 这些数据需要更新哦
    //    qDebug()<<width()<<height()<<boxXY.size()<<boxXY[0].size();
    for(int i=0;i<width();i++)          //给每个像素上色
    {
        for(int j=0;j<height();j++)
        {
            colorValue=boxXY[i][j];  /// 其实需要一份存档

            // 有大量的负值和超值
            if(colorValue>=colorTable.size() || colorValue<0)
            {
                qDebug()<<"Warning::paintData::Qusetion color value:"<<i<<j<<colorValue;
                if(colorValue<0)
                    colorValue=0;
                if(colorValue>=colorTable.size())
                    colorValue=colorTable.size()-1;
            }

            //            qDebug()<<i<<j<<colorValue;
            image.setPixel(i,j,colorTable[colorValue]);
        }
        //        qDebug()<<"i:"<<i;   //0 就结束了。。。
    }
    painter->drawImage(0,0,image);
}

void qjd_sgy_widget2::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,pixmap);

    if (rubberBandIsShown)
    {
        painter.setPen(Qt::magenta);
        painter.drawRect(rubberBandRect.normalized().adjusted(0, 0, -1, -1));
    }
}

void qjd_sgy_widget2::refreshPixmap()
{
    qDebug()<<"refreshPixmap";
    pixmap = QPixmap(size());
    pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);
    painter.initFrom(this);

    if(isSet==true && isZoomIn==false)
    {
//        qDebug()<<"isSet is true";
        setInterpolation(colorData); //将这些颜色来差值
        paintData(&painter);
    }
    if(isSet==true && isZoomIn==true)
    {
//        qDebug()<<"isZoomIn is true";
        setZoomInterpolation(showColorData); //将这些颜色来差值
        paintData(&painter);
    }

    update();       //作为立即生效使用，不写的话只是不立即显示，拖动之后仍然正常显示
}


void qjd_sgy_widget2::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

/*--------------设置颜色表-------------*/
void qjd_sgy_widget2::setColorTable()
{
    //    qDebug()<<"setColorTable";
    colorTable.clear();
    int  i, r, g, b;
    if(zero==1)
    {
        /*----------黑白色表----------*/
        for (i = 0; i <= 255; i++)
        {
            r = 255 - i;
            g = 255 - i;
            b = 255 - i;
            colorTable<<qRgb(r,g,b);
        }
    }

    if(one==1)
    {
        /*----------彩色色表----------*/
        for (i = 0; i < 32; i++)
        {
            r = 0;
            g = 0;
            b = 131 + i * 4;
            colorTable<<qRgb(r,g,b);
        }
        for (i = 0; i < 32; i++)
        {
            r = 0;
            g = 4 + i * 4;
            b = 255;
            colorTable<<qRgb(r,g,b);
        }
        for (i = 1; i < 32; i++)
        {
            r = 0;
            g = 127 + i * 4;
            b = 255;
            colorTable<<qRgb(r,g,b);
        }
        for (i = 0; i < 32; i++)
        {
            r = i * 4;
            g = 255;
            b = 255 - i * 4;
            colorTable<<qRgb(r,g,b);
        }
        for (i = 0; i < 32; i++)
        {
            r = 128 + i * 4;
            g = 255;
            b = 128 - i * 4;
            colorTable<<qRgb(r,g,b);
        }
        for (i = 0; i < 64; i++)
        {
            r = 255;
            g = 255 - i * 4;
            b = 0;
            colorTable<<qRgb(r,g,b);
        }
        for (i = 0; i < 33; i++)
        {
            r = 255 - i * 4;
            g = 0;
            b = 0;
            colorTable<<qRgb(r,g,b);
        }
    }
}

void qjd_sgy_widget2::setColorRange(int min, int max)
{
    useMin=min;
    useMax=max;
    isSetRange=true;
}
