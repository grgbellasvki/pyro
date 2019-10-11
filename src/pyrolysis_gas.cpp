#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "pyrolysis_gas.h"

namespace IcarusPyro { 

GasMixture::GasMixture(std::string& pyrolysis_gas_mixture, 
                       double T_low,
                       double T_high, 
                       int nT,
                       std::string T_scale, 
                       double p_low, 
                       double p_high,
                       int nP,
                       std::string p_scale,
                       std::string mu_algorithm, 
                       std::string k_algorithm) 
    : pyrolysis_gas(pyrolysis_gas_mixture), 
      viscosity_algorithm(mu_algorithm), 
      conductivity_algorithm(k_algorithm),
      temperature_scale(T_scale),
      pressure_scale(p_scale),
      thermo(nullptr),
      transport(nullptr),
      gasTable(pyrolysis_gas_mixture)
{
    Mutation::MixtureOptions* opts = new Mutation::MixtureOptions(pyrolysis_gas_mixture);
    thermo = new Mutation::Thermodynamics::Thermodynamics(opts->getSpeciesDescriptor(), 
                                                          opts->getThermodynamicDatabase(),
                                                          opts->getStateModel());
    transport = new Mutation::Transport::Transport(*thermo, 
                                                   viscosity_algorithm, 
                                                   conductivity_algorithm);
    delete opts;

    setTemperature(T_low, T_high, nT, T_scale);
    setPressure(p_low, p_high, nP, p_scale);
    computeProperties();
}

void GasMixture::computeProperties() { 
    int T_size = temperature.size();
    int p_size = pressure.size();

    internal_energy.resize(p_size);
    enthalpy.resize(p_size);
    cv.resize(p_size);
    cp.resize(p_size);
    molecular_weight.resize(p_size);
    density.resize(p_size);
    viscosity.resize(p_size);

    int nE = thermo->nElements();
    std::string N("N");
    std::string O("O");
    std::string C("C");
    std::string H("H");
    //std::vector<double> Xe(nE,0.0);
    double* Xe = new double[nE];
    Xe[thermo->elementIndex(N)] = 0.0;
    Xe[thermo->elementIndex(C)] = 0.206;
    Xe[thermo->elementIndex(H)] = 0.679;
    Xe[thermo->elementIndex(O)] = 0.115;

    for (int j = 0; j < p_size; j++) {
        double p = pressure[j];

        internal_energy[j].resize(T_size);
        enthalpy[j].resize(T_size);
        cv[j].resize(T_size);
        cp[j].resize(T_size);
        molecular_weight[j].resize(T_size);
        density[j].resize(T_size);
        viscosity[j].resize(T_size);

        for (int i = 0; i < T_size; i++) { 
            thermo->equilibrate(temperature[i], p, Xe);

            internal_energy[j][i] = thermo->mixtureEnergyMass();
            enthalpy[j][i] = thermo->mixtureHMass();
            cv[j][i] = thermo->mixtureFrozenCvMass();
            cp[j][i] = thermo->mixtureFrozenCpMass();
            molecular_weight[j][i] = thermo->mixtureMw() * 1000.0; // convert from kg/mol to kg/kmol
            density[j][i] = thermo->density();
            viscosity[j][i] = transport->viscosity();
        }
    }
}

void GasMixture::write(std::string gas_table, std::string gas_mixture_name) {
    gasTable.load("cp", 
                  "temperature", "pressure", 
                  temperature_scale, pressure_scale, 
                  temperature, pressure, cp);
    gasTable.load("cv", 
                  "temperature", "pressure", 
                  temperature_scale, pressure_scale,
                  temperature, pressure, cv);
    gasTable.load("internal_energy", 
                  "temperature", "pressure", 
                  temperature_scale, pressure_scale, 
                  temperature, pressure, internal_energy);
    gasTable.load("enthalpy", 
                  "temperature", "pressure", 
                   temperature_scale, pressure_scale, 
                   temperature, pressure, enthalpy);
    gasTable.load("density", 
                   "temperature", "pressure", 
                   temperature_scale, pressure_scale,
                   temperature, pressure, density);
    gasTable.load("molecular_weight", 
                   "temperature", "pressure", 
                   temperature_scale, pressure_scale,
                   temperature, pressure, molecular_weight);
    gasTable.load("viscosity", 
                   "temperature", "pressure", 
                   temperature_scale, pressure_scale,
                   temperature, pressure, viscosity);

    gasTable.write(gas_table, gas_mixture_name);
}

void GasMixture::setTemperature(double low, double high, int N, std::string& scale)
{
    temperature.resize(N);
    temperature_scale = scale;
    if (scale == "log10") { 
        log_range(low, high, temperature);
    } else { 
        linear_range(low, high, temperature);
    }
}

void GasMixture::setPressure(double low, double high, int N, std::string& scale)
{
    pressure.resize(N);
    pressure_scale = scale;
    if (scale == "log10") { 
        log_range(low, high, pressure);
    } else { 
        linear_range(low, high, pressure);
    }
}

void GasMixture::linear_range(double &low, double &high, std::vector<double>& var) { 
    int N = var.size();
    double dv = (high - low) / static_cast<double>(N - 1);
    for (int i = 0; i < N; i++)
        var[i] = low + dv * i;
}

void GasMixture::log_range(double &low, double &high, std::vector<double>& var) { 
    int N = var.size();
    double dv = (std::log10(high) - std::log10(low)) / static_cast<double>(N - 1);
    for (int i = 0; i < N; i++)
        var[i] = std::pow(10.0, std::log10(low) + dv * i);
}

// void GasMixture::print(double T, double p) 
// { 
//     std::cout << "Initializing Mutation++ model for " << pyrolysis_gas << std::endl;
//     std::cout << "Species List" << std::endl;
//     for (int n = 0; n < thermo->nSpecies(); n++) {
//         // Mutation::Thermodynamics::Species sp = thermo->species(n);
//         // std::cout << n << " : " << sp.name() << std::endl;
//         std::cout << n << " : " << thermo->speciesName(n) << std::endl; // ", Y = " << thermo->getDefaultComposition(n) << std::endl;
//     }
//     std::cout << "\nElement List" << std::endl;
//     for (int n = 0; n < thermo->nElements(); n++) {
//         Mutation::Thermodynamics::Element el = thermo->element(n);
//         std::cout << n << " : " << el.name() << std::endl;
//     }
//     std::cout << "" << std::endl;
//     std::cout << "Initial Mass Fractions: T = 300, p = 101325.0\n" << std::endl;

//     thermo->equilibrate(T, p);

//     double const * Y = thermo->Y();
//     for (int n = 0; n < thermo->nSpecies(); n++) {
//         double Y_n;
//         if (Y[n] > 1.0e-15) { 
//             Y_n = Y[n];
//         } else {
//             Y_n = 0.0;
//         }
//         std::cout << std::setw(20) << "Y_" + thermo->speciesName(n) << " = " << std::setw(15) << Y_n << std::endl;
//     }
// }

} // namespace IcarusPyro
