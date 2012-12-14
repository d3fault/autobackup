#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>

class mainWindow : public QWidget
{
    Q_OBJECT
    
public:
    mainWindow(QWidget *parent = 0);
    ~mainWindow();
};

#endif // MAINWINDOW_H
