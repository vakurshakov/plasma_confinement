#include "chosen_particles.hpp"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "../file_writers/bin_file.hpp"
#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../constants.h"

namespace fs = std::filesystem;


std::vector<int> choose_indexes()
{
    std::vector<int> indexes_of_chosen_particles;

    /*     for (decltype(points.size()) i = 0; i < points.size(); i += Npe) {
            int nx = int(roundf(points[i].x()/dx));
            int ny = int(roundf(points[i].y()/dy));

            if (nx == ny)
                indexes_of_chosen_particles.emplace_back(i);
        }
    */

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
    fs::create_directories(fs::path(directory_path));
    std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
    diagnostic_parameters_ << "#there are N particles" << std::endl;
    diagnostic_parameters_ << indexes_of_chosen_particles_.size() << std::endl;
    diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}

void chosen_particles::diagnose(const Particle_parameters& parameters, const std::vector<Point>& points, int t)
{
    if ((t % diagnose_time_step) == 0) {

    file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

    for(const auto& i : indexes_of_chosen_particles_) {

        file_for_results_->write(points[i].x()/dx);
        file_for_results_->write(points[i].y()/dy);

        file_for_results_->write(points[i].px());
        file_for_results_->write(points[i].py());
        file_for_results_->write(points[i].pz());
    }
}}