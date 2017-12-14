#include "../interface/Histogram_merger.h"

#include <memory>
// #include <optional>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	std::cout << "Executing histogram merger..." << std::endl;
	Histogram_merger_options options = 
	{
		.input_file_path_list = 
		{
			"/data/gridout/ahunyadi/SCS/Run2_CMSSW93X/SCS_93X_mcRun2_DynIneff_95_GEN_SIM_RAWTODIGI_evt100000/171212_101805/0000/Ntuple_1.root",
			"/data/gridout/ahunyadi/SCS/Run2_CMSSW93X/SCS_93X_mcRun2_DynIneff_95_GEN_SIM_RAWTODIGI_evt100000/171212_101805/0000/Ntuple_2.root"
		},
		.output_file_path     = "Ntuple_merged.root"
	};
	auto program = std::make_unique<Histogram_merger_program>(std::move(options));
	std::cout << "Done executing histogram merger..." << std::endl;
	return 0; 
}
