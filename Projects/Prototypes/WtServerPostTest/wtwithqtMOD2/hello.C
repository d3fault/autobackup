/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>

#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

#include "HelloApplication.h"
#include "QtObject.h"

// Needed when using WQApplication with Qt eventloop = true
//#include <QApplication>
#include <QtCore>

#include <QDebug>

using namespace Wt;

HelloApplication::HelloApplication(const WEnvironment& env, WServer &server)
    : WQApplication(env, true), m_Server(server)
{
  /*
   * Note: do not create any Qt objects from here. Initialize your
   * application from within the virtual create() method.
   */
}

void HelloApplication::create()
{
    m_SessionId = WApplication::instance()->sessionId();

  setTitle("Hello world");

  root()->addWidget(new WText("Num 0-10 to watch: "));
  nameEdit_ = new WLineEdit(root());
  nameEdit_->setFocus();

  WPushButton *b = new WPushButton("Watch For It", root());
  b->setMargin(5, Left);

  root()->addWidget(new WBreak());

  greeting_ = new WText(root());

  b->clicked().connect(this, &HelloApplication::propagateGreet);
  nameEdit_->enterPressed().connect(this, &HelloApplication::propagateGreet);

  qtSender_ = new QtObject(this);
  qtReceiver_ = new QtObject(this);

  QObject::connect(qtSender_, SIGNAL(greet(const QString&)),qtReceiver_, SLOT(doGreet(const QString&)), Qt::QueuedConnection);

  enableUpdates(true);
}

void HelloApplication::destroy()
{
  /*
   * Note: Delete any Qt object from here.
   */
  delete qtSender_;
  delete qtReceiver_;
}

void HelloApplication::propagateGreet()
{
  qtSender_->passGreet(toQString(nameEdit_->text()));
}

void HelloApplication::doGreet(const QString& qname)
{
    qDebug() << "############doGreet called";
    m_Server.post(m_SessionId, boost::bind(boost::bind(&HelloApplication::updateGuiCallback, this, _1), qname));
  //greeting_->setText("Number Appeared: " + toWString(qname));
}
void HelloApplication::updateGuiCallback(const QString &text)
{
    WApplication *app = WApplication::instance();
    if(app)
    {
        new WBreak(app->root());
        new WText(toWString(text), app->root());
        app->triggerUpdate();
    }
}
WApplication *createApplication(const WEnvironment& env, WServer &server)
{
  return new HelloApplication(env, server);
}

int main(int argc, char **argv)
{
  // Needed for Qt's eventloop threads to work
  //QApplication app(argc, argv);
    QCoreApplication a(argc, argv);

    Wt::WServer server(argv[0]);
    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    server.addEntryPoint(Wt::Application, boost::bind(createApplication, _1, boost::ref(server)));

    if (server.start())
    {
      int sig = Wt::WServer::waitForShutdown();
      std::cerr << "Shutting down: (signal = " << sig << ")" << std::endl;
      server.stop();
    }


  //return WRun(argc, argv, &createApplication);
}

