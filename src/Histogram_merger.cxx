#include "../interface/Histogram_merger.h"

// Example at: https://github.com/hunyadix/PhaseIEfficiencyPlotter/blob/master/src/programs/mergeSimhitPlots.cc

Histogram_merger_program::Histogram_merger_program(Histogram_merger_options&& t_options):
	m_input_file_path_list(t_options.input_file_path_list),
	m_output_file_path(t_options.output_file_path)
{
	std::for_each(m_input_file_path_list.begin(), m_input_file_path_list.end(), [&] (const std::string& input_file_name)
	{
		m_input_files.emplace_back(TFile::Open(input_file_name.c_str()));
		if(m_input_files.back() == nullptr)
		{
			std::cout << "Missing file: " << input_file_name << std::endl;
			e_file_missing();
		}
	});
	m_output_file = TFile::Open(m_output_file_path.c_str(), "RECREATE");
	fetch_histograms_from_input_files();
	m_output_file -> Write();
}

// Generate a list of histograms in each of the nested directories in order to handle the data
void Histogram_merger_program::fetch_histograms_from_input_files()
{
	std::for_each(m_input_files.begin(), m_input_files.end(), [&] (const auto& file)
	{
		file -> cd();
		std::cout << "\n --- Listing keys in file: " << file -> GetName() << " ---\n";
		this -> handle_data_object({}, file);
		// for(auto&& key: *(file -> GetListOfKeys()))
		// {
		// 	handle_data_object(gDirectory -> Get(key -> GetName()));
		// }
	});
}

// Handle data object located in path: loop on it if directory, copy if histo
void Histogram_merger_program::handle_data_object(const std::vector<std::string>& t_path, TObject* t_object)
{
	TDirectory* directory = dynamic_cast<TDirectory*>(t_object);
	if(directory)
	{
		std::string directory_name = directory -> GetName();
		const std::vector<std::string> new_path { append_to_path_copy(t_path, directory_name) };
		std::cout << "Looping on directory: " << directory_name << '\n'; 
		for(auto&& key: *(directory -> GetListOfKeys()))
		{
			std::cout << "Key name: " << key -> GetName() << std::endl;
			handle_data_object(new_path, directory -> Get(key -> GetName()));
		}
		return;
	}
	TH1* histogram = dynamic_cast<TH1*>(t_object);
	{
		std::cout << "Handling histogram: " << t_object -> GetName() << '\n';
		copy_histogram_to_output_file(t_path, histogram);
	}
}

std::vector<std::string> Histogram_merger_program::append_to_path_copy(const std::vector<std::string>& t_path, const std::string& t_directory)
{
	std::vector<std::string> res(t_path);
	res.push_back(t_directory);	
	return res;
}

void Histogram_merger_program::copy_histogram_to_output_file(const std::vector<std::string>& t_path, TH1* t_histogram)
{
	m_output_file -> cd();
	std::for_each(t_path.begin(), t_path.end(), [&] (const auto& directory_name)
	{
		if(gDirectory -> GetDirectory(directory_name.c_str()) == nullptr)
		{
			std::cout << "Creating " << directory_name << std::endl;
			gDirectory -> mkdir(directory_name.c_str());
		}
		std::cout << "Entering " << directory_name << std::endl;
		gDirectory -> cd(directory_name.c_str());
		gDirectory -> pwd();
	});
	std::string histogram_name = t_histogram -> GetName();
	TH1* histogram_in_output = dynamic_cast<TH1*>(gDirectory -> Get(histogram_name.c_str()));
	if(histogram_in_output)
	{
		std::cout << "Adding histogram to a previously existing one: " << histogram_name << '\n';
		histogram_in_output -> Add(t_histogram);
	}
	else
	{
		std::cout << "Creating new histogram: " << histogram_name << '\n';
		// gDirectory -> WriteObject(t_histogram);
		t_histogram -> Write();
	}
}