//\==============================================================================
// Advent of code 2023 Day 10
// Question: https://adventofcode.com/2023/day/10
// Gist:  Map of pipes find the circuit and the point furthest away
// Input data: a map of | - L F 7 J for directions and . for empty space
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

namespace day_10
{

	struct Map_Location
	{
		int32_t x;
		int32_t y;

		bool operator ==(const Map_Location& r) const 
		{
			return (x == r.x && y == r.y);
		}
	};

	struct Map
	{
		std::vector<std::string> map_data;
		Map_Location starting_point;
		int32_t width;
		int32_t height;
	};

	Map read_input_file( const std::string& input_file )
	{
		std::vector<std::string> map_data = {};
		Map_Location start_point = {};
		//file is a 2D map of x by y with symbols 'F, 7, L, J, -, | ' for pipes and . for empty space
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			//The first line is the directions
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					map_data.push_back( line );
					size_t sp = line.find( 'S' );
					if( sp != std::string::npos )
					{
						start_point = { static_cast<int32_t>(sp), static_cast<int32_t>(map_data.size()-1) };
					}
				}
			}
			file_input.close();
		}
		return {map_data, start_point, static_cast<int32_t>(map_data[0].size()), static_cast<int32_t>(map_data.size()) };
	}

	std::vector<std::pair<int32_t, int32_t>> get_exit_directions( const char& pipe )
	{
		switch( pipe )
		{
		case '|':
			return { {0, -1}, {0, 1} };
		case '-':
			return { {-1, 0}, {1, 0} };
		case '7':
			return { {-1, 0}, {0, 1} };
		case 'J':
			return { {-1, 0}, {0, -1} };
		case 'F':
			return { {1, 0}, {0, 1} };
		case 'L':
			return { {1, 0}, {0, -1} };
		default:
			return { {0, 0}, {0, 0} };
		}
	}

	std::vector<Map_Location> locate_main_loop( Map& map )
	{
		//first find the main loop in the file
		//we don't know what direction the starting point is so need to look at all points around it
		bool loop_found = false;
		std::pair<int32_t, int32_t> starting_directions[4] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
		std::vector<Map_Location> main_loop = { {map.starting_point } };
		for( const auto& dir : starting_directions )
		{
			//get the next location for the map
			Map_Location current_location = { main_loop[0].x + dir.first, main_loop[0].y + dir.second };
			while ( !loop_found )
			{
				//test to see if the next location is on the map
				if ( current_location.x >= 0 && current_location.x < map.width && current_location.y >= 0 && current_location.y < map.height )
				{
					const char map_node_value = map.map_data[current_location.y][current_location.x];
					if( map_node_value == 'S' )
					{
						loop_found = true;
						break;
					}
											
					//now follow the symbols to the next location
					std::vector<std::pair<int32_t, int32_t>> exits = get_exit_directions( map_node_value );
					//one of the exits should lead back to the previous location
					Map_Location next_location = {};
					bool valid_neighbour = false;
					for( const auto& exit : exits )
					{
						const Map_Location neighbour = { current_location.x + exit.first, current_location.y + exit.second };
						if( neighbour == main_loop[main_loop.size()-1] )
						{
							valid_neighbour = true;
							//this is a valid next location
							main_loop.push_back( { current_location } );
						}
						else
						{
							next_location = neighbour;
						}
					}

					if( valid_neighbour )
					{
						current_location = next_location;
					}
					else
					{
						main_loop = { {map.starting_point } };
						break;
					}					

				}
				else
				{
					main_loop = { {map.starting_point } };
					break;
				}
			}

		}
		//if there is a loop main_loop will not be empty and max distance from start will half the size of this vector
		return main_loop;
	}

	uint64_t part_01( std::vector<Map_Location> main_loop )
	{
		return (main_loop.size() >> 1) + main_loop.size() % 2;
	}

	uint64_t part_02( std::vector<Map_Location>& main_loop, Map& map )
	{
		//sort the loop by row then column
		std::sort( main_loop.begin(), main_loop.end(), []( Map_Location a, Map_Location b )
				   {
					   if ( a.y == b.y )
					   {
						   return a.x < b.x;
					   }
					   return (a.y < b.y);
				   } );
		uint64_t enclosed_tiles = 0;
		//iterate over the whole map space and determine how many crossings occur
		//count symbols '|, F, 7' as a crossing
		int32_t y ={};
		for( const auto& row : map.map_data )
		{
			uint32_t crossings = 0;
			int32_t x = {};
			for( const auto& col : row )
			{
				Map_Location loc = { x, y };
				//test to see if position is part of loop
				//only need to test first item in loop if matches then remove and carry on
				if ( main_loop.size() > 0 && main_loop[0] == loc )
				{
					//this is part of the main loop if the symbol is '|,F, 7' then increment the crossing value
					if ( col == '|' || col == 'F' || col == '7' )
					{
						++crossings;
					}
					main_loop.erase( main_loop.begin() );
				}
				else if( crossings % 2 != 0 )
				{
					//this position is enclosed by the loop
					++enclosed_tiles;
				}
				++x;
			}
			++y;
		}
		return enclosed_tiles;
	}


}

Result aoc::day_10()
{
	timer::start();
	day_10::Map map = day_10::read_input_file( "./input/day_10.txt" );
	//locate the main loop within the input data
	std::vector<day_10::Map_Location> main_loop = locate_main_loop( map );

	const uint64_t part_1_answer = day_10::part_01( main_loop );
	
	const uint64_t part_2_answer = day_10::part_02( main_loop, map );

	timer::stop();
	return { std::string( "10: Pipe Loops" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}