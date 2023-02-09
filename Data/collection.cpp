//
//  collection.cpp
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#include <fstream>
#include "collection.h"

namespace ws::data
{
	file_collection::file_collection() : m_extension(extension::DAT)
	{

	}

	bool file_collection::add(const std::filesystem::path & path, logger & logger)
	{
		// if files at given path already were added, return
		if (m_collection.contains(path.string()))
		{
			logger.log(std::format("Файл \"{}\" уже добавлен", path.filename().string()));
			return false;
		}
		else
		{
			std::unique_ptr<file> file {std::make_unique<ws::data::file>()};
			if (path.filename().extension().string() == extension::DAT)
			{
				if (file->load<extension::DAT>(path.string()))
				{
					m_collection.emplace(path.string(), std::make_pair(path.filename().string(), this->analyze(file)));
					return true;
				}
				else
				{
					logger.log(std::format("Не удалось открыть \"{}\"\n", path.filename().string()));
					return false;
				}
			}
			else
			{
				if (file->load<extension::TXT>(path.string()))
				{
					m_collection.emplace(path.string(), std::make_pair(path.filename().string(), this->analyze(file)));
					return true;
				}
				else
				{
					logger.log(std::format("Не удалось открыть \"{}\"\n", path.filename().string()));
					return false;
				}
			}
		}
	}

	void file_collection::add_all(const std::filesystem::path & path, logger & logger)
	{
		uint32_t file_count {};
		// check all files at given directory with recursive directory iterator
		for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (entry.path().filename().extension().string() == (m_extension == extension::DAT ? extension::DAT : extension::TXT))
			{
				if (this->add(entry.path(), logger))
				{
					++file_count;
				}
			}
		}
		if (!file_count)
		{
			logger.log(std::format("Нет файлов в \"{}\"\n", path.string()));
		}
		else
		{
			logger.log(std::format("{} файл(ов) добавлен(о)\n", file_count));
		}
	}

	bool file_collection::convert(const std::filesystem::path & path, logger & logger)
	{
		std::unique_ptr<file> file {std::make_unique<ws::data::file>()};
		if (file->load<extension::DAT>(path.string()))
		{
			std::filesystem::path new_path(path);
			new_path.replace_extension(".txt");
			file->save<extension::TXT>(new_path.string());
			logger.log(std::format("\"{}\" {} \"{}\"",
								   path.filename().string(),
								   utility::apply("->", utility::text::GREEN),
								   new_path.filename().string()));
			return true;
		}
		else
		{
			logger.log(std::format("Не удалось конвертировать \"{}\"\n", path.string()));
			return false;
		}
	}

	void file_collection::convert_all(const std::filesystem::path & path, logger & logger)
	{
		uint32_t file_count {};
		for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (entry.path().filename().extension().string() == extension::DAT)
			{
				if (this->convert(entry.path(), logger))
				{
					++file_count;
				}
			}
		}
		if (!file_count)
		{
			logger.log(std::format("Нет файлов для конвертирования\n"));
		}
		else
		{
			logger.log(std::format("\n{} файла(ов) конвертировано\n", file_count));
		}
	}

	void file_collection::save_data(const std::string_view filename, logger & logger) const
	{
		// replace all spaces between words with tab symbol so .txt file could be open
		// in 'MS Excel' later using tab delimiter
		std::ofstream fout {filename.data(), std::ios_base::out};
		if (!fout.is_open())
		{
			logger.log(std::format("Не удалось записать в файл \"{}\"\n", filename.data()));
		}
		for (const auto & data : m_collection)
		{
			// filename
			fout << std::format("{:-<74}\n", data.second.first);
			// first row
			fout << std::format("Heading:\t{}°\t", data.second.second->heading);
			fout << std::format("THdg:\t{}°{:>2}'{:>2}\"\t", data.second.second->thdg.degree, data.second.second->thdg.minute, data.second.second->thdg.second);
			fout << std::format("{}°{:0>2}'{:0>2}\"\t", data.second.second->thdg.degree_error, data.second.second->thdg.minute_error, data.second.second->thdg.second_error);
			fout << std::format("X\t{:.4f}\n", data.second.second->deviation_X);
			// second row
			fout << std::format("Duration:\t{} s.\t", data.second.second->duration);
			fout << std::format("Roll:\t{}°{}'{}\"\t", data.second.second->roll.degree, data.second.second->roll.minute, data.second.second->roll.second);
			fout << std::format("{}°{:0>2}'{:0>2}\"\t", data.second.second->roll.degree_error, data.second.second->roll.minute_error, data.second.second->roll.second_error);
			fout << std::format("Y\t{:.4f}\n", data.second.second->deviation_Y);
			// third row
			fout << std::format("Temperature:\t{}°C\t", (data.second.second->temperature_X + data.second.second->temperature_Y + data.second.second->temperature_Z) / 3);
			fout << std::format("Pitch:\t{}°{}'{}\"\t", data.second.second->pitch.degree, data.second.second->pitch.minute, data.second.second->pitch.second);
			fout << std::format("{}°{:0>2}'{:0>2}\"\t", data.second.second->pitch.degree_error, data.second.second->pitch.minute_error, data.second.second->pitch.second_error);
			fout << std::format("Z\t{:.4f}\n\n", data.second.second->deviation_Z);
		}
		logger.log(std::format("Анализ успешно записан в \"{}\"\n", filename.data()));
		fout.close();
	}

	bool file_collection::empty() const
	{
		return m_collection.empty();
	}

	void file_collection::set_extension()
	{
		m_extension == extension::DAT ? m_extension = extension::TXT : m_extension = extension::DAT;
	}

	extension file_collection::get_extension() const
	{
		return m_extension;
	}

	const std::map<std::string, std::pair<std::string, std::unique_ptr<file_collection::estimate>>> & file_collection::get_data() const
	{
		return m_collection;
	}

	std::unique_ptr<file_collection::estimate> file_collection::analyze(const std::unique_ptr<file> & new_file)
	{
		// find index where value of '600' is located in the array, all later calculations will use this index
		auto find_index {[](const std::unique_ptr<file> & file, const uint32_t row:: * member) -> std::size_t
		{
			// if file begins with value greater than '600', return '1' as index, i.e. first index
			if (file->get_data().front().*member > 600)
			{
				return static_cast<std::size_t>(1);
			}
			// else find '600'
			else
			{
				for (auto i {file->get_data().cbegin()}; i != file->get_data().cend(); ++i)
				{
					if ((*i).*member == 600)
					{
						return static_cast<std::size_t>(std::distance(file->get_data().cbegin(), i));
					}
				}
				// if file is too short (no value '600' found), return the lenght of file as index, i.e. last index
				return static_cast<std::size_t>(std::distance(file->get_data().cbegin(), file->get_data().cend()) - 1);
			}
		}};

		auto index {find_index(new_file, &row::count)};
		std::unique_ptr<estimate> result {std::make_unique<estimate>()};
		result->thdg = convert_degree(new_file->get_data().at(index).thdg);
		result->roll = convert_degree(new_file->get_data().at(index).roll);
		result->pitch = convert_degree(new_file->get_data().at(index).pitch);
		result->heading = static_cast<uint32_t>(std::roundf(new_file->get_data().at(index).thdg));
		if (result->heading == 360) { result->heading = 0; }
		result->duration = new_file->get_data().back().count;
		result->deviation_X = deviation(new_file, &row::gyro_X);
		result->deviation_Y = deviation(new_file, &row::gyro_Y);
		result->deviation_Z = deviation(new_file, &row::gyro_Z);
		auto size {static_cast<int>(new_file->get_data().size())};
		result->temperature_X = this->accumulate(new_file, &row::gyro_X_temperature) / size / 100;
		result->temperature_Y = this->accumulate(new_file, &row::gyro_Y_temperature) / size / 100;
		result->temperature_Z = this->accumulate(new_file, &row::gyro_Z_temperature) / size / 100;
		return result;
	}

	float file_collection::deviation(const std::unique_ptr<file> & file, const float row:: * member)
	{
		// standart deviation formula: σ = sqrt((∑(variable - average) ^ 2) / size - 1)
		// (*i).*member represents each veriable contained in given array (vector)
		// calculate average value of given array
		float average {this->accumulate<float>(file, member) / file->get_data().size()};
		std::vector<float> difference;
		difference.reserve(file->get_data().size());
		// calculate (variable - average) ^ 2 part
		for (auto i {file->get_data().cbegin()}; i != file->get_data().cend(); ++i)
		{
			difference.push_back(static_cast<float>(std::pow((*i).*member - average, 2)));
		}
		// calculate ∑(variable - average) ^ 2) part
		average = accumulate(difference);
		// calculate (∑(variable - average) ^ 2) / size - 1 part
		average /= file->get_data().size() - 1;
		// return sqrt((∑(variable - average) ^ 2) / size - 1)
		return std::sqrt(average);
	}

	file_collection::estimate::angle file_collection::convert_degree(float degree) const
	{
		// std::modf decomposes given floating point value into integral and fractional parts
		// as an example: given angle is 300.1523
		// 300 goes to angle.degree and 0.1523 * 60 goes to angle.minute (60 is amount of seconds in a minute)
		// 0.1523 * 60 = 9.138, so 9 goes to angle.minute, 0.138 * 60 goes to angle.seconds
		// then round these values to nearest integer using std::roundf function, so 300.1523 -> 300°9'8"
		// same deal with error values
		estimate::angle angle {};
		angle.minute = std::abs(std::modf(degree, &angle.degree) * 60);
		angle.second = std::abs(std::modf(angle.minute, &angle.minute) * 60);
		angle.second = std::roundf(angle.second);
		auto fraction_value {std::abs(std::roundf(degree) - degree)};
		angle.minute_error = std::modf(fraction_value, &angle.degree_error) * 60;
		angle.second_error = std::modf(angle.minute_error, &angle.minute_error) * 60;
		angle.second_error = std::roundf(angle.second_error);
		return angle;
	}

	template <typename T>
	T file_collection::accumulate(const std::unique_ptr<file> & file, const T row:: * member) const
	{
		T sum {};
		for (auto i {file->get_data().cbegin()}; i != file->get_data().cend(); ++i)
		{
			sum += (*i).*member;
		}
		return sum;
	}

	template <typename T>
	T file_collection::accumulate(const std::vector<T> & data)
	{
		float sum {};
		for (auto i : data)
		{
			sum += i;
		}
		return sum;
	}
}