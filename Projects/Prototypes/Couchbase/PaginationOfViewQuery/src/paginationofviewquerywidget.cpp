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
    m_PageSelectionSpinBox->setRange(1, 2);

    QVBoxLayout *myLayout = new QVBoxLayout(this);

    myLayout->addWidget(m_PageSelectionSpinBox);
    myLayout->addWidget(m_PageViewer);

    connect(m_PageSelectionSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(queryPageOfViewRequested(int)));
}
void PaginationOfViewQueryWidget::displayPageOfView(const ViewQueryPageContentsType &usernamesOnPage)
{
    m_PageViewer->clear();
    Q_FOREACH(const std::string &currentUsername, usernamesOnPage)
    {
        m_PageViewer->appendPlainText(QString::fromStdString(currentUsername));
    }
}
