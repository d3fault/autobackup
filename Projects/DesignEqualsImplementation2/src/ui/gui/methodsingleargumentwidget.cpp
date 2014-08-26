#include "methodsingleargumentwidget.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>

#include "tabemittinglineedit.h"

MethodSingleArgumentWidget::MethodSingleArgumentWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *myLayout = new QHBoxLayout();

    m_ArgumentTypeLineEdit = new QLineEdit();
    m_ArgumentTypeLineEdit->setPlaceholderText(tr("Arg Type"));
    m_ArgumentTypeLineEdit->setToolTip(tr("Arg Type"));

    m_ArgumentNameLineEdit = new TabEmittingLineEdit();
    m_ArgumentNameLineEdit->setPlaceholderText(tr("Arg Name"));
    m_ArgumentNameLineEdit->setToolTip(tr("Arg Name"));

    //TODOreq: it isn't clear that these up/down/delete buttons are for the arguments and not the slots themselves. i did try putting the argument stuff in a groupbox, but it looked ugly :-P. was fixable tho
    QPushButton *moveArgumentUpButton = new QPushButton();
    moveArgumentUpButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    QPushButton *moveArgumentDownButton = new QPushButton();
    moveArgumentDownButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    QPushButton *deleteArgumentButton = new QPushButton();
    deleteArgumentButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));

    myLayout->addWidget(m_ArgumentTypeLineEdit);
    myLayout->addWidget(m_ArgumentNameLineEdit);
    myLayout->addWidget(moveArgumentDownButton);
    myLayout->addWidget(moveArgumentUpButton);
    myLayout->addWidget(deleteArgumentButton);

    setLayout(myLayout);


    connect(m_ArgumentTypeLineEdit, SIGNAL(returnPressed()), this, SIGNAL(commitMethodequested()));
    connect(m_ArgumentNameLineEdit, SIGNAL(returnPressed()), this, SIGNAL(commitMethodequested()));
    connect(m_ArgumentNameLineEdit, SIGNAL(tabPressed()), this, SIGNAL(tabPressedInNameLineEdit()));
    connect(moveArgumentUpButton, SIGNAL(clicked()), this, SLOT(handleMoveArgumentUpButtonClicked()));
    connect(moveArgumentDownButton, SIGNAL(clicked()), this, SLOT(handleMoveArgumentDownButtonClicked()));
    connect(deleteArgumentButton, SIGNAL(clicked()), this, SLOT(handleDeleteArgumentButtonClicked()));
}
QString MethodSingleArgumentWidget::argumentType() const
{
    return m_ArgumentTypeLineEdit->text();
}
QString MethodSingleArgumentWidget::argumentName() const
{
    return m_ArgumentNameLineEdit->text();
}
void MethodSingleArgumentWidget::focusOnArgType()
{
    m_ArgumentTypeLineEdit->setFocus();
}
void MethodSingleArgumentWidget::handleMoveArgumentUpButtonClicked()
{
    emit moveArgumentUpRequested(this);
}
void MethodSingleArgumentWidget::handleMoveArgumentDownButtonClicked()
{
    emit moveArgumentDownRequested(this);
}
void MethodSingleArgumentWidget::handleDeleteArgumentButtonClicked()
{
    emit deleteArgumentRequested(this);
}
