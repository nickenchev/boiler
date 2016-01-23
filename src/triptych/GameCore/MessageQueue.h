//
//  MessageQueue.h
//  triptych
//
//  Created by Nick Enchev on 2014-12-27.
//
//

#ifndef __triptych__MessageQueue__
#define __triptych__MessageQueue__

#include <queue>
#include <string>
#include "Message.h"

class MessageSeries
{
    std::queue<Message> messages;
    MessageSpriteInfo spriteInfo;
    bool newSeries;
    
public:
    MessageSeries() : MessageSeries(MessageSpriteInfo()) { }
    MessageSeries(MessageSpriteInfo spriteInfo) : spriteInfo(spriteInfo)
    {
        newSeries = true;
    }

    MessageSpriteInfo getSpriteInfo() const { return spriteInfo; }
    bool isNewSeries() const { return newSeries; }
    bool hasMessage() const { return messages.size(); }
    void pushMessage(Message message) { messages.push(message); }
    bool hasSprite() const { return spriteInfo.spriteName.length() > 0; }
    Message getMessage() const { return messages.front(); }

    Message popMessage()
    {
        newSeries = false;
        Message message;
        if (messages.size())
        {
            message = messages.front();
            messages.pop();
        }
        
        return message;
    }
};

class MessageQueue
{
    std::queue<MessageSeries> seriesQueue;
    
public:
    void addSeries(MessageSeries series) { seriesQueue.push(series); }
    const MessageSeries &getSeries() const { return seriesQueue.front(); }
    bool hasMessage() const;
    Message nextMessage(bool &newSeries);
};

#endif /* defined(__triptych__MessageQueue__) */
