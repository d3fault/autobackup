#ifndef MAINCLEANOBSERVERPATTERNEXAMPLEWIDGET_H
#define MAINCLEANOBSERVERPATTERNEXAMPLEWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class MainCleanObserverPatternExampleWidget : public QWidget
{
    Q_OBJECT
public:
    MainCleanObserverPatternExampleWidget(QWidget *parent = 0);
private:
    //Even though all of our GUI objects are pointers, we do not have to worry about calling delete on them (rule of thumb: any time you 'new' something, you also need to 'delete' it. This is an exception) because of QObject's child/parent relationship. When we call this->setLayout(m_Layout), this becomes the parent of m_Layout and all child objects (everything we add to the layout becomes a child of the layout), and deletes it (and all children) for us when 'this' is destroyed
    QVBoxLayout *m_Layout;
    QLabel *m_StringToHashLabel;
    QLineEdit *m_StringToHashLineEdit;
    QLabel *m_NumberOfTimesToHashTheStringLabel;
    QLineEdit *m_NumberOfTimesToHashTheStringLineEdit;
    QPushButton *m_HashItButton;
    QLabel *m_MaximumPositiveNumberGeneratedLabel;
    QLineEdit *m_MaximumPositiveNumberGeneratedLineEdit; //0 is our minimum, for simplicities sake we do not deal with negatives
    QLabel *m_NumberToWatchForLabel;
    QLineEdit *m_NumberToWatchForLineEdit;
    QLabel *m_HasherOutputLabel;
    QPlainTextEdit *m_HasherOutputWindow; //Output for Backend Object 'A'
    QLabel *m_TimerOutputLabel;
    QPlainTextEdit *m_TimerOutputWindow; //Output for Backend Object 'B'

    //Hack to detect if changes actually occurred
    int m_CurrentMaxNumberToGenerate, m_CurrentNumberToWatchFor;
signals:
    //My two signals that my back-ends react to
    void updateRandomNumberGeneratorPropertiesRequested(int maxNum, int numberToWatchFor);
    void thrashHashStringNtimesRequested(const QString &, int);
public slots:
    void setupGui();

    //My two responses to GUI interactions:
    void handleHashItButtonClicked();
    void checkTextActuallyChangedAndEmitIfTheyDid();

    //My two responses to back-end signals:
    void handleHashGenerated(const QString &hash);
    void handleRandomNumberResultsGathered(const QString &stringResult);
};

#endif // MAINCLEANOBSERVERPATTERNEXAMPLEWIDGET_H
