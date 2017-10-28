#ifndef LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDER_H
#define LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDER_H

#include <QObject>

#include <QScopedPointer>
#include "libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h"

class LibStk_LoopAudioFileAndModifyItsPitchUsingAslider : public QObject
{
    Q_OBJECT
public:
    explicit LibStk_LoopAudioFileAndModifyItsPitchUsingAslider(QObject *parent = 0);
private:
    QScopedPointer<LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget> m_Gui;
private slots:
    void handleAudioOutputReadyForConnections(QObject *audioOutputAsQObject);
};

#endif // LIBSTK_LOOPAUDIOFILEANDMODIFYITSPITCHUSINGASLIDER_H
