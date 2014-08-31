#include "copyfilecontentstoclipboardwidget.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QClipboard>

#include "labellineeditbrowsebutton.h"

//this app servers two purposes, possibly more in the future
// 1) copying a bitcoin private key to clipboard for pasting into some other app (i've theorized this step, but the app i'd be pasting into does not yet exist)
// 2) right now i'm using it to paste 20mb of plaintext into libreoffice. opening it in mousepad and doing ctrl+a locks up the app (not even at ctrl+c step yet after like 30 mins of waiting), so i'm hoping this app will be faster since i'm not showing the contents to screen ever ((1) demands that. btw there's an accidental vuln when the desktop file browser 'opener' has a preview window and it shows a previw of a text doc, namely a bitcoin private key)
CopyFileContentsToClipboardWidget::CopyFileContentsToClipboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Copy File Contents To Clipboard"));
    QVBoxLayout *myLayout = new QVBoxLayout();

    m_TreatAsBinaryCheckbox = new QCheckBox(tr("Treat as binary:"));

    LabelLineEditBrowseButton *selectFileRow = new LabelLineEditBrowseButton(tr("File"), LabelLineEditBrowseButton::OpenFileMode, tr("Select File"));

    m_Output = new QPlainTextEdit(); //only ever says "the contents of <path> are now on clipboard" when finished

    myLayout->addWidget(m_TreatAsBinaryCheckbox);
    myLayout->addLayout(selectFileRow);
    myLayout->addWidget(m_Output);

    setLayout(myLayout);

    connect(selectFileRow, SIGNAL(pathSelectedFromBrowseDialog(QString)), this, SLOT(copyFileContentsToClipboardWidget(QString)));
}
void CopyFileContentsToClipboardWidget::copyFileContentsToClipboardWidget(const QString &filePath)
{
    //should this be on a background thread since it has the potential to lock up the gui? i think for my book case (20mb), this won't be a problem (only presenting those contents to screen is the problem)
    QFile file(filePath);
    QIODevice::OpenMode openMode = QIODevice::ReadOnly;
    if(!m_TreatAsBinaryCheckbox->isChecked())
    {
        openMode |= QIODevice::Text;
    }
    if(!file.open(openMode))
    {
        m_Output->appendPlainText("failed to open file for reading");
        return;
    }
    QClipboard *clipboard = QApplication::clipboard();
    if(m_TreatAsBinaryCheckbox->isChecked())
    {
        //TODOoptional: mb copy as mime data instead with mime type set to octet stream or whatever, as i think that way is better for qba
        QByteArray fileContents = file.readAll();
        clipboard->setText(fileContents);
    }
    else
    {
        QTextStream fileContentsTextStream(&file);
        QString fileContents = fileContentsTextStream.readAll();
        clipboard->setText(fileContents);
    }
    m_Output->appendPlainText("The contents of '" + filePath + "' have been copied to the clipboard.");
}
