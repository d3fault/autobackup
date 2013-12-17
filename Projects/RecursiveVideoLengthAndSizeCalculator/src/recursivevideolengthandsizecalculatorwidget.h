#ifndef RECURSIVEVIDEOLENGTHANDSIZECALCULATORWIDGET_H
#define RECURSIVEVIDEOLENGTHANDSIZECALCULATORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

class RecursiveVideoLengthAndSizeCalculatorWidget : public QWidget
{
    Q_OBJECT
public:
    RecursiveVideoLengthAndSizeCalculatorWidget(QWidget *parent = 0);
    ~RecursiveVideoLengthAndSizeCalculatorWidget();
private:
    QLineEdit *m_DirLineEdit;
    QPlainTextEdit *m_Debug;
signals:
    void recursivelyCalculateVideoLengthsAndSizesRequested(const QString &dir);
public slots:
    void handleStartButtonClicked();
    void handleD(const QString &msg);
};

#endif // RECURSIVEVIDEOLENGTHANDSIZECALCULATORWIDGET_H
