#include "whenstacktracefailswidget.h"

#include <QPushButton>

WhenStackTraceFailsWidget::WhenStackTraceFailsWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    //m_FirstHalfLineEdit = new QLineEdit("{ emit d(\"######## %i\"); ");
    m_FirstHalfLineEdit = new QLineEdit("{ qDebug() << \"######## %i\"; ");
    m_SecondHalfLineEdit = new QLineEdit(" }");
    QPushButton *resetCurrentIndexButtan = new QPushButton("Reset Current Index");
    m_DocLolling = new SpaceActivatedSpecialPlainTextEdit();

    myLayout->addWidget(m_FirstHalfLineEdit);
    myLayout->addWidget(m_SecondHalfLineEdit);
    myLayout->addWidget(resetCurrentIndexButtan);
    myLayout->addWidget(m_DocLolling);

    setLayout(myLayout);

    m_DocLolling->setNewFirstHalf(m_FirstHalfLineEdit->text());
    m_DocLolling->setNewSecondHalf(m_SecondHalfLineEdit->text());

    connect(resetCurrentIndexButtan, SIGNAL(clicked()), m_DocLolling, SLOT(resetCurrentIndex()));
    connect(m_FirstHalfLineEdit, SIGNAL(textChanged(QString)), m_DocLolling, SLOT(setNewFirstHalf(QString)));
    connect(m_SecondHalfLineEdit, SIGNAL(textChanged(QString)), m_DocLolling, SLOT(setNewSecondHalf(QString)));
}
WhenStackTraceFailsWidget::~WhenStackTraceFailsWidget()
{

}
