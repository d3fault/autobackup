#ifndef UMLITEMSWIDGET_H
#define UMLITEMSWIDGET_H

#include <QWidget>
#include <QBoxLayout>

class UmlItemsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UmlItemsWidget(QWidget *parent = 0, QBoxLayout::Direction direction = QBoxLayout::TopToBottom, int columnsIfVertical_RowsIfHorizontal = 2);
private:
    QBoxLayout *m_Layout;
};

#endif // UMLITEMSWIDGET_H
