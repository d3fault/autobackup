#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget(QWidget *parent)
    : QWidget(parent)
    , m_FirstNameLineEdit(new QLineEdit())
    , m_LastNameLineEdit(new QLineEdit())
{
    QVBoxLayout *myLayout = new QVBoxLayout(this);

    //for each uiVariable, create a QPlainTextEdit
#if 0
    QPlainTextEdit *uiVariablePlainTextEdit = new QPlainTextEdit();
    myLayout->addWidget(uiVariablePlainTextEdit);
#endif
    m_FirstNameLineEdit->setPlaceholderText("First Name:");
    myLayout->addWidget(m_FirstNameLineEdit);
    m_LastNameLineEdit->setPlaceholderText("Last Name:");
    myLayout->addWidget(m_LastNameLineEdit);

    QHBoxLayout *okCancelRow = new QHBoxLayout(); //I thought about changing 'this' to be a QDialog, but I don't want to subscribe to anything!
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okCancelRow->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    okCancelRow->addWidget(cancelButton);
    myLayout->addLayout(okCancelRow);

    connect(okButton, &QPushButton::clicked, this, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess);

    //okButton->setDefault(true);
    connect(m_FirstNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
    connect(m_LastNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);

    connect(cancelButton, &QPushButton::clicked, this, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked);
}
bool QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::parseArgs()
{
    bool ret = m_ArgParser.parseArgs();
    if(ret)
    {
        if(!m_ArgParser.firstNameDefaultValueParsedFromProcessArg().isEmpty())
            m_FirstNameLineEdit->setText(m_ArgParser.firstNameDefaultValueParsedFromProcessArg());
        if(!m_ArgParser.lastNameDefaultValueParsedFromProcessArg().isEmpty())
            m_LastNameLineEdit->setText(m_ArgParser.lastNameDefaultValueParsedFromProcessArg());
    }
    return ret;
}
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess()
{
    QString firstName = m_FirstNameLineEdit->text();
    QString lastName = m_LastNameLineEdit->text();
    if(firstName.isEmpty() || lastName.isEmpty())
    {
        QMessageBox::critical(this, "error", "no line edits can be empty");
        return;
    }

    emit finishedCollectingUiVariables(firstName, lastName);
    close(); //done. close so that the user can't change the line edits anymore
}
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked()
{
    //emit finishedCollectingUiVariables(false);
    close();
}
