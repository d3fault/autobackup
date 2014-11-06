#ifndef PAGINATIONOFVIEWQUERYWIDGET_H
#define PAGINATIONOFVIEWQUERYWIDGET_H

#include <QWidget>

#include "paginationofviewquerycommon.h"

class QSpinBox;
class QPlainTextEdit;

class PaginationOfViewQueryWidget : public QWidget
{
    Q_OBJECT
public:
    PaginationOfViewQueryWidget(QWidget *parent = 0);
private:
    QSpinBox *m_PageSelectionSpinBox;
    QPlainTextEdit *m_PageViewer;
signals:
    void queryPageOfViewRequested(int pageNum);
public slots:
    void displayPageOfView(const ViewQueryPageContentsType &usernamesOnPage);
};

#endif // PAGINATIONOFVIEWQUERYWIDGET_H
