#ifndef SAMPLESKEYBOARDPIANOGUI_H
#define SAMPLESKEYBOARDPIANOGUI_H

#include <QObject>
#include <QScopedPointer>
#include <QTextStream>

#include "sampleskeyboardpianoprofilemanagerwidget.h"
#include "sampleskeyboardpianowidget.h"

class SamplesKeyboardPiano;

class SamplesKeyboardPianoGui : public QObject
{
    Q_OBJECT
public:
    explicit SamplesKeyboardPianoGui(QObject *parent = 0);
private:
    SamplesKeyboardPiano *m_SamplesKeyboardPiano;
    QScopedPointer<SamplesKeyboardPianoProfileManagerWidget> m_SamplesKeyboardPianoProfileManagerWidget;
    QScopedPointer<SamplesKeyboardPianoWidget> m_SamplesKeyboardPianoWidget;
    QTextStream m_StdErr;

    void connectSamplesKeyboardPianoBackendToProfileManagerFrontendAndShow();
    void connectSamplesKeyboardPianoBackendToFrontendAndShow(SamplesKeyboardPianoProfile profile);
private slots:
    void handleSamplesKeyboardPianoProfileManagerProfileSelected();
    void handleE(const QString &msg);
};

#endif // SAMPLESKEYBOARDPIANOGUI_H
