#ifndef PAGINATIONOFVIEWQUERYGUI_H
#define PAGINATIONOFVIEWQUERYGUI_H

#include <QObject>

class PaginationOfViewQueryWidget;

class PaginationOfViewQueryGui : public QObject
{
    Q_OBJECT
public:
    explicit PaginationOfViewQueryGui(QObject *parent = 0);
    ~PaginationOfViewQueryGui();
private:
    PaginationOfViewQueryWidget *m_Gui;
public slots:
    void handlePaginationOfViewQueryInitialized(bool success);
    void handlePaginationOfViewQueryReadyForConnections(QObject *paginationOfViewQueryAsQObject);
private slots:
    void quit();
};

#endif // PAGINATIONOFVIEWQUERYGUI_H
