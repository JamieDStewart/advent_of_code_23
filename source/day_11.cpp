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

	struct Input_Map
	{
		std::vector<Galaxy_Location> locations;
		std::vector<uint64_t> x_offsets;
		std::vector<uint64_t> y_offsets;
		
	};

	Input_Map read_input_file( const std::string& input_file )
	{
		std::vector<Galaxy_Location> galaxy_locations = {};
		std::vector<uint64_t> x_offsets = {};
		std::vector<uint64_t> y_offsets = {};

		//file is a 2D map of x by y with symbols '#' for galaxies and '.' for empty space
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			uint64_t y_position = {};
			uint64_t y_offset = {};
			std::string blank_columns = {};
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//if blank_columns is empty initialize it
					if( blank_columns.empty() )
					{
						//set a string to be entirely blank each time a glalaxy is found fill in the location a x with '#'
						blank_columns = std::string( line.size(), '.' );
					}
					//find the first galaxy
					size_t sp = line.find_first_of( '#' );
					if( sp == std::string::npos )
					{
						//each time we have blank line increment the y offset 
						++y_offset;
					}
					y_offsets.push_back( y_offset );
					while ( sp != std::string::npos )
					{
						blank_columns[sp] = '#';
						galaxy_locations.push_back( { sp, y_position } );
						sp = line.find_first_of( '#', sp+1 );						
					}
					++y_position;
				}
			}
			//figure out which columns are still empty
			uint64_t x_offset = {};
			for ( const auto& c : blank_columns )
			{
				if( c == '.' )
				{
					++x_offset;
				}
				x_offsets.push_back( x_offset );
			}
			file_input.close();
		}

		//we now have a vector of all galaxy locations and two vectors of offset amounts in x and y directions
		//offset vectors are size of width and height of the galaxy map read in.
		return { galaxy_locations, x_offsets, y_offsets };
	}

	uint64_t accumulate_distances( const Input_Map& input_map, uint64_t offset )
	{
		std::vector<uint64_t> distances = {};
		Galaxy_Location first = {};

		const auto calc_distance = [&first, &input_map, offset]( Galaxy_Location gl )
		{
			//get the actual position of the galaxy with offsets applied from look up
			gl.x += (input_map.x_offsets[gl.x] * offset);
			gl.y += (input_map.y_offsets[gl.y] * offset);
			const uint64_t x_dist = std::max( first.x, gl.x ) - std::min( first.x, gl.x );
			const uint64_t y_dist = std::max( first.y, gl.y ) - std::min( first.y, gl.y );
			return x_dist + y_dist;
		};

		const auto& locations = input_map.locations;
		for ( auto iter = locations.begin(); iter != locations.end(); ++iter )
		{
			first = *iter;
			//get the actual position of the galaxy with the x/y offsets applied 
			first.x += (input_map.x_offsets[first.x] * offset);
			first.y += (input_map.y_offsets[first.y] * offset);

			std::transform( iter + 1, locations.end(), std::back_inserter( distances ), calc_distance	 );
		}			
		return std::accumulate( distances.begin(), distances.end(),0LLU );
	}
	
	uint64_t part_01( const Input_Map& input_map )
	{
		return accumulate_distances( input_map, 1 );
		
	}

	uint64_t part_02( const Input_Map& input_map )
	{
		return accumulate_distances( input_map, 999999 );
	}


}

Result aoc::day_11()
{
	timer::start();
	const day_11::Input_Map galaxy_locations = day_11::read_input_file( "./data/day_11_input.txt" );

	const uint64_t part_1_answer = day_11::part_01( galaxy_locations );

	const uint64_t part_2_answer = day_11::part_02( galaxy_locations );

	timer::stop();
	return { std::string( "11: Galaxy Maps" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}