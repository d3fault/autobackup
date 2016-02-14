#ifndef ANTIKEYANDMOUSELOGGERWIDGET_H
#define ANTIKEYANDMOUSELOGGERWIDGET_H

#include <QWidget>

#include "antikeyandmouselogger.h"

class QSignalMapper;
class QPushButton;
class QLineEdit;

class AntiKeyAndMouseLoggerWidget : public QWidget
{
    Q_OBJECT
public:
    AntiKeyAndMouseLoggerWidget(QWidget *parent = 0);
private:
    QSignalMapper *m_SignalMapper;
    QList<QPushButton*> m_Buttons;
    QLineEdit *m_PasswordLineEdit;
signals:
    void translateShuffledKeymapEntryRequested(const int &shuffledKeymapEntry);
public slots:
    void useShuffledKeymap(const KeyMap &shuffledKeymap);
    void appendTranslatedKeymapEntryToPasswordLineEdit(const KeyMapEntry &translatedKeymapEntry);
private slots:
    void disableAllButtons();
};

#endif // ANTIKEYANDMOUSELOGGERWIDGET_H
