#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

class clientWidget : public QWidget
{
    Q_OBJECT
public:
    clientWidget(QWidget *parent = 0);
    ~clientWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_AddSslClientButton;
signals:
    void addSslClientRequested();
public slots:
    void initGui();
    void handleD(const QString &msg);
};

#endif // CLIENTWIDGET_H
