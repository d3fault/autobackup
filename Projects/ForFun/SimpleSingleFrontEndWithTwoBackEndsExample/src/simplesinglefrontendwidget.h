/*
Copyright (c) 2012, d3fault <d3faultdotxbe@gmail.com>
Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE
*/
#ifndef SIMPLESINGLEFRONTENDWIDGET_H
#define SIMPLESINGLEFRONTENDWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class SimpleSingleFrontendWidget : public QWidget
{
    Q_OBJECT
public:
    SimpleSingleFrontendWidget(QWidget *parent = 0);
private:
    //Even though all of our GUI objects are pointers, we do not have to worry about calling delete on them (rule of thumb: any time you 'new' something, you also need to 'delete' it. This is an exception to that rule) because of QObject's child/parent relationship. When we call this->setLayout(m_Layout), 'this' becomes the parent of m_Layout and all child objects (everything we add to the layout becomes a sibling of m_Layout), and deletes it (and all children) for us when 'this' is destroyed
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

#endif // SIMPLESINGLEFRONTENDWIDGET_H
