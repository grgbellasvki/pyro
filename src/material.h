/* Define a material.
*/ 

#ifndef ICARUSPYRO_MATERIAL_H
#define ICARUSPYRO_MATERIAL_H

#include <string>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace IcarusPyro { 

class Material
{
 public:

    /** 
     * Construct object from data file
     */
    Material(const std::string database); 

 	/* Desctructor
 	 */
 	~Material() {}

    std::string get_name() const { 
        return name;
    }

    void set_state(const double temperature, const double pressure, const double density) {
        T_state = temperature;
        p_state = pressure;
        rho_state = density;
        Yv_state = virgin_mass_fraction(density);
        beta_state = decomposition_fraction(density);
    }

    void set_pressure(const double pressure) { 
        p_state = pressure;
    }

    void set_density(const double density) {
        rho_state = density;
        Yv_state = virgin_mass_fraction(density);
        beta_state = decomposition_fraction(density);
    }

    double enthalpy();

    void computeEnthalpy(const std::vector<double>& temperature, 
                         const std::vector<double>& pressure, 
                         const std::vector<double>& virgin_mass_fraction, 
                         std::vector<double>& h);

    void computeEnthalpy(const std::vector<double>& temperature, 
                         const std::vector<double>& virgin_mass_fraction, 
                         std::vector<double>& h);

    void computeEnthalpy(const std::vector<double>& temperature, 
                         std::vector<double>& h);

    void read_database(const char* datafile);

 private:

    double decomposition_fraction(const double density) const {
        return (rho_v - density) / (rho_v - rho_c);
    }

    double virgin_mass_fraction(const double density) const {
        return rho_v / (rho_v - rho_c) * (1.0 - rho_c / density);
    }

    double p_state; 
    double T_state;
    double rho_state;
    double Yv_state;
    double beta_state;

    bool pyrolyzing;
    std::string name;
    std::string state_model;
    double rho_v;
    double rho_c;
};

} // namespace IcarusPyro

#endif