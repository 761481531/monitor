#ifndef IBPPAINT_H
#define IBPPAINT_H
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QEvent>


class Ibppaint: public QWidget
{
    Q_OBJECT
public:
    explicit Ibppaint(QWidget *parent = nullptr);
public slots:
    void refreshPixmap();
protected:
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
private:
    QPixmap * pic_;

signals:

};

#endif // IBPPAINT_H
