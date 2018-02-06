#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QLabel>

QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget(QWidget *parent)
    : QWidget(parent)
    /*format2ui-compiling-template-example_BEGIN_liiueri93jrkjieruj*/
    , m_FirstNameLineEdit(new QLineEdit())
    , m_LastNameLineEdit(new QLineEdit())
    , m_Top5MoviesPlainTextEdit(new QPlainTextEdit())
    /*format2ui-compiling-template-example_END_liiueri93jrkjieruj*/
{
    QVBoxLayout *myLayout = new QVBoxLayout(this);

    //for each uiVariable, create a QWidget
    /*format2ui-compiling-template-example_BEGIN_kldsfoiure8098347824*/
    m_FirstNameLineEdit->setPlaceholderText("First Name:");
    myLayout->addWidget(m_FirstNameLineEdit);
    m_LastNameLineEdit->setPlaceholderText("Last Name:");
    myLayout->addWidget(m_LastNameLineEdit);
    myLayout->addWidget(new QLabel("Top 5 Movies:"));
    myLayout->addWidget(m_Top5MoviesPlainTextEdit);
    /*format2ui-compiling-template-example_END_kldsfoiure8098347824*/

    QHBoxLayout *okCancelRow = new QHBoxLayout(); //I thought about changing 'this' to be a QDialog, but I don't want to subscribe to anything!
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okCancelRow->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    okCancelRow->addWidget(cancelButton);
    myLayout->addLayout(okCancelRow);

    connect(okButton, &QPushButton::clicked, this, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess);

    //okButton->setDefault(true);
    /*format2ui-compiling-template-example_BEGIN_lksdjoirueo230480894*/
    connect(m_FirstNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
    connect(m_LastNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
    /*format2ui-compiling-template-example_END_lksdjoirueo230480894*/

    connect(cancelButton, &QPushButton::clicked, this, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked);
}
bool QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::parseArgs()
{
    bool ret = m_ArgParser.parseArgs();
    if(ret)
    {
        /*format2ui-compiling-template-example_BEGIN_lksdfjoiduf08340983409*/
        if(!m_ArgParser.firstNameDefaultValueParsedFromProcessArg().isEmpty())
            m_FirstNameLineEdit->setText(m_ArgParser.firstNameDefaultValueParsedFromProcessArg());
        if(!m_ArgParser.lastNameDefaultValueParsedFromProcessArg().isEmpty())
            m_LastNameLineEdit->setText(m_ArgParser.lastNameDefaultValueParsedFromProcessArg());
        /*format2ui-compiling-template-example_END_lksdfjoiduf08340983409*/
    }
    return ret;
}
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess()
{
    /*format2ui-compiling-template-example_BEGIN_ldkjsflj238423084*/
    QString firstName = m_FirstNameLineEdit->text();
    if(firstName.isEmpty())
    {
        QMessageBox::critical(this, "Error", "First Name cannot be empty");
        return;
    }
    QString lastName = m_LastNameLineEdit->text();
    if(lastName.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Last Name cannot be empty");
        return;
    }
    QStringList top5Movies = m_Top5MoviesPlainTextEdit->toPlainText().split("\n");
    /*format2ui-compiling-template-example_END_ldkjsflj238423084*/

    emit finishedCollectingUiVariables(firstName, lastName, top5Movies);
    close(); //done. close so that the user can't change the line edits anymore
}
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked()
{
    //emit finishedCollectingUiVariables(false);
    close();
}
