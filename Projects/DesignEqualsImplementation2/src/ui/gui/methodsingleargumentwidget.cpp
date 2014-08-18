#include "methodsingleargumentwidget.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

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

    QPushButton *moveArgumentUpButton = new QPushButton("^"); //TODOreq: this, "down", and "delete" should use a better pixmap (there is likely a good one included with Qt)
    QPushButton *moveArgumentDownButton = new QPushButton("v");
    QPushButton *deleteArgumentButton = new QPushButton("X"); //TODOreq: red X <3

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
