//
//  file.h
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#pragma once
#include <vector>
#include <string_view>

// File class is a basic building block for the program to start with. It uses 'row' struct which represents 
// one single line of source data. Each row then put to the std::vector of rows. Size of the
// m_data vector is the lenght of file. Enum class 'extension' represents format of files that
// could be read or saved. It has two overloads of comparation operator and std::formatter specialization,
// so it could be used as argument for std::format().
// 
// Class properties:
// - m_data: std::vector of raw data represented as a single row.
// 
// Class behaviors:
// - load<extension>(): read .dat or .txt source file;
// - save<extension>(): save .dat or .txt file;
// - get_data()       : return a const reference to 'm_data' member.

namespace ws::data
{
	struct row
	{
		uint32_t count, mode;
		float system_time, mode_time;
		float thdg, roll, pitch;
		float heading, azimuth;
		float latitude, longtitude;
		float H, Ve, Vn, Vu;
		float dAt_X, dAt_Y, dAt_Z;
		float dVt_X, dVt_Y, dVt_Z;
		float gyro_X, gyro_Y, gyro_Z;
		float acc_X, acc_Y, acc_Z;
		float U_cplc_X, U_cplc_Y, U_cplc_Z;
		float U_hfo_X, U_hfo_Y, U_hfo_Z;
		float F_out_X, F_out_Y, F_out_Z;
		float F_dith_X, F_dith_Y, F_dith_Z;
		int gyro_X_temperature, gyro_Y_temperature, gyro_Z_temperature;
		uint32_t acc_X_temperature, acc_Y_temperature, acc_Z_temperature;
		uint32_t dpb_X_temperature, dpb_Y_temperature, dpb_Z_temperature;
		float drift_X, drift_Y, drift_Z;
		uint32_t faults;
		float D12, D13, D21, D23, D31, D32;
		float Mg1, Mg2, Mg3;
		float Wo1, Wo2, Wo3;
		float E12, E13, E21, E23, E31, E32;
		float Ma1, Ma2, Ma3;
		float Ao1, Ao2, Ao3;
		uint32_t reserve, crc8, error;
	};

	enum class extension
	{
		DAT,
		TXT
	};

	bool operator == (extension, extension);
	bool operator == (const std::string_view, extension);

	class file
	{
	public:
		file() = default;
	public:
		template <extension>
		bool load(const std::string_view);
		template <extension>
		bool save(const std::string_view);
		const std::vector<row> & get_data() const;
	private:
		std::vector<row> m_data;
	};
}

template <>
struct std::formatter<ws::data::extension> : std::formatter<std::string_view>
{
	template <typename T>
	auto format(ws::data::extension extension, T & t)
	{
		return extension == ws::data::extension::DAT
			? std::formatter<std::string_view>::format("DAT", t)
			: std::formatter<std::string_view>::format("TXT", t);
	}
};