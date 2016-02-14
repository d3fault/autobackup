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
    connect(m_SignalMapper, SIGNAL(mapped(int)), this, SIGNAL(translateShuffledKeymapEntryRequested(int)));

    disableAllButtons();
    connect(this, SIGNAL(translateShuffledKeymapEntryRequested(int)), this, SLOT(disableAllButtons())); //TODOreq: disable the onKeyPress stuff too once it's implemented

    m_PasswordLineEdit = new QLineEdit();
    m_PasswordLineEdit->setReadOnly(true);
    gridLayout->addWidget(m_PasswordLineEdit, (i / AntiKeyAndMouseLogger_COLUMNS_PER_ROW)+1, 0, 1, AntiKeyAndMouseLogger_COLUMNS_PER_ROW);

    setLayout(gridLayout);
}
void AntiKeyAndMouseLoggerWidget::useShuffledKeymap(const KeyMap &shuffledKeymap)
{
    KeyMapIterator it(shuffledKeymap);
    int i = -1;
    while(it.hasNext())
    {
        it.next();
        QPushButton *button = m_Buttons.at(++i);
        button->setText(AntiKeyAndMouseLogger::keymapEntryToString(it.key()) + " = " + AntiKeyAndMouseLogger::keymapEntryToString(it.value()));
        button->setEnabled(true);
        m_SignalMapper->setMapping(button, it.key());
    }
}
void AntiKeyAndMouseLoggerWidget::appendTranslatedKeymapEntryToPasswordLineEdit(const KeyMapEntry &translatedKeymapEntry)
{
    m_PasswordLineEdit->setText(m_PasswordLineEdit->text() + AntiKeyAndMouseLogger::keymapEntryToString(translatedKeymapEntry));
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
