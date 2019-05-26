#ifndef MESSAGEDISPATCH_H
#define MESSAGEDISPATCH_H

#include <unordered_map>
#include <vector>
#include <functional>
#include "core/logger.h"

namespace Boiler
{

template<typename... T>
class MessageDispatch
{
	Logger logger;
	using ResponderHandler = std::function<void (T...)>;
	using ResponderList = std::vector<ResponderHandler>;

	ResponderList messageResponders;

	// disable copying
	MessageDispatch(const MessageDispatch &) = delete;

public:
	MessageDispatch() : logger("Message Dispatch") { }

	void dispatch(T&&... args)
	{
		for (ResponderHandler handler : messageResponders)
		{
			handler(std::forward<T...>(args...));
		}
	}

	void subscribe(ResponderHandler responder)
	{
		messageResponders.push_back(responder);
	}
};

}

#endif /* MESSAGEDISPATCH_H */
