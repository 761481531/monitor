#ifndef WAVE_H
#define WAVE_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QEvent>

class Wave : public QWidget
{
    Q_OBJECT
public:
    explicit Wave(QWidget *parent = nullptr);
public slots:
    void refreshPixmap();
protected:
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
private:
    QPixmap * pix_;

signals:

};

#endif // WAVE_H
