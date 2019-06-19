#include "helperFunctions.h"

bool isAboutInt(int this, int isAboutThis, int byThisMuch)
{
    return (((this-byThisMuch) < isAboutThis) && ((this + byThisMuch) > isAboutThis));
}
bool isAboutLong(long this, long isAboutThis, long byThisMuch)
{
    return (((this-byThisMuch) < isAboutThis) && ((this + byThisMuch) > isAboutThis));
}

bool isAboutFloat(float this, float isAboutThis, float byThisMuch)
{    
    return (((this-byThisMuch) < isAboutThis) && ((this + byThisMuch) > isAboutThis));
}