//\==============================================================================
// Advent of code 2023 Day 8
// Question: https://adventofcode.com/2023/day/8
// Gist:  Follow the commands to successfully navigate a binary tree
// Input data: Branching Instructions followed by a series of nodes with children
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"


#include <fstream>
#include <map>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>


namespace day_08
{
	struct Map
	{
		std::string directions;
		std::map<std::string, std::pair<std::string, std::string>> parent_children;
	};

	Map read_input_file( std::string input_file )
	{
		std::string directions;
		std::map<std::string, std::pair<std::string, std::string>> parent_children;
		//file is a series of lines made up of hand info and bit amount
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			//The first line is the directions
			std::getline( file_input, directions );
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//each line is split into a key followed by by two branch ID's 
					std::string key = line.substr( 0, line.find_first_of( ' ' ) );
					std::string left = line.substr( line.find_first_of( '(' ) + 1, 3 );
					std::string right = line.substr( line.find_last_of( ')' ) - 3, 3 );
					std::pair<std::string, std::string> branches = { left, right };
					//add pair into dictionary
					parent_children[key] = branches;
				}
	
			}
			file_input.close();
		}
		return {directions, parent_children};
	}


	uint64_t part_01( Map& map )
	{
		//need to start at node 'AAA'
		std::string current_node = "AAA";
		const std::string end_node = "ZZZ";
		uint64_t steps = {};

		while ( current_node != end_node )
		{			
			const std::pair<std::string, std::string> current_children = map.parent_children[current_node];
			const char direction = map.directions[steps % map.directions.length()];
			current_node = (direction == 'L') ? current_children.first : current_children.second;
			++steps;
		}
		return steps;
	}

	uint64_t part_02( Map& map )
	{
		//simultaneously start on all nodes that end in 'A' attempt to get to where all nodes end in 'Z'
		std::vector<std::string> current_nodes = {};
		for( const auto& key : map.parent_children | std::views::keys)
		{
			if ( key[2] == 'A' )
			{
				current_nodes.push_back( key );
			}
		}
		//Iterating over the size of the example works but iterating over the size of the input will take an exceedingly long time
		//But then there's std::lcm to find lowest common multiple so for each path find the location to get to the first ending in 'Z'
		//then evaluate as the paths will loop
		std::vector<uint64_t> steps = {};		
		
		for ( auto& node : current_nodes )
		{
			uint64_t node_steps = {};
			bool condition_met = false;
			while ( !condition_met )
			{
				const std::pair<std::string, std::string> children = map.parent_children[node];
				const char direction = map.directions[node_steps % map.directions.length()];
				node = (direction == 'L') ? children.first : children.second;
				//if this node is at a node ending in 'Z' then end condition has been met
				condition_met = (node[2] == 'Z');
				node_steps++;
			}
			steps.push_back( node_steps );
		}

		//now to work out the lcm of the values in steps
		uint64_t lcm = 1;
		for( const auto s : steps )
		{
			lcm = std::lcm( lcm, s );
		}		
		return lcm;
	}


}

Result aoc::day_08()
{
	timer::start();
	day_08::Map map = day_08::read_input_file( "./input/day_08.txt" );

	const uint64_t part_1_answer = part_01( map );
	const uint64_t part_2_answer = part_02( map );

	timer::stop();
	return { std::string( " 8: Desert Map" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}