//
//  MessageQueue.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-12-27.
//
//

#include "MessageQueue.h"

Message MessageQueue::nextMessage(bool &newSeries)
{
    MessageSeries &series = seriesQueue.front();
    newSeries = series.isNewSeries();
    Message message = series.popMessage();
    
    //pop off if the series is empty
    if (!series.hasMessage())
    {
        seriesQueue.pop();
    }

    return message;
}

bool MessageQueue::hasMessage() const
{
    bool hasMessage = false;
    if (seriesQueue.size())
    {
        MessageSeries series = seriesQueue.front();
        if (series.hasMessage())
        {
            hasMessage = true;
        }
    }
    
    return hasMessage;
}