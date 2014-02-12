class PseudoCode //My head hurts
{
    inline void PseudoMethod()
    {
        //If user-specific doc isn't already in "locked attempting to get a key from PageY of setN using UUID-per-key-request" mode
        {
            //setN = rand() % 1000
            //Get bitcoinKeySet[setN]_Page to see what page it's on (we don't use the 'one key always' strategy this time) and to see if it's locked
            //#ejfjdi83939#
            //If bitcoinKeySet[setN]_Page isn't in "fillingNextPage with UUID-per-fill starting from pageZ of hugeBitcoinKeyList" mode
            {
                //CAS-Swap-Lock attmempt a user-specific doc into "attempting to get a key from PageY of setN using UUID-per-key-request" (TODOreq: might be full by the time we get there, and recovery might see that it's full also (in both cases we PageY++ until the UUID-per-key-request is seen))
                //If above cas swap succeeds
                {
                    //#ooejeuiwuei# <- recovery entry point

                    //m_FirstTime0 = true; //note the jump goes just below this setting to true

                    //#sdofuou38# starts below, but the flow from here is BASICALLY this, (also wherever UUID CAS-swap is involved):
                    //while(uuidNotInDocGettedForCasSwapAnyways) //TODOreq: also checking there is one key spot remaining each time as well!
                    //{
                        //cas-swap-attempt-inserting-uuid <-- can fail or succeed and we STILL do the while() statement evaluation
                    //}
                    //Then (relative to that while pseudocode): we have the UUID to proceed foward with, but we do not know whether or not we are needed, so we proceed forward lightly/safely (can determine that we aren't needed if the cas swap fails and uuid shows up AFTER first get (neighbor is active))

                    //#sdofuou38#

                    //If(!m_FirstTime0)
                    {
                        //CAS-Swap attempt to add our username/UUID-per-key-request (aka claim a key) on variable page Y (Y may have been incremented since what we locked-it||saw-it-locked as). Y is member variable
                    }

                    //If(the previous CAS-Swap-key-claim failed || m_FirstTime0 == true)
                    {
                        //Get bitcoinKeySet[setN] in order to retrieve it's CAS, check for relevant-UUID, and see if room left
                    }

                    //m_FirstTime0 = false;

                    //Analyse bitcoinKeySet[setN] for UUID presence
                    //If (and when) relevant UUID-per-key-request is seen in doc
                    {
                        //##COMMON GET KEY CASE (after first skipping this if _once_, is uncommon but could happen that we get here on first try)### We got a key
                        //CAS-Swap-Unlock attempt user-specific doc to "just got key = theBitcoinKey", which must be marked "Confirmed+Done" before going into locked mode again
                        //If above CAS swap succeeds
                        {
                            //present key to user
                        }
                        else
                        {
                            //PRETTY SURE we just present key to user regardless of the cas-swap failing, as that just indicates a recovery neighbor beat us to that (the key chosen would have been identical!)

                            //Old
                            //#kue93ui#
                            //we got beat by recovery? doesn't necessarily mean this node is down, could just be slow? present key to user, or will it be the wrong/stale key possibly? Inclined to just say "do nothing + tell user to retry in a few seconds", but am unsure
                        }
                    }
                    else //Analyse bitcoinKeySet[setN] for space left
                    {
#if 0
                        //Someone else beat us to it (or neighbor recoverer inserted our own UUID), so check both for that UUID and that there is a key left on that page
                        //If the UUID was inserted by a neighbor recoverer
                        {
                            //Do we take that detected inserted UUID/key-claim and proceed forward with recover as if it was ours (LIGHTLY/SAFELY, NOT OVERWRITING 'stuff' (TODOReq))???? I think yes! Because we can fail just after the cas-swap-key-claim!

                            //Had:
                            //Do nothing, notify user to try again in a few seconds (MAYBE present the key? probably not though)
                        }
                        else
#endif
                        //#####COMMON CASE#### UUID not seen [in first get (would have been weird neighbor recover race condition otherwise)]
                        //If there is a space left
                        {
                            //GOTO #sdofuou38#
                        }
                        else //no keys left on page
                        {
                            //PageY++, then attempt to get it.
                            //If next page doesn't exist:
                            {
                                //Get hugeBitcoinKeyList_Page (pageZ) to see what page it's on and to see if it's locked
                                //If not in "locked fillingNextPage with UUID-per-fill on hugeBitcoinKeyList[pageZ]"
                                {
                                    //Attempt: bitcoinKeySet[setN]_Page CAS-swap-locked into "fillingNextPage with UUID-per-fill on pageZ of hugeBitcoinKeyList" mode (TODOreq: marking unusable [but light recovery proceeds i guess, and recovery starts from that page specified but doesn't rely on it being accurate])
                                    //If above CAS-swap succeeds
                                    {
                                        //#iopqjiw848#
                                        //Get hugeBitcoinKeyList[pageZ] attempt
                                        //If hugeBitcoinKeyList[pageZ] exists
                                        {
                                            //m_FirstTime1 = true; //note the jump goes just below this setting to true

                                            //#jdkiru939#
                                            //If(!m_FirstTime1)
                                            {
                                                //CAS-swap attempt hugeBitcoinKeyList[pageZ] claiming a range for the next page
                                            }

                                            //If(the previous CAS-Swap-claim-range failed || m_FirstTime1 == true)
                                            {
                                                //Get hugeBitcoinKeyList[pageZ] in order to retrieve it's CAS, check for relevant-UUID, and see if room left
                                            }

                                            //m_FirstTime1 = false;

                                            //If the UUID-per-fill is on hugeBitcoinKeyList[pageZ] (first attempt = race condition/neighbor, 2nd+ attempt = normal, but we account for all attempts!)
                                            {
                                                //Lightly create page using the range obtained from hugeBitcoinKeyList[pageZ] via an LCB_ADD-accepting fail (LCB_ADD fail means recovery already created it)
                                                    //NOPE: Get doc if the LCB_ADD fails -- ACTUALLY probably not necessary since recovery used our same UUID in "fillingNextPage with UUID-per-fill" (TODOreq: each CAS-swap-fail of BOTH levels/tiers of docs needs to check that the UUID wasn't just added (recovery race condition). If that happens, we... uhhh... maybe just stop and notify user to try again?? Or.... I don't know tbh.... ffff. Reminds me of #kue93ui# = can happen, but what do i do when it does?

                                                //CAS-swap-unlock bitcoinKeySet[setN]_Page into normal 'current page = Y' mode
                                                //If above CAS-swap succeeds we continue on no matter what so ignore two branches below
                                                {
                                                    //Nothing to see here, common case where we unlocked
                                                }
                                                else
                                                {
                                                    //neighbor recovery 'caught up' beat us to the bitcoinKeySet[setN]_Page unlocking..... which means.... NOTHING. We don't give a fuck, do nothing! HOWEVER it might mean that the UUID-per-key request is already being filled? MIGHT != GUARANTEED, since any UUID-per-key request could be handling that "next page creation"
                                                }

                                                //Refill/next-page-creation is complete [unless db error], so just GOTO #sdofuou38#
                                                //It is not needed to also change PageY in our locked doc (our in-memory one referenced in #sdofuou38# is already incremented), so don't (TODOreq: recovery handles that just fine)
                                            }
                                            else
                                            {
                                                //UUID not on hugeBitcoinKeyList[pageZ]

                                                //If there is space left on that hugeBitcoinKeyList[pageZ]
                                                {
                                                    //GOTO #jdkiru939#
#if 0 //old/merged
                                                    {

                                                    }
                                                    else
                                                    {
                                                        //as already mentioned, check that UUID we're trying to put in wasn't put in by neighbor recoverer. TODOreq: doing that UUID verification (at _EVERY_ cas swap fail) requires an additional LCB_GET, and it should be noted that that LCB_GET should also use the same exponential backoff timings (not it's own or incrementing it, just shared)
                                                        //If it wasn't put in by a neighbor recoverer
                                                        {
                                                            //CAS-swap attempt hugeBitcoinKeyList[pageZ] claiming a range for the next page using exponential backoff all in sexy loop, aka GOTO #jdkiru939#
                                                        }
                                                        else
                                                        {
                                                            //do nothing and notify user to check other tab???? Indicates multiple tabs (2nd one on recovery code path)I guess... idfk. That might also be solution to the #kue93ui# problem (and this might be the analogy/similar mentioned in "already locked to fill-next-page, do light recovery"
                                                        }
                                                    }
#endif
                                                }
                                                else
                                                {
                                                    //Update hugeBitcoinKeyList_Page to now point to the next page, regardless of whether or not it exists //TODOreq: set to non-existence page (to be added) in order to make the server not crash from out of memory when all keys run out and every Wt client then tries to get hugeBitcoinKeyList to determine if it's empty :-P
                                                    //^^^UNSURE if cas-swap needed, or just simple 'set' is sufficient(/SAFE)

                                                    //Because we are still doing a refill, attempt to get that next hugeBitcoinKeyList[pageZ], so aka GOTO #iopqjiw848#

#if 0 //used a goto instead (read comments to understand)
                                                    //If it exists
                                                    {
                                                        //Do traditional cas-swap-range-claim on it retrying with exponential backoff [all in sexy loop], aka GOTO #jdkiru939# [this is our first time going there if this code path is taken (won't be a go-BACK), it should be noted]

                                                        //TO DONEreq (used a goto lol): so what if by the time we get around to our hugeBitcoinKeyList[pageZ], the entire next pageZ is already taken!?!?!? rare so fuck that shit not accounting for it (famous last words) yet (may already even be accounted for because of the "aka goto", i have no idea)
                                                    }
                                                    else
                                                    {
                                                        //same erroring out as #oiei38849#, this one being a rarer race condition additionally
                                                    }
#endif
                                                }
                                            }
                                        }
                                        else
                                        {
                                            //#oiei38849# <- nobody jumps here, but an #ifdef'd out comment refers to it xD
                                            //no pages left on hugeBitcoinKeyList, so we do nothing and need to alert ourselves (email/etc) to do a manual LCB_ADD of next hugeBitcoinKeyList[pageZ] (that LCB_ADD failing doesn't make sense)
                                            //notify user to try again later.... waaaaaaay later.... like tomorrow or something :). I could even tell them to notify me (maybe one is within yelling distance (ex: "say this: 'OOGA BOOGA I AM THE HUGE BITCOIN KEY REFILL NOTIFICATION OUTSOURCED TO ONE OF YOUR SLAVE COCKSUCKERS'. I'll know what it means.").

                                            //OLD PSEUDO-PATH BEFORE REALIZING hugeBitcoinKeyList _NEEDS_ pages (without them, a fail after a successful cas-swap-claim of range could be forever forgotten if we manually CAS-swap updating the hugeBitcoinKeyList with all new keys. That fail cannot be recovered from without pages -_-
                                            //(it's up to us to keep track of the old hugeBitcoinKeyList somewhere not-in-the-system (perhaps a text file ;-P?)). We can do said manual CAS-swap at any point in time before it runs out of key ranges, and any key ranges not yet claimed at the time of our successful cas-swap can be safely put into a future pool. We know they haven't and won't be used (and they definitely won't EVER (excluding quantum computers etc) unless we make sure to re-merge them in later)
                                        }
                                    }
                                    else
                                    {
                                        //This indicates that another neighbor is already active and on top of it. Do nothing and tell user to try again in a few seconds
                                    }
                                }
                                else
                                {
                                    //GOTO #iopqjiw848# after filling in variables from locked doc, but need to make sure it can
                                }
                            }
                            else
                            {
                                //GOTO #sdofuou38#
                            }
                        }
                    }
                }
                else //cas swap fails
                {
                    //another web browser 'tab' requested a key at the same time, so error out on this one explaining that to user. MIGHT also make sense to do light/safe recovery, but eh probably not, just give the error and stop (user can re-initiate (handling recovery) if they want)
                }
            }
            else //already locked to fill-next-page, do recovery [by just doing the same shit rofl]
            {
                //GOTO #ooejeuiwuei# after filling in variables retrieved by locked doc (the one that got us here)
            }
        }
        else //already locked to get-bitcoin-key, do light recovery
        {
            //GOTO goto #ejfjdi83939# after filling in the page variables just above it (that we extracted from locked doc)
#if 0
            //series of light/safe recovery events which might CAUSE #kue93ui#

            //100ms sleep, see if now unlocked from get-bitcoin-key

            //If still locked
            {
                //Recovery = _I THINK JUST_ goto #ejfjdi83939# after filling in the page variables just above it (that we extracted from locked doc), but could be wrong! I _THINK_ my code is good and will 'stop' when detecting it's repeating/duplicating a neighbor recoverer or the first driver, BUT err am unsure of that (if it isn't the case, I want it to be the case!). It seems 'stupid' to me that I even lock it to begin with and then still proceed forward, but maybe it isn't? TODOreq: that would mean the final cas-swap-unlock needs to be able to fail (find + confirm, it may already be true :-P), knowing it was a neighbor recoverer/driver that did it (and he needs to be only one ABLE to do it (just like user-account)). This reminds me much of recovery possy, except this is all event driven (and giving up when detecting dupe, whereas recovery possy doesn't but should ^_^)
            }
            else
            {
                //Idfk, tell user to try again in a few seconds (it should then say "use this key first" :-P)? Or maybe we can NOW get that same key and serve it to end user? Idk
            }
#endif
        }
    }
}; //so much for KISS, but I think this one is still superior to the other attempt :)

//I think abstracting this "two tier'd" lock-2-get-key-from-set -> lock-2-refill-set thing and making it "N tier'd" with a specifiable (exponential ratio makes sense) amount of BLAH (set-size) at each tier is perhaps the key to having a dynamic as fuck ultra scalable journaled + distributed filesystem.... but could be wrong and it could just be some other handy tool thing...
//^^I think I have to solve all of the same hard problems for 2-tiers, so I might as well do n-tier'd, because that probably just requires a bit of better organization... and I feel inclined (premature optimization? most likely yes :P) to do it, despite not even being able to wrap my head around what _IT_ even is!

//Single consumer locks himself to get a single item out of a shared buffer (with many neighbor buffers), simply gets item in most normal cases, but when needed locks that buffer and refills it from yet a larger buffer, so on and so forth N times until ultimately there's one giant buffer that fills up the smaller ones that is rarely changed/accessed

//I surely don't NEED n-tier'd at this point in time, but [...]

//"All my ifs have elses, so I must not have missed any error cases ;-P" -Me/now (there appears to be a shred of sound logic behind it (but not enough for confidence, hence the wink+tongue), and/but it does appear to be true for this pseudo-code)

//Exponential back-off CAS swapping of refilled-from-exponentially-larger buffer (the refill process also uses exponential backoff ;-P? my head hurts). If the CAS-swapping on the smallest size doc fails too frequently, you need more of them. (had: "it is too small", but that makes no sense). Can make them smaller too, but you'd need to be good at math to find the sweet spot without using trial and error :-P (idgaf)

//The trick (that I'm still getting the hang of) is being able to code knowing that each step could fail or be beat by a neighbor recoverer doing the exact same thing, but that that doesn't necessarily indicate that you should stop going forward (sometimes does, sometimes doesn't). In a lot of cases, lcbOpFail is disregarded and you continue as normal (sometimes you just stop). I'm so confused
