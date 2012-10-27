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
#include "cleanthreadingfrontendwidget.h"

CleanThreadingFrontendWidget::CleanThreadingFrontendWidget(QWidget *parent)
    : QWidget(parent)
{
    //This isn't a necessity, but is a technique used to get an application to launch faster. Basically we are deferring the initializing of our GUI objects to a later date so we can just get the [empty] window up and shown asap. In this case, Qt::QueuedConnection is not redundant. If we left it out, Qt::AutoConnection would see that we're on the same thread as 'this' (obviously) and call setupGui() using a Qt::DirectConnection
    QMetaObject::invokeMethod(this, "setupGui", Qt::QueuedConnection);
}
void CleanThreadingFrontendWidget::setupGui()
{
    //Allocate and set up all of our GUI widgets
    m_Layout = new QVBoxLayout();

    m_StringToHashLabel = new QLabel("String To Hash:");
    m_StringToHashLineEdit = new QLineEdit("hash me please");

    m_NumberOfTimesToHashTheStringLabel = new QLabel("How Many Times?");
    m_NumberOfTimesToHashTheStringLineEdit = new QLineEdit("500000");

    m_HashItButton = new QPushButton("Thrash Hash The String! Also Observe How The GUI Remains Responsive <3 Qt");

    m_CurrentMaxNumberToGenerate = 10;
    m_MaximumPositiveNumberGeneratedLabel = new QLabel("Generate Random Numbers from Zero to:");
    m_MaximumPositiveNumberGeneratedLineEdit = new QLineEdit(QString::number(m_CurrentMaxNumberToGenerate));

    m_CurrentNumberToWatchFor = 3;
    m_NumberToWatchForLabel = new QLabel("Number To Watch For:");
    m_NumberToWatchForLineEdit = new QLineEdit(QString::number(m_CurrentNumberToWatchFor));

    m_HasherOutputLabel = new QLabel("Backend Hasher Object Output:");
    m_HasherOutputPlainTextEdit = new QPlainTextEdit();

    m_TimerOutputLabel = new QLabel("Backend Timer Object Output");
    m_TimerOutputPlainTextEdit = new QPlainTextEdit();

    m_DebugStopBackendsTestButton = new QPushButton("DEBUG: Stop Backends. Simulate Closing the window... without closing the window ;-)");

    //Now add all of the widgets to m_Layout, including some QHBoxLayouts to make certain content be horizontal
    QHBoxLayout *hasherParamsHboxLayout = new QHBoxLayout();
    hasherParamsHboxLayout->addWidget(m_StringToHashLabel);
    hasherParamsHboxLayout->addWidget(m_StringToHashLineEdit);
    hasherParamsHboxLayout->addWidget(m_NumberOfTimesToHashTheStringLabel);
    hasherParamsHboxLayout->addWidget(m_NumberOfTimesToHashTheStringLineEdit);
    m_Layout->addLayout(hasherParamsHboxLayout); //Just like widgets, the layout becomes a child object and gets deleted automatically

    m_Layout->addWidget(m_HashItButton);

    QHBoxLayout *timerParamsHboxLayout = new QHBoxLayout();
    timerParamsHboxLayout->addWidget(m_MaximumPositiveNumberGeneratedLabel);
    timerParamsHboxLayout->addWidget(m_MaximumPositiveNumberGeneratedLineEdit);
    timerParamsHboxLayout->addWidget(m_NumberToWatchForLabel);
    timerParamsHboxLayout->addWidget(m_NumberToWatchForLineEdit);
    m_Layout->addLayout(timerParamsHboxLayout);

    QHBoxLayout *outputViewsHeadersHboxLayout = new QHBoxLayout();
    outputViewsHeadersHboxLayout->addWidget(m_HasherOutputLabel);
    outputViewsHeadersHboxLayout->addWidget(m_TimerOutputLabel);
    m_Layout->addLayout(outputViewsHeadersHboxLayout);

    QHBoxLayout *outputViewsHBoxLayout = new QHBoxLayout();
    outputViewsHBoxLayout->addWidget(m_HasherOutputPlainTextEdit);
    outputViewsHBoxLayout->addWidget(m_TimerOutputPlainTextEdit);
    m_Layout->addLayout(outputViewsHBoxLayout);

    m_Layout->addWidget(m_DebugStopBackendsTestButton);

    //Finally, set the layout. This is when the layout, it's sub-layouts, and all the widgets it is organizing become children of 'this'. The sub-layouts become children of their parent layouts, but every widget added to every layout/sub-layout/sub-sub-layout (continuing infinitely) becomes a direct child of 'this'.
    this->setLayout(m_Layout);

    //Connect to our button's clicked() signal. Sometimes you can connect a button's clicked signal directly to the signal that our backend object is connected to (our thrashHashStringNtimesRequested() signal), but in this case we need to pull variables out of a few line edits before emitting thrashHashStringNtimesRequested(); Note: you could also use QMetaObject::invokeMethod() inside of handleHashItButtonClicked(), but it's easier to send arguments using emit in my opinion
    connect(m_HashItButton, SIGNAL(clicked()), this, SLOT(handleHashItButtonClicked()));
    //Connect to our random number properties' changed() signals... so we can sanitize them and emit them for our backend to eventually process
    connect(m_MaximumPositiveNumberGeneratedLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkTextActuallyChangedAndEmitIfTheyDid()));
    connect(m_NumberToWatchForLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkTextActuallyChangedAndEmitIfTheyDid()));

    connect(m_DebugStopBackendsTestButton, SIGNAL(clicked()), this, SIGNAL(debugStopBackendsRequested()));
}
void CleanThreadingFrontendWidget::handleHashItButtonClicked()
{
    //Sometimes it's better to do input sanitization on the backend object... sometimes it's better to do it in the GUI (here)... and sometimes you sanitize for both. Just to keep things simple I'm going to sanitize my input in the GUI object. Every application will be different
    QString stringToHash = m_StringToHashLineEdit->text();
    if(!stringToHash.trimmed().isEmpty())
    {
        bool convertFromStringToIntSuccess = false;
        int numberOfTimesToHashTheString = m_NumberOfTimesToHashTheStringLineEdit->text().trimmed().toInt(&convertFromStringToIntSuccess);
        if(convertFromStringToIntSuccess)
        {
            //Finally, we've passed out input sanitization so now we can send the values to our backend (Note: as mentioned earlier, we aren't sending it explicitly to the backend... we're merely emitting a signal that the backend just happens to be listening to)
            emit thrashHashStringNtimesRequested(stringToHash, numberOfTimesToHashTheString);
        }
        else
        {
            m_HasherOutputPlainTextEdit->appendPlainText("Enter a valid integer for number of times to hash");
        }
    }
    else
    {
        m_HasherOutputPlainTextEdit->appendPlainText("Enter a string to hash");
    }
}
void CleanThreadingFrontendWidget::checkTextActuallyChangedAndEmitIfTheyDid()
{
    //Sanitize the input as a string, make sure it isn't empty
    QString maxNumberToGenerate = m_MaximumPositiveNumberGeneratedLineEdit->text().trimmed();
    if(maxNumberToGenerate.isEmpty())
    {
        //Don't update the backend when the strings are empty.. just keep using the old values. But we also don't send an error because they might have pressed backspace and are intending on typing a number
        return;
    }
    QString numberToWatchFor = m_NumberToWatchForLineEdit->text().trimmed();
    if(numberToWatchFor.isEmpty())
    {
        //Same
        return;
    }

    //Sanitize the input as an integer, make sure it's valid
    bool convertSuccess = false;
    int newMaxNumberToGenerate = maxNumberToGenerate.toInt(&convertSuccess);
    if(!convertSuccess)
    {
        //This time they typed an illegal character, so display an error as well
        m_TimerOutputPlainTextEdit->appendPlainText("Enter a valid integer for maximum random number to generate");
        return;
    }
    convertSuccess = false;
    int newNumberToWatchFor = numberToWatchFor.toInt(&convertSuccess);
    if(!convertSuccess)
    {
        //Same
        m_TimerOutputPlainTextEdit->appendPlainText("Enter a valid integer for number to watch for");
        return;
    }

    //Make sure they aren't negative
    if(newMaxNumberToGenerate <= 0 || newNumberToWatchFor < 0)
    {
        //We can't have our max number to generate be anything less than 1, and we can't watch for anything less than 0
        m_TimerOutputPlainTextEdit->appendPlainText("Enter a valid integer within a sane range. Note: We cannot handle negatives");
        return;
    }

    //Make sure one of the values actually changed...
    if(newMaxNumberToGenerate == m_CurrentMaxNumberToGenerate && newNumberToWatchFor == m_CurrentNumberToWatchFor)
    {
        //Do nothing because nothing has changed. They backspaced and typed in the same number again?
        return;
    }

    m_CurrentMaxNumberToGenerate = newMaxNumberToGenerate;
    m_CurrentNumberToWatchFor = newMaxNumberToGenerate;

    //Finally, we've passed sanitization so we can update the backend with the new properties
    emit updateRandomNumberGeneratorPropertiesRequested(newMaxNumberToGenerate, newNumberToWatchFor);
}
void CleanThreadingFrontendWidget::handleHashGenerated(const QString &hash)
{
    //This slot gets called on the GUI thread whenever CleanThreadingBackend1::hashGenerated() is emitted
    m_HasherOutputPlainTextEdit->appendPlainText(hash);
}
void CleanThreadingFrontendWidget::handleRandomNumberResultsGathered(const QString &stringResult)
{
    //This slot gets called on the GUI thread whenever CleanThreadingBackend2::randomNumberResultsGathered() is emitted
    m_TimerOutputPlainTextEdit->appendPlainText(stringResult);
}
