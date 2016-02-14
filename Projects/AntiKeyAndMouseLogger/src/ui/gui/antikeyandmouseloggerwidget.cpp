#include "antikeyandmouseloggerwidget.h"

#include <QSignalMapper>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

//TODOoptional: where/what the fuck is my grid layout with a dynamic columns per row depending on the space available and column size, dynamically changing when the space available and column sizes do too
//TODOreq: a complete yet short description in a help dialog on how to use this program (ie, how to have computer security. the dos and donts)
//TODOreq: onKeyPressed/Released, 'click' corresponding button
#define AntiKeyAndMouseLogger_COLUMNS_PER_ROW 6
AntiKeyAndMouseLoggerWidget::AntiKeyAndMouseLoggerWidget(QWidget *parent)
    : QWidget(parent)
    , m_SignalMapper(new QSignalMapper(this))
{
    QGridLayout *gridLayout = new QGridLayout();
    int i = 0;
    for(; i < AntiKeyAndMouseLogger::numEntriesOnOneKeymapPage(); ++i)
    {
        QPushButton *button = new QPushButton();
        connect(button, SIGNAL(clicked()), m_SignalMapper, SLOT(map()));
        gridLayout->addWidget(button, i / AntiKeyAndMouseLogger_COLUMNS_PER_ROW, i % AntiKeyAndMouseLogger_COLUMNS_PER_ROW);
        m_Buttons << button;
    }
    connect(m_SignalMapper, SIGNAL(mapped(QString)), this, SIGNAL(translateShuffledKeymapEntryRequested(QString)));

    disableAllButtons();
    connect(this, SIGNAL(translateShuffledKeymapEntryRequested(QString)), this, SLOT(disableAllButtons())); //TODOreq: disable the onKeyPress stuff too once it's implemented

    m_PasswordLineEdit = new QLineEdit();
    m_PasswordLineEdit->setReadOnly(true);
    m_PasswordLineEdit->setEchoMode(QLineEdit::Password); //TODOreq: a "show password" checkbox. I was tempted to say that turning off echo like this is a waste of time, because if someone can see your monitor then all bets are off. But really asterisking is a LAYER of security that protects against casual onlookers such as your friends, roomates, family, etc... who might be standing nearby. While their eyes might not be fast enough to see your keystrokes, they sure as shit are fast enough to read a line edit's contents. TODOreq: THIS CONFLICTS WITH dictionary 'completion'. So maybe dictionary-completion should be turned off by default and enabled via a checkbox? When using 'dictionary completion' AND asterisk, we should only show 'the rest of the word' being completed (the letters typed so far remain asterisked), to minimize compromise. one thing I haven't thought of yet is "dictionary word boundary" detection. But as long as every word is 'dictionary completed', setting those won't be too difficult. Fuck this noise tho, that optimization is low as fuck priority
    gridLayout->addWidget(m_PasswordLineEdit, (i / AntiKeyAndMouseLogger_COLUMNS_PER_ROW)+1, 0, 1, AntiKeyAndMouseLogger_COLUMNS_PER_ROW);

    setLayout(gridLayout);
}
void AntiKeyAndMouseLoggerWidget::presentShuffledKeymapPage(const KeyMap &shuffledKeymapPage)
{
    KeyMapIterator it(shuffledKeymapPage);
    int i = -1;
    while(it.hasNext())
    {
        KeymapHashTypes currentShuffledKeymapEntry = it.next();
        QPushButton *button = m_Buttons.at(++i);
        button->setText(currentShuffledKeymapEntry.first + " = " + currentShuffledKeymapEntry.second);
        button->setEnabled(true);
        m_SignalMapper->setMapping(button, currentShuffledKeymapEntry.first);
    }
}
void AntiKeyAndMouseLoggerWidget::appendTranslatedKeymapEntryToPasswordLineEdit(const KeyMapEntry &translatedKeymapEntry)
{
    m_PasswordLineEdit->setText(m_PasswordLineEdit->text() + translatedKeymapEntry);
}
void AntiKeyAndMouseLoggerWidget::disableAllButtons()
{
    Q_FOREACH(QPushButton *button, m_Buttons)
    {
        button->setText("");
        m_SignalMapper->removeMappings(button);
        button->setDisabled(true);
    }
}
