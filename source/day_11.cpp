//\==============================================================================
// Advent of code 2023 Day 11
// Question: https://adventofcode.com/2023/day/11
// Gist:  Map of galaxies find Manhattan distance
// Input data: a map of galaxies '#' and space '.'
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
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

	void adjust_position( std::vector<Galaxy_Location>& galaxy_locations, const uint64_t offset_distance, const bool adjust_x )
	{
		//iterate over galaxies if there is a difference of more than 1 between x position then increment x values
		uint64_t offset = {};
		// var explanation: cl == current_location, pl == previous_location
		uint64_t pl = {};
		for ( auto& [x, y] : galaxy_locations )
		{
			uint64_t cl = (adjust_x) ? x + offset : y + offset;
			if ( const uint64_t diff = std::max( cl, pl ) - std::min( cl, pl );  diff > 1 )
			{
				const uint64_t displacement =(diff - 1) * (offset_distance - 1);
				offset += displacement;
				cl += displacement;
			}
			pl = cl;
			if( adjust_x)
			{
				x = cl;
			}
			else
			{
				y = cl;
			}
		}
	}

	std::vector<Galaxy_Location> adjust_for_empty_space( const std::vector<Galaxy_Location>& galaxy_locations, const uint64_t offset_distance )
	{
		std::vector<Galaxy_Location> adjusted_galaxies = galaxy_locations;
		//need to adjust for empty column spaces
		std::ranges::sort( adjusted_galaxies.begin(), adjusted_galaxies.end(), []( const Galaxy_Location& a, const Galaxy_Location& b )
						   {
								//sort based on x location
								return a.x < b.x;
						   } );
		adjust_position( adjusted_galaxies, offset_distance, true );

		//need to adjust for empty row spaces
		std::ranges::sort( adjusted_galaxies.begin(), adjusted_galaxies.end(), []( const Galaxy_Location& a, const Galaxy_Location& b )
						   {
							   //sort based on x location
							   return a.y < b.y;
						   } );
		adjust_position( adjusted_galaxies, offset_distance, false );

		return adjusted_galaxies;
	}

	uint64_t accumulate_distances( const std::vector<Galaxy_Location>& locations )
	{
		uint64_t accumulated_distance = {};
		for ( auto iter = locations.begin(); iter != locations.end(); ++iter )
		{
			const Galaxy_Location first = *iter;
			for ( auto inner_iter = iter + 1; inner_iter != locations.end(); ++inner_iter )
			{
				const Galaxy_Location second = *inner_iter;
				const uint64_t x_dist = std::max( first.x, second.x ) - std::min( first.x, second.x );
				const uint64_t y_dist = std::max( first.y, second.y ) - std::min( first.y, second.y );
				accumulated_distance += (x_dist + y_dist);
			}
		}
		return accumulated_distance;
	}
	
	uint64_t part_01( std::vector<Galaxy_Location>& galaxy_locations )
	{
		//adjust galaxies for an offset space of 1
		const std::vector<Galaxy_Location> adjusted_galaxies = adjust_for_empty_space( galaxy_locations, 2LLU );
		return accumulate_distances( adjusted_galaxies );
		
	}

	uint64_t part_02( std::vector<Galaxy_Location>& galaxy_locations )
	{
		//adjust galaxies for an offset space of 1
		const std::vector<Galaxy_Location> adjusted_galaxies = adjust_for_empty_space( galaxy_locations, 1000000LLU );
		return accumulate_distances( adjusted_galaxies );
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