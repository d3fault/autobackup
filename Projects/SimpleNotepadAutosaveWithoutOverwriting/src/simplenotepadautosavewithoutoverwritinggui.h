#ifndef SIMPLENOTEPADAUTOSAVEWITHOUTOVERWRITINGGUI_H
#define SIMPLENOTEPADAUTOSAVEWITHOUTOVERWRITINGGUI_H

#include <QObject>

class SimpleNotepadAutosaveWithoutOverwritingWidget;

class SimpleNotepadAutosaveWithoutOverwritingGui : public QObject
{
    Q_OBJECT
public:
    explicit SimpleNotepadAutosaveWithoutOverwritingGui(QObject *parent = 0);
private:
    SimpleNotepadAutosaveWithoutOverwritingWidget m_Gui;
signals:

public slots:

};

#endif // SIMPLENOTEPADAUTOSAVEWITHOUTOVERWRITINGGUI_H
