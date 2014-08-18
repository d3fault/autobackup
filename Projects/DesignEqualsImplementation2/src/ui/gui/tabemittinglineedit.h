#ifndef TABEMITTINGLINEEDIT_H
#define TABEMITTINGLINEEDIT_H

#include <QLineEdit>

class TabEmittingLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit TabEmittingLineEdit(QWidget *parent = 0);
protected:
    virtual bool event(QEvent *event);
signals:
    void tabPressed();
};

#endif // TABEMITTINGLINEEDIT_H
