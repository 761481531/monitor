#include "spo2paint.h"
#include "spo2wave.h"
#include <QPainter>
#include <QTimer>

SpO2paint::SpO2paint(QWidget *parent)
{
    pic = new QPixmap(this->width(), this->height());
    pic->fill(Qt::black);

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refreshPixmap()));
    timer->start(5);
}

void SpO2paint::refreshPixmap()
{
    static int index = 0 ;
    static int spo2DataIndex = 0 ;

    QPainter painter(pic);
    painter.setPen(QPen(QColor(255, 0, 0),3));

    int height = pic->height();
    int width = pic->width();

    painter.save();
    painter.setPen(QPen(Qt::black, 2));

    painter.drawLine(index , 0, index , pic->height());
    painter.drawLine(index+1, 0 , index+1 , pic->height());
    painter.drawLine(index+2, 0 , index+2 , pic->height());
    painter.drawLine(index+3, 0 , index+3 , pic->height());
    painter.restore();

    index = index + 2;
    if(index > pic->width())
    {
        index = 2;
    }

    QPoint lineStart;
    QPoint lineEnd;
    lineStart.setX(index - 2);
    lineEnd.setX(index);

    int y = round( Spo2Wave[spo2DataIndex]); //pic->height()/2-(((double)Spo2Wave[spo2DataIndex]-2048)*pic->height()/2)
    lineStart.setY(y);

    spo2DataIndex += 3;
    if(spo2DataIndex >= sizeof(Spo2Wave)/sizeof(int))
    {
        spo2DataIndex = 0;
    }

    lineEnd.setY(round(Spo2Wave[spo2DataIndex]));//pic->height()/2-(((double)Spo2Wave[spo2DataIndex]-2048)*pic->height()/2)

    painter.drawLine(lineStart , lineEnd);

    this->update();

}

void SpO2paint::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,pic->width(),pic->height(),*pic);
}

void SpO2paint::resizeEvent(QResizeEvent *event)
{
    *pic=pic->scaled(this->size());
}
