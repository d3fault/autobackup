#include <QtCore/QCoreApplication>

#include "gitunrollrerollcensorshipmachinecli.h"

//I hate this code, but unlike RpcGenerator it's going to be done relatively quick...

//To test the end results of this app:
//1) Run timestamp occurance rate analyzer and make sure the highest occurance rates are sane
//2) EasyTree_HASH_ the input and output, then compare the two results. The only thing missing should be the censored filepaths
//3) Record the starting point of execution, and make sure that none of the timestamps written in each revision are >= it (meaning we missed touching them)

//I've been up all night coding this, and although it seems to work, I am not happy. I am not happy solely because I don't trust that I did it right... but I've been analyzing it thoroughly and holy shit I think it fucking works! Sure it took like 2 hours to run xD but I mean.... WOWOWOWOWOW. Still haven't done the above 3 tests but those are for tomorrow :-)...

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    GitUnrollRerollCensorshipMachineCli cli;
    QMetaObject::invokeMethod(&cli, "parseArgsAndThenDoGitUnrollRerollCensoring", Qt::QueuedConnection);

    return a.exec();
}
