#ifndef MESSAGEDISPATCH_H
#define MESSAGEDISPATCH_H

#include <unordered_map>
#include <vector>
#include <functional>

using MessageId = int;

template <typename T>
class MessageDispatch
{
	using ResponderHandler = std::function<void (T)>;
	std::unordered_map<MessageId, std::vector<ResponderHandler> responders;
public:
    MessageDispatch();

	void dispatch(MessageId id)
	{
		auto
	}

	void addResponder(MessageId messageId, ResponderHandler responder)
	{
		if (responders.find(messageId) != responders.end())
		{
			responders[messageId].push_back(responder);
		}
		else
		{
			throw std::runtime_error("Provided MessageId is invalid");
		}
	}
};


#endif /* MESSAGEDISPATCH_H */
