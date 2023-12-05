//\==============================================================================
// Advent of code 2023 Day 4
// Question: https://adventofcode.com/2023/day/4
// Gist:  Elf gambling scratch cards
// Input data: The elf has a gambling problem help him add up winnings
//\==============================================================================


#include "days.h"
#include "result.h"
#include "timer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace day_04
{

	struct Card
	{
		std::vector<uint32_t> winning_numbers;
		std::vector<uint32_t> numbers_you_have;

		uint32_t copies = { 1 };
	};

	std::vector<Card> read_input_file( const std::string& input_file )
	{
		std::vector<Card> cards = {};
		//open the input file for and parse into a vector of games
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//find the ':' symbol and read from there up to the '|' symbol to get the vector of winning numbers
					size_t colon_pos = line.find_first_of( ':' ) + 1;
					size_t pipe_pos = line.find_first_of( '|', colon_pos );
					std::string winning = line.substr( colon_pos + 1, pipe_pos - colon_pos );
					//read from the '|' symbol to the end of line to find the numbers you have
					std::string numbers = line.substr( pipe_pos + 1 );
					std::vector<uint32_t> winning_numbers = {};
					//read out winning numbers
					uint32_t value = {};
					std::stringstream winning_stream( winning );
					while( winning_stream >> value )
					{
						winning_numbers.push_back( value );
					}
					std::vector<uint32_t> numbers_you_have = {};
					std::stringstream numbers_stream( numbers );
					while( numbers_stream >> value )
					{
						numbers_you_have.push_back( value );
					}
					cards.push_back( {winning_numbers, numbers_you_have} );
				}
			}
			file_input.close();
		}
		return cards;
	}

	

	uint32_t part_1( const std::vector<Card>& cards )
	{
		uint32_t accumulated_points = {};
		for( const auto& card : cards )
		{
			uint32_t matches = {};
			for( const auto& number : card.winning_numbers )
			{
				matches += static_cast<uint32_t>( std::count( card.numbers_you_have.begin(), card.numbers_you_have.end(), number ) );
			}
			if( matches != 0 )
			{
				accumulated_points += (1 << (matches - 1));
			}
		}
		
		return accumulated_points;
	}

	uint32_t part_2( std::vector<Card>& cards )
	{
		uint32_t index = 0;
		for ( const auto& card : cards )
		{
			uint32_t matches = {};
			for ( const auto& number : card.winning_numbers )
			{
				matches += static_cast<uint32_t>(std::count( card.numbers_you_have.begin(), card.numbers_you_have.end(), number ));
			}
			if ( matches != 0 )
			{
				//get the next matches number of cards and increment their copies value by the 1 * number of copies of this card
				for( uint32_t i = 1; i <= matches; ++i )
				{
					cards[index + i].copies += card.copies;
				}
			}

			++index; //increment the card index
		}

		return std::accumulate( cards.begin(), cards.end(), 0, 
								[]( uint32_t sum, const Card& curr )
								{
									return sum + curr.copies;
								});
	}


}

Result aoc::day_04()
{
	timer::start();
	std::vector<day_04::Card> cards = day_04::read_input_file( "./data/day_04_input.txt" );
	
	const uint64_t part_1_answer = day_04::part_1( cards );
	const uint64_t part_2_answer = day_04::part_2( cards );

	timer::stop();

	return { std::string( " 4: Scratchies!" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };
	
}