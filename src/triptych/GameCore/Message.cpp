//
//  Message.cpp
//  triptych
//
//  Created by Nick Enchev on 2015-03-05.
//
//

#include "Message.h"

Message::Message(std::string text) : text(text)
{
    alertShownHandler = nullptr;
    alertDismissedHandler = nullptr;
}