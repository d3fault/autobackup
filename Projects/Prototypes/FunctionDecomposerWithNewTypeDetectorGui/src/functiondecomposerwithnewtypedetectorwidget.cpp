#include "functiondecomposerwithnewtypedetectorwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

FunctionDecomposerWithNewTypeDetectorWidget::FunctionDecomposerWithNewTypeDetectorWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *functionDeclarationRow = new QHBoxLayout();
    m_FunctionDeclarationLineEdit = new QLineEdit();
    m_FunctionDeclarationLineEdit->setPlaceholderText(tr("Function declaration..."));
    QPushButton *parseFunctionDeclarationButton = new QPushButton(tr("Parse"));
    functionDeclarationRow->addWidget(m_FunctionDeclarationLineEdit);
    functionDeclarationRow->addWidget(parseFunctionDeclarationButton);

    m_Output = new QPlainTextEdit();
    m_Output->setReadOnly(true);

    myLayout->addLayout(functionDeclarationRow);
    myLayout->addWidget(m_Output);

    setLayout(myLayout);


    connect(m_FunctionDeclarationLineEdit, SIGNAL(returnPressed()), this, SLOT(handleParseFunctionDeclarationButtonClicked()));
    connect(parseFunctionDeclarationButton, SIGNAL(clicked()), this, SLOT(handleParseFunctionDeclarationButtonClicked()));
}
void FunctionDecomposerWithNewTypeDetectorWidget::handleSyntaxErrorDetected()
{
    m_Output->appendPlainText("syntax error. check stderr"); //there's gotta be a way to reroute this, but idk how/where ^_^
}
void FunctionDecomposerWithNewTypeDetectorWidget::handleO(const QString &msg)
{
    m_Output->appendPlainText(msg);
}
void FunctionDecomposerWithNewTypeDetectorWidget::handleParseFunctionDeclarationButtonClicked()
{
    m_Output->clear();
    if(m_FunctionDeclarationLineEdit->text().trimmed().isEmpty())
    {
        m_Output->appendPlainText("Function declaration must not be empty");
        return;
    }
    emit parseFunctionDeclarationRequested(m_FunctionDeclarationLineEdit->text());
}
