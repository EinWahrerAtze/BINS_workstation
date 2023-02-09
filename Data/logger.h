//
//  logger.h
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#pragma once
#include <queue>
#include <string>
#include <string_view>

// Simple logger class based on std::queue to notify user about events happening around.
// Class properties:
// - m_logs: std::queue to store messages as std::string
//
// Class behaviors:
// - log()    : add new message, the pointer to string is not const because std::move used later to move it to the queue;
// - extract(): pop first message out of queue;
// - empty()  : check if there are messages in the queue.

namespace ws::data
{
	class logger
	{
	public:
		logger();
	public:
		void log(std::string_view);
		std::string extract();
		bool empty() const;
	private:
		std::queue<std::string> m_logs;
	};
}
