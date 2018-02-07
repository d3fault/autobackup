#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QListWidget>
#include <QLabel>

#include "top5movieslistwidget.h"

QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget(QWidget *parent)
    : QWidget(parent)
    /*format2ui-compiling-template-example_BEGIN_liiueri93jrkjieruj*/
    , m_FirstNameLineEdit(new QLineEdit())
    , m_LastNameLineEdit(new QLineEdit())
    , m_Top5MoviesListWidget(new Top5MoviesListWidget())
    , m_FavoriteDinnerLineEdit(new QLineEdit())
    , m_FavoriteLunchLineEdit(new QLineEdit())
    /*format2ui-compiling-template-example_END_liiueri93jrkjieruj*/
{
    //for each uiVariable, create a QWidget derived object
    /*format2ui-compiling-template-example_BEGIN_kldsfoiure8098347824*/
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    m_FirstNameLineEdit->setPlaceholderText("First Name:");
    rootLayout->addWidget(m_FirstNameLineEdit);
    m_LastNameLineEdit->setPlaceholderText("Last Name:");
    rootLayout->addWidget(m_LastNameLineEdit);

    rootLayout->addWidget(new QLabel("Top 5 Movies:"));
    rootLayout->addWidget(m_Top5MoviesListWidget);

    QVBoxLayout *layout0 = new QVBoxLayout();
    layout0->addWidget(m_FavoriteDinnerLineEdit);
    layout0->addWidget(m_FavoriteLunchLineEdit);
    rootLayout->addLayout(layout0);

#if 0 //TODOreq:
    QVBoxLayout *layout1_ForWidgetList = new QVBoxLayout();

    rootLayout->addLayout(layout1_ForWidgetList);
#endif
    /*format2ui-compiling-template-example_END_kldsfoiure8098347824*/

    QHBoxLayout *okCancelRow = new QHBoxLayout(); //I thought about changing 'this' to be a QDialog, but I don't want to subscribe to anything!
    QPushButton *okButton = new QPushButton(tr("Ok"));
    okCancelRow->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    okCancelRow->addWidget(cancelButton);
    rootLayout->addLayout(okCancelRow);

    connect(okButton, &QPushButton::clicked, this, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleOkButtonClicked_aka_SanitizeAllAreNotEmptyBeforeEmittingSuccess);

    //okButton->setDefault(true);
    /*format2ui-compiling-template-example_BEGIN_lksdjoirueo230480894*/
    connect(m_FirstNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
    connect(m_LastNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
    connect(m_FavoriteDinnerLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
    connect(m_FavoriteLunchLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);
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
        //TODOreq: top5Movies and m_NestedLineEdit(s)
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

    //m_Top5MoviesListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    //QStringList top5Movies = m_Top5MoviesListWidget->toPlainText().split("\n");
    QStringList top5Movies = m_Top5MoviesListWidget->top5Movies(); //TODOreq: maybe with these I should silently drop any/all EMPTY (after trimmed()), as my form of "sanitization"
    QString favoriteDinner = m_FavoriteDinnerLineEdit->text();
    if(favoriteDinner.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Favorite Dinner cannot be empty");
        return;
    }
    QString favoriteLunch = m_FavoriteLunchLineEdit->text();
    if(favoriteLunch.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Favorite Lunch cannot be empty");
        return;
    }
    /*format2ui-compiling-template-example_END_ldkjsflj238423084*/

    emit finishedCollectingUiVariables(/*format2ui-compiling-template-example_BEGIN_lksdfjodusodsfudsflkjdskl983402824*/firstName, lastName, top5Movies, favoriteDinner, favoriteLunch/*format2ui-compiling-template-example_END_lksdfjodusodsfudsflkjdskl983402824*/);
    close(); //done. close so that the user can't change the line edits anymore
}
void QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::handleCancelButtonClicked()
{
    //emit finishedCollectingUiVariables(false);
    close();
}
