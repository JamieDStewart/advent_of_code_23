//\==============================================================================
// Advent of code 2023 Day 5
// Question: https://adventofcode.com/2023/day/5
// Gist:  Seeds need to go in appropriate soil all about changing ranges until location is found
// Input data: Starts with seed Id's then a series of values describing ranges
//\==============================================================================


#include "days.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace day_05
{
	/** 
	* @brief a structure to hold the mapping data for converting source to destination
	*/
	struct Map
	{
		uint64_t destination;	/** < the index value of the destination */
		uint64_t source;		/** < the index value of the source */
		uint64_t range;			/** < the range/number of indexes that this map covers */
	};

	/** 
	* @brief a structure that holds the data read in from file
	* a collection of seeds and the mapping data to convert from seed value to location value
	*/
	struct Seeds_Maps
	{
		std::vector<uint64_t> seed_ids;
		std::vector<std::vector<Map>> conversion_map;
	};

	Seeds_Maps read_input_file( std::string input_file )
	{
		//open the input file for and parse into a vector of games
		Seeds_Maps seeds_n_maps = {};
		std::vector<Map> parsed_maps;

		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//line with seeds in contains a ':' followed by list of seeds or end of range
					size_t colon_pos = line.find_first_of( ':' );
					if ( colon_pos != std::string::npos )
					{
						//test to see if this is the line of seeds
						std::string seed_ids = line.substr( colon_pos + 1 );
						if ( seed_ids.size() > 0 )
						{
							uint64_t value = {};
							std::stringstream ss( seed_ids );
							while ( ss >> value )
							{
								seeds_n_maps.seed_ids.push_back( value );
							}
						}
						//lines with a ':' with no following substring indicate that the previous ranges have ended
						else
						{
							//append ranges to ranges vector.
							if ( !parsed_maps.empty() )
							{
								//sort maps based on source from min to max
								std::sort( parsed_maps.begin(), parsed_maps.end(), []( const Map& a, const Map& b ) { return a.source < b.source; } );
								seeds_n_maps.conversion_map.push_back( parsed_maps );
								parsed_maps.clear();
							}
						}
					}
					//if there is no ':' this line is a range conversion so parse the line as a range
					else
					{
						Map map = {};
						std::stringstream ss( line );
						ss >> map.destination;
						ss >> map.source;
						ss >> map.range;

						parsed_maps.push_back( map );
					}
				}
			}
			//push the last parsed maps into the conversion map
			if ( !parsed_maps.empty() )
			{
				std::sort( parsed_maps.begin(), parsed_maps.end(), []( const Map& a, const Map& b ) { return a.source < b.source; } );
				seeds_n_maps.conversion_map.push_back( parsed_maps );
				parsed_maps.clear();
			}
		}
		file_input.close();

		return seeds_n_maps;
	}

	/** 
	* @brief this is the solution for part one
	*   Each seed is an individual seed value that is pushed through the mapping process to
	*   produce a location value. The lowest location value is returned to the calling location of this function
	*/
	uint64_t part_01( Seeds_Maps seeds_n_maps )
	{
		uint64_t lowest_location = INT_MAX;
		//go through each seed and pass it through the maps to discover it's final location
		for ( const auto& seed : seeds_n_maps.seed_ids )
		{
			uint64_t mapped_value = seed;
			for ( const auto& maps : seeds_n_maps.conversion_map )
			{
				//for each stage in the pipeline convert the seed
				for ( const auto& map : maps )
				{
					//is the value in the range of the map?
					if ( mapped_value > map.source && mapped_value < map.source + map.range )
					{
						//if the value is in range map it to the destination range
						const uint64_t offset = mapped_value - map.source;
						mapped_value = map.destination + offset;
						break;
					}
				}
			}
			//we have traversed this seed through to it's location mapped value
			if ( mapped_value < lowest_location )
			{
				lowest_location = mapped_value;
			}
		}
		return lowest_location;
	}

	/**
	* @brief a struct used to hold a seed range
	*/
	struct Range
	{
		uint64_t start; /** < The starting location of the range */
		uint64_t size; /** < The size of the range */
	};
	
	/**
	* @brief converts seed values read in from file to a seed range
	* @param seed and stage mapping data
	* @return the seed vales converted into a vector of ranges
	*/
	std::vector<Range> convert_seeds_to_seed_range( const Seeds_Maps& seeds_n_maps )
	{
		std::vector<Range> seed_ranges = {};
		for ( int i = 0; i < seeds_n_maps.seed_ids.size(); i += 2 )
		{
			seed_ranges.push_back( { seeds_n_maps.seed_ids[i], seeds_n_maps.seed_ids[i + 1] } );
		}
		return seed_ranges;
	}

	/** 
	* @brief function to map the range passed in to any of the maps that satisfy that range
	* @param the range to map
	* @param the vector of maps to use to map the range
	* @return a vector of mapped ranges
	*/
	std::vector<Range> map_ranges( Range range, std::vector<Map> maps )
	{
		std::vector<Range> converted_ranges = {};
		
		for ( const auto& map : maps )
		{
			if ( range.size == 0 ) //no more values to try to map so break out of this loop
			{
				break;
			}
			//test to see if range overlaps (this is very much an AABB test in a single dimension)
			if ( range.start < map.source + map.range && map.source < range.start + range.size )
			{
				//there is an overlap with this range.Find overlap minimal/maximal
				uint64_t overlap_start = std::max( range.start, map.source );
				uint64_t overlap_end = std::min( range.start + range.size, map.source + map.range );
				//calculate the range of the overlap
				uint64_t overlap_range = overlap_end - overlap_start;
				
				if ( overlap_start > range.start ) //any seeds prior to the start of this overlap should be sent through unconverted
				{
					Range sr = { range.start, overlap_start - range.start };
					//stick the seeds into the conversion ranges as they just pass through
					converted_ranges.push_back( sr );
				}
				//map start to destination value
				uint64_t offset = std::max( range.start, map.source ) - std::min( range.start, map.source );
				uint64_t destination = map.destination + offset;
				//push back the mapped range
				converted_ranges.push_back( { destination, overlap_range } );

				//reduce range by overlapped amount
				range.size -= overlap_end - range.start;
				
				//adjust starting point of current range
				range.start = overlap_start + overlap_range;
			}

		}
		//any trailing values that have not overlapped need to be sent through to the next mapping stage.
		if ( range.size != 0 )
		{
			converted_ranges.push_back( { range.start, range.size } );
		}
		return converted_ranges;
	}

	/** 
	* @brief Solution to part 2 of problem now seeds need to be treated as a pair of values with starting location and range.
	* @param the input data read in from the input dataset
	* @return the lowest location value for the mapped seeds
	*/
	uint64_t part_02( Seeds_Maps seeds_n_maps )
	{
		uint64_t lowest_location = INT_MAX;
		//convert seeds to seed ranges
		std::vector<Range> seed_ranges = convert_seeds_to_seed_range( seeds_n_maps );
		
		//for each range of seeds 
		for ( const auto& seeds : seed_ranges )
		{
			//set an active range to be mapping down to a location 
			std::vector<Range> active_ranges = {seeds};
			//for each mapping stage 
			for ( const auto& maps : seeds_n_maps.conversion_map )
			{
				//keep track of converted ranges for the next set of active ranges
				std::vector<Range> converted_ranges = {};
				//for each active range we will map it to the next stage of the mapping pipeline
				for ( auto& current_range : active_ranges )
				{
					//map the current range to the next stage of the pipeline
					std::vector<Range> conversions = map_ranges( current_range, maps );
					//append any converted ranges to keep track of conversions for each of the maps in this stage
					converted_ranges.insert( converted_ranges.end(), conversions.begin(), conversions.end() );
				}
				//sort the converted ranges by starting location - sorting the list allows us to ensure that anything prior to the first matching mapped range is passed through 
				std::sort( converted_ranges.begin(), converted_ranges.end(), []( const Range& a, const Range& b ) { return a.start < b.start; } );
				//change the active range to the converted range
				active_ranges = converted_ranges;
			}
			//keep tabs on the lowest location value returned from the mapping process
			if ( active_ranges[0].start < lowest_location )
			{
				lowest_location = active_ranges[0].start;
			}
		}
		return lowest_location;
	}


}

void aoc::day_05()
{
	day_05::Seeds_Maps seed_n_maps = day_05::read_input_file( "./data/day_05_input.txt" );

	std::cout << "Part 1: Lowest location number for input seed values is " << day_05::part_01( seed_n_maps ) << std::endl;
	std::cout << "Part 2: Lowest location number for input seed values is " << day_05::part_02( seed_n_maps ) << std::endl;

}