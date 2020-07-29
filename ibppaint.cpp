#include "ibppaint.h"
#include "ibpwave.h"
#include <QPainter>
#include <QTimer>

Ibppaint::Ibppaint(QWidget *parent)
{
    pic_ = new QPixmap(this->width(), this->height());
    pic_->fill(Qt::black);

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refreshPixmap()));
    timer->start(5);
}

void Ibppaint::refreshPixmap()
{
    static int index = 0 ;
    static int ibpDataIndex = 0 ;

    QPainter painter(pic_);
    painter.setPen(QPen(QColor(255, 255, 0),3));

    int height = pic_->height();
    int width = pic_->width();

    painter.save();
    painter.setPen(QPen(Qt::black, 2));

    painter.drawLine(index , 0, index , pic_->height());
    painter.drawLine(index+1, 0 , index+1 , pic_->height());
    painter.drawLine(index+2, 0 , index+2 , pic_->height());
    painter.drawLine(index+3, 0 , index+3 , pic_->height());
    painter.restore();

    index = index + 2;
    if(index > pic_->width())
    {
        index = 2;
    }

    QPoint lineStart;
    QPoint lineEnd;
    lineStart.setX(index - 2);
    lineEnd.setX(index);

    int y = round( IbpWave[ibpDataIndex]*4); //pic->height()/2-(((double)Spo2Wave[spo2DataIndex]-2048)*pic->height()/2)
    lineStart.setY(y);

    ibpDataIndex += 1;
    if(ibpDataIndex >= sizeof(IbpWave)/sizeof(int))
    {
        ibpDataIndex = 0;
    }

    lineEnd.setY(round(IbpWave[ibpDataIndex]*4));

    painter.drawLine(lineStart , lineEnd);

    this->update();
}

void Ibppaint::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,pic_->width(),pic_->height(),*pic_);
}

void Ibppaint::resizeEvent(QResizeEvent *event)
{
    *pic_=pic_->scaled(this->size());
}
