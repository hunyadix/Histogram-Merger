#pragma once

#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <iostream>
#include <type_traits>

#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"

struct Histogram_merger_options
{
	Histogram_merger_options(const Histogram_merger_options&) = delete; // Move only (or copy assign explicitly)
	std::vector<std::string> input_file_path_list;
	std::string output_file_path;
	bool save_histograms_as_images;
	std::string histogram_save_directory;
	std::string histogram_image_format;
};

class Histogram_merger_program
{
	public:
		// Histogram_merger_program() = delete;
		Histogram_merger_program(Histogram_merger_options&& t_options);
		~Histogram_merger_program() {};
		void fetch_histograms_from_input_files();
		void handle_data_object(const std::vector<std::string>& t_path, const TObject* t_object);
		void copy_histogram_to_output_file(const std::vector<std::string>& t_path, const TH1* histogram);
		void save_all_histograms_as_images();
		static std::vector<std::string> append_to_path_copy(const std::vector<std::string>& t_path, const std::string& t_directory);
	private:
		[[noreturn, gnu::cold]] static void e_file_missing() { throw std::runtime_error("Input file is missing."); }
	private:
		std::vector<std::string> m_input_file_path_list;
		std::string m_output_file_path;
		std::vector<TFile*> m_input_files;
		TFile* m_output_file;
		bool m_save_histograms_as_images;
		std::string m_histogram_save_directory; 
		std::string m_histogram_image_format; 
};