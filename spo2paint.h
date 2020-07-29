#ifndef SPO2PAINT_H
#define SPO2PAINT_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QEvent>


class SpO2paint: public QWidget
{
    Q_OBJECT
public:
    explicit SpO2paint(QWidget *parent = nullptr);
public slots:
    void refreshPixmap();
protected:
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
private:
    QPixmap * pic;

signals:

};

#endif // SPO2PAINT_H
