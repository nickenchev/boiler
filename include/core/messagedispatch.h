#ifndef MESSAGEDISPATCH_H
#define MESSAGEDISPATCH_H

#include <unordered_map>
#include <vector>
#include <functional>

namespace Boiler
{

using MessageId = int;

template <typename T>
class MessageDispatch
{
	using ResponderHandler = std::function<void (T)>;
	using ResponderList = std::vector<ResponderHandler>;

	std::unordered_map<MessageId, ResponderList> responders;

public:
    MessageDispatch();

	void dispatch(MessageId messageId)
	{
		auto itr = responders.find(messageId);
		assert(itr != responders.end());

		for (ResponderHandler handler : itr)
		{
			ResponderHandler();
		}
	}

	void addResponder(MessageId messageId, ResponderHandler responder)
	{
		auto itr = responders.find(messageId);
		assert(itr != responders.end());

		responders[messageId].push_back(responder);
	}
};

}

#endif /* MESSAGEDISPATCH_H */
