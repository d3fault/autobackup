#ifndef CONCURRENTLOSSYDIRECTORYCOPIERWIDGET_H
#define CONCURRENTLOSSYDIRECTORYCOPIERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

class ConcurrentLossyDirectoryCopierWidget : public QWidget
{
    Q_OBJECT
public:
    ConcurrentLossyDirectoryCopierWidget(QWidget *parent = 0);
    ~ConcurrentLossyDirectoryCopierWidget();
private:
    QLineEdit *m_SourceDirLineEdit;
    QLineEdit *m_DestinationDirLineEdit;
    QPlainTextEdit *m_Debug;

    int m_MinProgress;
    int m_MaxProgress;
signals:
    void lossilyCopyDirectoryUsingAllAvailableCoresRequested(const QString &sourceDirToLossilyCopyString, const QString &destinationDirToLossilyCopyToString);
    void cancelAfterAllCurrentlyEncodingVideosFinishRequested();
public slots:
    void recordUpdatedProgressMinsAndMaxes(int minProgress, int maxProgress);
    void updateProgressValue(int newProgress);
    void handleD(const QString &msg);
private slots:
    void handleStartButtonClicked();
};

#endif // CONCURRENTLOSSYDIRECTORYCOPIERWIDGET_H
