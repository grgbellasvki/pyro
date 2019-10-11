#include <iostream>
#include <fstream>

#include <string>

#include <catch2/catch.hpp>

#include "../pyrolysis_gas.h"

using namespace IcarusPyro;

TEST_CASE("1: Load a pyrolysis gas mxiture.", "[GasMixture]") {
    
    std::string gas_mixture = "air5";
    GasMixture Air5(gas_mixture);

}

TEST_CASE("2: Load a pyrolysis gas mxiture.", "[GasMixture]") {

    std::string gas_mixture = "tacot24";
    GasMixture TACOT(gas_mixture, 300, 4000, 76, "linear", 1.01325, 1013250, 6, "log10", "Wilke");
    TACOT.write("tacot_gas_table.h5");
}

// TEST_CASE("3: Load a pyrolysis gas mixture database.", "[GasMixture]") {

//     std::string gas_mixture = "tacot24";
//     std::string gas_database = "gas_table.h5";
//     GasMixture TACOT(gas_mixture, "Wilke");

//     std::string cp("cp"); 
//     std::string x_variable("temperature");
//     std::string y_variable("pressure");
//     std::string x_scale("linear");
//     std::string y_scale("linear");
//     int nx(3);
//     int ny(2);
//     std::vector<double> x(nx,0);
//     std::vector<double> y(ny,0);
//     std::vector<std::vector<double>> z;
//     x[0] = 300.0;
//     x[1] = 3000.0;
//     x[2] = 5000.0;
//     y[0] = 10132.50;
//     y[1] = 101325.0;
//     z.resize(2);
//     for (int j = 0; j < ny; j++) {
//         z[j].resize(nx);
//         for (int i = 0; i < nx; i++) {
//             z[j][i] = static_cast<double>(i+j);
//         }
//     }

//     TACOT.load(cp, x_variable, y_variable, x_scale, y_scale, x, y, z); 
// }
