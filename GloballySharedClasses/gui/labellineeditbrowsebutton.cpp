#include "labellineeditbrowsebutton.h"

#include <QLabel>
#include <QPushButton>

LabelLineEditBrowseButton::LabelLineEditBrowseButton(const QString &labelText, FileDialogMode fileDialogMode, const QString &pushButtonText, Direction direction, QWidget *parent) :
    QBoxLayout(direction, parent)
{
    QLabel *myLabel = new QLabel(labelText);
    m_MyLineEdit = new QLineEdit();
    myLabel->setBuddy(m_MyLineEdit);
    QPushButton *browseButton = new QPushButton(pushButtonText);

    //global
    m_FileDialog.setWindowModality(Qt::WindowModal);
    m_FileDialog.setViewMode(QFileDialog::List);

    //directories
    if(fileDialogMode == OpenDirectoryMode)
    {
        m_FileDialog.setFileMode(QFileDialog::Directory);
        m_FileDialog.setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
        m_FileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    }
    else //files
    {
        m_FileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
        if(fileDialogMode == SaveFileMode)
        {
            m_FileDialog.setAcceptMode(QFileDialog::AcceptSave);
        }
        else
        {
            m_FileDialog.setFileMode(QFileDialog::ExistingFile);
        }
    }

    addWidget(myLabel);
    addWidget(m_MyLineEdit);
    addWidget(browseButton);

    connect(browseButton, SIGNAL(clicked()), this, SLOT(handleBrowseButtonClicked()));
}
QLineEdit *LabelLineEditBrowseButton::lineEdit()
{
    return m_MyLineEdit;
}
QFileDialog &LabelLineEditBrowseButton::fileDialog()
{
    return m_FileDialog;
}
void LabelLineEditBrowseButton::handleBrowseButtonClicked()
{
    if(m_FileDialog.exec() == QDialog::Accepted && m_FileDialog.selectedFiles().size() > 0)
    {
        //const QString &selectedFile0 = m_FileDialog.selectedFiles().at(0); -- intermitent bug when using const reference. the first time i tried it on a small file, it worked. the second time ona  20mb file, it didn't work. and from then on even on small files it didn't work again. weird that i'm not even at the reading of file contents stage right here. the filename was showing up blank, but the very first time clearly was not. was working on CopyFileContentsToClipboard when it happened
        QString selectedFile0 = m_FileDialog.selectedFiles().at(0);
        m_MyLineEdit->setText(selectedFile0);
        emit pathSelectedFromBrowseDialog(selectedFile0);
    }
}
