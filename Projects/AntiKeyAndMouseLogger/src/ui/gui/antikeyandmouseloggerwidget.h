#ifndef ANTIKEYANDMOUSELOGGERWIDGET_H
#define ANTIKEYANDMOUSELOGGERWIDGET_H

#include <QWidget>

#include "antikeyandmouselogger.h"

class QSignalMapper;
class QPushButton;

class LineEditThatIgnoresKeyPressedEvents;

class AntiKeyAndMouseLoggerWidget : public QWidget
{
    Q_OBJECT
public:
    AntiKeyAndMouseLoggerWidget(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *keyEvent);
private:
    QSignalMapper *m_SignalMapper;
    QList<QPushButton*> m_Buttons;
    LineEditThatIgnoresKeyPressedEvents *m_PasswordLineEdit;
signals:
    void translateShuffledKeymapEntryRequested(const QString &shuffledKeymapEntry);
public slots:
    void presentShuffledKeymapPage(const KeyMap &shuffledKeymapPage);
    void appendTranslatedKeymapEntryToPasswordLineEdit(const KeyMapEntry &translatedKeymapEntry);
private slots:
    void disableAllButtons();
    void handleShowPasswordCheckboxToggled(bool checked);
};

#endif // ANTIKEYANDMOUSELOGGERWIDGET_H
