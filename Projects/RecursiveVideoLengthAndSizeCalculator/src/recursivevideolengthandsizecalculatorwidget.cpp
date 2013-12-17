#include "recursivevideolengthandsizecalculatorwidget.h"

RecursiveVideoLengthAndSizeCalculatorWidget::RecursiveVideoLengthAndSizeCalculatorWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *theLayout = new QVBoxLayout();
    QHBoxLayout *dirRow = new QHBoxLayout();
    QLabel *dirLabel = new QLabel("Dir:");
    m_DirLineEdit = new QLineEdit();
    QPushButton *startButton = new QPushButton("Calculate Total Duration and Size For All Videos In Dir");
    m_Debug = new QPlainTextEdit();

    dirRow->addWidget(dirLabel);
    dirRow->addWidget(m_DirLineEdit);
    theLayout->addLayout(dirRow);

    theLayout->addWidget(startButton);
    theLayout->addWidget(m_Debug);

    setLayout(theLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));
}
RecursiveVideoLengthAndSizeCalculatorWidget::~RecursiveVideoLengthAndSizeCalculatorWidget()
{

}
void RecursiveVideoLengthAndSizeCalculatorWidget::handleStartButtonClicked()
{
    if(m_DirLineEdit->text().trimmed().isEmpty())
    {
        handleD("select a dir");
        return;
    }
    emit recursivelyCalculateVideoLengthsAndSizesRequested(m_DirLineEdit->text());
}
void RecursiveVideoLengthAndSizeCalculatorWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
