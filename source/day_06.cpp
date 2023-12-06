//\==============================================================================
// Advent of code 2023 Day 5
// Question: https://adventofcode.com/2023/day/5
// Gist:  Seeds need to go in appropriate soil all about changing ranges until location is found
// Input data: Starts with seed Id's then a series of values describing ranges
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace day_06
{
	struct Race
	{
		uint64_t time;
		uint64_t distance;
	};

	std::vector<Race> read_input_file( std::string input_file )
	{
		std::vector<Race> races = {};
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			//input file contains two lines race duration followed by race distance record
			std::string race_times = {};
			std::getline( file_input, race_times );
			std::stringstream rs( race_times.substr(race_times.find_first_of("0123456789" ) ) );
			
			std::string race_distances = {};
			std::getline( file_input, race_distances );
			std::stringstream rd( race_distances.substr( race_distances.find_first_of( "0123456789" ) ) );

			uint64_t time = {};
			uint64_t distance = {};
			//iterate over substrings and add races to the races vector
			while( rs >> time )
			{
				rd >> distance; //file input guarantees that there are the same number of entries for races & distance so safety is off.
				races.push_back( { time, distance } );
			}
			file_input.close();
		}
		return races;
	}

	bool meets_win_conditions( const uint64_t speed, const uint64_t remaining_time, const uint64_t record )
	{
		const uint64_t distance_travelled = speed * remaining_time;
		return distance_travelled > record;
	}

	uint64_t find_win_time( Race race )
	{
		uint64_t low = 0;
		uint64_t high = race.time;
		//start in the middle
		uint64_t mid_point = (low + high) >> 1;
		uint64_t winning_time = 0;
		while ( winning_time == 0 )
		{
			//is this the minimum winning condition?
			const uint64_t neighbouring_time = mid_point - 1;
			const bool neighbouring_win = meets_win_conditions( neighbouring_time, race.time - neighbouring_time, race.distance );
			const bool mid_point_win = meets_win_conditions( mid_point, race.time - mid_point, race.distance );
			if ( !neighbouring_win && mid_point_win )
			{
				winning_time = mid_point;
			}
			else if ( !neighbouring_win && !mid_point_win ) //both losses move forward in time
			{
				low = mid_point;
			}
			else //both wins so look further towards beginning
			{
				high = mid_point ;
			}
			mid_point = (low + high) >> 1;
			if ( mid_point == 0 || mid_point > race.time ) { break; }
		}
		return winning_time;
	}

	uint64_t calc_winning_range( Race race )
	{
		//use quadratic formula to find point parabola crosses the x axis if we treat race time as b and c as race distance (+1 to beat the distance)
		const double discriminant = sqrt( race.time * race.time - 4 * (race.distance + 1) );
		//determinant is just 2 as a value resolves to 1.
		const auto min = static_cast<uint64_t>(ceil((static_cast<double>(race.time) - discriminant)/2.0));
		const auto max = static_cast<uint64_t>(floor( (static_cast<double>(race.time) + discriminant)/2.0 ));
		return max - min + 1;
	}

	
	uint64_t part_01( std::vector<Race> races )
	{
		//To see how much margin of error you have, determine the number of ways you can beat the record in each race
		//Initial thought is to utlilise a binary search to find the lowest point that satisfies
		//winning condition then find upper point that satisfies winning condition then take range
		//returns multiplicitave of winning ranges
		std::vector<uint64_t> num_ways_to_win = {};
		for( const auto& race : races )
		{
			const uint64_t calc_opportunity_count = calc_winning_range( race );
			//const uint64_t min_time = find_win_time( race );
			////It's a symmetric curve so sub min from total time.
			//const uint64_t max_time = race.time - min_time;
			//const uint64_t opportunity_count = max_time - min_time +1;
			num_ways_to_win.push_back( calc_opportunity_count );
		}
		return std::accumulate( num_ways_to_win.begin(), num_ways_to_win.end(), 1, std::multiplies<>() );
	}

	uint64_t part_02( std::vector<Race> races )
	{
		//There was a kerning error - wouldn't you know it, lousy input.
		//There is only one race, and one distance to beat so combine input into a single race.
		std::stringstream rt;
		std::stringstream rd;
		for ( const auto& race : races )
		{
			rt << race.time;
			rd << race.distance;
		}
		uint64_t race_time = {};
		uint64_t race_distance = {};
		rt >> race_time;
		rd >> race_distance;
		Race race = { race_time, race_distance };

		const uint64_t opportunity_count = calc_winning_range( race );

		//const uint64_t min_time = find_win_time( race );
		////It's a symmetric curve so sub min from total time.
		//const uint64_t max_time = race.time - min_time;
		//const uint64_t opportunity_count = max_time - min_time + 1;
		return opportunity_count;
	}


}

Result aoc::day_06()
{
	timer::start();
	std::vector<day_06::Race> races = day_06::read_input_file( "./data/day_06_input.txt" );

	const uint64_t part_1_answer = day_06::part_01( races );
	const uint64_t part_2_answer = day_06::part_02( races );

	timer::stop();
	return { std::string( " 6: Boat Races" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}