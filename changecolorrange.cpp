#include "changecolorrange.h"
#include "ui_changecolorrange.h"

changeColorRange::changeColorRange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::changeColorRange)
{
    ui->setupUi(this);
    setWindowTitle("ChangeRange");

    ui->spinMax->setMaximum(1000000);
    ui->spinMin->setMaximum(1000000);
}

changeColorRange::~changeColorRange()
{
    delete ui;
}

void changeColorRange::on_btnCancle_clicked()
{
    close();
}

void changeColorRange::on_btnOK_clicked()
{
    int min=ui->spinMin->value();
    int max=ui->spinMax->value();
    emit rangeChanged(min,max);
}
