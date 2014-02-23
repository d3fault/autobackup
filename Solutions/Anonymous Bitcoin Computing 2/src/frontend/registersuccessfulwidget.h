#ifndef REGISTERSUCCESSFULWIDGET_H
#define REGISTERSUCCESSFULWIDGET_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class RegisterSuccessfulWidget : public WContainerWidget
{
public:
    RegisterSuccessfulWidget(const std::string &username, WContainerWidget *parent = 0);
    void setUsername(const std::string &username);
private:
    WText *m_Message;
};

#endif // REGISTERSUCCESSFULWIDGET_H
