//\==============================================================================
// Advent of code 2023 Day 11
// Question: https://adventofcode.com/2023/day/11
// Gist:  Map of galaxies find manhattan distance
// Input data: a map of galaxies '#' and space '.'
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

namespace day_11
{
	struct Galaxy_Location
	{
		uint64_t x;
		uint64_t y;
	};

	std::vector<Galaxy_Location> read_input_file( const std::string& input_file )
	{
		std::vector<Galaxy_Location> galaxy_locations = {};
		
		//file is a 2D map of x by y with symbols '#' for galaxies and '.' for empty space
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			uint64_t y_position = 0;
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//find the first galaxy
					size_t sp = line.find_first_of( '#' );
					while ( sp != std::string::npos )
					{
						galaxy_locations.push_back( { sp, y_position } );
						sp = line.find_first_of( '#', sp+1 );						
					}
					++y_position;
				}
			}
			file_input.close();
		}		
		return galaxy_locations;
	}

	std::vector<Galaxy_Location> adjust_for_empty_space( std::vector<Galaxy_Location>& galaxy_locations, uint64_t offset_distance )
	{
		std::vector<Galaxy_Location> adjusted_galaxies = {};
		//need to adjust for empty column spaces
		std::ranges::sort( galaxy_locations.begin(), galaxy_locations.end(), []( const Galaxy_Location& a, const Galaxy_Location& b )
						   {
								//sort based on x location
								return a.x < b.x;
						   } );
		//iterate over galaxies if there is a difference of more than 1 between x position then increment x values
		uint64_t offset = {};		
		// var explanation: cl == current_location, pl == previous_location
		uint64_t pl = {};
		for ( const auto& location : galaxy_locations )
		{
			uint64_t cl = location.x + offset;
			const uint64_t diff = std::max( cl, pl ) - std::min(cl, pl );
			if ( diff > 1 )
			{
				const uint64_t displacement = (diff > 1) ? (diff - 1) * (offset_distance - 1): 0LLU;
				offset += displacement;
				cl += displacement;
			}
			pl = cl;
			adjusted_galaxies.push_back( {cl, location.y} );
		}

		//need to adjust for empty row spaces
		std::ranges::sort( adjusted_galaxies.begin(), adjusted_galaxies.end(), []( const Galaxy_Location& a, const Galaxy_Location& b )
						   {
							   //sort based on x location
							   return a.y < b.y;
						   } );
		//iterate over galaxies if there is a difference of more than 1 between x position then increment x values
		offset = 0;
		// var explanation: cl == current_location, pl == previous_location
		pl = 0;
		for ( auto& location : adjusted_galaxies )
		{
			uint64_t cl = location.y + offset;
			const uint64_t diff = std::max( cl, pl ) - std::min( cl, pl );
			if ( diff > 1 )
			{
				const uint64_t displacement = (diff > 1) ? (diff - 1) * (offset_distance -1) : 0LLU;
				offset += displacement;
				cl += displacement;
			}
			pl = cl;
			location.y = cl;
		}

		return adjusted_galaxies;
	}
	
	uint64_t part_01( std::vector<Galaxy_Location>& galaxy_locations )
	{
		//adjust galaxies for an offset space of 1
		std::vector<Galaxy_Location> adjusted_galaxies = adjust_for_empty_space( galaxy_locations, 2LLU );
		//sort galaxies into pairs
		std::vector<std::pair<Galaxy_Location, Galaxy_Location>> galaxy_pairs = {};
		for( auto iter = adjusted_galaxies.begin(); iter != adjusted_galaxies.end(); ++iter )
		{
			for ( auto inner_iter = iter+1; inner_iter != adjusted_galaxies.end(); ++inner_iter )
			{
				galaxy_pairs.push_back( { *iter, *inner_iter } );
			}
		}
		//now all galaxies are in pairs go through and get their Manhattan distance
		std::vector<uint64_t> distances = {};
		for( const auto& pair : galaxy_pairs )
		{
			const uint64_t x_dist = std::max(pair.first.x, pair.second.x) - std::min( pair.first.x, pair.second.x );
			const uint64_t y_dist = std::max( pair.first.y, pair.second.y ) - std::min( pair.first.y, pair.second.y );
			distances.push_back( x_dist + y_dist );
		}

		return std::accumulate(distances.begin(), distances.end(), 0LLU );
	}

	uint64_t part_02( std::vector<Galaxy_Location>& galaxy_locations )
	{
		//adjust galaxies for an offset space of 1
		std::vector<Galaxy_Location> adjusted_galaxies = adjust_for_empty_space( galaxy_locations, 1000000LLU );
		//sort galaxies into pairs
		std::vector<std::pair<Galaxy_Location, Galaxy_Location>> galaxy_pairs = {};
		for ( auto iter = adjusted_galaxies.begin(); iter != adjusted_galaxies.end(); ++iter )
		{
			for ( auto inner_iter = iter + 1; inner_iter != adjusted_galaxies.end(); ++inner_iter )
			{
				galaxy_pairs.push_back( { *iter, *inner_iter } );
			}
		}
		//now all galaxies are in pairs go through and get their Manhattan distance
		std::vector<uint64_t> distances = {};
		for ( const auto& pair : galaxy_pairs )
		{
			const uint64_t x_dist = std::max( pair.first.x, pair.second.x ) - std::min( pair.first.x, pair.second.x );
			const uint64_t y_dist = std::max( pair.first.y, pair.second.y ) - std::min( pair.first.y, pair.second.y );
			distances.push_back( x_dist + y_dist );
		}

		return std::accumulate( distances.begin(), distances.end(), 0LLU );
	}


}

Result aoc::day_11()
{
	timer::start();
	std::vector<day_11::Galaxy_Location> galaxy_locations = day_11::read_input_file( "./data/day_11_input.txt" );

	const uint64_t part_1_answer = day_11::part_01( galaxy_locations );

	const uint64_t part_2_answer = day_11::part_02( galaxy_locations );

	timer::stop();
	return { std::string( "11: Galaxy Maps" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}