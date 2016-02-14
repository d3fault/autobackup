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
{ }
void AntiKeyAndMouseLoggerWidget::useShuffledKeymap(const KeyMap &shuffledKeymap)
{
    QSignalMapper *signalMapper = new QSignalMapper(this);
    QGridLayout *gridLayout = new QGridLayout;
    //for (int i = 0; i < shuffledKeymap.size(); ++i)
    KeyMapIterator it(shuffledKeymap);
    int i = 0;
    while(it.hasNext())
    {
        it.next();
        QPushButton *button = new QPushButton(AntiKeyAndMouseLogger::keymapEntryToString(it.key()) + " = " + AntiKeyAndMouseLogger::keymapEntryToString(it.value()));
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, it.key());
        gridLayout->addWidget(button, i / AntiKeyAndMouseLogger_COLUMNS_PER_ROW, i % AntiKeyAndMouseLogger_COLUMNS_PER_ROW);
        ++i;
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(translateShuffledKeymapEntryRequested(int)));

    m_PasswordLineEdit = new QLineEdit();
    m_PasswordLineEdit->setReadOnly(true);
    gridLayout->addWidget(m_PasswordLineEdit, (i / AntiKeyAndMouseLogger_COLUMNS_PER_ROW)+1, 0, 1, AntiKeyAndMouseLogger_COLUMNS_PER_ROW);

    setLayout(gridLayout);
}
void AntiKeyAndMouseLoggerWidget::appendTranslatedKeymapEntryToPasswordLineEdit(const KeyMapEntry &translatedKeymapEntry)
{
    m_PasswordLineEdit->setText(m_PasswordLineEdit->text() + AntiKeyAndMouseLogger::keymapEntryToString(translatedKeymapEntry));
}
