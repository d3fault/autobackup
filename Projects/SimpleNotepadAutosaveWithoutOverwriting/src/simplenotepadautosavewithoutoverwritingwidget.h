#ifndef SIMPLENOTEPADAUTOSAVEWITHOUTOVERWRITINGWIDGET_H
#define SIMPLENOTEPADAUTOSAVEWITHOUTOVERWRITINGWIDGET_H

#include <QWidget>

class SignalEmittingOnKeyEventsPlainTextEdit;
class QTimer;

class SimpleNotepadAutosaveWithoutOverwritingWidget : public QWidget
{
    Q_OBJECT
public:
    SimpleNotepadAutosaveWithoutOverwritingWidget(const QString &autoSaveTemplate, QWidget *parent = 0);
    private:
    const QString &m_AutoSaveTemplate;
private:
    SignalEmittingOnKeyEventsPlainTextEdit *m_PlainTextEdit;
    QTimer *m_HaveStoppedTypingForThreeSecondsTimer;

    bool save();
private slots:
    void handlePlainTextEditKeyPressed(qint64 timestamp, int key);
    void haveStoppedTypingForThreeSecondsTimerTimedOut();

};

#endif // SIMPLENOTEPADAUTOSAVEWITHOUTOVERWRITINGWIDGET_H
