#ifndef __TABLE_ENTRY_H__
#define __TABLE_ENTRY_H__

#include <iostream>

namespace IcarusPyro {

template<class T>
class array2d {
public:
    array2d(int nxx, int nyy) : nx(nxx), ny(nyy) {
        size = nx * ny;
        data = new T[size];
    }

    array2d(const array2d<T>& rhs) {
        nx = rhs.nx;
        ny = rhs.ny;
        size = static_cast<size_t>(nx * ny);
        data = new T[size];
        for (unsigned int i = 0; i < size; i++) data[i] = rhs.data[i];
    }

    T operator()(int x, int y) const {
        return data[y + ny * x];
    }

    T& operator()(int x, int y) {
        return data[y + ny * x];
    }

    ~array2d() {
        delete [] data;
    }

private:
    int nx, ny;
    size_t size;
    T* data;
};

template<class T>
class TableEntry {
public:
    TableEntry(
        int nxx, int nyy,
        const std::string& xvar, const std::string& yvar,
        const std::string& xscale, const std::string& yscale)
        : nx(nxx),
          ny(nyy),
          x_variable(xvar),
          y_variable(yvar),
          x_scale(xscale),
          y_scale(yscale)
    {
        nz = static_cast<size_t>(nx * ny);
        x = new T[nx];
        y = new T[ny];
        z = new array2d<T>(nx, ny);
    }

    ~TableEntry() {
        delete [] x;
        delete [] y;
        delete z;
    }

    const int nx, ny;
    const std::string x_variable, y_variable;
    const std::string x_scale, y_scale;

    size_t nz;
    T* x;
    T* y;
    array2d<T>* z;
};

} // namespace IcarusPyro
#endif