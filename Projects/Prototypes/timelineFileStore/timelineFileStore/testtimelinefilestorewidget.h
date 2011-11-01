#ifndef TESTTIMELINEFILESTOREWIDGET_H
#define TESTTIMELINEFILESTOREWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDateTime>

#include <timelinewithfilestore.h>

class testTimelineFileStoreWidget : public QWidget
{
    Q_OBJECT

public:
    testTimelineFileStoreWidget(QWidget *parent = 0);
    ~testTimelineFileStoreWidget();
private:
    //gui
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_NewItemTitle;
    QPlainTextEdit *m_NewItemContent;
    QPushButton *m_SubmitButton;

    //backend
    TimelineWithFileStore *m_Timeline;
private slots:
    void handleSubmitButtonClicked();
};

#endif // TESTTIMELINEFILESTOREWIDGET_H
