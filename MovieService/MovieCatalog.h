#pragma once
#include "Movie.h"
#include <string>
#include <vector>

class MovieCatalog {
    std::vector<Movie> movies;
public:
    MovieCatalog();
    std::vector<Movie> find(const std::string& title) const;
};
