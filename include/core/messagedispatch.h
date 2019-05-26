#ifndef MESSAGEDISPATCH_H
#define MESSAGEDISPATCH_H

#include <unordered_map>
#include <vector>
#include <functional>
#include "core/logger.h"

namespace Boiler
{

using MessageId = int;

class MessageDispatch
{
	Logger logger;
	using ResponderHandler = std::function<void ()>;
	using ResponderList = std::vector<ResponderHandler>;

	std::vector<ResponderList> messageResponders;

	// disable copying
	MessageDispatch(const MessageDispatch &) = delete;

public:
	MessageDispatch() : logger("Message Dispatch") { }

	constexpr MessageId createMessageId()
	{
		messageResponders.push_back(std::vector<ResponderHandler>());
		MessageId newId = messageResponders.size() - 1;
		logger.log("Created new message with ID: " + std::to_string(newId));
		return newId;
	}

	void dispatch(MessageId messageId)
	{
		assert(messageId < messageResponders.size());
		
		for (ResponderHandler handler : messageResponders[messageId])
		{
			handler();
		}
	}

	void subscribe(MessageId messageId, ResponderHandler responder)
	{
		assert(messageId < messageResponders.size());

		messageResponders[messageId].push_back(responder);
	}
};

}

#endif /* MESSAGEDISPATCH_H */
