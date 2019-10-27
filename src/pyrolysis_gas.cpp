#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "pyrolysis_gas.h"
#include "mutation++.h"

using namespace std;
using namespace Mutation;

namespace IcarusPyro {

GasMixture::GasMixture(
    const string& pyrolysis_gas_mixture,
    const double& T_low,
    const double& T_high,
    const int& nT,
    const std::string& T_scale,
    const double& p_low,
    const double& p_high,
    const int& nP,
    const std::string& p_scale,
    const std::string& mu_algorithm,
    const std::string& k_algorithm,
    const std::string& state_model)
    : pyrolysis_gas(pyrolysis_gas_mixture),
      viscosity_algorithm(mu_algorithm),
      conductivity_algorithm(k_algorithm),
      temperature_scale(T_scale),
      pressure_scale(p_scale),
      gasTable(pyrolysis_gas_mixture)
{
    MixtureOptions opts(pyrolysis_gas_mixture);
    opts.setViscosityAlgorithm(mu_algorithm);
    opts.setThermalConductivityAlgorithm(k_algorithm);
    opts.setStateModel(state_model);

    // I avoid using thermo and transport. Soonish this part of the code
    // where Mixture is defined with inheritence will be removed.
    // I would personally add an option for thermo_db.
    mix = new Mutation::Mixture(opts);

    // This is what I would do to avoid unnecessary resizing.
    // setTPAxes(nT, T_scale, nP, p_scale);
    // setTAxis(nT, T_scale); // Not used here
    // setTAxis(nP, p_scale); // Not used here
    // setTemperatureRange(T_low, T_high, nT);
    // setPressureRange(p_low, p_high, nP);

    // I would put this here!
    int nE = mix->nElements();
    string N("N");
    string O("O");
    string C("C");
    string H("H");
    //std::vector<double> Xe(nE,0.0);
    // double* Xe = mix->defaultElementalComposition();
    // double* Xe = new double[nE];
    /* Xe[thermo->elementIndex(N)] = 0.0;
    Xe[thermo->elementIndex(C)] = 0.206;
    Xe[thermo->elementIndex(H)] = 0.679;
    Xe[thermo->elementIndex(O)] = 0.115; */
    // setElementalComp(vector<double>& xe)

    setTemperature(T_low, T_high, nT, T_scale);
    setPressure(p_low, p_high, nP, p_scale);

    computeProperties();
}

GasMixture::~GasMixture(){ delete mix; }

void GasMixture::computeProperties()
{
//    int T_size = temperature.size();
//    int p_size = pressure.size();

//    std::cout << m_nT << std::endl;
//    std::cout << m_nP << std::endl;

    internal_energy.resize(m_nP);
    enthalpy.resize(m_nP);
    cv.resize(m_nP);
    cp.resize(m_nP);
    molecular_weight.resize(m_nP);
    density.resize(m_nP);
    viscosity.resize(m_nP);

    for (int j = 0; j < m_nP; j++) {
        double p = pressure[j];

        internal_energy[j].resize(m_nT);
        enthalpy[j].resize(m_nT);
        cv[j].resize(m_nT);
        cp[j].resize(m_nT);
        molecular_weight[j].resize(m_nT);
        density[j].resize(m_nT);
        viscosity[j].resize(m_nT);

        for (int i = 0; i < m_nT; i++) {
            // mix->equilibrate(temperature[i], p, Xe);
            // mix->SetState();

            // internal_energy[j][i] = mix->mixtureEnergyMass();
            // enthalpy[j][i] = mix->mixtureHMass();
            // cv[j][i] = mix->mixtureFrozenCvMass();
            // cp[j][i] = mix->mixtureFrozenCpMass();
            // molecular_weight[j][i] = mix->mixtureMw() * 1000.0; // convert from kg/mol to kg/kmol
            // density[j][i] = mix->density();
            // viscosity[j][i] = mix->viscosity();
        }
    }
}

void GasMixture::write(
    const std::string& gas_table,
    const std::string& gas_mixture_name)
{
    gasTable.load(
        "cp", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, cp);
    gasTable.load(
        "cv", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, cv);
    gasTable.load(
        "internal_energy", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, internal_energy);
    gasTable.load(
        "enthalpy", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, enthalpy);
    gasTable.load(
        "density", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, density);
    gasTable.load(
        "molecular_weight", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, molecular_weight);
    gasTable.load(
        "viscosity", "temperature", "pressure",
        temperature_scale, pressure_scale,
        temperature, pressure, viscosity);

    gasTable.write(gas_table, gas_mixture_name);
}

void GasMixture::setTemperature(
    const double& low, const double& high,
    const int& N, const std::string& scale)
{
    m_nT = N;

    temperature.resize(m_nT);
    temperature_scale = scale;
    if (scale == "log10") {
        log_range(low, high, temperature);
    } else {
        linear_range(low, high, temperature);
    }
}

// Maybe this can be written just once
// Maybe an enumerate?
void GasMixture::setPressure(
    const double& low, const double& high,
    const int& N, const std::string& scale)
{
    m_nP = N;

    pressure.resize(N);
    pressure_scale = scale;
    if (scale == "log10") {
        log_range(low, high, pressure);
    } else {
        linear_range(low, high, pressure);
    }
}

void GasMixture::linear_range(
    const double& low, const double& high, std::vector<double>& var)
{
    const int N = var.size();
    const double dv = (high - low) / static_cast<double>(N - 1);
    for (int i = 0; i < N; i++)
        var[i] = low + dv*i;
}

void GasMixture::log_range(
    const double& low, const double& high, std::vector<double>& var)
{
    const int N = var.size();
    const double dv = (std::log10(high) - std::log10(low)) / static_cast<double>(N - 1);
    for (int i = 0; i < N; i++)
        var[i] = std::pow(10.0, std::log10(low) + dv*i);
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
