#include <iostream>
#include <fstream>

#include <string>

#include <catch2/catch.hpp>

#include "../pyrolysis_gas.h"
#include "../gas_table.h"

using namespace IcarusPyro;
using namespace std;

TEST_CASE("Load a tacot gas mixture database.", "[GasTable]")
{
    const string gas_mixture =
        "/Users/grgbellas/Documents/Programs/pyro/src/testing/data/tacotair_35_LTE";
    const string gas_database = "gas_table.h5";

    GasTable tacot(gas_mixture);

    const string cp("cp"); // Why was it cv?
    const string x_variable("temperature");
    const string y_variable("pressure");
    const string x_scale("linear");
    const string y_scale("linear");
    const int nx(3);
    const int ny(2);
    std::vector<double> x(nx, 0.);
    std::vector<double> y(ny, 0.);
    std::vector<std::vector<double>> z;
    x[0] = 300.0;
    x[1] = 3000.0;
    x[2] = 5000.0;
    y[0] = 10132.50;
    y[1] = 101325.0;

    z.resize(ny);
    for (int j = 0; j < ny; j++) {
        z[j].resize(nx);
        for (int i = 0; i < nx; i++) {
            z[j][i] = static_cast<double>(i+j);
        }
    }

    tacot.load(cp, x_variable, y_variable, x_scale, y_scale, x, y, z);

    std::cout << x[0] << " " << z[0][0] << std::endl;

    std::cout << "Specific Heat at Constant Pressure" << std::endl;
    std::cout << "  Dependent Variables : " << tacot.cp->x_variable << ", " << tacot.cp->y_variable << std::endl;
    std::cout << "  Number of Points : " << tacot.cp->nx << ", " << tacot.cp->ny << std::endl;
    // std::cout << "  " << (*(TACOT.cp->z))(0,1) << std::endl;
    // for (int i = 0; i < TACOT.cp->ny; i++) {
    //     std::cout << i << " " << TACOT.cv->y[i] << std::endl;
    // }
    tacot.write("new_gas_table.h5");
}

// TEST_CASE("2: Load a pyrolysis gas mixture database.", "[GasTable]") {

    // std::string gas_mixture = "24sp-tacot-pyro";
    // std::string gas_database = "gas_table.h5";
    // GasTable TACOT(gas_mixture, gas_database);

    // std::cout << "Specific Heat at Constant Pressure" << std::endl;
    // std::cout << "  Dependent Variables : " << TACOT.cv->x_variable << ", " << TACOT.cv->y_variable << std::endl;
    // std::cout << "  Number of Points : " << TACOT.cv->nx << ", " << TACOT.cv->ny << std::endl;
    // // //std::cout << "  " << (*(TACOT.cv->z))(868,1) << std::endl;
    // // // for (int i = 0; i < TACOT.cp->ny; i++) { 
    // // //     std::cout << i << " " << TACOT.cv->y[i] << std::endl;
    // // // }

    // TACOT.write("new_gas_table.h5");
// }

