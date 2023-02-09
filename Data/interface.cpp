//
//  interface.cpp
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#include <locale>
#include <iostream>
#include "interface.h"

// C++23 std::print() function to use with std::format
constexpr void print(const std::string_view string, auto && ... args)
{
	fputs(std::vformat(string, std::make_format_args(args...)).c_str(), stdout);
}

namespace ws::data
{
	interface::interface() : m_quit(), m_error_state(), m_output()
	{
	#if defined(_WIN32)
		// user input could contain in most cases cyrillic characters, 
		// so set the input locale to russian
		std::cin.imbue(std::locale("ru_RU"));
	#endif
	}

	template <>
	void interface::output<interface::menu::MAIN>()
	{
		clear();
		print(std::format("[{}]{:>21}{:>10}{:>16}{:>17}{:>13}\n\n",
						  m_collection.get_extension(),
						  utility::apply("[Анализ]", utility::text::BOLD),
						  "Файлы",
						  "Cохранить",
						  "Конвертация",
						  "Помощь"));

		if (!m_collection.empty())
		{
			for (const auto & data : m_collection.get_data())
			{
				// data.second.first  - name of the added file
				// data.second.second - 'estimate' struct (see 'collection.h')
				print(std::format("{:-<74}\n", data.second.first));
				print(std::format("{}", *data.second.second));
			}
		}
		this->get_logs();
	}

	template <>
	void interface::output<interface::menu::FILELIST>()
	{
		clear();
		print(std::format("[{}]{:>12}{:>20}{:>15}{:>17}{:>13}\n\n",
						  m_collection.get_extension(),
						  "Анализ",
						  utility::apply("[Файлы]", utility::text::BOLD),
						  "Cохранить",
						  "Конвертация",
						  "Помощь"));
		if (!m_collection.empty())
		{
			print("Файлов добавлено: {}\n\n", m_collection.get_data().size());
			// file.first - path given by user where added files are located
			// change global local here because path string could contain cyrillic characters
			// and they won't display properly if utf8 locale is used (only Windows aware)
		#if defined (_WIN32)
			std::locale::global((std::locale("ru_RU")));
		#endif
			for (const auto & file : m_collection.get_data())
			{
				print("{}\n", file.first);
			}
			print("\n");
			// switch locale back to utf8
		#if defined (_WIN32)
			std::locale::global((std::locale("en_US.utf8")));
		#endif
		}
		this->get_logs();
	}

	template <>
	void interface::output<interface::menu::SAVE>()
	{
		clear();
		print(std::format("[{}]{:>12}{:>11}{:>25}{:>16}{:>13}\n\n",
						  m_collection.get_extension(),
						  "Анализ",
						  "Файлы",
						  utility::apply("[Cохранить]", utility::text::BOLD),
						  "Конвертация",
						  "Помощь"));
		this->get_logs();
	}

	template <>
	void interface::output<interface::menu::CONVERT>()
	{
		clear();
		print(std::format("[{}]{:>12}{:>11}{:>16}{:>26}{:>12}\n\n",
						  m_collection.get_extension(),
						  "Анализ",
						  "Файлы",
						  "Cохранить",
						  utility::apply("[Конвертация]", utility::text::BOLD),
						  "Помощь"));
		this->get_logs();
	}

	template <>
	void interface::output<interface::menu::HELP>()
	{
		clear();
		print(std::format("[{}]{:>12}{:>11}{:>16}{:>17}{:>22}\n\n",
						  m_collection.get_extension(),
						  "Анализ",
						  "Файлы",
						  "Cохранить",
						  "Конвертация",
						  utility::apply("[Помощь]", utility::text::BOLD)));

		print("BINS_workstation: DATA - анализ файлов изделий СГСКЛГ/БИНС2М-С. Федоров Денис, 2023.\n\n"
		      "Управление меню:\n"
			  "1 или 'Q' - главный экран, после добавления файлов доступен результат анализа.\n"
			  "2 или 'F' - список всех добавленных файлов.\n"
			  "3 или 'S' - сохранение анализа в .txt файл.\n"
			  "4 или 'C' - конвертирование файлов из .dat в .txt.\n"
			  "5 или 'H' - помощь.\n"
			  "'A' - добавить все .dat или .txt файлы в папке, где расположен .exe файл программы.\n"
			  "'V' - выбор формата добавляемых файлов.\n"
			  "'X' - завершение работы программы.\n\n"
		      "Чтобы добавить файлы, необходимо указать путь к папке или одиночному файлу и нажать \"enter\",\n"
			  "после чего выполнится автоматический поиск .dat или .txt файлов (в зависимости от выбранного формата).\n"
			  "Анализ включает в себя: погрешности определения курса, крена, тангажа и СКО (стандартного отклонения)\n"
			  "гироскопов X, Y и Z. Результат можно сохранить в .txt файл, который затем удобно открыть в \"MS Excel\"\n"
			  "с указанием разделителя \"табуляция\".\n"
			  "Чтобы выполнить преобразование из .dat в .txt, из меню \"конвертация\" укажите путь к папке или файлу и нажмите\n"
			  "\"enter\". Программа выполнит автоматическую конвертацию найденных файлов и сохранит результат по тому же адресу.\n\n");
		this->get_logs();
	}

	void interface::start()
	{
		std::string input;
		m_output = std::mem_fn(&interface::output<menu::MAIN>);
		while (!m_quit)
		{
			m_output(this);
			this->get_input(input);
			// std::filesystem::path could throw exception sometimes if incorrect string is given as source
			try
			{
				// if input string size is less than 2, so manage command is entered, else string contains
				// file location or just wrong
				if (input.size() < 2)
				{
					this->execute(input);
				}
				else if (std::filesystem::is_directory(std::filesystem::path(input)))
				{
					m_collection.add_all(std::filesystem::path(input), m_logger);
				}
				else if (std::filesystem::is_regular_file(std::filesystem::path(input)))
				{
					m_logger.log(std::format("Файл \"{}\" добавлен\n", input));
				}
				else
				{
					m_logger.log("Неправильный ввод\n");
				}
			}
			catch (const std::invalid_argument &)
			{
				m_logger.log("Ошибка при добавлении файлов, перезапустите программу\n");
				m_error_state = true;
			}
			catch (const std::system_error &)
			{
				m_logger.log("Ошибка при добавлении файлов, перезапустите программу\n");
				m_error_state = true;
			}
		}
	}

	void interface::execute(std::string & input)
	{
		// invariant: input.front() always has a value
		switch (input.front())
		{
			// add all files with set extension at folder where .exe is located
			case 'a':
			case 'A':
			{
				m_collection.add_all(std::filesystem::current_path(), m_logger);
				break;
			}
			// main menu
			case 'q':
			case 'Q':
			case '1':
			{
				m_output = std::mem_fn(&interface::output<menu::MAIN>);
				if (m_collection.empty())
				{
					m_logger.log("Введите путь к папке или файлу\n");
				}
				break;
			}
			// filelist menu
			case 'f':
			case 'F':
			case '2':
			{
				m_output = std::mem_fn(&interface::output<menu::FILELIST>);
				if (m_collection.empty())
				{
					m_logger.log("Список файлов пуст\n");
				}
				break;
			}
			// ask user for filename to save results as .txt file
			case 's':
			case 'S':
			case '3':
			{
				m_output = std::mem_fn(&interface::output<menu::SAVE>);
				if (!m_collection.empty())
				{
					m_logger.log("Введите имя файла с указанием расширения (.txt)\n");
				}
				else
				{
					m_logger.log("Нет добавленных файлов чтобы сохранить\n");
				}
				m_output(this);
				this->get_input(input);
				if (input.size() > 1)
				{
					m_collection.save_data(input, m_logger);
				}
				else
				{
					this->execute(input);
				}
				break;
			}
			// ask user for path to convert files
			case 'c':
			case 'C':
			case '4':
			{
				m_output = std::mem_fn(&interface::output<menu::CONVERT>);
				m_logger.log("Введите путь к файлу или папке\n");
				m_output(this);
				this->get_input(input);
				// same as before, catch any exception thrown by std::filesystem::path
				try
				{
					if (input.size() < 2)
					{
						// recursive call to this function to execute command
						this->execute(input);
					}
					if (std::filesystem::is_directory(std::filesystem::path(input)))
					{
						m_collection.convert_all(std::filesystem::path(input), m_logger);
					}
					if (std::filesystem::is_regular_file(std::filesystem::path(input)))
					{
						m_collection.convert(std::filesystem::path(input), m_logger);
					}
				}
				catch (const std::invalid_argument &)
				{
					m_logger.log("Ошибка при конвертации, перезапустите программу\n");
					m_error_state = true;
				}
				catch (const std::system_error &)
				{
					m_logger.log("Ошибка при конвертации, перезапустите программу\n");
					m_error_state = true;
				}
				break;
			}
			// help menu
			case 'h':
			case 'H':
			case '5':
			{
				m_output = std::mem_fn(&interface::output<menu::HELP>);
				break;
			}
			// change format of files to load
			case 'v':
			case 'V':
			{
				m_collection.set_extension();
				m_logger.log("Формат загружаемых файлов изменён\n");
				break;
			}
			// quit the program
			case 'x':
			case 'X':
			{
				m_quit = true;
				break;
			}
			default:
			{
				m_logger.log("Незвестная команда\n");
			}
		}
	}

	void interface::get_logs()
	{
		while (!m_logger.empty())
		{
			print("{}\n", m_logger.extract());
		}
		if (!m_error_state)
		{
			print(ws::data::utility::apply(": ", ws::data::utility::text::GREEN));
		}
		else
		{
			print(ws::data::utility::apply(": ", ws::data::utility::text::RED));
		}
	}

	void interface::get_input(std::string & input)
	{
		std::getline(std::cin, input);
		while (input.empty())
		{
			print(": ");
			std::getline(std::cin, input);
			if (!std::cin) { std::cin.clear(); }
		}
	}

	void interface::clear() const
	{
	#if defined(_WIN32)
		system("cls");
	#else
		system("clear");
	#endif
	}
}