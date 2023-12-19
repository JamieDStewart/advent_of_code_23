//\==============================================================================
// Advent of code 2023 Day 14
// Question: https://adventofcode.com/2023/day/14
// Gist:  Rolling rocks and blocks, tilt the board and reposition round rocks
// Input data: A map of round rocks 'O', square rocks '#', and empty spaces '.'
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace day_14
{
	
	std::vector<std::string> read_input_file( const std::string& input_file )
	{
		std::vector<std::string> map;
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//find the locations of all movable rocks
					map.push_back( line );
				}
			}
			file_input.close();
		}

		return map;
	}

	void print_map( std::vector<std::string> map )
	{
		std::cout << std::endl;
		for ( auto row : map )
		{
			std::cout << row << std::endl;
		}
		std::cout << std::endl;
	}

	void move_north( std::vector<std::string>& map )
	{
		std::vector<int32_t> movement_vector( map[0].length(), 0 );

		uint32_t y = 0;
		for ( auto& row : map )
		{
			//iterate over the row moving any rocks by the amount in the movement vector
			for ( uint32_t i = 0; i < row.length(); ++i )
			{
				if ( row[i] == '.' ) { ++movement_vector[i]; continue; }	//increment the movement for this column
				if ( row[i] == '#' ) { movement_vector[i] = 0; continue; }//reset the movement
				//we've not got to move the 'O' by the amount in the movement vector
				if ( const uint32_t destination_row = y - movement_vector[i]; destination_row != y ) //don't set this if the row is the current one
				{
					map[destination_row][i] = 'O';
					row[i] = '.';
				}
			}
			++y;
		}
	}

	void move_west( std::vector<std::string>& map )
	{
		for ( auto& row : map )
		{
			uint32_t movement = 0;
			//iterate over the row moving any rocks by the amount in the movement vector
			for ( uint32_t i = 0; i < row.length(); ++i )
			{
				if ( row[i] == '.' ) { ++movement; continue; }	//increment the movement for this column
				if ( row[i] == '#' ) { movement = 0; continue; }//reset the movement
				//we've not got to move the 'O' by the amount in the movement vector
				if ( const uint32_t destination = i - movement; destination != i ) //don't set this if the row is the current one
				{
					row[destination] = 'O';
					row[i] = '.';
				}
			}
		}
	}

	void move_south( std::vector<std::string>& map )
	{
		std::vector<int32_t> movement_vector( map[0].length(), 0 );
		
		for ( auto r = static_cast<int32_t>(map.size() - 1); r >= 0; --r )
		{
			int32_t y = r;
			std::string& row = map[r];
			//iterate over the row moving any rocks by the amount in the movement vector
			for ( int32_t i = 0; i < row.length(); ++i )
			{
				if ( row[i] == '.' ) { ++movement_vector[i]; continue; }	//increment the movement for this column
				if ( row[i] == '#' ) { movement_vector[i] = 0; continue; }//reset the movement
				//we've not got to move the 'O' by the amount in the movement vector
				if ( const int32_t destination_row = y + movement_vector[i]; destination_row != y ) //don't set this if the row is the current one
				{
					map[destination_row][i] = 'O';
					row[i] = '.';
				}
			}
		}
	}

	void move_east( std::vector<std::string>& map )
	{
		for ( auto& row : map )
		{
			int32_t movement = 0;
			//iterate over the row moving any rocks by the amount in the movement vector
			for ( auto i = static_cast<int32_t>(row.length() - 1); i >= 0; --i )
			{
				if ( row[i] == '.' ) { ++movement; continue; }	//increment the movement for this column
				if ( row[i] == '#' ) { movement = 0; continue; }//reset the movement
				//we've not got to move the 'O' by the amount in the movement vector
				if ( const uint32_t destination = i + movement; destination != i ) //don't set this if the row is the current one
				{
					row[destination] = 'O';
					row[i] = '.';
				}
			}
		}
	}

	
	uint64_t part_01( std::vector<std::string>& map )
	{
		//print_map(map);
		move_north( map );
		// now go through the map and calculate load
		uint64_t row_load = map.size();
		uint64_t load_sum = 0;
		for ( auto row : map )
		{
			const uint64_t num_rocks = std::ranges::count( row.begin(), row.end(), 'O' );
			load_sum += num_rocks * row_load;
			--row_load;
		}
		return load_sum;
	}

	uint64_t part_02( std::vector<std::string>& map, uint64_t total_cycles )
	{
		std::map<std::vector<std::string>, std::pair<uint64_t, uint64_t>> cycle_map = {};
		//rotate the level N, W, S, E a billion times!
		//This will either take forever or there's a cycle! Rotate the level until a cycle is detected
		uint64_t cycle_count = 1;
		uint64_t lookup_index = 0;
		while ( lookup_index == 0 )
		{
			//undertake a cycle 
			move_north( map );
			move_west( map );
			move_south( map );
			move_east( map );
			
			//does this output exist in the cycle_map
			if( auto result = cycle_map.find( map ); result != cycle_map.end() )
			{
				//this item is in the map already work out the offset from the start
				auto [counter, load_sum] = result->second;
				const uint64_t loop_length = cycle_count - counter;
				lookup_index = counter + (total_cycles - counter) % loop_length;
				continue;
			}
			//calculate the load on the north wall
			// now go through the map and calculate load
			uint64_t row_load = map.size();
			uint64_t load_sum = 0;
			for ( auto row : map )
			{
				const uint64_t num_rocks = std::ranges::count( row.begin(), row.end(), 'O' );
				load_sum += num_rocks * row_load;
				--row_load;
			}
			//record the outcome in a dictionary
			cycle_map[map] = std::make_pair( cycle_count, load_sum );
			++cycle_count;
		}

		for( const auto& [key, value] : cycle_map )
		{
			if ( value.first == lookup_index )
			{
				return value.second;
			}
		}
		
		return 0LLU;
		
	}


}

Result aoc::day_14()
{
	timer::start();
	std::vector<std::string> map = day_14::read_input_file( "./input/day_14.txt" );

	const uint64_t part_1_answer = day_14::part_01( map );

	const uint64_t part_2_answer = day_14::part_02( map, 1000000000 );

	timer::stop();
	return { std::string( "14: Rock n Roll!" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}