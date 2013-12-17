#ifndef LOSSYDIRECTORYCOPIERWIDGET_H
#define LOSSYDIRECTORYCOPIERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

class LossyDirectoryCopierWidget : public QWidget
{
    Q_OBJECT
public:
    LossyDirectoryCopierWidget(QWidget *parent = 0);
    ~LossyDirectoryCopierWidget();
private:
    QVBoxLayout *m_Layout;
    QLabel *m_SourceDirLabel;
    QLineEdit *m_SourceDirLineEdit;
    QLabel *m_DestDirLabel;
    QLineEdit *m_DestDirLineEdit;
    QPushButton *m_StartButton;
    QPushButton *m_StopButton;
    QPlainTextEdit *m_Debug;
signals:
    void lossilyCopyEveryDirectoryEntryAndJustCopyWheneverCantCompressRequested(const QString &sourceDirString, const QString &destDirString);
    void stopRequested();
public slots:
    void handleD(const QString &msg);
    void handleStartButtonClicked();
};

#endif // LOSSYDIRECTORYCOPIERWIDGET_H
