#include "LengthMap.h"
#include "TourModifier.h"
#include "fileio/fileio.h"
#include "primitives.h"
#include "solver.h"
#include "utility.h"

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

    // Initialize segments.
    std::vector<Segment> segments;
    for (auto id : initial_tour)
    {
        segments.emplace_back(id, tour_modifier.next(id), tour_modifier.length_map());
    }

    // Initial tour stats.
    const auto initial_tour_length = verify::tour_length(segments);
    std::cout << "Initial tour length: " << initial_tour_length << std::endl;
    std::cout << "Average segment length: " << initial_tour_length / static_cast<primitives::space_t>(segments.size()) << std::endl;
    // Optimization loop.
    auto filename = utility::extract_filename(argv[1]);
    solver::hill_climb(initial_tour, segments, tour_modifier, filename);
    return 0;
}
