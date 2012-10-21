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
#ifndef SIMPLESINGLEFRONTENDWITHTWOBACKENDSEXAMPLETEST_H
#define SIMPLESINGLEFRONTENDWITHTWOBACKENDSEXAMPLETEST_H

#include <QCoreApplication>
#include <QObject>
#include <QThread>

#include "simplesinglefrontendwidget.h"
#include "simplesinglebackend1.h"
#include "simplesinglebackend2.h"

//The SimpleSingleFrontendWithTwoBackendsExampleTest class exists for a few reasons, but isn't strictly necessary:
//a) To keep main() as empty/clean/concise as possible. To provide a consistent entry point into your code
//b) To keep the GUI and backends oblivious of each other, we are in charge of setting up the connections between them
//c) Following the rapid-prototyping (also known as "component-driven") development model, it is very likely that the gui and/or backends will not be used as-is. They will most likely be incorporated (after prototyping ;-P) into a larger application, which gives you the flexibility to decide how and where to set them up. A 'test' class gives you something you can easily test/prototype with (and refer to later when you forget what's going on), but can still very easily throw away since it doesn't contain much actual logic
class SimpleSingleFrontendWithTwoBackendsExampleTest : public QObject
{
    Q_OBJECT
public:
    explicit SimpleSingleFrontendWithTwoBackendsExampleTest(QObject *parent = 0);
    void startExample();
private:
    //GUI QWidget. He is completely oblivious of the back end objects and what thread they live on
    SimpleSingleFrontendWidget m_Gui;

    //Our backend objects (which will be moved to their own threads). They are completely oblivious of the GUI
    SimpleSingleBackend1 m_SimpleSingleBackend1;
    SimpleSingleBackend2 m_SimpleSingleBackend2;

    //Qt's thread helper class, QThread, greatly simplifies managing native threads in a cross platform manner. Note that QThread is not actually a thread, nor does it live on the thread it manages. In this case, the QThread objects live on the GUI thread
    QThread m_BackendThread1;
    QThread m_BackendThread2;
public slots:
    void handleAboutToQuit();
};

#endif // SIMPLESINGLEFRONTENDWITHTWOBACKENDSEXAMPLETEST_H
