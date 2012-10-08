#include "mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent), m_EasyTreeBusiness(0)
{
    m_Layout = new QVBoxLayout();
    m_DirToTreeLe = new QLineEdit("/home/d3fault/Temp");
    m_BrowseButton = new QPushButton("Browse");
    m_ClearAndThenTreeButton = new QPushButton("Clear + Tree!");
    m_Debug = new QPlainTextEdit();

    m_Layout->addWidget(m_DirToTreeLe);
    m_Layout->addWidget(m_BrowseButton);
    m_Layout->addWidget(m_ClearAndThenTreeButton);
    m_Layout->addWidget(m_Debug);

    this->setLayout(m_Layout);

    connect(m_BrowseButton, SIGNAL(clicked()), this, SLOT(handleBrowseButtonClicked()));
    connect(m_ClearAndThenTreeButton, SIGNAL(clicked()), this, SLOT(handleClearAndThenTreeButtonClicked()));
}
void mainWidget::setBusiness(EasyTree *easyTreeBusiness)
{
    m_EasyTreeBusiness = easyTreeBusiness;
    connectToBusiness();
}
mainWidget::~mainWidget()
{
    
}
void mainWidget::connectToBusiness()
{
    connect(this, SIGNAL(treeTextGenerationRequested(QString,QList<QString>*,QList<QString>*,QList<QString>*,QList<QString>*)), m_EasyTreeBusiness, SLOT(generateTreeText(QString,QList<QString>*,QList<QString>*,QList<QString>*,QList<QString>*)));
    connect(m_EasyTreeBusiness, SIGNAL(treeTextGenerated(const QString &)), this, SLOT(handleD(const QString &)));
}
void mainWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void mainWidget::handleBrowseButtonClicked()
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);
    if(dialog.exec())
    {
        if(dialog.selectedFiles().size() > 0)
        {
            m_DirToTreeLe->setText(dialog.selectedFiles().at(0));
        }
    }
}
void mainWidget::handleClearAndThenTreeButtonClicked()
{
    m_Debug->clear();
    if(!m_DirToTreeLe->text().trimmed().isEmpty())
    {
        QList<QString> *emptyListToTestWith = new QList<QString>();
        emit treeTextGenerationRequested(m_DirToTreeLe->text(), emptyListToTestWith, emptyListToTestWith, emptyListToTestWith, emptyListToTestWith);
    }
}
