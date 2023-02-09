//
//  logger.cpp
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#include "logger.h"

namespace ws::data
{
	logger::logger()
	{
		this->log("Добро пожаловать в программу анализа файлов! Нажмите 'H' для помощи\n");
	}

	void logger::log(std::string_view message)
	{
		m_logs.push(std::move(message.data()));
	}

	std::string logger::extract()
	{
		if (!m_logs.empty())
		{
			std::string s {std::move(m_logs.front())};
			m_logs.pop();
			return s;
		}
		else
		{
			return "";
		}
	}

	bool logger::empty() const
	{
		return m_logs.empty();
	}
}
