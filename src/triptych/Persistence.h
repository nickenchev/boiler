//
//  Persistence.h
//  triptych
//
//  Created by Nick Enchev on 2015-06-25.
//
//

#ifndef __triptych__Persistence__
#define __triptych__Persistence__

#include <string>

class Persistence
{
public:
    static bool isFlagSet(std::string flag);
    static void setFlag(std::string flag, bool value);
    static void resetFlag(std::string flag);
};

#endif /* defined(__triptych__Persistence__) */
