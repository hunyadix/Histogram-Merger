#include "../interface/Histogram_merger.h"

// Example at: https://github.com/hunyadix/PhaseIEfficiencyPlotter/blob/master/src/programs/mergeSimhitPlots.cc

Histogram_merger_program::Histogram_merger_program(Histogram_merger_options&& t_options):
	m_input_file_path_list(t_options.input_file_path_list),
	m_output_file_path(t_options.output_file_path),
	m_save_histograms_as_images(t_options.save_histograms_as_images),
	m_histogram_save_directory(t_options.histogram_save_directory),
	m_histogram_image_format(t_options.histogram_image_format)
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
	if(m_save_histograms_as_images) save_all_histograms_as_images();
}

// Generate a list of histograms in each of the nested directories in order to handle the data
void Histogram_merger_program::fetch_histograms_from_input_files()
{
	std::for_each(m_input_files.begin(), m_input_files.end(), [&] (const auto& file)
	{
		file -> cd();
		std::cout << "\n --- Listing keys in file: " << file -> GetName() << " ---\n";
		this -> handle_data_object({}, file);
	});
}

// Handle data object located in path: loop on it if directory, copy if histo
// This seemed easier to implement recursively than sequencially - could be subject to heavy optimizations (if anyone cared enough :))
void Histogram_merger_program::handle_data_object(const std::vector<std::string>& t_path, const TObject* t_object)
{
	const TDirectory* directory = dynamic_cast<const TDirectory*>(t_object);
	if(directory)
	{
		std::string directory_name = directory -> GetName();
		const std::vector<std::string> new_path { append_to_path_copy(t_path, directory_name) };
		std::cout << "Looping on directory: " << directory_name << '\n'; 
		for(auto&& key: *(directory -> GetListOfKeys()))
		{
			std::cout << "Key name: " << key -> GetName() << std::endl;
			const TObject* new_object = const_cast<TDirectory*>(directory) -> Get(key -> GetName());
			handle_data_object(new_path, new_object);
		}
		return;
	}
	const TH1* histogram = dynamic_cast<const TH1*>(t_object);
	if(histogram)
	{
		std::cout << "Handling histogram: " << t_object -> GetName() << '\n';
		copy_histogram_to_output_file(t_path, histogram);
	}
}

void Histogram_merger_program::copy_histogram_to_output_file(const std::vector<std::string>& t_path, const TH1* t_histogram)
{
	if(t_path.size() == 0) return; // Should not happen, is at least expected to contain the file name
	m_output_file -> cd();
	// Adding + 1 to begin to omit the filename
	std::for_each(t_path.begin() + 1, t_path.end(), [&] (const auto& directory_name)
	{
		if(gDirectory -> GetDirectory(directory_name.c_str()) == nullptr)
		{
			std::cout << "Creating " << directory_name << '\n';
			gDirectory -> mkdir(directory_name.c_str());
		}
		std::cout << "Entering " << directory_name << std::endl;
		gDirectory -> cd(directory_name.c_str());
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

std::vector<std::string> Histogram_merger_program::append_to_path_copy(const std::vector<std::string>& t_path, const std::string& t_directory)
{
	std::vector<std::string> res(t_path);
	res.push_back(t_directory);	
	return res;
}

void Histogram_merger_program::save_all_histograms_as_images()
{
	std::cout << "Saving histograms...\n";
	std::stack<TObject*> objects_to_handle(std::deque<TObject*>{ m_output_file });
	while(objects_to_handle.size())
	{
		const TObject* object = objects_to_handle.top();
		objects_to_handle.pop();
		const TDirectory* directory = dynamic_cast<const TDirectory*>(object);
		if(directory)
		{
			std::cout << "Entering directory " << directory -> GetName() << '\n';
			for(auto&& key: *(directory -> GetListOfKeys()))
			{
				objects_to_handle.emplace(const_cast<TDirectory*>(directory) -> Get(key -> GetName()));
			}
			continue;
		}
		const TH1* histogram = dynamic_cast<const TH1*>(object);
		if(histogram)
		{
			TCanvas* canvas = new TCanvas(); // Not even bothering with trying to manage memory correctly
			canvas -> cd();
			const_cast<TH1*>(histogram) -> Draw();
			canvas -> SaveAs((m_histogram_save_directory + '/' + histogram -> GetName() + '.' + m_histogram_image_format).c_str());
		}
	}
	std::cout << "Done saving histograms.\n";
}
