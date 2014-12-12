#include <QCoreApplication>

#include <iostream>

//#include "abc2couchbaseandjsonkeydefines.h"
#include <cstdio>
#include <cmath>

using namespace std;

//Because 3% of 0.00000001 -- aka 1 satoshi (which, although way below our minimum, is still a valid STEP amount), is 0.0000000003 (which rounds down to zero), and yet 3% of 0.00000002 -- aka 2 satoshis, is 0.0000000006 (which ALSO rounds down to zero (and yet we have still paid out another satoshi!)). In this prototype I'm going to try to figure out the math behind "always round up". I know it's possible, I just suck at math. This should be hilarious to math majors.

//Hmm ok just gonna type as I think and try random shit until I can prove to myself that it's correct. So basically I need to find the point of "even divide" or something. Like uhh the point where 3% gives me a single full satoshi (with no remainder). Shit perhaps since it's 3 it won't divide evenly into anything because it's a prime number or fuck idfk. Arbitrary anyways so I can use 4 if I need to.

/*
0.00000025*.04 = 0.00000001
what is the significance of that?
it means that a withdrawal amount of 0.00000001 through 0.00000025 should incur a 1 satoshi fee 0.00000026 should jump to 2 satoshis for the fee
0.00000026*.04 = .0000000104
so it appears i can do "rounding up always" by simply seeing if IT'S greater than .00000001 (and i need enough precision -- c/c++ doubles provide plenty (so long as i'm not on some shit platform (perhaps i should be talking in satoshis instead, but idk if i can then use those 9th and 10th digits anymore so wtf)))
i also don't know how to abstractly calculate "IT" in the above line. it is something along the lines of "the next lowest evenly divisible point" or some shit

so uhh let's just make a function and tweak it's body until i get what i want from it :-P

*/

#define WITHDRAWAL_FEE_MULTIPLIER 0.04

inline std::string doubleToStringWith8DecimalPlaces(double inputDouble)
{
    char buf[30]; snprintf(buf, 30, "%.8f", inputDouble); std::string ret(buf, strlen(buf)); return ret;
    //char buf[30]; snprintf(buf, 30, "%.15f", inputDouble); std::string ret(buf, strlen(buf)); return ret; //.15 so i can see how close it's getting or something
}
double calculateWithdrawalFeeAlwaysRoundingUp(double desiredWithdrawalAmount)
{
    //return desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    //^this is the regular non-rounding-up way to do it

    //i think all i need to do is add 0.00000009 to every something, perhaps 0.000000009 instead -- then just let truncation do it's thing~ or shit maybe it's 0.9 i need to add. and really i don't even know what to add it xD

    //since i suck at math let's do the trial n error approach

    //return (desiredWithdrawalAmount+.9) * WITHDRAWAL_FEE_MULTIPLIER;
    //^nope

    //return (desiredWithdrawalAmount+.00000009) * WITHDRAWAL_FEE_MULTIPLIER;
    //^nope

    //return ((desiredWithdrawalAmount*WITHDRAWAL_FEE_MULTIPLIER)+.000000009);
    //^nope -- CLOSEST SO FAR

    //return ((desiredWithdrawalAmount*WITHDRAWAL_FEE_MULTIPLIER)+.0000000009);
    //^nope

    //return (desiredWithdrawalAmount*WITHDRAWAL_FEE_MULTIPLIER)+.000000005;
    //^nope

    //return (desiredWithdrawalAmount*WITHDRAWAL_FEE_MULTIPLIER)+(0.00000001/.04);
    //^nope

    //return (desiredWithdrawalAmount*WITHDRAWAL_FEE_MULTIPLIER)+(0.04/0.00000001);
    //^nope

    //maybe it's because i'm doing proper rounding on top of improper rounding xD (only used proper rounding because that was my way of spitting it out into a string with 8 decimal places). commenting out that old shiz now

    //and now re-testing each of the above...
    //^nope

    //fuck i'm out of ideas. *stares at screen*

    //oh yea i forgot to convert to int64 and then back after adding the .000000009 lol, so the truncation never took place!

#if 0
    double withdrawalFee = desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    cout << "(withdrawal fee not rounded: " << doubleToStringWith8DecimalPlaces(withdrawalFee) << ", ";
    withdrawalFee += 0.000000009; //always round up
    cout << "withdrawal fee with a nine added to try to round up: " << doubleToStringWith8DecimalPlaces(withdrawalFee) << ", ";
    long long int withdrawalFeeRoundedUpInSatoshis = (long long int)(withdrawalFee * 1e8); //truncates
    cout << "withdrawal fee hopefully rounded up and truncated into satoshis: " << withdrawalFeeRoundedUpInSatoshis << ") final result: ";
    double withdrawalFeeRoundedUp = (double)withdrawalFeeRoundedUpInSatoshis / 1e8;
    return withdrawalFeeRoundedUp;
#endif

#if 0
    //I think maybe this is the formula I should use: if "one less satoshi than the amount we are evaluating comes out to be the same (after truncation), then i add a single satoshi to the result" -- side steps a lot of math (while still 'doing' it xD (don't ask, my brain works weird (mathematicians could write it out and understand it, i can't)))
    double withdrawalFeeExact = desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    double withdrawalFeeExactMinusOneSatoshi = withdrawalFeeExact - 0.00000001; //TODOreq: zero satoshis and 1 satoshis as input might need special logic
    //now i check to see if they are the same after truncation. if they are, that means that the withdrawalFeeExact should be "rounding up" because it had some bullshit at the end
    long long int withdrawalFeeTruncated = (long long int)(withdrawalFeeExact*1e8);
    long long int withdrawalFeeMinusOneSatoshiTruncated = (long long int)(withdrawalFeeExactMinusOneSatoshi*1e8);
    bool roundUpOneSatoshi = (withdrawalFeeTruncated == withdrawalFeeMinusOneSatoshiTruncated);
    if(roundUpOneSatoshi)
        withdrawalFeeExact += 0.00000001;
    return withdrawalFeeExact;
#endif

#if 0
    //Maybe: if the difference between input and input minus one satoshi, when both have fee multiplier applied, is greater than or equal to one satoshi, then i add one satoshi to the result

    //special handling of 0 satoshi input:
    if(desiredWithdrawalAmount == 0.00000000)
        return 0;
    //special handling of 1 satoshi input:
    if(desiredWithdrawalAmount == 0.00000001)
        return 0.00000001;

    double inputMinusOneSatoshi = desiredWithdrawalAmount - 0.00000001;

    double withdrawalFee = desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    double inputMinusOneSatoshiWithdrawalFee = inputMinusOneSatoshi * WITHDRAWAL_FEE_MULTIPLIER;

    if((withdrawalFee - inputMinusOneSatoshiWithdrawalFee) >= 0.00000001)
    {
        withdrawalFee += 0.00000001;
    }
    return withdrawalFee;
#endif

    //I think I'm getting warmer...

#if 0
    //Hmm maybe I'm overcomplicating this shit
    double witihdrawalFee = desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    long long int withdrawalFeeRoundedUpAndTruncated = (long long int)((witihdrawalFee * 1e8) + .9);
    double withdrawalFeeRoundedUp = (double)withdrawalFeeRoundedUpAndTruncated / 1e8;
    return withdrawalFeeRoundedUp;
#endif

#if 0
    //Maybe I'm doing the rounding up at the wrong stage...
    long long int withdrawalAmountRoundedUpAndTruncated = (long long int)((desiredWithdrawalAmount * 1e8) + .9);
    double withdrawalAmountRoundedUp = (double)withdrawalAmountRoundedUpAndTruncated / 1e8;
    return withdrawalAmountRoundedUp;
    //^wtf? how is my input my output... oh derp i didn't even apply withdrawal fee. guh this is getting annoying...
#endif

    //The formula up above with the input and input minus one satoshi hackery was close, but I should check that the result (some result) is > 0 to determine whether or not to add 1 satoshi to the result. If the
    //Blah lost my train of thought but I think I'm onto it finally!

#if 0
    //I calcaulte their difference subtract one satoshi to account for the one i added, and then see if it's > 0 --- or SOMETHING. I'll try that since it's all I got so far...

    //special handling of 0 satoshi input:
    if(desiredWithdrawalAmount == 0.00000000)
        return 0;
    //special handling of 1 satoshi input:
    if(desiredWithdrawalAmount == 0.00000001)
        return 0.00000001;

    double inputMinusOneSatoshi = desiredWithdrawalAmount - 0.00000001;

    double withdrawalFee = desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    double inputMinusOneSatoshiWithdrawalFee = inputMinusOneSatoshi * WITHDRAWAL_FEE_MULTIPLIER;

    double withdrawalFeeDifference = withdrawalFee - inputMinusOneSatoshiWithdrawalFee;
    //if((withdrawalFeeDifference-0.00000001) > 0)
    //if(withdrawalFeeDifference > 0)
    double withdrawalFeeForOneSatoshi = 0.00000001 * WITHDRAWAL_FEE_MULTIPLIER;
    //if((withdrawalFeeDifference - withdrawalFeeForOneSatoshi) > 0)
    if((withdrawalFeeDifference - withdrawalFeeForOneSatoshi) > 0.00000001)
    {
        withdrawalFee += 0.00000001;
    }
    return withdrawalFee;
#endif

    //Been a few hours since I wrote most of this prototype and decided to just chill and play teeworlds and hope the answer would come to me. mfw ceil(). I'm still a little worried about losing some precision, but bleh I'm not sure it's justified worry. My worry comes from the fact that I have to multiply by 1e8 to get the "rest of the decimals" in position for ceil to make use of, and in doing so I'll be making the typically-to-left-of-decimal-places even further left and blah idk if it goes out of range or not so eh I'm adding the max bitcoin to my test vectors xD
    double withdrawalFeeExact = desiredWithdrawalAmount * WITHDRAWAL_FEE_MULTIPLIER;
    double withdrawalFeeReadyForCeil = withdrawalFeeExact * 1e8;
    double withdrawalInSatoshisRoundedUp = ceil(withdrawalFeeReadyForCeil);
    double withdrawalFeeRoundedUp = (double)withdrawalInSatoshisRoundedUp / 1e8;
    return withdrawalFeeRoundedUp;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if 0
    cout << "0.00000000: " << jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(calculateWithdrawalFeeAlwaysRoundingUp(0.00000000)) << endl;
    cout << "0.00000001: " << jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(calculateWithdrawalFeeAlwaysRoundingUp(0.00000001)) << endl;
    cout << "0.00000001: " << jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(calculateWithdrawalFeeAlwaysRoundingUp(0.00000025)) << endl;
    cout << "0.00000002: " << jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(calculateWithdrawalFeeAlwaysRoundingUp(0.00000026)) << endl;
#endif

    cout << endl;
    cout << "'0.00000000' input, expecting '0.00000000': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(0.00000000)) << endl;

    cout << "'0.00000001' input, expecting '0.00000001': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(0.00000001)) << endl;

    cout << "'0.00000005' input, expecting '0.00000001': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(0.00000005)) << endl;

    cout << "'0.00000025' input, expecting '0.00000001': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(0.00000025)) << endl;

    cout << "'0.00000026' input, expecting '0.00000002': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(0.00000026)) << endl;

    cout << "'20999999.99999999' input, expecting '840000': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(20999999.99999999)) << endl;

    cout << "'21000000' input, expecting '840000': ";
    cout << doubleToStringWith8DecimalPlaces(calculateWithdrawalFeeAlwaysRoundingUp(21000000)) << endl;

    return 0;
}

//TODOreq: when i'm an old cunt, calculate how much extra this rounding up always shit has earned me
