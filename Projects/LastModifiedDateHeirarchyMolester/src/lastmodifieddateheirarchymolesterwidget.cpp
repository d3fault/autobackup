#include "lastmodifieddateheirarchymolesterwidget.h"

LastModifiedDateHeirarchyMolesterWidget::LastModifiedDateHeirarchyMolesterWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();

    m_DirectoryHeirarchyCorrespingToEasyTreeFileLabel = new QLabel("Base Directory Corresponding To Easy Tree File:");
    m_DirectoryHeirarchyCorrespingToEasyTreeFileLineEdit = new QLineEdit();
    m_DirectoryHeirarchyCorrespingToEasyTreeFileBrowseButton = new QPushButton("Browse");


    m_EasyTreeFilePathLabel = new QLabel("Easy Tree File:");
    m_EasyTreeFilePathLineEdit = new QLineEdit();
    m_EasyTreeFilePathBrowseButton = new QPushButton("Browse");

    m_EasyTreeLinesHaveCreationDateCheckBox = new QCheckBox("Entries in Easy Tree File Have Creation Date (outdated)");

    m_MolestButton = new QPushButton("Molest");

    m_Debug = new QPlainTextEdit();


    QHBoxLayout *directoryRow = new QHBoxLayout();
    directoryRow->addWidget(m_DirectoryHeirarchyCorrespingToEasyTreeFileLabel);
    directoryRow->addWidget(m_DirectoryHeirarchyCorrespingToEasyTreeFileLineEdit);
    directoryRow->addWidget(m_DirectoryHeirarchyCorrespingToEasyTreeFileBrowseButton);
    m_Layout->addLayout(directoryRow);

    QHBoxLayout *easyTreeFileRow = new QHBoxLayout();
    easyTreeFileRow->addWidget(m_EasyTreeFilePathLabel);
    easyTreeFileRow->addWidget(m_EasyTreeFilePathLineEdit);
    easyTreeFileRow->addWidget(m_EasyTreeFilePathBrowseButton);
    m_Layout->addLayout(easyTreeFileRow);

    m_Layout->addWidget(m_EasyTreeLinesHaveCreationDateCheckBox);

    m_Layout->addWidget(m_MolestButton);

    m_Layout->addWidget(m_Debug);

    setLayout(m_Layout);

    connect(m_DirectoryHeirarchyCorrespingToEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleDirectoryHeirarchyCorrespingToEasyTreeFileBrowseButtonClicked()));
}
void LastModifiedDateHeirarchyMolesterWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void LastModifiedDateHeirarchyMolesterWidget::handleDirectoryHeirarchyCorrespingToEasyTreeFileBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Dir Corresponding to Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_DirectoryHeirarchyCorrespingToEasyTreeFileLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void LastModifiedDateHeirarchyMolesterWidget::handleEasyTreeFilePathBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_EasyTreeFilePathLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void LastModifiedDateHeirarchyMolesterWidget::handleMolestButtonClicked()
{
    if(m_DirectoryHeirarchyCorrespingToEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select a directory heirarchy to molest modification dates in");
        return;
    }
    if(m_EasyTreeFilePathLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select an Easy Tree File to get modification dates from");
        return;
    }
    emit lastModifiedDateHeirarchyMolestationRequested(m_DirectoryHeirarchyCorrespingToEasyTreeFileLabel->text(), m_EasyTreeFilePathLineEdit->text(), m_EasyTreeLinesHaveCreationDateCheckBox->isChecked());
}
