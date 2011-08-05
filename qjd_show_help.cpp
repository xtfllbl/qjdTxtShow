#include "qjd_show_help.h"
#include "ui_qjd_show_help.h"
#include <QFile>
#include <QTextStream>

qjdHelp::qjdHelp(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Help)
{
    m_ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    QFile file(":/styles/stylesheet.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString s = in.readAll();
    this->setStyleSheet(s);
}

qjdHelp::~qjdHelp()
{
    delete m_ui;
}

void qjdHelp::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
