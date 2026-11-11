#include "MovieCatalog.h"

MovieCatalog::MovieCatalog() {
    // навчальні вигадані фільми
    movies.push_back({"Зоряна мандрівка", 2020, "Фантастика", "Команда досліджує нову планету.", 8});
    movies.push_back({"Зоряний лист", 2022, "Драма", "Лист змінює життя двох друзів.", 7});
    movies.push_back({"Таємниця міста", 2019, "Детектив", "Журналіст шукає зниклий архів.", 9});
    movies.push_back({"Літній маршрут", 2021, "Пригоди", "Друзі вирушають у подорож.", 7});
}

std::vector<Movie> MovieCatalog::find(const std::string& title) const {
    std::vector<Movie> result;
    for (const Movie& movie : movies) {
        if (movie.title.find(title) != std::string::npos) result.push_back(movie);
    }
    return result;
}
