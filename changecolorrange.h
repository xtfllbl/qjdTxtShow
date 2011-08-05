#ifndef CHANGECOLORRANGE_H
#define CHANGECOLORRANGE_H

#include <QWidget>
#include "ui_changecolorrange.h"
namespace Ui {
    class changeColorRange;
}

class changeColorRange : public QWidget
{
    Q_OBJECT

public:
    explicit changeColorRange(QWidget *parent = 0);
    ~changeColorRange();

    Ui::changeColorRange *ui;
private slots:
    void on_btnCancle_clicked();

    void on_btnOK_clicked();

private:
signals:
    void rangeChanged(int min,int max);
};

#endif // CHANGECOLORRANGE_H
