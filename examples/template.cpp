#include <string>

int main() {

    const std::string material = 'TACOT';
    const std::string pyrolysis_gas_mixture = 'TACOT-Pyro';
    const std::string gas_surface_mixture = 'TACOT-Pyro-GSI';

    // Initialize a Pyro object. 

    Pyro = IcarusPyro(material, pyrolysis_gas_mixture, gas_surface_mixture);

    double rho_s = Pyro->Material.get_density();
    double rho_g = Pyro->GasMixture.get_density();
    double b_prime = Pyro->GSI.get_bprime();
   
}