#include "LengthMap.h"
#include "TourModifier.h"
#include "fileio.h"
#include "primitives.h"
#include "solver.h"

#include <iostream>

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::cout << "Arguments: point_set_file_path optional_tour_file_path" << std::endl;
        return 0;
    }

    // Read input files.
    const auto coordinates {fileio::read_coordinates(argv[1])};
    const auto& x {coordinates[0]};
    const auto& y {coordinates[1]};
    const auto initial_tour = fileio::initial_tour(argc, argv, x.size());

    // Initialize distance table.
    TourModifier tour_modifier(initial_tour, x, y);

    // Initial tour stats.
    const auto initial_tour_length {tour_modifier.length()};
    std::cout << "Initial tour length: " << initial_tour_length << std::endl;

    // Optimization loop.
    solver::hill_climb(tour_modifier);

    // Save result.
    const auto final_length {tour_modifier.length()};
    std::cout << "final length: " << final_length << std::endl;
    if (initial_tour_length > final_length)
    {
        auto save_file_prefix {fileio::extract_filename(argv[1])};
        fileio::write_ordered_points(tour_modifier.order()
            , "saves/" + save_file_prefix + "_" + std::to_string(final_length) + ".txt");
    }
    return 0;
}
