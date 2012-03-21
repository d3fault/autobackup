#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
using namespace Wt;

class AnonymousBitcoinComputing : public WContainerWidget
{
public:
    AnonymousBitcoinComputing(WContainerWidget *parent = 0);
private:
    void zeroOutEachViewPointer();
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
