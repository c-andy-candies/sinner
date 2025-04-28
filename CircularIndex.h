
#include "constants.h"

    inline int previous(int index)
    {
        if(index != 0)
        {
            return index - 1;
        }
        else 
        {
            return HISTORY_LENGTH - 1;
        }
    }

    inline int next(int index)
    {
        if(index != HISTORY_LENGTH - 1)
        {
            return index + 1;
        }
        else 
        {
            return 0;
        }
    }