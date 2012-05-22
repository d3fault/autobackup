#ifndef AUTOGENERATEDRPCCLIENT_H
#define AUTOGENERATEDRPCCLIENT_H

#include <QObject>

//I guess it would be wide to decide lol sppner lul drucks
//i guess it would be wise to decide oh fuck what was i talking about
//it would be wise to decide how much functionality i want the first rpc prototypte to have
//i think the auto-object-caching is out of the question... though research and documentation/comments should be littered throughout the source as i stumble upon scenarios that would relate to it, because it is something i would definitely want to put in a future revision
//the target goal for me right now is just a methods-that-use-built-in-Qt-Object-parameters defined interface (so parameters are string/literal-specified) and a hardcoded server that implements said interface (it will always be hardcoded. it is the "business logic"). i will also have a [also-hardcoded, but wouldn't be in the prototype] AutoGendRpcClientsHelper, which uses string replacing mechanisms for the defined interface values. (method signatures are parsed in as xml structure). for now, our brain will be the parser. i should still copy/paste the hardcoded (but would be auto-generated in a prototype) Interface pure virtual class that is generated from the parsing of the interface xml data. the rpc server / always-typed-by-user "business logic" of the RPC (which i HOPE to use/abstract more to use object caching in the future (BUT NOT NOW)) Connection (Client/Server relationship. this is the bank itself performing modifications. it responds using the interface it implements to handle messages. err need to think that out more.

//all of the #2 types of race condition detection can be done in autogenerated code (in the auto-generated Rpc Client which we will be hardcoding) only if we abstract the object itself from the invokation of it. We'd also probably need a generic Message (that our interface inherits. that might already be what i'm doing idk yet)
//the bank/RPC server doesn't even have the concept of a that race condition, because it is the final source of data (oh god i'm not getting into data passing am i? could both caches and master sources be combined more elaborately? as in both the appdb being a master db for wt front-end _AND_ bank server being the master db for appdb. could they be generically hooked to each other so that no AppLogic code is needed? that makes no sense. but i guess you could say in a way just the symbolizing of which slot that the Wt-FrontEnd <--> AppDb interface defines connects to what signal that the AppDb <--> Bank Server combines in some xml data like if you include other interfaces in an App (AppDb,Wt-FrontEnd,BankServer are each respectively Apps. and each one of them either implements or USES an interface (which means the interface is the defining point of contact). some Apps just implement a server interface. some link to the client rpc (auto-generated code) for that server. some will both implement an rpc server which servers as a master source for a different rpc interface specific/exposed-to-some-third-server to that appdb/middle object. the rpc server has specific functionality that periodically triggers the rpc client code to the bank/first server). abstracting the object logic into the interface would suffice, but that sounds akin to rebuilding all of C++ in xml markup

//i think the bank server rpc definition as well as the appdb rpc definition would be somewhat similar in the methods they use to modify their master db/object. there will probably be specifics for each, but i'm betting that they have a common base list-of-object-type operations similarity
//i would be right. the object is used in the rpc server's user-written code that implements the interface however.
class AutoGeneratedRpcClient : public QObject
{
    Q_OBJECT
public:
    explicit AutoGeneratedRpcClient(QObject *parent = 0);

signals:

public slots:

};

#endif // AUTOGENERATEDRPCCLIENT_H