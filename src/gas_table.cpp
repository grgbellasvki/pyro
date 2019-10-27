#include <string>
#include <iostream>
#include <iomanip>

#include "gas_table.h"
#include "table_entry.h"

using namespace H5;

namespace IcarusPyro {

GasTable::GasTable(
    const std::string& pyrolysis_gas_mixture,
    const std::string& database)
    : pyrolysis_gas(pyrolysis_gas_mixture)
{
    H5std_string FILE_NAME(database);
    H5File* file(nullptr);
    try {
        Exception::dontPrint();
        file = new H5File(FILE_NAME, H5F_ACC_RDONLY);
    } catch (FileIException error) {
        throw std::runtime_error("Could not open database.");
    }
    Group* gas = new Group(file->openGroup(pyrolysis_gas));

    cp = readDataSet(gas, H5Names.cp);

    cv = readDataSet(gas, H5Names.cv);

    eint = readDataSet(gas, H5Names.internal_energy);

    enthalpy = readDataSet(gas, H5Names.enthalpy);

    viscosity = readDataSet(gas, H5Names.viscosity);

    density = readDataSet(gas, H5Names.density);

    mw = readDataSet(gas, H5Names.molecular_weight);

    delete gas;
    delete file;
}

void GasTable::load(
    std::string varname,
    std::string x_variable,
    std::string y_variable,
    std::string x_scale,
    std::string y_scale,
    std::vector<double>& x,
    std::vector<double>& y,
    std::vector<std::vector<double>>& z)
{
    const int nx = x.size();
    const int ny = y.size();

    TableEntry<double>* var(nullptr);
    if (varname == "cp") {
        cp = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = cp;
    } else if (varname == "cv") {
        cv = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = cv;
    } else if (varname == "internal_energy") {
        eint = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = eint;
    } else if (varname == "enthalpy") {
        enthalpy = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = enthalpy;
    } else if (varname == "molecular_weight") {
        mw = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = mw;
    } else if (varname == "density") {
        density = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = density;
    } else if (varname == "viscosity") {
        viscosity = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);
        var = viscosity;
    } else {
        return;
    }
    // Copy data
    for (int i = 0; i < nx; i++) {
        var->x[i] = x[i];
    }
    for (int i = 0; i < ny; i++) {
        var->y[i] = y[i];
    }
    for (int i = 0; i < var->ny; i++) {
        for (int j = 0; j < var->nx; j++) {
            (*(*var).z)(j,i) = z[i][j];
        }
    }
}

void GasTable::write(const std::string& database, const std::string& gas_mixture_name)
{
    std::string gas_name(pyrolysis_gas);
    if (!(gas_mixture_name.empty())) {
        gas_name = gas_mixture_name;
    }
    // std::cout << "Writing database file : " << database
    //          << " for gas mixutre : " << gas_name << std::endl;

    H5std_string FILE_NAME(database);
    H5File* file(nullptr);
    try {
        file = new H5File(FILE_NAME, H5F_ACC_TRUNC);
    } catch (FileIException error) {
        return;
    }
    Group* gas = new Group(file->createGroup(gas_name));

    // std::cout << "   Writing cp data " << std::endl;
    if (cp) writeDataSet(gas, H5Names.cp, cp);

    // std::cout << "   Writing cv data " << std::endl;
    if (cv) writeDataSet(gas, H5Names.cv, cv);

    // std::cout << "   Writing internal energy data " << std::endl;
    if (eint) writeDataSet(gas, H5Names.internal_energy, eint);

    // std::cout << "   Writing enthalpy data " << std::endl;
    if (enthalpy) writeDataSet(gas, H5Names.enthalpy, enthalpy);

    // std::cout << "   Writing molecular weight data " << std::endl;
    if (mw) writeDataSet(gas, H5Names.molecular_weight, mw);

    // std::cout << "   Writing density data " << std::endl;
    if (density) writeDataSet(gas, H5Names.density, density);

    // std::cout << "   Writing viscosity data " << std::endl;
    if (viscosity) writeDataSet(gas, H5Names.viscosity, viscosity);

    delete gas;
    delete file;
}

void GasTable::writeDataSet(Group* gas, H5std_string &variable, TableEntry<double>* var)
{
    const static int RANK = 1;
    DataSet *x_data, *y_data, *z_data;
    hsize_t xdims[RANK];
    hsize_t ydims[RANK];
    hsize_t zdims[RANK];

    Attribute attr;
    DataSpace attr_dataspace = DataSpace(H5S_SCALAR);
    H5std_string buffer;
    StrType stype(PredType::C_S1, H5T_VARIABLE);
    stype.setCset(H5T_CSET_ASCII);

    Group* group = new Group(gas->createGroup(variable));

    attr = Attribute(group->createAttribute(H5Names.nx, PredType::NATIVE_INT, attr_dataspace));
    attr.write(PredType::NATIVE_INT, &var->nx);

    attr = Attribute(group->createAttribute(H5Names.ny, PredType::NATIVE_INT, attr_dataspace));
    attr.write(PredType::NATIVE_INT, &var->ny);

    attr = Attribute(group->createAttribute(H5Names.x_scale, stype, attr_dataspace));
    buffer = var->x_scale;
    attr.write(stype, buffer);

    attr = Attribute(group->createAttribute(H5Names.y_scale, stype, attr_dataspace));
    buffer = var->y_scale;
    attr.write(stype, buffer);

    attr = Attribute(group->createAttribute(H5Names.x_variable, stype, attr_dataspace));
    buffer = var->x_variable;
    attr.write(stype, buffer);

    attr = Attribute(group->createAttribute(H5Names.y_variable, stype, attr_dataspace));
    buffer = var->y_variable;
    attr.write(stype, buffer);

    xdims[0] = var->nx;
    DataSpace x_dataspace = DataSpace(RANK, xdims);
    x_data = new DataSet(group->createDataSet(H5Names.x_data, PredType::NATIVE_DOUBLE, x_dataspace));
    x_data->write(var->x, PredType::NATIVE_DOUBLE);
    delete x_data;

    ydims[0] = var->ny;
    DataSpace y_dataspace = DataSpace(RANK, ydims);
    y_data = new DataSet(group->createDataSet(H5Names.y_data, PredType::NATIVE_DOUBLE, y_dataspace));
    y_data->write(var->y, PredType::NATIVE_DOUBLE);
    delete y_data;

    for (int i = 0; i < var->ny; i++) {
        H5std_string zvar = H5Names.z_data(i);
        z_data = new DataSet(group->createDataSet(zvar, PredType::NATIVE_DOUBLE, x_dataspace));

        double data_out[xdims[0]];
        for (int j = 0; j < var->nx; j++) {
            data_out[j] = (*(*var).z)(j,i);
        }
        z_data->write(data_out, PredType::NATIVE_DOUBLE);
        delete z_data;
    }

    delete group;
}

TableEntry<double>* GasTable::readDataSet(Group* gas, H5std_string &variable)
{
    DataSpace x_dataspace, y_dataspace, z_dataspace;
    DataSet *x_data, *y_data, *z_data;
    Attribute* attr;
    DataType dtype;
    int ndims; 
    hsize_t xdims[1], ydims[1], zdims[1];

    Group* group(nullptr);
    try {
        Exception::dontPrint();
        group = new Group(gas->openGroup(variable));
    } catch (...) {
        std::cout << "Creating empty variable object for " << variable << 
                     ". It does not exist in the HDF5 file." << std::endl;
        TableEntry<double>* var = new TableEntry<double>(1, 1, "temperature", "pressure", "linear", "linear");
        return var;
    }

    attr = new Attribute(group->openAttribute(H5Names.x_scale));
    dtype = attr->getDataType();
    H5std_string buffer("");
    attr->read(dtype, buffer);
    std::string x_scale(buffer);
    delete attr;

    attr = new Attribute(group->openAttribute(H5Names.y_scale));
    dtype = attr->getDataType();
    buffer = "";
    attr->read(dtype, buffer);
    std::string y_scale(buffer);
    delete attr; 

    attr = new Attribute(group->openAttribute(H5Names.x_variable));
    dtype = attr->getDataType();
    buffer = "";
    attr->read(dtype, buffer);
    std::string x_variable(buffer);
    delete attr; 

    attr = new Attribute(group->openAttribute(H5Names.y_variable));
    dtype = attr->getDataType();
    buffer = "";
    attr->read(dtype, buffer);
    std::string y_variable(buffer);
    delete attr; 

    x_data = new DataSet(group->openDataSet(H5Names.x_data));
    x_dataspace = x_data->getSpace();
    y_data = new DataSet(group->openDataSet(H5Names.y_data));
    y_dataspace = y_data->getSpace();

    ndims = x_dataspace.getSimpleExtentDims(xdims, nullptr);
    int nx = xdims[0];

    ndims = y_dataspace.getSimpleExtentDims(ydims, nullptr);
    int ny = ydims[0];

    TableEntry<double>* var = new TableEntry<double>(nx, ny, x_variable, y_variable, x_scale, y_scale);

    x_data->read(var->x, PredType::NATIVE_DOUBLE, DataSpace(1,xdims), x_dataspace);
    delete x_data;

    y_data->read(var->y, PredType::NATIVE_DOUBLE, DataSpace(1,ydims), y_dataspace);
    delete y_data;

    for (int i = 0; i < var->ny; i++) {
        H5std_string zvar = H5Names.z_data(i);

        z_data = new DataSet(group->openDataSet(zvar));
        z_dataspace = z_data->getSpace();
        ndims = z_dataspace.getSimpleExtentDims(zdims, nullptr);
        double data_in[zdims[0]];
        z_data->read(data_in, PredType::NATIVE_DOUBLE, DataSpace(1,zdims), z_dataspace);
        delete z_data;
        
        for (int j = 0; j < var->nx; j++) {
            (*(*var).z)(j,i) = data_in[j];
        }
    }
    delete group;
    return var;
}

} // namespace IcarusPyro
