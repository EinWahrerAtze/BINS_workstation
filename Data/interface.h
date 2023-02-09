//
//  interface.h
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#pragma once
#include <functional>
#include "collection.h"

#ifdef interface
#undef interface
#endif

// Console interface for the program. It consist of five menus, described as 'menu' enum class and template
// component function 'output' to show it.
// 
// Class properties:
// - m_quit       : bool value for main program loop, if set to 'true', program ends;
// - m_error_state: bool value set to true if std::filesystem throws an exception and no more input is allowed
//                  (needs to be resolved in the future);
// - m_logger     : an instance of logger class (see 'logger.h') to show messages to user;
// - m_collection : an instance of file_clollection class (see 'collection.h');
// - m_output     : pointer to the tamplate 'output' fucntion to switch between menus.
// 
// Class behaviors:
// - start()    : main program loop;
// - output()   : show different menus of the program;
// - execute()  : runs commands depending on user's input;
// - get_logs() : reads any messages stored in 'm_logger';
// - get_input(): reads and demands proper input;
// - clear()    : clears console.

namespace ws::data
{
	class interface
	{
	public:
		interface();
		void start();
	private:
		enum class menu : uint32_t
		{
			MAIN,
			FILELIST,
			SAVE,
			CONVERT,
			HELP
		};
	private:
		template <menu>
		void output();
		void execute(std::string &);
		void get_logs();
		void get_input(std::string &);
		void clear() const;
	private:
		bool m_quit;
		bool m_error_state;
		logger m_logger;
		file_collection m_collection;
		std::function<void(interface *)> m_output;
	};
}
