#ifndef __GAS_TABLE_H__
#define __GAS_TABLE_H__

#include <string>
#include <vector>

#include "table_entry.h"
#include "H5Cpp.h"

using namespace H5;

namespace IcarusPyro {

class HDF5Names {
public:
    HDF5Names() 
         : cv("cv"),
           cp("cp"),
           internal_energy("eint"), 
           enthalpy("enthalpy"), 
           molecular_weight("mw"),
           density("density"),
           viscosity("viscosity"), 
           nx("nx"), 
           ny("ny"), 
           x_variable("x_variable"), 
           y_variable("y_variable"),
           x_scale("x_scale"), 
           y_scale("y_scale"), 
           x_data("x"), 
           y_data("y") {}

    ~HDF5Names() {}; 

    H5std_string z_data(int i) const { return "z_" + std::to_string(i); }

    H5std_string cv;
    H5std_string cp;
    H5std_string internal_energy;
    H5std_string enthalpy;
    H5std_string molecular_weight;
    H5std_string density;
    H5std_string viscosity;
    H5std_string nx;
    H5std_string ny;
    H5std_string x_variable;
    H5std_string y_variable;
    H5std_string x_scale;
    H5std_string y_scale;
    H5std_string x_data;
    H5std_string y_data;
};

class GasTable { 
public:
    /** 
     * Object constructor used for generating a gas table database.
     * 
     * @param pyrolysis_gas_mixture The name of the pyrolysis gas mixture.
     */
    GasTable(const std::string& pyrolysis_gas_mixture)
        : pyrolysis_gas(pyrolysis_gas_mixture), 
          cv(nullptr),
          cp(nullptr), 
          eint(nullptr), 
          enthalpy(nullptr), 
          mw(nullptr),
          density(nullptr),
          viscosity(nullptr) {}

    /** 
     * A constructor that will initialize the object from a previous gas table 
     * database.
     * 
     * @param pyrolysis_gas_mixture The name of the pyrolysis gas mixture.
     * @param database The name (and/or full path) of the gas table database.
     */
    GasTable(const std::string& pyrolysis_gas_mixture, const std::string& database);

    /**
     * Object deconstructor.
     */
    ~GasTable() {
        delete cp;
        delete cv;
        delete eint;
        delete enthalpy;
        delete mw;
        delete density;
        delete viscosity;
    }

    /** 
     * Write the gas mixture property data to an HDF5 database file.
     * 
     * @param[in] database Name or full path of the database file. Default is gas_table.h5.
     */
    void write(std::string database="gas_table.h5", std::string gas_mixture_name="");

    /**
     * Set the table entry values for a gas mixture property where the 
     * `varname` must be a valid gas mixture property, and where the entry
     *  a function of two independent variables. 
     * 
     * @param[in] varname Name of the gas mixture property.
     * @param[in] x_variable Name of the x-independent variable.
     * @param[in] y_variable Name of the y-independent variable.
     * @param[in] x_scale Scale of the x-independent variable, linear or logarthmic.
     * @param[in] y_scale Scale of the y-independent variable, linear or logarthmic.
     * @param[in] x One-dimensional vector of the x-independent variable, e.g., temperature.
     * @param[in] y One-dimensional vector of the x-independent variable, e.g., pressure.
     * @param[in] z Two-dimensional vector of the gas mixture property, e.g., enthalpy.
     */
    void load(std::string varname, 
              std::string x_variable, 
              std::string y_variable,
              std::string x_scale, 
              std::string y_scale,
              std::vector<double>& x, 
              std::vector<double>& y, 
              std::vector<std::vector<double>>& z);

    std::string pyrolysis_gas;
    TableEntry<double>* cp;
    TableEntry<double>* cv;
    TableEntry<double>* eint;
    TableEntry<double>* enthalpy;
    TableEntry<double>* mw;
    TableEntry<double>* density;
    TableEntry<double>* viscosity;

private:

    HDF5Names H5Names;
    TableEntry<double>* readDataSet(Group* group, H5std_string& name);
    void writeDataSet(Group* group, H5std_string& name, TableEntry<double>* var);
};

} // end namespace IcarusPyro
#endif