#ifndef ICARUSPYRO_GAS_H
#define ICARUSPYRO_GAS_H

#include <string>
#include <vector>

#include "gas_table.h"

namespace Mutation {class Mixture;}

namespace IcarusPyro {

class GasMixture {
public:

    /**
     * Constructor for the pyrolysis gas mixture object.
     *
     * Upon return, the Mutation++ objects are initialized, and all the mixture
     * properties at each point in the temperature and pressure ranges are 
     * computed.
     *
     * @param[in] pyrolysis_gas_mixture The name of the Mutation++ mixture file.
     * @param[in] T_low Lowest value in the temperature range. Default is 200.
     * @param[in] T_high Highest value in the temperature range. Default is 4000.
     * @param[in] nT Number of discrete temperature points within range. Default
     *     is 76. Provides 50 K intervals.
     * @param[in] T_scale Temperature range distributed either on a linear or
     *     log scale. Default is linear.
     * @param[in] p_low Lowest value in the pressure range. Default is 1.01325.
     * @param[in] p_high Highest value in the pressure range. Default is 1013250.0
     * @param[in] nP Number of discrete pressure points within range. Default
     *     is 6. Assumeds logarthmic scaling.
     * @param[in] p_scale Pressure range distributed either on a linear or log
     *     scale. Default is log10.
     * @param[in] mu_algorithm Method used to compute the viscosity. Default
     *     is Chapmann-Enskog.
     * @param[in] k_algorithm Method used to compute the thermal conductivity.
     *     Default is Wilke. Not used at this time.
     */
    GasMixture(
        const std::string& pyrolysis_gas_mixture,
        const double& T_low = 200.0,
        const double& T_high = 4000.0,
        const int& nT = 76,
        const std::string& T_scale = "linear",
        const double& p_low = 1.01325,
        const double& p_high = 1013250.0,
        const int& nP = 6,
        const std::string& p_scale = "log10",
        const std::string& mu_algorithm = "Chapmann-Enskog_CG",
        const std::string& k_algorithm = "Wilke",
        const std::string& state_model = "Equil");

    /**
     * Destructor
     */
    ~GasMixture();

    /**
     * Define a temperature at which to compute the equilibrium
     * pyrolysis gas mixture properties.
     *
     * @param[in] T Temperature of the pyrolysis gas mixture.
     */
    void setTemperature(const double& T) {
        m_nT = 1;
        temperature.resize(m_nT);
        temperature[0] = T;
    }

    /**
     * Define the range of temperature values at which to compute the equilibrium
     * pyrolysis gas mixture properties.
     *
     * @param[in] low Lowest value in the temperature range.
     * @param[in] high Highest value in the temperature range.
     * @param[in] N Number of discrete temperature points within range.
     * @param[in] scale Temperature range distributed either on a linear or log scale.
     */
    void setTemperature(
        const double& low, const double& high, const int& N, const std::string& scale);

    /**
     * Define a pressure at which to compute the equilibrium
     * pyrolysis gas mixture properties.
     *
     * @param[in] p Pressure of the pyrolysis gas mixture.
     */
    void setPressure(const double& p) {
        m_nP = 1;
        pressure.resize(m_nP);
        pressure[0] = p;
    }

    /**
     * Define the range of pressure values at which to compute the equilibrium
     * pyrolysis gas mixture properties.
     *
     * @param[in] low Lowest value in the pressure range.
     * @param[in] high Highest value in the pressure range.
     * @param[in] N Number of discrete pressure points within range.
     * @param[in] scale Pressure range distributed either on a linear or log scale.
     */
    void setPressure(
        const double& low, const double& high, const int& n, const std::string& scale);

    /**
     * Using the specified temperature and pressure ranges, compute the equilibrium
     * mixture properties of the pyrolysis gas mixture at each pressure and temperature
     * point. The properties are stored in two-dimensional arrays with pressure as the
     * outer dimension and temperature as the inner dimension.
     */
    void computeProperties();

    /**
     * Write the properties to a HDF5 file.
     *
     * @param[in] gas_table Name of the gas table database file.
     */
    void write(
        const std::string& gas_table="gas_table.h5",
        const std::string& gas_mixture_name="default"); // With "" I get an error.

private:
    size_t m_nT;
    size_t m_nP;

    const std::string pyrolysis_gas;
    const std::string state_model;
    const std::string viscosity_algorithm;
    const std::string conductivity_algorithm;
    std::string pressure_scale;
    std::string temperature_scale;

//    Mutation::Thermodynamics::Thermodynamics* thermo;
//    Mutation::Transport::Transport* transport;
    Mutation::Mixture* mix;

    GasTable gasTable;

    // Is it ok to use Eigen?
    std::vector<double> temperature;
    std::vector<double> pressure;
    std::vector< std::vector<double> > cv;
    std::vector< std::vector<double> > cp;
    std::vector< std::vector<double> > internal_energy;
    std::vector< std::vector<double> > enthalpy;
    std::vector< std::vector<double> > viscosity;
    std::vector< std::vector<double> > molecular_weight;
    std::vector< std::vector<double> > density;

    void linear_range(const double& low, const double& high, std::vector<double>& var);

    void log_range(const double& low, const double& high, std::vector<double>& var);
};

} // namespace IcarusPyro

#endif
