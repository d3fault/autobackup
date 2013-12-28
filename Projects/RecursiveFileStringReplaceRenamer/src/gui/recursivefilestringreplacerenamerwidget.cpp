#include "recursivefilestringreplacerenamerwidget.h"

RecursiveFileStringReplaceRenamerWidget::RecursiveFileStringReplaceRenamerWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_Directory = new LabelLineEditBrowseButton("Directory To Recurse:", LabelLineEditBrowseButton::OpenDirectoryMode);
    QLabel *textToReplaceLabel = new QLabel("Text To Replace:");
    m_TextToReplaceLineEdit = new QLineEdit();
    QLabel *replacementTextLabel = new QLabel("Replacement Text:");
    m_ReplacementTextLineEdit = new QLineEdit();
    QPushButton *startButton = new QPushButton("Recursively Rename Files");
    m_Debug = new QPlainTextEdit();

    myLayout->addLayout(m_Directory);
    QHBoxLayout *textToReplaceRow = new QHBoxLayout();
    textToReplaceRow->addWidget(textToReplaceLabel);
    textToReplaceRow->addWidget(m_TextToReplaceLineEdit);
    myLayout->addLayout(textToReplaceRow);
    QHBoxLayout *replacementTextRow = new QHBoxLayout();
    replacementTextRow->addWidget(replacementTextLabel);
    replacementTextRow->addWidget(m_ReplacementTextLineEdit);
    myLayout->addLayout(replacementTextRow);
    myLayout->addWidget(startButton);
    myLayout->addWidget(m_Debug);

    setLayout(myLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));
}
RecursiveFileStringReplaceRenamerWidget::~RecursiveFileStringReplaceRenamerWidget()
{ }
void RecursiveFileStringReplaceRenamerWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void RecursiveFileStringReplaceRenamerWidget::handleStartButtonClicked()
{
    if(m_Directory->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("select a directory");
        return;
    }
    if(m_TextToReplaceLineEdit->text().trimmed().isEmpty())
    {
        handleD("enter text to replace");
        return;
    }
    if(m_ReplacementTextLineEdit->text().trimmed().isEmpty())
    {
        handleD("enter replacement text");
        return;
    }
    emit recursivelyRenameFilesDoingSimpleStringReplaceRequested(m_Directory->lineEdit()->text(), m_TextToReplaceLineEdit->text(), m_ReplacementTextLineEdit->text());
}
