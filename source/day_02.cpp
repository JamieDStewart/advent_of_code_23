//\==============================================================================
// Advent of code 2023 Day 2
// Question: https://adventofcode.com/2023/day/2
// Gist:  Play a game of probability with an elf and a bag of coloured cubes
// Input data: some possible game values from series of sets of a game of how many cubes in the bag
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace day_02
{
	//a set contains a number of coloured rgb cubes
	struct RGB_set
	{
		uint32_t r;
		uint32_t g;
		uint32_t b;
	};

	//a game is a number of sets with an ID
	struct Game
	{
		uint32_t id;
		std::vector<RGB_set> sets;
	};

	std::vector<Game> g_games;



	uint32_t get_game_id_from_string( const std::string& input )
	{
		const std::string digits = "0123456789";
		uint32_t game_id = {};
		//find the first digit in the line then then ':' to get the game ID
		size_t id_start = input.find_first_of( digits );
		size_t id_end = input.find_first_of( ':' );
		std::string game_string = std::string( input, id_start, id_end - id_start );
		std::stringstream ss( game_string );
		ss >> game_id;
		return game_id;
	}

	std::vector<RGB_set> get_game_sets_from_string( const std::string& input )
	{
		std::vector<RGB_set> sets = {};
		//split the string on the ';' symbol
		std::string set_string{};
		std::stringstream input_stream( input );
		//split the input string on the ';' character
		while ( std::getline( input_stream, set_string, ';' ) )
		{
			RGB_set current_set = {};
			//string is now in the format "# colour, # colour"
			//split on ','
			std::string count_colour_string{};
			std::stringstream count_colour_stream( set_string );
			while ( std::getline( count_colour_stream, count_colour_string, ',' ) )
			{
				//string will be in format " # colour"
				//strip any leading spaces
				size_t start = input.find_first_of( "0123456789" );
				uint32_t value = {};
				std::string colour = {};
				//use stream to get colour string and value
				std::stringstream count_colour( count_colour_string.substr( start ) );
				count_colour >> value;
				count_colour >> colour;
				if ( colour == "red" )
				{
					current_set.r = value;
				}
				else if ( colour == "blue" )
				{
					current_set.b = value;
				}
				else
				{
					current_set.g = value;
				}
			}
			sets.push_back( current_set );
		}
		return sets;
	}

	void read_input_from_file( std::string filename )
	{

		//open the input file for and parse into a vector of games
		std::ifstream file_input;
		file_input.open( filename );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//Get the games id from the line
					const uint32_t game_id = get_game_id_from_string( line );
					//get the number sets for this game
					const size_t game_data_start = line.find_first_of( ':' ) + 1;
					const std::vector<RGB_set> game_sets = get_game_sets_from_string( line.substr( game_data_start ) );
					g_games.push_back( { game_id, game_sets } );
				}

			}
			//close the input file
			file_input.close();
		}
	}

	uint32_t part_1()
	{
		//bag is loaded with max 12 red, 13 green, 14 blue
		constexpr uint32_t valid_red = 12;
		constexpr uint32_t valid_green = 13;
		constexpr uint32_t valid_blue = 14;
		//games that contain fewer  cubes than these values are valid
		uint32_t sum_valid_game_ids = {};
		for ( const auto& game : g_games )
		{
			bool is_valid = true;
			for ( const auto& set : game.sets )
			{
				if ( set.r > valid_red ||
					 set.g > valid_green ||
					 set.b > valid_blue )
				{
					is_valid = false;
				}
			}
			if ( is_valid )
			{
				sum_valid_game_ids += game.id;
			}
		}
		return sum_valid_game_ids;
	}

	uint32_t part_2()
	{
		//work out powers of each set based off min cubes needed from sets.
		uint32_t sum_game_power = {};
		for ( const auto& game : g_games )
		{
			uint32_t min_red = {};
			uint32_t min_green = {};
			uint32_t min_blue = {};

			for ( const auto& set : game.sets )
			{
				//find the min number of cubes of colour needed to validate the game
				min_red = set.r > min_red ? set.r : min_red;
				min_green = set.g > min_green ? set.g : min_green;
				min_blue = set.b > min_blue ? set.b : min_blue;
			}

			sum_game_power += min_red * min_green * min_blue;
		}
		return sum_game_power;

	}
}


Result aoc::day_02()
{
	timer::start();
	//read file input
	day_02::read_input_from_file( "./data/day_02_input.txt" );

	const uint32_t part_1_answer = day_02::part_1();
	const uint32_t part_2_answer = day_02::part_2();
	timer::stop();

	return { std::string( " 2: Cubes in Bags" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}
