#ifndef CLASSDIAGRAMUMLITEMSWIDGET_H
#define CLASSDIAGRAMUMLITEMSWIDGET_H

#include <QListWidget>

class ClassDiagramUmlItemsWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ClassDiagramUmlItemsWidget(QWidget *parent = 0);
protected:
    virtual void startDrag(Qt::DropActions supportedActions);
};

#endif // CLASSDIAGRAMUMLITEMSWIDGET_H
