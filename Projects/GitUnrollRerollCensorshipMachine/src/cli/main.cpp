#include <QtCore/QCoreApplication>

#include "gitunrollrerollcensorshipmachinecli.h"

//I hate this code, but unlike RpcGenerator it's going to be done relatively quick...

//To test the end results of this app:
//1) Run timestamp occurance rate analyzer and make sure the highest occurance rates are sane
//2) EasyTree_HASH_ the input and output, then compare the two results. The only thing missing should be the censored filepaths
//3) Record the starting point of execution, and make sure that none of the timestamps written in each revision are >= it (meaning we missed touching them)
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    GitUnrollRerollCensorshipMachineCli cli;
    QMetaObject::invokeMethod(&cli, "parseArgsAndThenDoGitUnrollRerollCensoring", Qt::QueuedConnection);

    return a.exec();
}
