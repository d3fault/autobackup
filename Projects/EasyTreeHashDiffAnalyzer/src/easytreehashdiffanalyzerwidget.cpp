#include "easytreehashdiffanalyzerwidget.h"

#include <QHBoxLayout>

EasyTreeHashDiffAnalyzerWidget::EasyTreeHashDiffAnalyzerWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();

    QHBoxLayout *inputRow = new QHBoxLayout();

    m_Input1Label = new QLabel(tr("Easy Tree Input 1:"));
    m_Input1LineEdit = new QLineEdit();
    m_Input1BrowseButton = new QPushButton(tr("Browse"));

    m_Input2Label = new QLabel(tr("Easy Tree Input 2:"));
    m_Input2LineEdit = new QLineEdit();
    m_Input2BrowseButton = new QPushButton(tr("Browse"));

    inputRow->addWidget(m_Input1Label);
    inputRow->addWidget(m_Input1LineEdit, 1);
    inputRow->addWidget(m_Input1BrowseButton);
    inputRow->addWidget(m_Input2Label);
    inputRow->addWidget(m_Input2LineEdit, 1);
    inputRow->addWidget(m_Input2BrowseButton);

    m_DiffAndAnalyzeButton = new QPushButton(tr("Diff and Analyze"));

    m_Debug = new QPlainTextEdit();

    m_Layout->addLayout(inputRow);
    m_Layout->addWidget(m_DiffAndAnalyzeButton);
    m_Layout->addWidget(m_Debug);

    setLayout(m_Layout);

    connect(m_DiffAndAnalyzeButton, SIGNAL(clicked()), this, SLOT(handleDiffAndAnalyzeButtonClicked()));
    connect(m_Input1BrowseButton, SIGNAL(clicked()), this, SLOT(handleInput1BrowseButtonClicked()));
    connect(m_Input2BrowseButton, SIGNAL(clicked()), this, SLOT(handleInput2BrowseButtonClicked()));
}
void EasyTreeHashDiffAnalyzerWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void EasyTreeHashDiffAnalyzerWidget::handleInput1BrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Easy Tree Input 1"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_Input1LineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void EasyTreeHashDiffAnalyzerWidget::handleInput2BrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Easy Tree Input 2"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_Input2LineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void EasyTreeHashDiffAnalyzerWidget::handleDiffAndAnalyzeButtonClicked()
{
    if(m_Input1LineEdit->text().trimmed().isEmpty())
    {
        handleD("Input 1 is empty");
        return;
    }
    if(m_Input2LineEdit->text().trimmed().isEmpty())
    {
        handleD("Input 2 is empty");
        return;
    }
    emit diffAndAnalyzeRequested(m_Input1LineEdit->text(), m_Input2LineEdit->text());
}
