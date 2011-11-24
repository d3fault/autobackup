#ifndef LAYOUTAPPENDUPDATESGUI_H
#define LAYOUTAPPENDUPDATESGUI_H

#include <QtGui/QMainWindow>

#include <QVBoxLayout>
#include <QPushButton>

class LayoutAppendUpdatesGui : public QMainWindow
{
    Q_OBJECT

public:
    LayoutAppendUpdatesGui(QWidget *parent = 0);
    ~LayoutAppendUpdatesGui();
private:
    QVBoxLayout *m_LayoutToAppendTo;
    QPushButton *m_ItemToAppend;
    //bool doesLayoutAppendUpdateGui();
private slots:
    void appendItem();
};

#endif // LAYOUTAPPENDUPDATESGUI_H
