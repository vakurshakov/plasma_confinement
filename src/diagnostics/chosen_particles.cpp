#include "chosen_particles.hpp"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "../file_writers/bin_file.hpp"
#include "../particles/particle/particle.hpp"
#include "../constants.h"


// Будем отдельно прицеплять эту диагностику
std::vector<int> choose_indexes(const std::vector<Particle>& particles)
{
    std::vector<int> indexes_of_chosen_particles;
    
    for(decltype(particles.size()) i = 0; i < particles.size(); i += Npe)
    {
        auto& point = particles[i].get_point();
        
        double x = round(point.x() / dx);
        double y = round(point.y() / dy);

        if(fabs(x - y) < 1e-6)
        {
            indexes_of_chosen_particles.push_back(i);
        }
    }
    
    return indexes_of_chosen_particles;
}


chosen_particles::chosen_particles(std::string directory_path, std::vector<int> indexes_of_chosen_particles)
    :   Particles_diagnostic(directory_path),
        indexes_of_chosen_particles_(std::move(indexes_of_chosen_particles))
    {
        this->save_parameters(directory_path);
        
        for(int i = 0; i < indexes_of_chosen_particles_.size(); ++i)
        {
            files_for_results_.emplace_back(std::make_unique<BIN_File>(directory_path_, to_string(i) + "_particle"));
        }
    }

void chosen_particles::save_parameters(std::string directory_path)
{
    namespace fs = std::filesystem;

    fs::create_directories(fs::path(directory_path));
    std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
    diagnostic_parameters_ << "#there are " << indexes_of_chosen_particles_.size() << " choosen particles" << std::endl;
    diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}

void chosen_particles::diagnose(const Parameters&, const std::vector<Particle>& particles, int t)
{   
    // Будем смотреть каждый шаг за частицами
    // if ((t % diagnose_time_step) == 0)
    // {
        int ith_particle = 0;
        for(const auto& i : indexes_of_chosen_particles_)
        {
            const Point& ith_point = particles[i].get_point(); 

            files_for_results_[ith_particle]->write(ith_point.x() / dx);
            files_for_results_[ith_particle]->write(ith_point.y() / dy);

            const vector3& p = ith_point.p();

            files_for_results_[ith_particle]->write(p.x());
            files_for_results_[ith_particle]->write(p.y());
            files_for_results_[ith_particle]->write(p.z());
            
            double x = ith_point.x() - 0.5 * SIZE_X * dx;
            double y = ith_point.y() - 0.5 * SIZE_Y * dy;
            double r = sqrt(x * x + y * y);

            double p_r = p.x() * (+x / r) + p.y() * (+y / r);
	        double p_a = p.x() * (-y / r) + p.y() * (+x / r);

            files_for_results_[ith_particle]->write(p_r  );
            files_for_results_[ith_particle]->write(p_a  );
            files_for_results_[ith_particle]->write(p.z());

            files_for_results_[ith_particle]->flush();
            ith_particle++;
        }
    // }
}
