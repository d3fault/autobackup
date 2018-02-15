/*format2ui-import-as-triggered-sdlfjsdofuo08340832408934*/
#ifndef SOMEWIDGETLIST_H
#define SOMEWIDGETLIST_H

#include <QWidget>

#include <QDebug>
#include <QList>

class QVBoxLayout;
class SomeWidgetListEntryTypeWidget;

struct SomeWidgetListEntryType
{
    QString SomeWidgetList_AnEntry0;
    QString SomeWidgetList_AnotherEntry1;
};
class SomeWidgetList : public QWidget
{
    Q_OBJECT
public:
    explicit SomeWidgetList(QWidget *parent = 0);
    QList<SomeWidgetListEntryType> someWidgetListValues() const;
private:
    void addNewRow();

    QVBoxLayout *m_MyLayout;
    QList<SomeWidgetListEntryTypeWidget*> m_SomeWidgetListEntryTypeWidgets;
};

#endif // SOMEWIDGETLIST_H
