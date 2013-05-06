#include "mainwidget.h"

#include <QHBoxLayout>

//TODOoptimization: radio box for "What to do with un-readable/un-openable file situation:" [a] - Test All Read/Open-able before starting; [b] - Skip (notify? notify is yet another TODO -- it could be just a GUI notification or an error log file, but obviously shouldn't be in the EasyTreeHash output file...)
//^^For now I'm just going to run as root and fucking error out and stop and say what went wrong... with no way to resume the operation :-/ (delete destination, retry [after fixing bugs or solving the problem somehow]). I'd definitely have to generate a list of files that will be copied over if I wanted 'resume' functionality (and if I did that, then I could have an accurate progress bar too for overall progress (for now I'm just going to ????? maybe have it on a 1-file basis or maybe just not have it at all fuck it, debug output is good enough really :-P) and of course would can/should verify the read/open-ability while calculating those filesizes as well)

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent), m_CopyInProgress(false), m_BeginCopyString(tr("Begin Copy"))
{
    //Begin Layout
    m_Layout = new QVBoxLayout();

        QHBoxLayout *sourceDirectoryRow = new QHBoxLayout();
            m_SourceDirectoryLabel = new QLabel(tr("Source Directory:"));
            m_SourceDirectoryLineEdit = new QLineEdit();
            m_SourceDirectoryBrowseButton = new QPushButton(tr("Browse"));
        sourceDirectoryRow->addWidget(m_SourceDirectoryLabel);
        sourceDirectoryRow->addWidget(m_SourceDirectoryLineEdit, 1);
        sourceDirectoryRow->addWidget(m_SourceDirectoryBrowseButton);

        QHBoxLayout *emptyDestinationDirectoryRow = new QHBoxLayout();
            m_EmptyDestinationDirectoryLabel = new QLabel(tr("Empty Destination Directory:"));
            m_EmptyDestinationDirectoryLineEdit = new QLineEdit();
            m_EmptyDestinationDirectoryBrowseButton = new QPushButton(tr("Browse"));
        emptyDestinationDirectoryRow->addWidget(m_EmptyDestinationDirectoryLabel);
        emptyDestinationDirectoryRow->addWidget(m_EmptyDestinationDirectoryLineEdit, 1);
        emptyDestinationDirectoryRow->addWidget(m_EmptyDestinationDirectoryBrowseButton);

        QHBoxLayout *easyTreeHashOutputFilePathRow = new QHBoxLayout();
            m_EasyTreeHashOutputFilePathLabel = new QLabel(tr("Easy Tree Hash Output File:"));
            m_EasyTreeHashOutputFilePathLineEdit = new QLineEdit();
            m_EasyTreeHashOutputFilePathBrowseButton = new QPushButton(tr("Browse"));
        easyTreeHashOutputFilePathRow->addWidget(m_EasyTreeHashOutputFilePathLabel);
        easyTreeHashOutputFilePathRow->addWidget(m_EasyTreeHashOutputFilePathLineEdit, 1);
        easyTreeHashOutputFilePathRow->addWidget(m_EasyTreeHashOutputFilePathBrowseButton);

        QHBoxLayout *controlsRow = new QHBoxLayout();
            m_CryptographicHashAlgorithmComboBox = new QComboBox();
                //Taking out Md4 because it's the same size (128 bits) as Md5... and the size is how my parser will determine the hash used! I wanted to have CRC32/MD5/SHA1 to begin with and thought CRC32 was one of the enums. I know it's available via qChecksum, but imo it's essentially a cryptographic hash... or at least performs MORE OR LESS the same functionality (inb4 correction)... albeit "insecurely" (hence not the same). Oh wow I stand corrected, it's CRC16 lmfao. I'll just stick with Md5/SHa1. KISS.
                //m_CryptographicHashAlgorithmComboBox->insertItem(0, tr("Md4"), qVariantFromValue(QCryptographicHash::Md4));
                m_CryptographicHashAlgorithmComboBox->insertItem(0, "MD5", qVariantFromValue(QCryptographicHash::Md5));
                m_CryptographicHashAlgorithmComboBox->insertItem(1, "SHA-1", qVariantFromValue(QCryptographicHash::Sha1));

                m_CryptographicHashAlgorithmComboBox->setCurrentIndex(0);
            m_BeginOrCancelCopyButton = new QPushButton(m_BeginCopyString);
        controlsRow->addWidget(m_CryptographicHashAlgorithmComboBox);
        controlsRow->addWidget(m_BeginOrCancelCopyButton, 1);

        QHBoxLayout *outputRow = new QHBoxLayout();
            m_Debug = new QPlainTextEdit();
            m_Error = new QPlainTextEdit();
        outputRow->addWidget(m_Debug);
        outputRow->addWidget(m_Error);

    m_Layout->addLayout(sourceDirectoryRow);
    m_Layout->addLayout(emptyDestinationDirectoryRow);
    m_Layout->addLayout(easyTreeHashOutputFilePathRow);
    m_Layout->addLayout(controlsRow);
    m_Layout->addLayout(outputRow);

    setLayout(m_Layout);
    //End Layout

    //Begin Connections
    connect(m_SourceDirectoryBrowseButton, SIGNAL(clicked()), this, SLOT(handleSourceDirectoryBrowseButtonClicked()));
    connect(m_EmptyDestinationDirectoryBrowseButton, SIGNAL(clicked()), this, SLOT(handleEmptyDestinationDirectoryBrowseButtonClicked()));
    connect(m_EasyTreeHashOutputFilePathBrowseButton, SIGNAL(clicked()), this, SLOT(handleEasyTreeHashOutputFilePathBrowseButtonClicked()));

    connect(m_BeginOrCancelCopyButton, SIGNAL(clicked()), this, SLOT(handleBeginOrCancelCopyButtonClicked()));
    //End Connections

    handleD("Depending on your situation, it may be advisable/REQUIRED that you run this application as root (this is a note to myself because my NTFS partitions are owned by..... who the fuck knows? In my experiences, user can almost never read them though lol)");
}
void MainWidget::updateGuiToReflectCopyInProgress()
{
    m_SourceDirectoryLineEdit->setEnabled(!m_CopyInProgress);
    m_SourceDirectoryBrowseButton->setEnabled(!m_CopyInProgress);

    m_EmptyDestinationDirectoryLineEdit->setEnabled(!m_CopyInProgress);
    m_EmptyDestinationDirectoryBrowseButton->setEnabled(!m_CopyInProgress);

    m_EasyTreeHashOutputFilePathLineEdit->setEnabled(!m_CopyInProgress);
    m_EasyTreeHashOutputFilePathBrowseButton->setEnabled(!m_CopyInProgress);

    m_CryptographicHashAlgorithmComboBox->setEnabled(!m_CopyInProgress);

    m_BeginOrCancelCopyButton->setText(m_CopyInProgress ? tr("Cancel") : m_BeginCopyString);
}
void MainWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void MainWidget::handleE(const QString &msg)
{
    m_Error->appendPlainText(msg);
}
void MainWidget::handleSourceDirectoryBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Source Directory To Copy/Tree/Hash From"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_SourceDirectoryLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void MainWidget::handleEmptyDestinationDirectoryBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Empty Destination Directory To Copy To"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_EmptyDestinationDirectoryLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void MainWidget::handleEasyTreeHashOutputFilePathBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Save Output Tree File Where?"));
    aFileDialog.setFileMode(QFileDialog::AnyFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptSave);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_EasyTreeHashOutputFilePathLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void MainWidget::handleBeginOrCancelCopyButtonClicked()
{
    if(m_CopyInProgress)
    {
        //cancel it?

        //m_CopyInProgress = false; //TODOreq: the business should let us know when the cancel goes through... which brings my brain to a very difficult problem: how the business will process the cancel request! It will slow down the copy operation a bit if we use a queued slot invocation for each file copy (and then the cancel isn't processed for a long time on a large file)... and if we don't do that method and also don't call processEvents() periodically, we'll never receive the cancel request to begin with! Finding that sweet spot for how often to call processEvents (you could have a counter for how many bytes have been copied and only call processEvents every... say... 10mb or something -- which is file independent. It could be 10mb "or" 100 files. The "or 100 files" part is just in case we're copying tons and tons of tiny ass files that would make it take a long time to reach 10mb. TODOreq: the problem has just been solved for this use case woot. You should generalize/abstract the logic that you're performing so it can be used in every/other use case[s]) is a bitch (and I've tried to generalize/automate(automatically/dynamically solve) the problem on numerous occasions for various use cases, of which this is just one)
        //^^For now, cancel button does nothing :)... but we still should have the sexy gui functionality where input gets disabled and re-enabled when the copy is complete

        //TEMPORARY debug message (hey we're on the GUI thread after all <3):
        handleD("Cancel doesn't work yet rofl");
    }
    else
    {
        if(m_SourceDirectoryLineEdit->text().trimmed().isEmpty())
        {
            handleD("Select A Source Directory");
            return;
        }
        if(m_EmptyDestinationDirectoryLineEdit->text().trimmed().isEmpty())
        {
            handleD("Select A Destination Directory");
            return;
        }
        if(m_EasyTreeHashOutputFilePathLineEdit->text().trimmed().isEmpty())
        {
            handleD("Select An Easy Tree Hash Output File Path");
            return;
        }
        handleD("GUI is Requesting Copy Operation...");
        emit copyOperationRequested(m_SourceDirectoryLineEdit->text(), m_EmptyDestinationDirectoryLineEdit->text(), m_EasyTreeHashOutputFilePathLineEdit->text(), m_CryptographicHashAlgorithmComboBox->itemData(m_CryptographicHashAlgorithmComboBox->currentIndex()).value<QCryptographicHash::Algorithm>());
        m_CopyInProgress = true;
    }
    updateGuiToReflectCopyInProgress();
}
void MainWidget::handleCopyOperationComplete()
{
    handleD("GUI has been notified that the copy operation has completed successfully");
    m_CopyInProgress = false;
    updateGuiToReflectCopyInProgress();
}
