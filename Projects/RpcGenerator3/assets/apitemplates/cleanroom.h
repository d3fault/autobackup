#ifndef CLEANROOM_H
#define CLEANROOM_H

#include "i%API_NAME_LOWERCASE%.h"

class %API_NAME% : public I%API_NAME%
{
    Q_OBJECT
public:
    explicit %API_NAME%(QObject *parent = 0);
public slots:
%API_CALLS_DECLARATIONS%
};

#endif // CLEANROOM_H
