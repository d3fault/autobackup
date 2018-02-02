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
    connect(cancelButton, &QPushButton::clicked, this, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked);
}
QString QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::firstName() const
{
    return m_FirstNameLineEdit->text();
}
QString QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::lastName() const
{
    return m_LastNameLineEdit->text();
}
QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::~QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget()
{ }
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess()
{
    if(firstName().isEmpty() || lastName().isEmpty())
    {
        QMessageBox::critical(this, "error", "no line edits can be empty");
        return;
    }

    emit finishedCollectingUiVariables(true);
    close(); //done. close so that the user can't change the line edits anymore
}
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked()
{
    emit finishedCollectingUiVariables(false);
    close();
}
