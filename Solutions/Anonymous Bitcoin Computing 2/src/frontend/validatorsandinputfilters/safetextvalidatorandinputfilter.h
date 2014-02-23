#ifndef SAFETEXTVALIDATORANDINPUTFILTER_H
#define SAFETEXTVALIDATORANDINPUTFILTER_H

#define SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_REGEX "[a-zA-Z0-9 .-]" //if changing this, change the constructor's use of it which currently appends {1,maxLen}
#define SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_ACCEPTABLE_RANGE_STRING "Max length: 64. Lower and upper case letters and all digits "
#define SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_INVALID_MESSAGE "Invalid Input: " SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_ACCEPTABLE_RANGE_STRING

#include <Wt/WRegExpValidator>
using namespace Wt;

class SafeTextValidatorAndInputFilter : public WRegExpValidator
{
public:
    SafeTextValidatorAndInputFilter(std::string maxCharacters = "64", WObject *parent = 0);
    virtual std::string inputFilter() const;
};

#endif // SAFETEXTVALIDATORANDINPUTFILTER_H
