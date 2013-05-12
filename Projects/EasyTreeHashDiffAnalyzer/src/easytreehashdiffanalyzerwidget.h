#ifndef EASYTREEHASHDIFFANALYZERWIDGET_H
#define EASYTREEHASHDIFFANALYZERWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFileDialog>

class EasyTreeHashDiffAnalyzerWidget : public QWidget
{
    Q_OBJECT
public:
    EasyTreeHashDiffAnalyzerWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;

    QLabel *m_Input1Label;
    QLineEdit *m_Input1LineEdit;
    QPushButton *m_Input1BrowseButton;

    QLabel *m_Input2Label;
    QLineEdit *m_Input2LineEdit;
    QPushButton *m_Input2BrowseButton;

    QPushButton *m_DiffAndAnalyzeButton;

    //QPlainTextEdit *m_FilesExistIn1ButNot2;
    //QPlainTextEdit *m_FilesExistIn2ButNot1;
    //QPlainTextEdit *m_FilesExistInBothButHashesDontMatch; //includes file size mismatches, which are merely optimization-check of hashes really (we should still debug output the file size mismatch, though the 'end result' is the same (1 != 2))
    QPlainTextEdit *m_Debug;
signals:
    void diffAndAnalyzeRequested(const QString &input1, const QString &input2);
private slots:
    void handleD(const QString &msg);
    void handleInput1BrowseButtonClicked();
    void handleInput2BrowseButtonClicked();
    void handleDiffAndAnalyzeButtonClicked();
};

#endif // EASYTREEHASHDIFFANALYZERWIDGET_H
