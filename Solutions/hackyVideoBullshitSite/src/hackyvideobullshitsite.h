#ifndef HACKYVIDEOBULLSHITSITE_H
#define HACKYVIDEOBULLSHITSITE_H

#include <Wt/WServer>
#include <Wt/WApplication>
using namespace Wt;

#include <QString>

class HackyVideoBullshitSite
{
public:
    int startHackyVideoBullshitSiteAndWaitForFinished(int argc, char* argv[]);
private:
    static WApplication *hackyVideoBullshitSiteGuiEntryPoint(const WEnvironment &env);
    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        return inputString.endsWith("/") ? inputString : (inputString + "/");
    }
};

#endif // HACKYVIDEOBULLSHITSITE_H
