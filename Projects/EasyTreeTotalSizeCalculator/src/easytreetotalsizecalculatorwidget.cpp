#include "easytreetotalsizecalculatorwidget.h"

#include <QHBoxLayout>

EasyTreeTotalSizeCalculatorWidget::EasyTreeTotalSizeCalculatorWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();

    m_FilepathOfEasyTreeFileLabel = new QLabel("Easy Tree Filepath:");
    m_FilepathOfEasyTreeFileLineEdit = new QLineEdit();
    m_FilepathOfEasyTreeFileBrowseButton = new QPushButton("Browse");
    m_EasyTreeIsReplacementFormatted = new QCheckBox("Easy Tree File Is 'Replacements' Formatted");
    m_CalculateTotalSizeOfEasyTreeEntriesButton = new QPushButton("Calculate Total Size");
    m_Debug = new QPlainTextEdit();

    QHBoxLayout *filePathRow = new QHBoxLayout();
    filePathRow->addWidget(m_FilepathOfEasyTreeFileLabel);
    filePathRow->addWidget(m_FilepathOfEasyTreeFileLineEdit);
    filePathRow->addWidget(m_FilepathOfEasyTreeFileBrowseButton);
    m_Layout->addLayout(filePathRow);

    m_Layout->addWidget(m_CalculateTotalSizeOfEasyTreeEntriesButton);

    m_Layout->addWidget(m_Debug);

    setLayout(m_Layout);

    connect(m_FilepathOfEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleFilepathOfEasyTreeFileBrowseButtonClicked()));
    connect(m_CalculateTotalSizeOfEasyTreeEntriesButton, SIGNAL(clicked()), this, SLOT(handleCalculateTotalSizeOfEasyTreeEntriesButtonClicked()));
}
void EasyTreeTotalSizeCalculatorWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void EasyTreeTotalSizeCalculatorWidget::handleFilepathOfEasyTreeFileBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_FilepathOfEasyTreeFileLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void EasyTreeTotalSizeCalculatorWidget::handleCalculateTotalSizeOfEasyTreeEntriesButtonClicked()
{
    if(m_FilepathOfEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD("Please Select An Easy Tree File");
        return;
    }
    emit calculateTotalSizeOfEasyTreeEntriesRequested(m_FilepathOfEasyTreeFileLineEdit->text(), m_EasyTreeIsReplacementFormatted->isChecked());
}
