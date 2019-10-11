#ifndef ICARUSPYRO_GAS_H
#define ICARUSPYRO_GAS_H

#include <string>
#include <vector>

#include "MixtureOptions.h"
#include "Thermodynamics.h"
#include "Transport.h"

#include "gas_table.h"

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
    GasMixture(std::string& pyrolysis_gas_mixture,
               double T_low = 200.0,
               double T_high = 4000.0, 
               int nT = 76,
               std::string T_scale = "linear", 
               double p_low = 1.01325, 
               double p_high = 1013250.0,
               int nP = 6,
               std::string p_scale = "log10",
               std::string mu_algorithm = "Chapmann-Enskog_CG", 
               std::string k_algorithm = "Wilke");

    /**
     * Deconstructor
     */
    ~GasMixture() {
        delete thermo;
        delete transport;
    }

    /** 
     * Define the range of temperature values at which to compute the equilibrium 
     * pyrolysis gas mixture properties.
     * 
     * @param[in] T Temperature of the pyrolysis gas mixture. 
     */
    void setTemperature(double T) { 
        temperature.resize(1);
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
    void setTemperature(double low, double high, int N, std::string& scale);

    /** 
     * Define the range of pressure values at which to compute the equilibrium 
     * pyrolysis gas mixture properties.
     * 
     * @param[in] p Pressure of the pyrolysis gas mixture. 
     */
    void setPressure(double p) { 
        pressure.resize(1);
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
    void setPressure(double low, double high, int n, std::string& scale);

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
    void write(std::string gas_table="gas_table.h5", std::string gas_mixture_name="");

private:
    std::string pyrolysis_gas;
    std::string viscosity_algorithm;
    std::string conductivity_algorithm;
    std::string pressure_scale;
    std::string temperature_scale;

    Mutation::Thermodynamics::Thermodynamics* thermo;
    Mutation::Transport::Transport* transport;

    GasTable gasTable;

    std::vector<double> temperature;
    std::vector<double> pressure;
    std::vector< std::vector<double> > cv;
    std::vector< std::vector<double> > cp;
    std::vector< std::vector<double> > internal_energy;
    std::vector< std::vector<double> > enthalpy;
    std::vector< std::vector<double> > viscosity;
    std::vector< std::vector<double> > molecular_weight;
    std::vector< std::vector<double> > density;

    void linear_range(double &low, double &high, std::vector<double>& var);
    
    void log_range(double &low, double &high, std::vector<double>& var);

};

} // namespace IcarusPyro

#endif
