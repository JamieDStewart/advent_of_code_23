//\==============================================================================
// Advent of code 2023 Day 17
// Question: https://adventofcode.com/2023/day/17
// Gist: Pathfinding with Dijkstra. Only allowed to move three squares in a direction before needing to turn
// Input data: Grid to navigate with costs per movement tile
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <fstream>
#include <iostream>
#include <queue>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace day_17
{

	using Map = std::vector<std::vector<uint32_t>>;

	uint32_t map_width = {};
	uint32_t map_height = {};

	Map read_input_file( const std::string& input_file )
	{
		Map map = {};
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					std::vector<uint32_t> map_row;
					for( const auto c : line )
					{
						map_row.push_back( std::atoi(&c) );
					}
					map.push_back( map_row );
				}				
			}
			map_width = static_cast<uint32_t>(map[0].size());
			map_height = static_cast<uint32_t>(map.size());
			file_input.close();
		}
		return map;
	}

	struct V2
	{
		int32_t x;
		int32_t y;

		bool operator ==( const V2& r ) const
		{
			return (x == r.x && y == r.y);
		}

		V2& operator +=( const V2& r )
		{
			x += r.x; y += r.y;
			return *this;
		}

		V2 operator +( const V2& r ) const
		{
			return { x + r.x, y + r.y };
		}
	};

	struct State
	{
		V2 pos;
		V2 dir;
		uint32_t distance;

		bool operator ==( const State& r) const
		{
			return pos == r.pos && dir == r.dir && distance == r.distance;
		}
	};

	template <class T>
	void hash_combine( std::size_t & s, const T & v )
	{
		std::hash<T> h;
		s ^= h( v ) + 0x9e3779b9 + (s << 6) + (s >> 2);
	}

	struct StateHash
	{
		std::size_t operator()( State const& s ) const
		{
			std::size_t res = 0;
			hash_combine( res, s.pos.x );
			hash_combine( res, s.pos.y );
			hash_combine( res, s.dir.x );
			hash_combine( res, s.dir.y );
			hash_combine( res, s.distance );
			return res;
		}
	};

	std::vector<std::pair<uint64_t, State>> get_neighbour_states( const std::pair<uint64_t,State>& state_cost, const Map& map, const uint32_t min_dist, const uint32_t max_dist )
	{
		auto [cost, state] = state_cost;
		std::vector<std::pair<uint64_t, State>> neighbors = {};
		//left, right, forward
		V2 directions[3] = { state.dir, {state.dir.y, -state.dir.x}, {-state.dir.y, state.dir.x} };
		for( const auto& dir : directions )
		{

			V2 next_pos = state.pos + dir;
			//check this is still inside the map bounds
			if ( next_pos.x < 0 || next_pos.x >= static_cast<int32_t>(map_width) || next_pos.y < 0 || next_pos.y >= static_cast<int32_t>(map_height) ) { continue; }//out of map range
			//get the cost of this neighbour
			const uint32_t new_cost = cost + map[next_pos.y][next_pos.x];
			uint32_t distance = state.distance + 1;
			bool allow_move = false;
			if( dir == state.dir )
			{
				//continuing in same line
				allow_move = distance <= max_dist;
			}
			else
			{
				allow_move = distance > min_dist;
				distance = 1;
			}
			if ( allow_move )
			{
				State next_state = { next_pos, dir, distance };
				neighbors.emplace_back( std::make_pair( new_cost, next_state) );
			}
		}
		return neighbors;		
	}

	uint64_t calculate_cost_to_move_to_location( const Map& map, const V2& start, const V2& end, const uint32_t min_dist, const uint32_t max_dist )
	{
		//custom less than operator for priority queue
		auto state_evaluator = []( const std::pair<uint64_t, State>& l, const std::pair<uint64_t, State>& r ) -> bool
		{
			return l.first > r.first;
		};

		//create a priority queue of states that will be used to navigate the map
		std::priority_queue< std::pair<uint64_t, State>, std::vector<std::pair<uint64_t, State>>, decltype(state_evaluator)> open_queue( state_evaluator );
		//push the two starting direction states into the queue (can start moving south or east)
		State south = { {0,0}, {0,1}, 1 };
		State east = { {0,0}, {1,0}, 1 };
		open_queue.push( std::make_pair(0, south ) );
		open_queue.push( std::make_pair( 0, east ) );
		//set up a dictionary for visited states
		std::unordered_map<State, uint64_t, StateHash> seen_states = {};

		while (!open_queue.empty())
		{
			//get the cheapest state off the stack
			auto [cost, current_state] = open_queue.top();
			open_queue.pop();
			//is this the location we've been looking for
			if( current_state.pos == end )
			{
				return cost;
			}
			//if it's not the state then get active neighbours
			std::vector<std::pair<uint64_t, State>> neighbours = get_neighbour_states( std::make_pair(cost, current_state), map, min_dist, max_dist );
			for( const auto& pair : neighbours )
			{
				//add each neighbor into the open queue
				if( !seen_states.contains(pair.second ) )
				{
					//add this state to the seen_states closed list
					seen_states[pair.second] = pair.first;

					//we've not been here yet
					open_queue.push( pair );
				}
			}
			
		}
		return 0LLU;
	}
	
	uint64_t part_01( const Map& map )
	{
		return calculate_cost_to_move_to_location( map, {0,0}, {static_cast<int32_t>(map_width-1), static_cast<int32_t>(map_height-1)}, 0, 3 );
	}

	uint64_t part_02( const Map& map )
	{
		return calculate_cost_to_move_to_location( map, { 0,0 }, { static_cast<int32_t>(map_width - 1), static_cast<int32_t>(map_height - 1) }, 4, 10 );;
	}


}

Result aoc::day_17()
{
	timer::start();
	const day_17::Map map = day_17::read_input_file( "./input/day_17.txt" );

	const uint64_t part_1_answer = day_17::part_01( map );

	const uint64_t part_2_answer = day_17::part_02( map );

	timer::stop();
	return { std::string( "17: Dijkstra's Lava" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}