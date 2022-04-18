#include "chosen_particles.hpp"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "../file_writers/bin_file.hpp"
#include "../particles/particle/particle.hpp"
#include "../constants.h"


std::vector<int> choose_indexes()
{
    std::vector<int> indexes_of_chosen_particles;

    return indexes_of_chosen_particles;
}


chosen_particles::chosen_particles(std::string directory_path, std::vector<int> indexes_of_chosen_particles)
    :   Particles_diagnostic(directory_path),
        indexes_of_chosen_particles_(std::move(indexes_of_chosen_particles))
    {
        this->save_parameters(directory_path);
    }

void chosen_particles::save_parameters(std::string directory_path)
{
    namespace fs = std::filesystem;

    fs::create_directories(fs::path(directory_path));
    std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
    diagnostic_parameters_ << "#there are N particles" << std::endl;
    diagnostic_parameters_ << indexes_of_chosen_particles_.size() << std::endl;
    diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}

void chosen_particles::diagnose(const Parameters&, const std::vector<Particle>& particles, int t)
{
    if ((t % diagnose_time_step) == 0)
    {
        file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));
        for(const auto& i : indexes_of_chosen_particles_)
        {
            const Point& ith_point = particles[i].get_point(); 

            file_for_results_->write(ith_point.x() / dx);
            file_for_results_->write(ith_point.y() / dy);

            file_for_results_->write(ith_point.px());
            file_for_results_->write(ith_point.py());
            file_for_results_->write(ith_point.pz());
        }
    }
}