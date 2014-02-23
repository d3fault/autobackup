#include "safetextvalidatorandinputfilter.h"

SafeTextValidatorAndInputFilter::SafeTextValidatorAndInputFilter(std::string maxCharacters, WObject *parent)
    : WRegExpValidator(SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_REGEX "{1," + maxCharacters + "}", parent)
{
    setMandatory(true);
    setInvalidNoMatchText(SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_INVALID_MESSAGE);
    setNoMatchText(SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_INVALID_MESSAGE);
}
std::string SafeTextValidatorAndInputFilter::inputFilter() const
{
    return SAFETEXT_REGEXPVALIDATOR_AND_INPUT_FILTER_REGEX;
}
