#ifndef EASYTREETOTALSIZECALCULATORWIDGET_H
#define EASYTREETOTALSIZECALCULATORWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFileDialog>

class EasyTreeTotalSizeCalculatorWidget : public QWidget
{
    Q_OBJECT    
public:
    EasyTreeTotalSizeCalculatorWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;
    QLabel *m_FilepathOfEasyTreeFileLabel;
    QLineEdit *m_FilepathOfEasyTreeFileLineEdit;
    QPushButton *m_FilepathOfEasyTreeFileBrowseButton;
    QPushButton *m_CalculateTotalSizeOfEasyTreeEntriesButton;
    QPlainTextEdit *m_Debug;
signals:
    void calculateTotalSizeOfEasyTreeEntriesRequested(const QString &filepathOfEasyTreeFile);
private slots:
    void handleFilepathOfEasyTreeFileBrowseButtonClicked();
    void handleCalculateTotalSizeOfEasyTreeEntriesButtonClicked();
public slots:
    void handleD(const QString &msg);
};

#endif // EASYTREETOTALSIZECALCULATORWIDGET_H
