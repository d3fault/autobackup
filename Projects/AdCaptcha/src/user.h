#ifndef USER_H
#define USER_H

#include <Wt/Dbo/Dbo>

class User;

class User : public Wt::Dbo::Dbo<User>
{
public:
    User();
};

#endif // USER_H
