#ifndef SOMEWIDGETLISTENTRYTYPEWIDGET_H
#define SOMEWIDGETLISTENTRYTYPEWIDGET_H

#include <QWidget>

class QLineEdit;

class SomeWidgetListEntryTypeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SomeWidgetListEntryTypeWidget(QWidget *parent = 0);
    QString anEntry0() const;
    QString anotherEntry1() const;
private:
    QLineEdit *m_AnEntry0LineEdit;
    QLineEdit *m_AnotherEntry1LineEdit;
};

#endif // SOMEWIDGETLISTENTRYTYPEWIDGET_H
