#include "brain.h"

//Was considering deriving this from computer, but it's subjective whether or not they are anyways
Brain::Brain()
    : ILogicProcessor()
{ }
void Brain::processLogic(ILogic *logic)
{
    throw new NotImplementedException(logic);
}
