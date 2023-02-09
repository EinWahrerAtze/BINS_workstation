//
//  file.cpp
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#include <format>
#include <fstream>
#include "file.h"

namespace ws::data
{
	bool operator == (extension lhs, extension rhs)
	{
		return static_cast<int>(lhs) == static_cast<int>(rhs);
	}

	bool operator == (const std::string_view string, extension extension)
	{
		return (extension == extension::DAT ? string == ".dat" : string == ".txt");
	}

	// read .dat file
	template<>
	bool file::load<extension::DAT>(const std::string_view filename)
	{
		std::ifstream fin {filename.data(), std::ios_base::binary | std::ios_base::in};
		if (!fin.is_open()) { return false; }
		// find starting line to begin reading
		// raw input data before line 60 very unstable and not required for later analysis
		// 301 is lenght of single line in bytes
		uint32_t row_count {};
		constexpr uint32_t starting_row {60};
		while (!fin.eof())
		{
			fin.read(reinterpret_cast<char *>(&row_count), 2);
			if (row_count < starting_row)
			{
				fin.seekg(fin.tellg() += 299);
			}
			else
			{
				// the file pointer is now set at line 60 and so we can read proper data
				fin.seekg(fin.tellg() -= 2);
				break;
			}
		}
		// the file has no proper content or too short -- could not be used
		if (fin.bad() || row_count < starting_row) { return false; }
		// continue reading input data line by line
		ws::data::row row {};
		m_data.reserve(1200);
		while (fin.good())
		{
			fin.read(reinterpret_cast<char *>(&row.count), 2);
			fin.read(reinterpret_cast<char *>(&row.mode), 2);
			fin.read(reinterpret_cast<char *>(&row.system_time), 4);
			fin.read(reinterpret_cast<char *>(&row.mode_time), 4);
			fin.read(reinterpret_cast<char *>(&row.pitch), 4);
			fin.read(reinterpret_cast<char *>(&row.roll), 4);
			fin.read(reinterpret_cast<char *>(&row.heading), 4);
			fin.read(reinterpret_cast<char *>(&row.azimuth), 4);
			fin.read(reinterpret_cast<char *>(&row.thdg), 4);
			fin.read(reinterpret_cast<char *>(&row.latitude), 4);
			fin.read(reinterpret_cast<char *>(&row.longtitude), 4);
			fin.read(reinterpret_cast<char *>(&row.H), 4);
			fin.read(reinterpret_cast<char *>(&row.Ve), 4);
			fin.read(reinterpret_cast<char *>(&row.Vn), 4);
			fin.read(reinterpret_cast<char *>(&row.Vu), 4);
			fin.read(reinterpret_cast<char *>(&row.dAt_X), 4);
			fin.read(reinterpret_cast<char *>(&row.dAt_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.dAt_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.dVt_X), 4);
			fin.read(reinterpret_cast<char *>(&row.dVt_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.dVt_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.gyro_X), 4);
			fin.read(reinterpret_cast<char *>(&row.gyro_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.gyro_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.acc_X), 4);
			fin.read(reinterpret_cast<char *>(&row.acc_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.acc_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.U_cplc_X), 4);
			fin.read(reinterpret_cast<char *>(&row.U_cplc_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.U_cplc_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.U_hfo_X), 4);
			fin.read(reinterpret_cast<char *>(&row.U_hfo_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.U_hfo_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.F_out_X), 4);
			fin.read(reinterpret_cast<char *>(&row.F_out_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.F_out_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.F_dith_X), 4);
			fin.read(reinterpret_cast<char *>(&row.F_dith_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.F_dith_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.gyro_X_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.gyro_Y_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.gyro_Z_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.acc_X_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.acc_Y_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.acc_Z_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.dpb_X_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.dpb_Y_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.dpb_Z_temperature), 4);
			fin.read(reinterpret_cast<char *>(&row.drift_X), 4);
			fin.read(reinterpret_cast<char *>(&row.drift_Y), 4);
			fin.read(reinterpret_cast<char *>(&row.drift_Z), 4);
			fin.read(reinterpret_cast<char *>(&row.faults), 2);
			fin.read(reinterpret_cast<char *>(&row.D12), 4);
			fin.read(reinterpret_cast<char *>(&row.D13), 4);
			fin.read(reinterpret_cast<char *>(&row.D21), 4);
			fin.read(reinterpret_cast<char *>(&row.D23), 4);
			fin.read(reinterpret_cast<char *>(&row.D31), 4);
			fin.read(reinterpret_cast<char *>(&row.D32), 4);
			fin.read(reinterpret_cast<char *>(&row.Mg1), 4);
			fin.read(reinterpret_cast<char *>(&row.Mg2), 4);
			fin.read(reinterpret_cast<char *>(&row.Mg3), 4);
			fin.read(reinterpret_cast<char *>(&row.Wo1), 4);
			fin.read(reinterpret_cast<char *>(&row.Wo2), 4);
			fin.read(reinterpret_cast<char *>(&row.Wo3), 4);
			fin.read(reinterpret_cast<char *>(&row.E12), 4);
			fin.read(reinterpret_cast<char *>(&row.E13), 4);
			fin.read(reinterpret_cast<char *>(&row.E21), 4);
			fin.read(reinterpret_cast<char *>(&row.E23), 4);
			fin.read(reinterpret_cast<char *>(&row.E31), 4);
			fin.read(reinterpret_cast<char *>(&row.E32), 4);
			fin.read(reinterpret_cast<char *>(&row.Ma1), 4);
			fin.read(reinterpret_cast<char *>(&row.Ma2), 4);
			fin.read(reinterpret_cast<char *>(&row.Ma2), 4);
			fin.read(reinterpret_cast<char *>(&row.Ao1), 4);
			fin.read(reinterpret_cast<char *>(&row.Ao2), 4);
			fin.read(reinterpret_cast<char *>(&row.Ao3), 4);
			fin.read(reinterpret_cast<char *>(&row.reserve), 1);
			fin.read(reinterpret_cast<char *>(&row.crc8), 1);
			fin.read(reinterpret_cast<char *>(&row.error), 1);
			m_data.push_back(row);
		}
		if (!fin.eof()) { return false; }
		fin.close();
		m_data.shrink_to_fit();
		return true;
	}

	// read .txt file
	template<>
	bool file::load<extension::TXT>(const std::string_view filename)
	{
		std::ifstream fin {filename.data(), std::ios_base::in};
		if (!fin.is_open()) { return false; }
		uint32_t row_count {};
		constexpr uint32_t starting_row {60};
		while (fin >> row_count)
		{
			if (row_count < starting_row)
			{
				fin.ignore(std::numeric_limits <std::streamsize>::max(), '\n');
			}
			else
			{
				fin.seekg(fin.tellg() -= 2);
				break;
			}
		}
		if (fin.bad() || row_count < starting_row) { return false; }
		ws::data::row row {};
		m_data.reserve(1200);
		while (fin.good())
		{
			fin >> row.count;
			fin >> row.mode;
			fin >> row.system_time;
			fin >> row.mode_time;
			fin >> row.pitch;
			fin >> row.roll;
			fin >> row.heading;
			fin >> row.azimuth;
			fin >> row.thdg;
			fin >> row.latitude;
			fin >> row.longtitude;
			fin >> row.H;
			fin >> row.Ve >> row.Vn >> row.Vu;
			fin >> row.dAt_X >> row.dAt_Y >> row.dAt_Z;
			fin >> row.dVt_X >> row.dVt_Y >> row.dVt_Z;
			fin >> row.gyro_X >> row.gyro_Y >> row.gyro_Z;
			fin >> row.acc_X >> row.acc_Y >> row.acc_Z;
			fin >> row.U_cplc_X >> row.U_cplc_Y >> row.U_cplc_Z;
			fin >> row.U_hfo_X >> row.U_hfo_Y >> row.U_hfo_Z;
			fin >> row.F_out_X >> row.F_out_Y >> row.F_out_Z;
			fin >> row.F_dith_X >> row.F_dith_Y >> row.F_dith_Z;
			fin >> row.gyro_X_temperature >> row.gyro_Y_temperature >> row.gyro_Z_temperature;
			fin >> row.acc_X_temperature >> row.acc_Y_temperature >> row.acc_Z_temperature;
			fin >> row.dpb_X_temperature >> row.dpb_Y_temperature >> row.dpb_Z_temperature;
			fin >> row.drift_X >> row.drift_Y >> row.drift_Z;
			fin >> row.faults;
			fin >> row.D12 >> row.D13 >> row.D21 >> row.D23 >> row.D31 >> row.D32;
			fin >> row.Mg1 >> row.Mg2 >> row.Mg3;
			fin >> row.Wo1 >> row.Wo2 >> row.Wo3;
			fin >> row.E12 >> row.E13 >> row.E21 >> row.E23 >> row.E31 >> row.E32;
			fin >> row.Ma1 >> row.Ma2 >> row.Ma3;
			fin >> row.Ao1 >> row.Ao2 >> row.Ao3;
			fin >> row.reserve;
			fin >> row.crc8;
			fin >> row.error;
			m_data.push_back(row);
		}
		if (!fin.eof()) { return false; }
		fin.close();
		m_data.shrink_to_fit();
		return true;
	}

	// save read data as .dat file -- will be used later in future
	template<>
	bool file::save<extension::DAT>(const std::string_view filename)
	{
		std::ofstream fout {filename.data(), std::ios_base::out | std::ios_base::binary};
		if (!fout.is_open()) { return false; }
		for (ws::data::row & row : m_data)
		{
			fout.write(reinterpret_cast<char *>(&row.count), 2);
			fout.write(reinterpret_cast<char *>(&row.mode), 2);
			fout.write(reinterpret_cast<char *>(&row.system_time), 4);
			fout.write(reinterpret_cast<char *>(&row.mode_time), 4);
			fout.write(reinterpret_cast<char *>(&row.pitch), 4);
			fout.write(reinterpret_cast<char *>(&row.roll), 4);
			fout.write(reinterpret_cast<char *>(&row.heading), 4);
			fout.write(reinterpret_cast<char *>(&row.azimuth), 4);
			fout.write(reinterpret_cast<char *>(&row.thdg), 4);
			fout.write(reinterpret_cast<char *>(&row.latitude), 4);
			fout.write(reinterpret_cast<char *>(&row.longtitude), 4);
			fout.write(reinterpret_cast<char *>(&row.H), 4);
			fout.write(reinterpret_cast<char *>(&row.Ve), 4);
			fout.write(reinterpret_cast<char *>(&row.Vn), 4);
			fout.write(reinterpret_cast<char *>(&row.Vu), 4);
			fout.write(reinterpret_cast<char *>(&row.dAt_X), 4);
			fout.write(reinterpret_cast<char *>(&row.dAt_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.dAt_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.dVt_X), 4);
			fout.write(reinterpret_cast<char *>(&row.dVt_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.dVt_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.gyro_X), 4);
			fout.write(reinterpret_cast<char *>(&row.gyro_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.gyro_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.acc_X), 4);
			fout.write(reinterpret_cast<char *>(&row.acc_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.acc_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.U_cplc_X), 4);
			fout.write(reinterpret_cast<char *>(&row.U_cplc_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.U_cplc_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.U_hfo_X), 4);
			fout.write(reinterpret_cast<char *>(&row.U_hfo_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.U_hfo_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.F_out_X), 4);
			fout.write(reinterpret_cast<char *>(&row.F_out_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.F_out_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.F_dith_X), 4);
			fout.write(reinterpret_cast<char *>(&row.F_dith_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.F_dith_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.gyro_X_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.gyro_Y_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.gyro_Z_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.acc_X_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.acc_Y_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.acc_Z_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.dpb_X_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.dpb_Y_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.dpb_Z_temperature), 4);
			fout.write(reinterpret_cast<char *>(&row.drift_X), 4);
			fout.write(reinterpret_cast<char *>(&row.drift_Y), 4);
			fout.write(reinterpret_cast<char *>(&row.drift_Z), 4);
			fout.write(reinterpret_cast<char *>(&row.faults), 2);
			fout.write(reinterpret_cast<char *>(&row.D12), 4);
			fout.write(reinterpret_cast<char *>(&row.D13), 4);
			fout.write(reinterpret_cast<char *>(&row.D21), 4);
			fout.write(reinterpret_cast<char *>(&row.D23), 4);
			fout.write(reinterpret_cast<char *>(&row.D31), 4);
			fout.write(reinterpret_cast<char *>(&row.D32), 4);
			fout.write(reinterpret_cast<char *>(&row.Mg1), 4);
			fout.write(reinterpret_cast<char *>(&row.Mg2), 4);
			fout.write(reinterpret_cast<char *>(&row.Mg3), 4);
			fout.write(reinterpret_cast<char *>(&row.Wo1), 4);
			fout.write(reinterpret_cast<char *>(&row.Wo2), 4);
			fout.write(reinterpret_cast<char *>(&row.Wo3), 4);
			fout.write(reinterpret_cast<char *>(&row.E12), 4);
			fout.write(reinterpret_cast<char *>(&row.E13), 4);
			fout.write(reinterpret_cast<char *>(&row.E21), 4);
			fout.write(reinterpret_cast<char *>(&row.E23), 4);
			fout.write(reinterpret_cast<char *>(&row.E31), 4);
			fout.write(reinterpret_cast<char *>(&row.E32), 4);
			fout.write(reinterpret_cast<char *>(&row.Ma1), 4);
			fout.write(reinterpret_cast<char *>(&row.Ma2), 4);
			fout.write(reinterpret_cast<char *>(&row.Ma2), 4);
			fout.write(reinterpret_cast<char *>(&row.Ao1), 4);
			fout.write(reinterpret_cast<char *>(&row.Ao2), 4);
			fout.write(reinterpret_cast<char *>(&row.Ao3), 4);
			fout.write(reinterpret_cast<char *>(&row.reserve), 1);
			fout.write(reinterpret_cast<char *>(&row.crc8), 1);
			fout.write(reinterpret_cast<char *>(&row.error), 1);
		}
		if (fout.bad()) { return false; }
		fout.close();
		return true;
	}

	// save data as .txt file -- used when converting files from .dat to .txt format
	template<>
	bool file::save<extension::TXT>(const std::string_view filename)
	{
		std::ofstream fout {filename.data(), std::ios_base::out};
		if (!fout.is_open()) { return false; }
		for (const ws::data::row & row : m_data)
		{
			fout << std::format("{}\t", row.count);
			fout << std::format("{}\t", row.mode);
			fout << std::format("{}\t", row.system_time);
			fout << std::format("{}\t", row.mode_time);
			fout << std::format("{: .5f}\t", row.pitch);
			fout << std::format("{: .5f}\t", row.roll);
			fout << std::format("{: .2f}\t", row.heading);
			fout << std::format("{: .5f}\t", row.azimuth);
			fout << std::format("{: .5f}\t", row.thdg);
			fout << std::format("{: .5f}\t", row.latitude);
			fout << std::format("{: .5f}\t", row.longtitude);
			fout << std::format("{: .2f}\t", row.H);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.Ve, row.Vn, row.Vu);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.dAt_X, row.dAt_Y, row.dAt_Z);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.dVt_X, row.dVt_Y, row.dVt_Z);
			fout << std::format("{: .2f}\t{: .2f}\t{: .2f}\t", row.gyro_X, row.gyro_Y, row.gyro_Z);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.acc_X, row.acc_Y, row.acc_Z);
			fout << std::format("{: .2f}\t{: .2f}\t{: .2f}\t", row.U_cplc_X, row.U_cplc_Y, row.U_cplc_Z);
			fout << std::format("{: .2f}\t{: .2f}\t{: .2f}\t", row.U_hfo_X, row.U_hfo_Y, row.U_hfo_Z);
			fout << std::format("{: .1f}\t{: .1f}\t{: .1f}\t", row.F_out_X, row.F_out_Y, row.F_out_Z);
			fout << std::format("{: .1f}\t{: .1f}\t{: .1f}\t", row.F_dith_X, row.F_dith_Y, row.F_dith_Z);
			fout << std::format("{}\t{}\t{}\t", row.gyro_X_temperature, row.gyro_Y_temperature, row.gyro_Z_temperature);
			fout << std::format("{}\t{}\t{}\t", row.acc_X_temperature, row.acc_Y_temperature, row.acc_Z_temperature);
			fout << std::format("{}\t{}\t{}\t", row.dpb_X_temperature, row.dpb_Y_temperature, row.dpb_Z_temperature);
			fout << std::format("{: .0f}\t{: .0f}\t{: .0f}\t", row.drift_X, row.drift_Y, row.drift_Z);
			fout << std::format("{}\t", row.faults);
			fout << std::format("{: .2f}\t{: .2f}\t{: .2f}\t{: .2f}\t{: .2f}\t{: .2f}\t", row.D12, row.D13, row.D21, row.D23, row.D31, row.D32);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.Mg1, row.Mg2, row.Mg3);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.Wo1, row.Wo2, row.Wo3);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t{: .5f}\t{: .5f}\t{: .5f}\t", row.E12, row.E13, row.E21, row.E23, row.E31, row.E32);
			fout << std::format("{: .1f}\t{: .1f}\t{: .5f}\t", row.Ma1, row.Ma2, row.Ma3);
			fout << std::format("{: .5f}\t{: .5f}\t{: .5f}\t", row.Ao1, row.Ao2, row.Ao3);
			fout << std::format("{}\t", row.reserve);
			fout << std::format("{}\t", row.crc8);
			fout << std::format("{}\n", row.error);
		}
		if (fout.bad()) { return false; }
		fout.close();
		return true;
	}

	const std::vector<row> & file::get_data() const
	{
		return m_data;
	}
}
