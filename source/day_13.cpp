//\==============================================================================
// Advent of code 2023 Day 13
// Question: https://adventofcode.com/2023/day/13
// Gist:  Mirrors on a map, find the point of reflection 
// Input data: A map of mirrors find the reflection point part 2 find the smudge and alternate reflection point
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <bitset>
#include <fstream>
#include <string>
#include <vector>

namespace day_13
{
	struct AshMap
	{
		std::vector<std::string> data;
		std::vector<std::string> rotated_data;
		int32_t h_reflection_row;
		int32_t v_reflection_row;
	};

	//honestly it's easier to rotate the map and keep this rotated data that calculate it all the time
	std::vector<std::string> rotate_map_data(const std::vector<std::string>& data )
	{
		int current_column = {};
		int current_row = {};
		std::vector<std::string> rotated = { data[0].length(), ""};
		while ( current_column < rotated.size() )
		{			
			for ( const auto& row : data )
			{
				rotated[current_column].push_back( row[current_row] );
			}
			current_column++;
			current_row++;
		}
		return rotated;
	}
	
	std::vector<AshMap> read_input_file( const std::string& input_file )
	{
		std::vector<AshMap> ash_maps;
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			std::vector<std::string> map;
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					map.push_back( line );
				}
				else
				{
					//end of a map
					const auto rotated = rotate_map_data( map );
					ash_maps.push_back( { map, rotated, -1, -1 } );
					map.clear();
				}
			}
			const auto rotated = rotate_map_data( map );
			ash_maps.push_back( { map, rotated, -1, -1 } );
			file_input.close();
		}

		return ash_maps;
	}

	//this looks for two rows in the dataset that are identical and then works outward from there to determine if adjacent rows are reflected
	// current reflection point is passed in as part 2 needs this as there should be a different point.
	int32_t find_reflection( const std::vector<std::string>& map_data, int32_t current_reflection_point )
	{
		for( int row = 0; row < map_data.size(); ++row )
		{
			int current_row = row;
			int next_row = row + 1;
			bool are_same = false;
			while ( current_row >= 0 && next_row < map_data.size() )
			{
				are_same = (map_data[current_row] == map_data[next_row]);
				if( are_same )
				{
					if( current_reflection_point == (row+1) ) //already found this point look for another further on
					{
						are_same = false;
						break;
					}
					//rows match need to test proceeding row and next row
					--current_row; ++next_row;
				}
				else
				{
					break;
				}
				
			}
			if( are_same )
			{
				return row + 1;
			}			
		}
		return -1;
	}


	uint64_t part_01( std::vector<AshMap>& ash_maps )
	{
		uint64_t reflective_sum = {};
		//looking for lines of reflection in the maps
		for( auto& map : ash_maps )
		{
			const int32_t horizontal_reflection_row = find_reflection( map.data, map.h_reflection_row );
			if( horizontal_reflection_row != -1 )
			{
				map.h_reflection_row = horizontal_reflection_row;
				reflective_sum += 100 * horizontal_reflection_row;
				continue;
			}
			const int32_t vertical_reflection_row = find_reflection( map.rotated_data, map.v_reflection_row );
			if( vertical_reflection_row != -1 )
			{
				map.v_reflection_row = vertical_reflection_row;
				reflective_sum += vertical_reflection_row;
			}

		}
		return reflective_sum;
	}

	struct Location
	{
		int32_t x;
		int32_t y;
	};

	std::vector<Location> detect_potential_smudge_locations( const std::vector<std::string> map_data )
	{
		const auto len = static_cast<uint32_t>(map_data[0].length());
		std::vector<Location> smudge_locations = {};
		for ( int32_t r = 0; r < map_data.size()-1; ++r )
		{
			for ( int32_t nr = r+1; nr < map_data.size(); ++nr )
			{
				std::bitset<32> r1( map_data[r], 0, map_data[r].length(), '.', '#' );
				std::bitset<32> r2( map_data[nr], 0, map_data[nr].length(), '.', '#' );
				const auto result = r1 ^ r2;
				if ( result.count() == 1 )
				{
					auto bit_string = result.to_string( '.', '#' );
					//trim leading zeroes
					bit_string = bit_string.substr( bit_string.size() - len );
					const auto x = static_cast<int32_t>(bit_string.find( '#' ));
					smudge_locations.push_back( { x, r } );
				}
			}

		}
		return smudge_locations;

	}

	int32_t test_potential_smudges_for_reflection( const std::vector<Location>& locations, std::vector<std::string>& data, const int32_t previous_reflection_point )
	{
		//for each h_location change the value and test for symmetry
		for ( const auto location : locations )
		{
			const char c = data[location.y][location.x];
			data[location.y][location.x] = (c == '#') ? '.' : '#';

			const int32_t reflection_row = find_reflection( data, previous_reflection_point );
			//switch the character back
			data[location.y][location.x] = c;
			if ( reflection_row != -1 && reflection_row != previous_reflection_point )
			{
				//now reflection point found
				return reflection_row;
			}
		}
		return -1;
	}

	uint64_t part_02( std::vector<AshMap>& ash_maps )
	{
		uint64_t reflective_sum = {};
		//There are smudges on the mirrors
		//horizontal reflection takes precedence over vertical
		//find the smudge -- search horizontally then vertically
		for( auto& map : ash_maps )
		{
			//not the best wording but looks like horizontal reflection takes place over vertical 
			std::vector<Location> h_locations = detect_potential_smudge_locations( map.data );
			int32_t reflection_point = test_potential_smudges_for_reflection( h_locations, map.data, map.h_reflection_row );
			if( reflection_point != -1 )
			{
				reflective_sum += 100LLU * reflection_point;
				continue;
			}
			std::vector<Location> v_locations = detect_potential_smudge_locations( map.rotated_data );
			reflection_point = test_potential_smudges_for_reflection( v_locations, map.rotated_data, map.v_reflection_row );
			if ( reflection_point != -1 )
			{
				reflective_sum += reflection_point;
			}			
		}

		return reflective_sum;
	}


}

Result aoc::day_13()
{
	timer::start();
	std::vector< day_13::AshMap> ash_maps = day_13::read_input_file( "./data/day_13_input.txt" );

	const uint64_t part_1_answer = day_13::part_01( ash_maps );

	const uint64_t part_2_answer = day_13::part_02( ash_maps );

	timer::stop();
	return { std::string( "13: Ash Maps" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}