%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#ifndef %API_NAME_UPPERCASE%_H
#define %API_NAME_UPPERCASE%_H

#include "i%API_NAME_LOWERCASE%.h"

class %API_NAME% : public I%API_NAME%
{
    Q_OBJECT
public:
    explicit %API_NAME%(QObject *parent = 0);
public slots:
%API_CALLS_DECLARATIONS%
};

#endif // %API_NAME_UPPERCASE%_H
