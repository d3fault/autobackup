//This explains why I start each and every morning with a fap

void fap()
{
    splurt();
    erection = 0;
}
bool needToFap()
{
    if(erection >= FAP_LIMIT)
        return true;
    return false;
}
void think()
{
    if(needToBreath())
        breath();
    else if(needToDrinkWater())
        drinkWater();
    else if(needToEat())
        eat();
    else if(haveOrgasmedRecently())
    {
        //all of "meaningful life" exists within this scope/block
        DoableThing thingToDo = tryToThinkOfSomethingToDo();
        if(thingToDo.isWorthDoing())
        {
            doThing(thingToDo);
        }
    }
    else if(canHaveSex())
        haveSex();
    else if(needToFap())
        fap();
    else
    {
        if(even)
        {
            thinkAboutSex();
            ++erection;
        }
        else
            thinkAboutHowToObtainSex();
        even = !even;
    }
}
int main()
{
    while(true)
    {
        think();
    }
}
