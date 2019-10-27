#include <iostream>
// #include <fstream>

#include <string>

#include <catch2/catch.hpp>
#include "../pyrolysis_gas.h" // Fix
// #include "mutation++.h"

using namespace std;

using namespace IcarusPyro;

TEST_CASE("Testing an air gas mixture.", "[GasMixture]")
{
    // Mutation::GlobalOptions::workingDirectory("/Users/grgbellas/Documents/Programs/pyro/src/testing/data/");

    // GB: Will fix that
    string gas_mixture =
        "/Users/grgbellas/Documents/Programs/pyro/src/testing/data/air_5_LTE";
    GasMixture air5(gas_mixture);

    // If it doesn't fail it should succeed.
}

TEST_CASE("Writing a tacot gas mixture.", "[GasMixture]")
{
    string gas_mixture =
        "/Users/grgbellas/Documents/Programs/pyro/src/testing/data/tacotair_35_LTE";

    GasMixture tacot(
        gas_mixture,
        300., 4000., 76, "linear",
        1.01325, 1013250, 6, "log10",
        "Wilke");

    // Fix the name
    tacot.write("tacot_gas_table.h5", "tacotair_35_LTE");

    // Comparison with a default one
}

/* TEST_CASE("Loading a tacot gas mixture database.", "[GasMixture]")
{
    string gas_mixture = "tacot24";
    string gas_database = "gas_table.h5";
    GasMixture tacot(gas_mixture); // , "Wilke");

    string cp("cp");
    string x_variable("temperature");
    string y_variable("pressure");
    string x_scale("linear");
    string y_scale("linear");
    int nx(3);
    int ny(2);
    vector<double> x(nx,0);
    vector<double> y(ny,0);
    vector<std::vector<double>> z;
    x[0] = 300.0;
    x[1] = 3000.0;
    x[2] = 5000.0;
    y[0] = 10132.50;
    y[1] = 101325.0;
    z.resize(2);
    for (int j = 0; j < ny; j++) {
        z[j].resize(nx);
        for (int i = 0; i < nx; i++) {
            z[j][i] = static_cast<double>(i+j);
        }
    }

    // tacot.load(cp, x_variable, y_variable, x_scale, y_scale, x, y, z);
} */
