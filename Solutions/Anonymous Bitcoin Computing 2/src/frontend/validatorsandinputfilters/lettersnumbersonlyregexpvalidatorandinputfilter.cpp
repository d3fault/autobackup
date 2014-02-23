#include "lettersnumbersonlyregexpvalidatorandinputfilter.h"

LettersNumbersOnlyRegExpValidatorAndInputFilter::LettersNumbersOnlyRegExpValidatorAndInputFilter(std::string maxCharacters, WObject *parent)
    : WRegExpValidator(LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_REGEX "{1," + maxCharacters + "}", parent) //TODOreq: verify max length is working like i think it is
{
    setMandatory(true);
    setInvalidNoMatchText(LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_INVALID_MESSAGE);
    setNoMatchText(LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_INVALID_MESSAGE);
}
std::string LettersNumbersOnlyRegExpValidatorAndInputFilter::inputFilter() const
{
    return LETTERS_NUMBERS_ONLY_REGEXPVALIDATOR_AND_INPUT_FILTER_REGEX;
}
