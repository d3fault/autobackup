class PseudoCode //My head hurts
{
    inline void PseudoMethod()
    {
        //Get user-account doc and to store it's CAS for locking momentarily (if unlocked)
        //If user-specific doc isn't already in "locked attempting to get a key starting from PageY of setN using UUID-per-key-request" mode
        {
            //m_UserAccountBitcoinLocked = false;
            //setN = rand() % 1000
            //#ejfjdi83939#
            //Get bitcoinKeySet[setN]_CurrentPage to see what page it's on, and to see if it's locked (if jumping in via recovery/#ejfjdi83939#, we already know what page it's on, BUT we still want to check if it's locked)
            //If bitcoinKeySet[setN]_CurrentPage isn't in "fillingNextPage with UUID-per-fill starting from pageZ of hugeBitcoinKeyList" mode
            {
                //#ndjgir784#
                //if(m_UserAccountBitcoinLocked)
                {
                    //GOTO #sdofuou38#
                }
                //CAS-Swap-Lock attmempt a user-specific doc into "attempting to get a key from PageY of setN using UUID-per-key-request" (TODOreq: might be full by the time we get there, and recovery might see that it's full also (in both cases we PageY++ until the UUID-per-key-request is seen)). Save the output CAS for safe unlocking after we get the key
                //If above cas swap succeeds
                {
                    /*
                     * PSEUDO-PSEUDO
                     *
                     * skldjfsdk333:
                     *
                     * Get (save CAS)
                     * If UUID present: GOTO eouwerjsfd09328
                     * If Space Left: CAS Swap
                     * If CAS Swap Fail: GOTO skldjfsdk333
                     *
                     * eouwerjsfd09328:
                     *
                     * UUID definitely on either last GET'd or last STORE'd at this point. On store'd if the cas swap succeeded, otherwise on get'd
                     *
                    */

                    //#sdofuou38#

                    //Get-saving-CAS bitcoinKeySet[setN] Page Y

                    //If (and when) relevant UUID-per-key-request is seen on bitcoinKeySet[setN] Page Y
                    {
                        //#jjdieutit93749#
                        //##COMMON UUID SEEN CASE (after first skipping this if _once_, is uncommon but could happen that we get here on first try)### We got a key
                        //CAS-Swap-Unlock attempt user-specific doc to "just got key = theBitcoinKey", which must be marked "Confirmed+Done" before going into locked mode again
                        //If above CAS swap succeeds
                        {
                            //present key to user
                        }
                        else
                        {
                            //Yell at them for multiple tabs again, they purchased a slot probably (but it may have been recovery code that beat them to it). Tell them to retry in both cases (KISS). TODOreq: "Get Key" when already in "HaveKey" just... you know... gives them the key already haved!

                            //Overcomplicated:
                            //also k. TODOreq: cas swap might fail because slot fill aka buy takes presedence (this isn't true if bitcoin stuff is on it's own doc). we COULD still give them the key at this point, but it won't be in the db until they 'retry' (retry what? get add funds key?).... BUT they will get the same key no matter what so it is ok to present it to them and even let them start sending payments to it (BUT TODOreq it would probably be some gross hard to decipher error if i let them transition it to "NoKey" (creditting the amount), so I need to do the retry somewhere (forcing them to sounds simplest) before we get TOO far along)

                            //yep:
                            //PRETTY SURE we just present key to user regardless of the cas-swap failing, as that just indicates a recovery neighbor beat us to that (the key chosen would have been identical!)

                            //Old
                            //#kue93ui#
                            //we got beat by recovery? doesn't necessarily mean this node is down, could just be slow? present key to user, or will it be the wrong/stale key possibly? Inclined to just say "do nothing + tell user to retry in a few seconds", but am unsure
                        }

                    }
                    else //UUID not on bitcoinKeySet[setN] Page Y
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
                            //CAS-Swap attempt to add our username/UUID-per-key-request (aka claim a key) on variable page Y (Y may have been incremented since what we locked-it||saw-it-locked as) of bitcoinKeySet[setN]. Y is member variable and can be incrememented in various following code paths
                            //If the previous CAS-Swap-key-claim succeeded
                            {
                                //#####COMMON CAS-SWAP-KEY-CLAIM#######
                                //Get'd = Store'd (we _KNOW_ UUID is on store'd, but the code we jump to analyzes the get'd), then GOTO #jjdieutit93749#
                            }
                            else //CAS-Swap-key-claim failed
                            {
                                //GOTO #sdofuou38#
                            }
                        }
                        else //no space left on page Y
                        {
                            //PageY++, then attempt to get it. The reason we don't rely on _CurrentPage here is because locked/recovery could be pointing to an "old" page, and we need to try all pages from what they specified up to the current page (including creating a new page if no space empty on current page)
                            //If next page doesn't exist:
                            {
#if 0 //appears to be dupe/moved-way-up-top
                                //We MIGHT have already gotten bitcoinKeySet[setN]_CurrentPage previously, but if we did "GettingKey" recovery, then not necessarily! KISS = get it again fuck it (TODOoptimization) :)
                                //Get bitcoinKeySet[setN]_CurrentPage to see what page it's on, to see if it's locked, and to store it's CAS for locking momentarily (if unlocked)
                                //If not in "locked fillingNextPage with UUID-per-fill on hugeBitcoinKeyList[pageZ]"
                                {
#endif
                                //Get hugeBitcoinKeyList_CurrentPage to see what page it's on
                                //m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont = true; //baller
                                //Attempt: bitcoinKeySet[setN]_CurrentPage CAS-swap-locked into "fillingNextPage with UUID-per-fill on pageZ of hugeBitcoinKeyList" mode (TODOreq: marking unusable [but light recovery proceeds i guess, and recovery starts from that page specified but doesn't rely on it being accurate])
                                //If above CAS-swap succeeds
                                {
                                    //#iopqjiw848#
                                    //Get hugeBitcoinKeyList[pageZ] attempt in order to see if it exists, retrieve it's CAS, check for relevant-UUID, and see if room left (if the last two are false and true respectively, we try to cas-swap insert our UUID claiming a range)
                                    //If hugeBitcoinKeyList[pageZ] exists
                                    {
                                        //If the UUID-per-fill is on hugeBitcoinKeyList[pageZ] (first attempt = race condition/neighbor/recovery, 2nd+ attempt = normal, but we account for all attempts!)
                                        {
                                            //#hhjsbvu8748#
                                            //Lightly create bitcoinKeySet[setN] Page Y using the range obtained from hugeBitcoinKeyList[pageZ] via an LCB_ADD-accepting fail (LCB_ADD fail means recovery already created it)
                                                //NOPE: Get doc if the LCB_ADD fails -- ACTUALLY probably not necessary since recovery used our same UUID in "fillingNextPage with UUID-per-fill" (TODOreq: each CAS-swap-fail of BOTH levels/tiers of docs needs to check that the UUID wasn't just added (recovery race condition). If that happens, we... uhhh... maybe just stop and notify user to try again?? Or.... I don't know tbh.... ffff. Reminds me of #kue93ui# = can happen, but what do i do when it does?

                                            //CAS-swap-unlock bitcoinKeySet[setN]_CurrentPage into normal 'current page = Y' mode
                                            //If above CAS-swap succeeds we continue on no matter what so ignore two branches below, but it still needs to be a CAS-swap so we don't fuck up future changes (if this node/code is waaaaay behind)
                                            {
                                                //Nothing to see here, common case where we unlocked
                                                //GOTO #ndjgir784#
                                            }
                                            else
                                            {
                                                //neighbor recovery 'caught up' beat us to the bitcoinKeySet[setN]_CurrentPage unlocking..... which means.... NOTHING. We don't give a fuck, do nothing! HOWEVER it might mean that the UUID-per-key request is already being filled? MIGHT != GUARANTEED, since any UUID-per-key request could be handling that "next page creation"

                                                //TODOreq: neighbor recovery might have put it into fillingNextPage mode YET AGAIN in a race condition, so i don't think the jump below is safe.... ffffffffff. It is safe if the cas-swap succeeded though (i think (man i'm so lost because can't the neighbor/recovery then unlock it (oh wait that's how i am here))). We should maybe go to #ejfjdi83939# if the cas-swap fails?
                                            }

#if 0
                                            //possibly do something with PageY (now valid) before the GOTO, but maybe nothing since it's member variable?
                                            //GOTO #ndjgir784#
#endif
#if 0 //OLD'ish:
                                            //TODOreq: getting here did mean the bitcoinSet was locked, but the user-account itself may or may not have been locked, so i think the following jump is incorrect!
                                            //Refill/next-page-creation is complete [unless db error], so just GOTO #sdofuou38# -- ACTUALLY NOW THINKING: #ndjgir784#
                                            //It is not needed to also change PageY in our locked doc (our in-memory one referenced in #sdofuou38#/#ndjgir784# is already incremented), so don't (TODOreq: recovery handles that just fine)
#endif
                                        }
                                        else
                                        {
                                            //UUID not on hugeBitcoinKeyList[pageZ]

                                            //If there is space left on that hugeBitcoinKeyList[pageZ]
                                            {
                                                //CAS-swap attempt hugeBitcoinKeyList[pageZ] claiming a range for the next bitcoinKeySet[setN] page
                                                //If the previous CAS-Swap-claim-range succeeded
                                                {
                                                    //Get'd = Store'd (we _KNOW_ UUID is on store'd, but the code we jump to analyzes the get'd), then GOTO #hhjsbvu8748#
                                                }
                                                else //CAS-swap failed
                                                {
                                                    //GOTO #iopqjiw848#
                                                }
#if 0 //old/merged
                                                {

                                                }
                                                else
                                                {
                                                    //as already mentioned, check that UUID we're trying to put in wasn't put in by neighbor recoverer. TODOreq: doing that UUID verification (at _EVERY_ cas swap fail) requires an additional LCB_GET, and it should be noted that that LCB_GET should also use the same exponential backoff timings (not it's own or incrementing it, just shared)
                                                    //If it wasn't put in by a neighbor recoverer
                                                    {
                                                        //CAS-swap attempt hugeBitcoinKeyList[pageZ] claiming a range for the next page using exponential backoff all in sexy loop, aka GOTO #iopqjiw848#
                                                    }
                                                    else
                                                    {
                                                        //do nothing and notify user to check other tab???? Indicates multiple tabs (2nd one on recovery code path)I guess... idfk. That might also be solution to the #kue93ui# problem (and this might be the analogy/similar mentioned in "already locked to fill-next-page, do light recovery"
                                                    }
                                                }
#endif
                                            }
                                            else //no space left on pageZ (but there might be a next page)
                                            {
                                                //++PageZ
                                                //if(m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont)
                                                {
                                                    //put PageZ into the db in _CurrentPage via cas swap
                                                }

                                                //GOTO #iopqjiw848#

                                                //TODOreq: the following says to update _CurrentPage, but I think we just want to ++PageZ member variable and loop around to trying it again? We do want to make _CurrentPage point to next (potentially non-existent) page at some point though!!!
                                                //^How the fuck do I differentiate between "++PageZ Only" and "++PageZ and store it in _CurrentPage", where both of them then go onto looping around and trying to actually utilize PageZ. I guess it's just if PageZ was populated USING _CurrentPage (then it isn't recovery (recovery could be pointing to hella old PageZ)), but even then we don't know that _CurrentPage doesn't already point to PageZ+2... although I suppose CAS-Swap protects us against overwriting that?

                                                //Semi-old:
                                                //Update hugeBitcoinKeyList_CurrentPage to now point to the next page, regardless of whether or not it exists //TODOreq: set to non-existence page (to be added) in order to make the server not crash from out of memory when all keys run out and every Wt client then tries to get hugeBitcoinKeyList to determine if it's empty :-P
                                                //^^^UNSURE if cas-swap needed, or just simple 'set' is sufficient(/SAFE)

                                                //Because we are still doing a refill, attempt to get that next hugeBitcoinKeyList[pageZ], so aka GOTO #iopqjiw848#

#if 0 //used a goto instead (read comments to understand)
                                                //If it exists
                                                {
                                                    //Do traditional cas-swap-range-claim on it retrying with exponential backoff [all in sexy loop], aka GOTO #iopqjiw848# [this is our first time going there if this code path is taken (won't be a go-BACK), it should be noted]

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
                                        //If the "locked starting from page z" value isn't the page we just tried to get (that doesn't exist), we should 're-lock' it to the page that doesn't exist, for the sole reason of not running out of memory because every single bitcoin key request pulls the entire already-full hugeBitcoinKeyList page (pageZ-1). It is safe for both recovery and normal code to do this because recovery will have already checked for the UUID already in all the previous ones

                                        //#oiei38849# <- nobody jumps here, but an #ifdef'd out comment refers to it xD
                                        //no pages left on hugeBitcoinKeyList, so we do nothing and need to alert ourselves (email/etc) to do a manual LCB_ADD of next hugeBitcoinKeyList[pageZ] (that LCB_ADD failing doesn't make sense)
                                        //notify user to try again later.... waaaaaaay later.... like tomorrow or something :). I could even tell them to notify me (maybe one is within yelling distance (ex: "say this: 'OOGA BOOGA I AM THE HUGE BITCOIN KEY REFILL NOTIFICATION OUTSOURCED TO ONE OF YOUR SLAVE COCKSUCKERS'. I'll know what it means.").

                                        //OLD PSEUDO-PATH BEFORE REALIZING hugeBitcoinKeyList _NEEDS_ pages (without them, a fail after a successful cas-swap-claim of range could be forever forgotten if we manually CAS-swap updating the hugeBitcoinKeyList with all new keys. That fail cannot be recovered from without pages -_-
                                        //(it's up to us to keep track of the old hugeBitcoinKeyList somewhere not-in-the-system (perhaps a text file ;-P?)). We can do said manual CAS-swap at any point in time before it runs out of key ranges, and any key ranges not yet claimed at the time of our successful cas-swap can be safely put into a future pool. We know they haven't and won't be used (and they definitely won't EVER (excluding quantum computers etc) unless we make sure to re-merge them in later)
                                    }
                                }
                                else
                                {
                                    //This indicates that another neighbor (could be a different user) is already active and on top of it. Do nothing and tell user to try again in a few seconds (either that or try to implement complicated logic. fuck it)
                                }
#if 0 //appears to be dupe/moved-way-up-top
                                }
                                else
                                {
                                    //GOTO #iopqjiw848# after filling in variables from locked doc, but need to make sure it can
                                }
#endif
                            }
                            else
                            {
                                //TODOreq: ALSO weird race condition or possibly even failure of neighbor after page creation, we don't know that bitcoinKeySet[setN]_CurrentPage was updated (we don't know that it wasn't). Whoever created it and made us go to this race condition may have failed just after the creation but before the updating of bitcoinKeySet[setN]_CurrentPage. It also might not matter at all and might just be overly optimistic and improve some future operation by an insignificant amount. Or hey, maybe the system crashes :). Derp teh code comes along and seeing a page is full, gets the next one and updates the page. Methinks it's only an insignificant optimization to worry about _CurrentPage here, especially given how rare this race condition is.

                                //GOTO #sdofuou38#
                            }
                        }
                    }
                }
                else //cas swap fails
                {
                    //another web browser 'tab' requested a key at the same time, so error out on this one explaining that to user. MIGHT also make sense to do light/safe recovery, but eh probably not, just give the error and stop (user can re-initiate (handling recovery) if they want)
                    //TODOreq: anywhere a cas-swap of user account fails and there's a "GOTO", we need to first check that slotToAttemptToFillAkaPurchase isn't now set (if it is, we stop and error out and notify or whatever)
                }
            }
            else //already locked to fill-next-page. do recovery, then jump back to next step (depending on user account lock state)
            {
                //despite being locked for filling next page, we still want to proceed forward because we will then be doing light recovery of it (TODOreq: recovery detects dupe work, but we still want to continue getting the key from the newly generated page (i guess if we are lazy we can just error out and say try again sorry))
                //m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont = false;
                //GOTO #iopqjiw848#, which does the page fill and then jumps back to #ndjgir784#

                //OLD(??):
                //GOTO #ndjgir784# after filling in variables retrieved by locked doc (the one that got us here)
                //^^^^^now starting to think i should GOTO right around #iopqjiw848# instead (or above it a tad), BUT doesn't being there require user-account to already be locked?
            }
        }
        else //already locked to get-bitcoin-key, do light recovery
        {
            //first check that slotToAttemptToFillAkaPurchase isn't now set
            //m_UserAccountBitcoinLocked = true;
            //GOTO #ejfjdi83939# after filling in the page variables just above it (that we extracted from locked doc)
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
