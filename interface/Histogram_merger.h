#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <type_traits>

#include "TFile.h"
#include "TH1.h"

struct Histogram_merger_options
{
	Histogram_merger_options(const Histogram_merger_options&) = delete; // Move only (or copy assign explicitly)
	std::vector<std::string> input_file_path_list;
	std::string output_file_path;
};

class Histogram_merger_program
{
	public:
		// Histogram_merger_program() = delete;
		Histogram_merger_program(Histogram_merger_options&& t_options);
		~Histogram_merger_program() {};
		void fetch_histograms_from_input_files();
		void handle_data_object(const std::vector<std::string>& t_path, TObject* t_object);
		void copy_histogram_to_output_file(const std::vector<std::string>& t_path, TH1* histogram);
		static std::vector<std::string> append_to_path_copy(const std::vector<std::string>& t_path, const std::string& t_directory);
	private:
		std::vector<std::string> m_input_file_path_list;
		std::string m_output_file_path;
		std::vector<TFile*> m_input_files;
		TFile* m_output_file;
		[[noreturn, gnu::cold]] static void e_file_missing() { throw std::runtime_error("Input file is missing."); }
};
