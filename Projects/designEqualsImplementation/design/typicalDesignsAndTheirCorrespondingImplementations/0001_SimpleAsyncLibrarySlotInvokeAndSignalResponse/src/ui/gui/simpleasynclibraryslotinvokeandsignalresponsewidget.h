#ifndef SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEWIDGET_H
#define SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEWIDGET_H

#include <QWidget>

class QLineEdit;
class QPlainTextEdit;

class SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget : public QWidget
{
    Q_OBJECT
public:
    SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget(QWidget *parent = 0);
    ~SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget();
private:
    QLineEdit *m_LineEdit;
    QPlainTextEdit *m_Output;
signals:
    void fooSlotRequested(QString);
private slots:
    void handleFooSlotButtonPressed();
public slots:
    void handleFooSignal(bool success);
};

#endif // SIMPLEASYNCLIBRARYSLOTINVOKEANDSIGNALRESPONSEWIDGET_H
