// This may look like C code, but it's really -*- C++ -*-
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
#ifndef HELLO_APPLICATION_H_
#define HELLO_APPLICATION_H_

#include "WQApplication"

class QtObject;
class QString;

#include <Wt/WServer>

using namespace Wt;

namespace Wt {
  class WLineEdit;
  class WText;
}

/*! \class HelloApplication
 *  \brief The 'hello' application modified to use QtCore
 * 
 * A sample application that uses objects from the QtCore library.
 */
class HelloApplication : public WQApplication
{
public:
  HelloApplication(const WEnvironment& env, WServer &server);

  void doGreet(const QString&);

  virtual void create();
  virtual void destroy();

private:
  WLineEdit *nameEdit_;
  WText     *greeting_;

  QtObject  *qtSender_, *qtReceiver_;

  void propagateGreet();

  WServer &m_Server;
  std::string m_SessionId; //since we'll only write this once in the create() method, accessing it (reading it) is assumed to be thread safe. we are on a different thread when we access it via server.post()

  void updateGuiCallback(const QString &text);
};

#endif // HELLO_APPLICATION_H_
