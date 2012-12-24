#ifndef REDPARENTHESISCLOSECONTEXTREFRESHERWIDGET_H
#define REDPARENTHESISCLOSECONTEXTREFRESHERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "redparenthesisclosecontextrefresherplaintextedit.h"

class RedParenthesisCloseContextRefresherWidget : public QWidget
{
    Q_OBJECT
public:
    RedParenthesisCloseContextRefresherWidget(QWidget *parent = 0);
    ~RedParenthesisCloseContextRefresherWidget();
private:
    QVBoxLayout *m_Layout;
    QPushButton *m_LeftButton;
    QPushButton *m_RightButton;
    RedParenthesisCloseContextRefresherPlainTextEdit *m_Red;
    QPlainTextEdit *m_Debug;
};

#endif // REDPARENTHESISCLOSECONTEXTREFRESHERWIDGET_H
