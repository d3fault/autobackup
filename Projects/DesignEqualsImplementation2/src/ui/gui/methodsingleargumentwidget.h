#ifndef METHODSINGLEARGUMENTWIDGET_H
#define METHODSINGLEARGUMENTWIDGET_H

#include <QWidget>

class QLineEdit;

class TabEmittingLineEdit;

class MethodSingleArgumentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MethodSingleArgumentWidget(QWidget *parent = 0);
    QString argumentType() const;
    QString argumentName() const;
    void focusOnArgType();
private:
    QLineEdit *m_ArgumentTypeLineEdit;
    TabEmittingLineEdit *m_ArgumentNameLineEdit;
signals:
    void tabPressedInNameLineEdit();
    void commitMethodequested(); //enter in either field (granted both are non-empty. but we don't sanitize, the slot (method) level does)

    void moveArgumentUpRequested(MethodSingleArgumentWidget *argumentToMoveUp);
    void moveArgumentDownRequested(MethodSingleArgumentWidget *argumentToMoveDown);
    void deleteArgumentRequested(MethodSingleArgumentWidget *argumentToDelete);
private slots:
    void handleMoveArgumentUpButtonClicked();
    void handleMoveArgumentDownButtonClicked();
    void handleDeleteArgumentButtonClicked();
};

#endif // METHODSINGLEARGUMENTWIDGET_H
