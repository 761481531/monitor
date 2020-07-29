#include "wave.h"
#include <QPainter>
#include <QTimer>
#include "ecgWave.h"

Wave::Wave(QWidget *parent) : QWidget(parent)
{
    //pix_=new QPixmap("../boxinghuizhi/image/pic2.PNG");
    //this->resize(400,300);

    pix_ = new QPixmap(this->width(), this->height());
    pix_->fill(Qt::black);

    QTimer *timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refreshPixmap()));
    timer->start(5);
}

void Wave::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,pix_->width(),pix_->height(),*pix_);
}

void Wave::resizeEvent(QResizeEvent *event)
{
    *pix_=pix_->scaled(this->size());
}

//更新图片内容，将波形画到pixmap上，更新，调用update
void Wave::refreshPixmap()
{
    static int index = 0 ;          //索引：x
    static int ecgDataIndex = 0 ;   //索引：y



    QPainter painter(pix_);
    painter.setPen(QPen(QColor(0,255,0),3));
    int height = pix_->height();
    int width = pix_->width();


    painter.save();
    painter.setPen(QPen(Qt::black, 2));

    painter.drawLine(index , 0, index , pix_->height());
    painter.drawLine(index+1, 0 , index+1 , pix_->height());
    painter.drawLine(index+2, 0 , index+2 , pix_->height());
    painter.drawLine(index+3, 0 , index+3 , pix_->height());
    painter.restore();

    index = index + 2;
    if(index > pix_->width())
    {
        index = 2;
    }
    //设置起始和结束的x,y坐标
    QPoint lineStart;
    QPoint lineEnd;
    lineStart.setX(index - 2);
    lineEnd.setX(index);

    int y = round( pix_->height()/2-(((double)ecgWave[ecgDataIndex]-2048)/600*pix_->height()/2));
    lineStart.setY(y);

    //更新参数循环取数
    ecgDataIndex += 10;
    if(ecgDataIndex >= sizeof(ecgWave)/sizeof(int))
    {
        ecgDataIndex = 0;
    }

    lineEnd.setY(round( pix_->height()/2-(((double)ecgWave[ecgDataIndex]-2048)/600*pix_->height()/2)));

    //绘制直线
    painter.drawLine(lineStart , lineEnd);

    //painter.drawLine(ppointX, ppointY, pointX, pointY);
        //if(pointX > width)
        //{
            //pointX = 0;
        //}

    this->update();



}
