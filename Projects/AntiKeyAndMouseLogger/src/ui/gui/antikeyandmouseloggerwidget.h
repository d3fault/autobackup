#ifndef ANTIKEYANDMOUSELOGGERWIDGET_H
#define ANTIKEYANDMOUSELOGGERWIDGET_H

#include <QWidget>

#include "antikeyandmouselogger.h"

class QLineEdit;

class AntiKeyAndMouseLoggerWidget : public QWidget
{
    Q_OBJECT
public:
    AntiKeyAndMouseLoggerWidget(QWidget *parent = 0);
private:
    QLineEdit *m_PasswordLineEdit;
signals:
    void translateShuffledKeymapEntryRequested(const int &shuffledKeymapEntry);
public slots:
    void useShuffledKeymap(const KeyMap &shuffledKeymap);
    void appendTranslatedKeymapEntryToPasswordLineEdit(const KeyMapEntry &translatedKeymapEntry);
};

#endif // ANTIKEYANDMOUSELOGGERWIDGET_H
