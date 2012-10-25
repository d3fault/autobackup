#ifndef CLEANTHREADINGFRONTENDWIDGET_H
#define CLEANTHREADINGFRONTENDWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class CleanThreadingFrontendWidget : public QWidget
{
    Q_OBJECT
public:
    CleanThreadingFrontendWidget(QWidget *parent = 0);
private:
    //Even though all of our GUI objects are pointers, we do not have to worry about calling delete on them (rule of thumb: any time you 'new' something, you also need to 'delete' it. This is an exception to that rule) because of QObject's child/parent relationship. When we call this->setLayout(m_Layout), 'this' becomes the parent of m_Layout and all child objects (everything we add to the layout (or sub-layouts) becomes a direct child of 'this'), and deletes it (and all children) for us when 'this' is destroyed
    QVBoxLayout *m_Layout;
    QLabel *m_StringToHashLabel;
    QLineEdit *m_StringToHashLineEdit;
    QLabel *m_NumberOfTimesToHashTheStringLabel;
    QLineEdit *m_NumberOfTimesToHashTheStringLineEdit;
    QPushButton *m_HashItButton;
    QLabel *m_MaximumPositiveNumberGeneratedLabel;
    QLineEdit *m_MaximumPositiveNumberGeneratedLineEdit; //1 is our implied minimum. For simplicity's sake, we do not deal with negatives. We filter them out in our "checkTextActuallyChangedAndEmitIfTheyDid" slot
    QLabel *m_NumberToWatchForLabel;
    QLineEdit *m_NumberToWatchForLineEdit;
    QLabel *m_HasherOutputLabel;
    QPlainTextEdit *m_HasherOutputPlainTextEdit; //Output for Backend Object 1
    QLabel *m_TimerOutputLabel;
    QPlainTextEdit *m_TimerOutputPlainTextEdit; //Output for Backend Object 2

    //We store the current numbers to detect if changes actually occurred when the user types in a new number
    int m_CurrentMaxNumberToGenerate, m_CurrentNumberToWatchFor;
signals:
    //The two signals that the backends react to
    void updateRandomNumberGeneratorPropertiesRequested(int maxNum, int numberToWatchFor);
    void thrashHashStringNtimesRequested(const QString &, int);
public slots:
    void setupGui();

    //The two responses to GUI interactions:
    void handleHashItButtonClicked();
    void checkTextActuallyChangedAndEmitIfTheyDid();

    //The two responses to backend signals:
    void handleHashGenerated(const QString &hash);
    void handleRandomNumberResultsGathered(const QString &stringResult);
};

#endif // CLEANTHREADINGFRONTENDWIDGET_H
