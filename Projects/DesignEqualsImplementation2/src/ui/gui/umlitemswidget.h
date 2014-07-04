#ifndef UMLITEMSWIDGET_H
#define UMLITEMSWIDGET_H

#include <QListWidget>

class UmlItemsWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit UmlItemsWidget(QWidget *parent = 0);
protected:
    virtual void startDrag(Qt::DropActions supportedActions);
};

#endif // UMLITEMSWIDGET_H
