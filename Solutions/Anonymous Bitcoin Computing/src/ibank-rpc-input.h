#ifndef IBANKRPCINPUT_H
#define IBANKRPCINPUT_H

#include <QObject>

//a future version of this hypothetical rpc generator might parse a header file... but initially i'm going to specify all of the interface in an xml file. it also makes the parameter modification easier.

class IBank : public QObject
{
    Q_OBJECT
public slots:
    virtual void createBankAccount(const QString &username)=0; //this far in I already wonder what the parameters should be... for example, if a first parameter needs to be a request identifier (or a request object... but then the RPC auto-gen'd impl will depend on object code that the RPC server impl will use directly). username makes sense... but how will the auto-gen'd rpc impl know which signal/response goes with which slot/request??

    //ok this really boils down to 2 problems. getting the reply matching back up with the request.... AND getting the right appId. it all depends on what design I CHOOSE TO TAKE. there is no right/wrong way (there is only better and worse)
    //to help me understand/future-proof, i'm going to put the equivalent slot/request that i will be putting in my IAppDb (TODO: IAppDb only has createBankAccount etc and IAbcAppDb has all the ABC specific slots. AbcAppDb rpc server impl inherits both):

    //virtual void createBankAccount(const QString &username); //see, even on AbcAppDb, i have the same problem. but on appdb, it only boils down to one issue: getting the reply to match back up with the request. oh and ACTUALLY, we'd be broadcasting the information to everyone [else?] after/during? the reply. there is no appId in that context... but there is a wtId. HOWEVER, the wtId is not used to determine a sub-appdb like the appId is used to determine a sub-bank-account

    //so i want a future proof/decouple-able design that encompasses both use cases

    //-----BANK------
    //for bank rpc server impl, the AppDbClientsHelper always adds two uints to the beginning of the parameter list. the first is the appId and the second is the requestId. they are both signal'd back to the AppDbClientsHelper as is.
    //------APPDB------
    //for appdb rpc server impl, the WtFrontEndsClientsHelper could always add two uints to the beginning of the parameter list. the first is the wtId and the second is the requestId. they are both signal'd back to the WtFrontEndsClientHelper

    //the requestId will no doubt overflow (TODO: handle), but the appId and wtId identifiers probably never will
    //for broadcasting, we could have the rpc compiler generate duplicates of all the signals. one for regular reply mode and one for reply + broadcast mode

    //EDIT
    //if the AppDb ever splits in order to scale (lol good luck with that), then the Bank also needs to incorporate broadcast mode and it should use it for every single per-appId bank action

    //bank use case is simpler
    //but in the end, they are both very similar

    //in both cases, i think replying to the requestor FIRST before broadcasting is an ideal priority to improve responsiveness of user interactions. the user that makes a request will most likely be the first user to actually see a response... and so that response should have priority over the [probably not seen] broadcast of the same message


    //do they both need requestIds? is a wtId and appId enough?


    //for bank server, the fact that we're passing in the appId means that the rpc server impl is what will take care of selecting the sub-bank-account. this somewhat breaks the future-proof'ness i'm trying to create, especially with object generation + auto-level-caching.

    //OK I'VE FOUND THE PROBLEM (NO SOLUTION YET):
    //the bank server needs an appId to work on a sub-bank
    //and the appdb server needs a requestId/wtId (either really, they are the same) to work with the one and only db

    //meh the problem made better sense the first time i thought it up
    //AppDb RPC: ManyWts <---> 1x AppDb (many to one)
    //Bank RPC: ManyAppDbs <-----> ManyBanks (one to one)

    //this reminds me of database foreign key shits.
    //my head hurts

    //at the end of the day, you really just *need* to know who the request is from
    //that request id can be used as a sub-selection identifier (sub-bank) if the rpc implemenation chooses to do so
    //while this design is entirely sound for my first hypothetical prototype... it is not future proof for the object generation / auto-level-caching ideas. i'm thinking that in those cases, even the rpc server implementation is auto-generated. so the rpc implementation that i'm typing now doesn't need to theoretically be typed. seeing as i'm nowhere near there, i guess (famous last words) that it's ok to let my manual code typing be the selector of the sub-bank. the auto-generated objects (and associated auto-generated server impl code operating with said objects) just need to implicity have the concept of a sub-object. you might call a sub-bank an account... but this would just confuse the fuck out of you when you tried to use an account of a sub-bank. i guess it could just be account + sub-account (or appIdAccount and userAccount respectively), but at this point i'm just getting into terminology.
    //somewhere in the interface declaration, you could MAYBE label it many-to-one vs. one-to-one. here's some theory that might mean maybe i can't:
    //sure, for each appdb there is only ever 1 bank (a sub-bank)
    //and for each wt-front-end there is only ever 1 appdb
    //BUT, the [AUTO?]caching between the two is divided up differently
    //for bank RPC, there's 1x master db and 1x (unless appdb scales? then 1 verbatim/full copy on each) bank cache on the appdb
    //for appdb RPC, there's 1x master db (unless it scales.. then WTF?) and 1x appdb cache on the wt-front-end
    //if appDb scales, then each appDb will need to hold half of the domain of the available data... and each wt-front-end would need to hold the sum total of each of the data segments. maybe a union select or something would come in handy on the wt-front-end for selecting an object from the appdb cache on the wt-front-end
    //bank does not scale. you simply use a new bank for a new appId (if your existing one is slowing down). but shit, if the [MULTIPLE]appdb <--> bank requests for just one appId manage to bring the bank server to a crawl... needing it needs to scale... i _GUESS_ it could scale using the same domain partitioning mechanism that appdb wants to/tries to use. communicating inter-server automatically would also be a problem. a user would have X dollars on bank 1 and Y dollars on bank 2. except user Z is partitioned to only use bank 1? based on the sha1 of his username + some salt. whatever that sha1 + salting mechanism used, it needs to be the same on each appdb implementation so that that user always gets the right bank
    //IDFK.

    //application, and especially server, scalability is a huuuuuuuuuuge
    //if i could write an auto-scaling RPC generator..... ESPECIALLY IF I MANAGED TO ALLOW IT TO SCALE UP (not down. fuck down) WITHOUT GOING OFFLINE... PULLING THE EXISTING DATA, HALVING IT... NOTIFYING IT'S CLIENTS OF THE PARTITION... ETC......... i could be a billion dollars.
    //naw but seriously
    //a) that would be amazing
    //b) i could sell the software for gold (licenses or buyout)
    //shit'd probably be worth way more than ABC altogether


    //while i do think that kind of RPC generation utility would be HELLA FUCKING AMAZING BY ITSELF..... i don't like the data partition strategy i currently have thought up. it's a shit way to scale. it doesn't effectively/efficiently use all of the available resources. Nothing's to stop a bunch of users from overloading bank server 1 just because of what their usernames are. it could be completely unintentional on their part and the server is not 'elastic' (?????) enough to know to balance it out
    //this is where elastic cloud compute shit that amazon ec2 sells comes in i think
    //or whatever their scalable cloud-db shit is called

    //THAT, on the other hand... is a complete fucking jackpot.
    //a) my rpc generator (and therefore abc code?? what about the bank itself???) uses amazon auto-scaling db
    //b) i figure out distributed databases bettar (holy shit that's such a complex topic in itself) and become A BILLION DOLLARS selling the rpc generator + service (or they can use their own machines, idgaf)


    //scalability is always on my mind
    //not just with d3fault (though mostly with d3fault)
    //but also for regular server apps like this one. it still needs to scale horizontally
    //d3fault will scale in a circle and uniformly across the network
    //server scaling for a website is a bit different... but when it boils down to it a lot of the concepts are the same (for the lower levels that need to scale outward)

    //so... do i take YET ANOTHER TANGENT (YAT) or should i just code this shit to not be scalable?
    //pro: future proofing yourself + future server apps, makes rpc generator way more valuable. research can/would pour into d3fault
    //con: takes longer to make any fucking money

    //ok after looking into Amazon DynamoDb and SimpleDB (simpleDb cheap as fuck, scales less... but probably sufficient.....
    //......i've come to realize that both solutions suck
    //both of my 'databases' need custom C++ code alongside them. so while i _could_ offload the database to Amazon... i'd still have a bottlenck of the server with the code on it. i don't see any thing that mentions being able to run your own executables... their 'application hosting' page just says 'use these OTHER amazong web services to build your app ;-)' ... worthless.
    //the virtual private server solution from amazon kinda looks promising
    //but what would i even put on it? wt front-ends + the appdb? it does look like it might work... but it still has the issue of 'update push'. like if an appdb changes some shit and replies to a wt client... it still has to upate all the other wt clients of the broadcast. except that in all cases the db will be singular/shared (right?). there is no 'cache' db or 'master db'. there is just the db. so my solution is more efficient in the short term... but there's obviously scales better. that's the whole point of it.

signals:
    void bankAccountCreated(const QString &username);
};

#endif // IBANKRPCINPUT_H
