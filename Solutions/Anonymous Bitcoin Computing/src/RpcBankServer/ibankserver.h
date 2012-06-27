#ifndef IBANKRPCOUTPUT_H
#define IBANKRPCOUTPUT_H

#include <QObject>

#include "bankservermessagedispenser.h"
#include "Messages/Actions/createbankaccountmessagedispenser.h"

//i was confused for a minute on why this was in the rpc shared folder... but it's because "bankserverhelper" (the auto generated rpc client code) will implement this interface as well
//hmm actually this has implications on my message recycling. do i pass in a QString &username or do i fill out an ArgObject before making the request?

//a future version of this hypothetical rpc generator might parse a header file... but initially i'm going to specify all of the interface in an xml file. it also makes the parameter modification easier.
//the BankServerHelper code generated that is located on the RPC client and is used by the RPC client's code implements this, and so does your custom Bank Server implementation on the RPC Server

class IBankServer : public QObject
{
    Q_OBJECT
public:
    IBankServer() { m_BankMessageDispenser = new BankServerMessageDispenser(); }
    BankServerMessageDispenser *messageDispenser() { return m_BankMessageDispenser; }
private:
    BankServerMessageDispenser *m_BankMessageDispenser;
public slots:
    virtual void createBankAccount(CreateBankAccountMessage *createBankAccountMessage)=0; //this far in I already wonder what the parameters should be... for example, if a first parameter needs to be a request identifier (or a request object... but then the RPC auto-gen'd impl will depend on object code that the RPC server impl will use directly). username makes sense... but how will the auto-gen'd rpc impl know which signal/response goes with which slot/request??

    //ok this really boils down to 2 problems. getting the reply matching back up with the request.... AND getting the right appId. it all depends on what design I CHOOSE TO TAKE. there is no right/wrong way (there is only better and worse)
    //to help me understand/future-proof, i'm going to put the equivalent slot/request that i will be putting in my IAppDb (TODO: IAppDb only has createBankAccount etc and IAbcAppDb has all the ABC specific slots. AbcAppDb rpc server impl inherits both):

    //virtual void createBankAccount(const QString &username); //see, even on AbcAppDb, i have the same problem. but on appdb, it only boils down to one issue: getting the reply to match back up with the request. oh and ACTUALLY, we'd be broadcasting the information to everyone [else?] after/during? the reply. there is no appId in that context... but there is a wtId. HOWEVER, the wtId is not used to determine a sub-appdb like the appId is used to determine a sub-bank-account

    //so i want a future proof/decouple-able design that encompasses both use cases

    //-----BANK------
    //for bank rpc server impl, the AppDbClientsHelper always adds two uints to the beginning of the parameter list. the first is the appId and the second is the requestId. they are both signal'd back to the AppDbClientsHelper as is.
    //------APPDB------
    //for appdb rpc server impl, the WtFrontEndsClientsHelper could always add two uints to the beginning of the parameter list. the first is the wtId and the second is the requestId. they are both signal'd back to the WtFrontEndsClientHelper

    //the requestId will no doubt overflow (TODO: handle), but the appId and wtId identifiers probably never will
    //for broadcasts, we could have the rpc compiler generate duplicates of all the signals. one for regular reply mode and one for reply + broadcast mode. note, there are no cases where i want to use broadcast mode (not to be confused with a broadcast itself. i should probably change the wording. broadcast mode = send to every client vs. broadcast = a response that was server initiated) on the server. oh fuck i think it even effected my design thoughts. re-reading that sentence says: "broadcasts (server initiated updates) uses reply + broadcast mode slot"
    //ok fixed, now the broadcast/updates are called signals

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
    //d3fault will scale in a circle (lol wut?) and uniformly across the network
    //server scaling for a website is a bit different... but when it boils down to it a lot of the concepts are the same (for the lower levels that need to scale outward)

    //so... do i take YET ANOTHER TANGENT (YAT) or should i just code this shit to not be scalable?
    //pro: future proofing yourself + future server apps, makes rpc generator way more valuable. research can/would pour into d3fault
    //con: takes longer to make any fucking money

    //ok after looking into Amazon DynamoDb and SimpleDB (simpleDb cheap as fuck, scales less... but probably sufficient.....
    //......i've come to realize that both solutions suck
    //both of my 'databases' need custom C++ code alongside them. so while i _could_ offload the database to Amazon... i'd still have a bottlenck of the server with the code on it. i don't see any thing that mentions being able to run your own executables... their 'application hosting' page just says 'use these OTHER amazong web services to build your app ;-)' ... worthless.
    //the virtual private server solution from amazon kinda looks promising
    //but what would i even put on it? wt front-ends + the appdb? it does look like it might work... but it still has the issue of 'update push'. like if an appdb changes some shit and replies to a wt client... it still has to upate all the other wt clients of the broadcast. except that in all cases the db will be singular/shared (right?). there is no 'cache' db or 'master db'. there is just the db. so my solution is more efficient in the short term... but there's obviously scales better. that's the whole point of it.

    //ok so ec2 might work
    //basically all my designing is stupid + pointless
    //i'd just use a Wt front-end + MAYBE a qt/thread separated data channel for accessing the aws db shit. or maybe i'm just fine using the Wt postgres/simplexml built in shit for everything? The Bank, The AbcAppDb, and the Wt usernames?
    //all that on one OpenBSD virtual machine that i then upload and.... is that it?
    //am i overcomplicating the FUCK out of this?
    //i think i might be. i don't want to depend on amazon servers...
    //...but i also really don't need to... per se

    //sure, i'd have to use my real name initially (well not really... fake identity works)
    //i just think aws might get all like 'wtf' and do better identity checking than most places...
    //all i'd _REALLY_ need to do is periodically log into the shit through tor + vpn (because tor would look suspicious i bet) and upload a new stack of 'me-payout-keys'. the bitcoin thread (as i definitely couldn't use the wt thread for this) would periodically pay out all profits to me... etc (and perhaps even more. have a MAX_STORED and a MAX_USER_PAYOUT (not a max-me-payout. it's for the user per day or something. but idk that idea kinda sucks. having a MAX_STORED doesn't sound too bad but it'd mean that some large payments couldn't be instant. i'd have to send funds to myself. i guess i could have a list of 'to-pay-back-so-i-can-do-a-large-payout' preloaded keys so i could just do a regular bitcoin payment to it from wherever, without ever touching the ec2. ec2 detects the payment, knows that it's from the to-pay-back queue (TODO: should definitely be a queue... no point checking all of them, and i control when they are put in anyways)

    //and all i'd REALLY need to do to verify my security, and that i'm still up and running... is to check the amount that a given key has received. i don't need to ever touch the ec2 to do this. tor + bitcoin works (or hell, even the public bitcoin db would suffice the majority of the time)

    //cons: short lived tor connections *might* associate my identity/moneys with short-lived bitcoin connections...
    //solution: a long-term bitcoin node that i connect through would solve that

    //also, i'm unsure of amazon's policies on bitcoin. i guess also tor... since the bitcoin manual says you should always run bitcoin through tor.
    //it wouldn't be a lot of bitcoin bandwidth or even tor bandwidth... mostly just https connections and crunching of bitcoins to see if they're valid etc

    //the scalability of this solution is theoretically similar. it has a ceiling, whereas my approach wouldn't... but that ceiling is pretty damn high.
    //the 20ghz 7gb ram (??? bandwidth ???) would be $475.20/mo if i pay after per month...
    //...or $288/mo if i pay $712 up front (security deposit?) and sign a 1 year contract

    //the differences between the pre-paid and pay-as-you-go plans for the small/default is night and day
    //as you go: $57.60/mo - no security deposit
    //prepay (light): $28.08/mo - $69 security deposit + 1yr contract
    //prepay (medium): $17.28/mo - $160 sec d + 1yr
    //there's also a heavy

    //prepaid is definitely the way to go for ec2/aws to be worth it

    //tl;dr: idfkwtf to do
    
    
    
    
    //hmm i'm thinking now maybe each Wt is a node to a Wt-username-db + AppDb Cache all in a single CouchBase + Memcached on each
    //i guess some can be failover but ideally i want every node utilized. hopefully there's that mode + auto-failover
    //The AppDbs can/will/do have their own CoucheBase + Memcached... even if it's just 1 node/db during the beginning. Scaling Problems = Fixed. Fuck Amazon
    
    //so basically i do a QDataStream into b64 and then use that as the 'value' on the CouchBase put
    //the Key is 
    //upon entry, you get Key "INDEX" and that gives you a nigger samwich
    
    //fuck i'm lost
    //ok,
    //all of the following would, in an ideal rpc generator (i almost said 'world'), be auto-generated (seriously though, you can write software to control... over a cluster of computers (or just one, except that we also want to expose that interface to the users, so multiple is better (AWS front-end on amazon.com))... how many nodes of a separate cluster of computers... a user has. i'm talking auto-scaling for specific users, the automatic deployment of binaries over a network. automated paying, etc. of course the RPC Generator fits nicely into the mix. used alone, the rpc generator is a badass tool. used with automation (auto-scaling of either of the Back-End's, including the renting out of such a service (it is very useful to me on it's own as well)), it is a think to be feared)

    //re: AWS ^
    //we can supply them with this header and tell them to write code (just 1 object... header/source) that implements it. they submit it, we make sure the header hasn't changed, then we compile it (security issues? vm?) and run it on our auto-generated rpc shit with auto-scaling dht/infrastructure

    //"1) you define an interface in xml/web-gui \n2) you implement the given header and upload the created object \n3) we compile it in with our proprietary(rofl) rpc server implementation"

    //you know, i feel in this situation a proprietary solution is actually better. but then again, probably only for me/business. but it's like an order of magnitude better. by giving your opponents the code, you give them the infrastructure that does not need to be distributed to directly compete with you
    //i'm not factoring in the user's rpc server impl object licensing, which may or may not need exceptions in the (proprietary? free software?) sever code's license (should i choose to distribute it)

    //so yes i want ABC to be gpl
    //but can i call it gpl if it uses generated code and the code generator is not included / free software?
    //i don't see why not so long as the generated code is gpl'd...
    //and to be honest, it would be pretty damn easy to make an rpc generator just by looking at the source and recognizing it's patterns. i'm about to do it right now/soon



    //re: all the above shit. I'm going with CouchBase, fuck coding my own and fuck paying amazon. i want to sell/rent servers too
    //con of CouchBase: json necessary for object serialization. NOT A BIGGY. was looking forward to QDataStream shit (and will on my own version, should i make one. but right now i need to prioritize launching and making one takes too long for that) oh well.
signals:
    void initialized();
    void d(const QString &);

    //TODO: these should become signals on the CreateBankAccountMessage... and RpcClientsHelper should set up the connections for me so idgaf when using them
    /*void createBankAccountCompleted(const QString &username);
    void createBankAccountFailedUsernameAlreadyExists(const QString &username);
    void createBankAccountFailedPersistError(const QString &username);*/
};

#endif // IBANKRPCOUTPUT_H
