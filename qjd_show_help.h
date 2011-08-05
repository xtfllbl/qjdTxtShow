#ifndef HELP_H
#define HELP_H

#include <QtGui/QDialog>
#include "ui_qjd_show_help.h"

namespace Ui {
    class Help;
}

class qjdHelp : public QDialog {
    Q_OBJECT
public:
    qjdHelp(QWidget *parent = 0);
    ~qjdHelp();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Help *m_ui;
};

#endif // HELP_H
