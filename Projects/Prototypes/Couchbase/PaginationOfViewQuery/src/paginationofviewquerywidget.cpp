#include "paginationofviewquerywidget.h"

#include <QVBoxLayout>
#include <QSpinBox>
#include <QPlainTextEdit>

PaginationOfViewQueryWidget::PaginationOfViewQueryWidget(QWidget *parent)
    : QWidget(parent)
    , m_PageSelectionSpinBox(new QSpinBox())
    , m_PageViewer(new QPlainTextEdit())
{
    m_PageSelectionSpinBox->setPrefix(tr("Page "));
    m_PageSelectionSpinBox->setMinimum(1);

    QVBoxLayout *myLayout = new QVBoxLayout(this);

    myLayout->addWidget(m_PageSelectionSpinBox);
    myLayout->addWidget(m_PageViewer);

    connect(m_PageSelectionSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(queryPageOfViewRequested(int)));
}
void PaginationOfViewQueryWidget::displayPageOfView(const ViewQueryPageContentsType &usernamesOnPage, bool internalServerErrorOrJsonError)
{
    m_PageViewer->clear();
    if(internalServerErrorOrJsonError)
    {
        m_PageViewer->appendPlainText(tr("500 Internal Server"));
        return;
    }
    if(usernamesOnPage.isEmpty())
    {
        m_PageViewer->appendPlainText(tr("The page you requested does not exist"));
        return;
    }
    Q_FOREACH(const std::string &currentUsername, usernamesOnPage)
    {
        m_PageViewer->appendPlainText(QString::fromStdString(currentUsername));
    }
}
