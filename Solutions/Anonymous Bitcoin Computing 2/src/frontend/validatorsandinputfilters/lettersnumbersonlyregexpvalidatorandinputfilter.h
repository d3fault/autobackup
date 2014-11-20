#ifndef LETTERSNUMBERSONLYREGEXPVALIDATORANDINPUTFILTER_H
#define LETTERSNUMBERSONLYREGEXPVALIDATORANDINPUTFILTER_H

#define LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_REGEX "[a-zA-Z0-9]" //if changing this, change the constructor's use of it which currently appends {1,64}
#define LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_ACCEPTABLE_RANGE_STRING "Max length: 64. Lower and upper case letters and numbers " /* TODOreq: max len should reflect what they gave the constructor */
#define LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_INVALID_MESSAGE "Invalid Input: " LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_ACCEPTABLE_RANGE_STRING

#include <Wt/WRegExpValidator>
using namespace Wt;

class LettersNumbersOnlyRegExpValidatorAndInputFilter : public WRegExpValidator
{
public:
    LettersNumbersOnlyRegExpValidatorAndInputFilter(std::string maxCharacters = "64", WObject *parent = 0);
    virtual std::string inputFilter() const;
};

#endif // LETTERSNUMBERSONLYREGEXPVALIDATORANDINPUTFILTER_H
