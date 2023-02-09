//
//  collection.h
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#pragma once
#include <map>
#include <memory>
#include <filesystem>
#include "file.h"
#include "logger.h"
#include "utility.h"

// File_collection class represents group of files (see 'file.h'). Struct 'estimate' is the essential of the program.
// It contains calculated results based on a single file. Struct contains 'angle' member, which represents converted
// decimal angle to degrees, minutes and seconds. It also has degree_error, minute_error and second_error - INS
// deviation from set course. Has std::formatter specialization, thus it could be used as argument for std::format().
// 
// Class properties:
// - m_extension : extension (see file.h);
// - m_collection: std::map - key:   - std::string - path given by user where all source files located;
//                          - value: - std::pair   - first : std::string     - name of a single source file;
//                                                 - second: std::unique_ptr - pointer to the 'estimate' data structure.
// Class behaviors:
// - add()           : loads a single file from given path;
// - add_all()       : loads all files with set extenstion from given path to a folder;
// - convert()       : converts a single .dat file to .txt;
// - convert_all()   : converts all .dat files at given path to folder to .txt;
// - save_data()     : saves all calculated data from 'm_collection' to .txt file;
// - empty()         : checks if files were loaded;
// - set_extension() : sets the 'm_extension' member to load .dat or .txt files;
// - get_extension() : returns current state of 'm_extension' member;
// - get_data()      : returns a const reference to 'm_collection' member;
// - analyze()       : takes raw data and returns calculated 'estimate' data structure;
// - convert_degree(): converts decimal angle to degrees °, minutes ' and seconds ";
// - deviation()     : calculates standard deviation;
// - accumulate()    : calculates sum of all values in an array of data, similar to std::reduce or std::accumulate from <numeric>.

namespace ws::data
{
	class file_collection
	{
	public:
		file_collection();
	private:
		struct estimate
		{
			struct angle
			{
				float degree;
				float minute;
				float second;
				float degree_error;
				float minute_error;
				float second_error;
			};
			angle thdg;
			angle roll;
			angle pitch;
			uint32_t heading;
			uint32_t duration;
			float deviation_X;
			float deviation_Y;
			float deviation_Z;
			int32_t temperature_X;
			int32_t temperature_Y;
			int32_t temperature_Z;
		};
	public:
		bool add(const std::filesystem::path &, logger &);
		void add_all(const std::filesystem::path &, logger &);
		bool convert(const std::filesystem::path &, logger &);
		void convert_all(const std::filesystem::path &, logger &);
		void save_data(const std::string_view, logger &) const;
		bool empty() const;
		void set_extension();
		extension get_extension() const;
		const std::map<std::string, std::pair<std::string, std::unique_ptr<estimate>>> & get_data() const;
		friend std::formatter<ws::data::file_collection::estimate>;
	private:
		std::unique_ptr<estimate> analyze(const std::unique_ptr<file> &);
		estimate::angle convert_degree(float) const;
		float deviation(const std::unique_ptr<file> &, const float row:: *);
		template <typename T>
		T accumulate(const std::unique_ptr<file> &, const T row:: *) const;
		template <typename T>	
		T accumulate(const std::vector<T> &);
	private:
		extension m_extension;
		std::map<std::string, std::pair<std::string, std::unique_ptr<estimate>>> m_collection;
	};
}

template <>
struct std::formatter<ws::data::file_collection::estimate> : std::formatter<std::string_view>
{
	template <typename T>
	auto format(const ws::data::file_collection::estimate & data, T & t)
	{
		std::string thdg_error;
		std::string roll_error;
		std::string pitch_error;

		// if thdg error value is greater than 0°7'2" (7 * 60 + 0.2 * 60 = 432), mark it with red colour
		if (data.thdg.minute_error * 60.0f + data.thdg.second_error > 432.0f)
		{
			thdg_error = std::format("{:>15}°{:>11}'{:>11}\"",
									 ws::data::utility::apply(data.thdg.degree_error, ws::data::utility::text::RED),
									 ws::data::utility::apply(data.thdg.minute_error, ws::data::utility::text::RED),
									 ws::data::utility::apply(data.thdg.second_error, ws::data::utility::text::RED));
		}
		else
		{
			thdg_error = std::format("{:6}°{:>2}'{:>2}\"", data.thdg.degree_error, data.thdg.minute_error, data.thdg.second_error);
		}
		// if roll error value is greater than 0°1'8" (1 * 60 + 0.8 * 60 = 108), mark it with red colour
		if (data.roll.minute_error * 60.0f + data.roll.second_error > 108.0f)
		{
			roll_error = std::format("{:>15}°{:>11}'{:>11}\"",
									 ws::data::utility::apply(data.roll.degree_error, ws::data::utility::text::RED),
									 ws::data::utility::apply(data.roll.minute_error, ws::data::utility::text::RED),
									 ws::data::utility::apply(data.roll.second_error, ws::data::utility::text::RED));
		}
		else
		{
			roll_error = std::format("{:>6}°{:>2}'{:>2}\"", data.roll.degree_error, data.roll.minute_error, data.roll.second_error);
		}
		// same as roll
		if (data.pitch.minute_error * 60.0f + data.pitch.second_error > 108.0f)
		{
			pitch_error = std::format("{:>15}°{:>11}'{:>11}\"",
									  ws::data::utility::apply(data.pitch.degree_error, ws::data::utility::text::RED),
									  ws::data::utility::apply(data.pitch.minute_error, ws::data::utility::text::RED),
									  ws::data::utility::apply(data.pitch.second_error, ws::data::utility::text::RED));
		}
		else
		{
			pitch_error = std::format("{:>6}°{:>2}'{:>2}\"", data.pitch.degree_error, data.pitch.minute_error, data.pitch.second_error);
		}

		return std::formatter<std::string_view>::format(std::format("{}{:15}°{:>10}{: 7}°{:>2}'{:>2}\"{}{:>6}{:>10.4f}\n"
																	"{}{:7}°C{:>10}{: 7}°{:>2}'{:>2}\"{}{:>6}{:>10.4f}\n"
																	"{}{:6} c.{:>11}{: 5}°{:>2}'{:>2}\"{}{:>6}{:>10.4f}\n\n",
																	// first row
																	"Румб:",
																	data.heading,
																	"Курс:",
																	data.thdg.degree,
																	data.thdg.minute,
																	data.thdg.second,
																	thdg_error,
																	"X",
																	data.deviation_X,
																	// second row
																	"Температура:",
																	(data.temperature_X + data.temperature_Y + data.temperature_Z) / 3,
																	"Крен:",
																	data.roll.degree,
																	data.roll.minute,
																	data.roll.second,
																	roll_error,
																	"Y",
																	data.deviation_Y,
																	// third row
																	"Длительность:",
																	data.duration,
																	"Тангаж:",
																	data.pitch.degree,
																	data.pitch.minute,
																	data.pitch.second,
																	pitch_error,
																	"Z",
																	data.deviation_Z), t);
	}
};