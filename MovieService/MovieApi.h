#pragma once
#include "MovieCatalog.h"
#include <string>

class MovieApi {
    MovieCatalog catalog;
public:
    std::string handle(const std::string& request) const;
};
