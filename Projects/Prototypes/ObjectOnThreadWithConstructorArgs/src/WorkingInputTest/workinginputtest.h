#ifndef WORKINGINPUTTEST_H
#define WORKINGINPUTTEST_H

#include <QObject>

#include "workinginputtestbusiness.h"

#define Q_DECLARE_OBJECT_FOR_THREAD(asdf) ; //hack to make it compile? the generator will have stripped it from the pre-moc code by now (though the IDE would still underline it and bitch about it (can fix this, just like the forward declare below))

Q_DECLARE_OBJECT_FOR_THREAD(WorkingInputTestBusiness(int simpleArg1, QString implicitlySharedQString, QObject *parent = 0)) //call the actual macro (which is actually just a magic string that our qmake hack uses) to do the thread hackery :)

//This forward is only necessary for me to not get IDE errors. Compiling SHOULD still work, as the class itself is generated before a call to qmake/make. That the private member below is a pointer is also not necessary, but IS necessary only for using forward declares
class WorkingInputTestBusinessOnThreadHelper;

class WorkingInputTest : public QObject
{
    Q_OBJECT
public:
    explicit WorkingInputTest(int simpleIntArg1, QString implicitlySharedStringArg2, QObject *parent = 0);
    ~WorkingInputTest();
private:
    WorkingInputTestBusinessOnThreadHelper *m_WorkingInputTestBusiness;
signals:
    
private slots:
    void handleWorkingInputTestBusinessInstantiated(WorkingInputTestBusiness *workingInputTestBusiness);
};

#endif // WORKINGINPUTTEST_H
