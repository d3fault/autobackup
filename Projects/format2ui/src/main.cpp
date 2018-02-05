#include <QCoreApplication>

#include "format2ui.h"

//after writing this a bit, I think maybe it would be better if I called it a "slot method signature 2 ui" converter. a BusinessObject::someSlot(const QString &name, const QString &favoriteColor) eg could be converted to 2 UIVariables, that, for example on the widgets would show up as QPlainTextEdits but on CLI would show up as CLI args (unless we have "--interactive", like planned, in which case it'd be shown as an interactive cin/cout query. it's not too terrible to have my own "custom json" format for now as it greatly simplifies things
//TODOreq: writing the above comment made me realize that the "widgets" ui generator will be async (set up QPlainTextEdits and await for the widget to give Accepted/OK signal/button -- ___WHEREAS__ cli "--interactive" will be synchronous/blocking!! step by step we ask the user for each UIVariable. I need to make sure my code is compatible with BOTH! might be tricky, might be ez
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    format2ui business;
    business.beginFormat2ui("/home/user/text/Projects/format2ui/sample.input.2.json", "--qtwidgets"); //TODOreq: connect to business::e etc!

    return 0;
}
