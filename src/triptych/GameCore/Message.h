//
//  Message.h
//  triptych
//
//  Created by Nick Enchev on 2015-03-05.
//
//

#ifndef __triptych__Message__
#define __triptych__Message__

#include <string>
#include <functional>
#include "MessageSpriteInfo.h"

class Message
{
    std::string text;
    
public:
    std::function<void ()> alertShownHandler;
    std::function<void ()> alertDismissedHandler;
    
    Message() : Message(std::string()) { }
    Message(std::string text);
    
    std::string getText() const { return text; }
};

#endif /* defined(__triptych__Message__) */
