#include <stdlib.h> 

#include "icaruspyro.h"

bool optionExists(int argc, char** argv, const std::string& option);
std::string getOption(int argc, char** argv, const std::string& option);

int main(int argc, char** argv) {
    if (!(argc > 1)) {
        std::cout << "Must provide the name of the pyrolysis gas mixture." << std::endl; 
        return -1;
    }
    std::string pyrogas_mixture(argv[1]);

    double T_low = 200.0;
    double T_high = 4000.0;
    int nT = 76;
    double p_low = 1.01325;
    double p_high = 1013250.0;
    int nP = 6;
    std::string T_scale("linear");
    std::string p_scale("log10");
    std::string database("gas_table.h5");
    std::string gas_mixture_name;

    std::string mu_algorithm("Wilke");
    std::string k_algorithm("Wilke");

    if (optionExists(argc, argv, "--T_low")) { 
        T_low = atof(getOption(argc, argv, "--T_low").c_str());
    }
    if (optionExists(argc, argv, "--T_high")) { 
        T_high = atof(getOption(argc, argv, "--T_high").c_str());
    }
    if (optionExists(argc, argv, "--p_low")) { 
        p_low = atof(getOption(argc, argv, "--p_low").c_str());
    }
    if (optionExists(argc, argv, "--p_high")) { 
        p_high = atof(getOption(argc, argv, "--p_high").c_str());
    }
    if (optionExists(argc, argv, "--T_scale")) { 
        T_scale = getOption(argc, argv, "--T_scale");
    }
    if (optionExists(argc, argv, "--p_scale")) { 
        p_scale = getOption(argc, argv, "--p_scale");
    }
    if (optionExists(argc, argv, "--nT")) { 
        nT = atoi(getOption(argc, argv, "--nT").c_str());
    }
    if (optionExists(argc, argv, "--nP")) { 
        nP = atoi(getOption(argc, argv, "--nP").c_str());
    }
    if (optionExists(argc, argv, "--gas-mixture-name")) { 
        gas_mixture_name = getOption(argc, argv, "--gas-mixture-name");
    } else { 
        gas_mixture_name = pyrogas_mixture;
    }
    if (optionExists(argc, argv, "--database-file")) { 
        database = getOption(argc, argv, "--database-file");
    }

    IcarusPyro::GasMixture gas(pyrogas_mixture, 
                               T_low, T_high, nT, T_scale, 
                               p_low, p_high, nP, p_scale, 
                               mu_algorithm, k_algorithm);
    gas.write(database, gas_mixture_name);

    return 0;
}

 // Checks if an option is present
 bool optionExists(int argc, char** argv, const std::string& option)
 {
    return (std::find(argv, argv+argc, option) != argv+argc);
 }

std::string getOption(int argc, char** argv, const std::string& option) { 
    std::string value;
    char** ptr = std::find(argv, argv+argc, option);

    if (ptr == argv+argc || ptr+1 == argv+argc)
        value = "";
    else
        value = *(ptr+1);
    return value;
}
