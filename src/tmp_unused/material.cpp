/* Define a material.
*/

#include <string>
#include <vector>

#include "material.h"

using std::string;

namespace IcarusPyro {

Material::Material(const string database) {
    // Parse the database
    YAML::Node inputs = YAML::LoadFile(database);
    name = inputs["name"].as<std::string>();
}

double Material::enthalpy() {
    return 0.0;
}

void Material::computeEnthalpy(
    const std::vector<double>& temperature,
    const std::vector<double>& pressure,
    const std::vector<double>& virgin_mass_fraction,
    std::vector<double>& h)
{
    for (std::vector<double>::iterator it = h.begin(); it != h.end(); ++it) {
        *it = 1.0;
    }
}

void Material::computeEnthalpy(
    const std::vector<double>& temperature,
    const std::vector<double>& virgin_mass_fraction,
    std::vector<double>& h) {
    for (std::vector<double>::iterator it = h.begin(); it != h.end(); ++it) {
        *it = 2.0;
    }
}

void Material::computeEnthalpy(const std::vector<double>& temperature,
                               std::vector<double>& h) {
    for (std::vector<double>::iterator it = h.begin(); it != h.end(); ++it) {
        *it = 3.0;
    }
}

} // namespace IcarusPyro