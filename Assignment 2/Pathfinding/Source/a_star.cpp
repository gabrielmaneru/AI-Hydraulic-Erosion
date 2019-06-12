#include "Stdafx.h"
#include <functional>

bool a_star::initialize(vec3 start, vec3 goal, int flags, float heuristicWeight, int heuristicMethod)
{
	m_goal = goal;
	m_flags = flags;
	m_heuristic_weight = heuristicWeight;
	m_heuristic_method = heuristicMethod;
	reset();

	// Check start == goal
	coord c_start{ start }, c_goal{ m_goal };
	if (c_start == c_goal	||	(is_flag(e_straightline) && is_clear_path(c_start, c_goal))	)
	{
		m_waypoints.push_back(m_goal);
		return true;
	}
	
	insert_open_list(node{ c_start, utils.c_root, 0.0f });
	return false;
}

bool a_star::iterate()
{
	if (m_open_list.empty())
		return true;

	// find node minimum cost
	node next_node = find_node_minimum_cost();

	// terminating check
	if (next_node.m_coord == coord{ m_goal })
	{
		m_waypoints.push_back(m_goal);
		while (!(next_node.m_parent == utils.c_root))
		{
			m_waypoints.push_front(g_terrain.GetCoordinates(next_node.m_parent.r, next_node.m_parent.c));
			for (auto it = m_closed_list.begin(); it != m_closed_list.end(); it++)
				if (it->m_coord == next_node.m_parent)
				{
					next_node = *it;
					break;
				}
		}
		return true;
	}

	// for each possible neighboor
	for (auto n : utils.neighborhood)
	{
		// Compute neighboor coordinates
		coord c_neighboor = next_node.m_coord + n.offset;

		// Check Map Boundaries
		if (!g_terrain.CheckBoundaries(c_neighboor.r, c_neighboor.c)) continue;

		// Check Availability
		if (!is_clear_path(next_node.m_coord, c_neighboor))continue;

		// Compute new cost
		float new_cost = next_node.m_cost + n.cost;

		// Check same node is already in closed list
		std::list<node>::iterator same_node = m_closed_list.end();
		for (auto it = m_closed_list.begin(); it != m_closed_list.end(); it++)
			if (it->m_coord == c_neighboor)
			{
				same_node = it;
				break;
			}
		if (same_node != m_closed_list.end()) continue;

		// Check same node is already in open list
		same_node = m_open_list.end();
		for (auto it = m_open_list.begin(); it != m_open_list.end(); it++)
			if (it->m_coord == c_neighboor)
			{
				same_node = it;
				break;
			}
		if (same_node == m_open_list.end())
			insert_open_list(node{ c_neighboor, next_node.m_coord, new_cost });
		else if(new_cost < same_node->m_cost)
		{
			same_node->m_parent = next_node.m_coord;
			same_node->m_cost = new_cost;
		}
	}
	return false;
}

void a_star::reset()
{
	m_waypoints.clear();
	m_open_list.clear();
	m_closed_list.clear();
}

bool a_star::is_clear_path(coord a, coord b)
{
	int r_min = (a.r < b.r) ? a.r : b.r;
	int r_max = (a.r > b.r) ? a.r : b.r;
	int c_min = (a.c < b.c) ? a.c : b.c;
	int c_max = (a.c > b.c) ? a.c : b.c;
	for (int r = r_min; r <= r_max; r++)
		for (int c = c_min; c <= c_max; c++)
			if (g_terrain.IsWall(r, c))
				return false;
	return true;
}

void a_star::insert_open_list(const node && c)
{
	g_terrain.SetColor(c.m_coord.r, c.m_coord.c, DEBUG_COLOR_BLUE);
	m_open_list.emplace_back(c);
}

a_star::node a_star::find_node_minimum_cost()
{
	std::list<node>::iterator minimum_cost = m_open_list.begin();
	for (auto it = minimum_cost; it != m_open_list.end(); it++)
		if (it->m_cost < minimum_cost->m_cost)
			minimum_cost = it;

	// take it out from the list
	node next_node = *minimum_cost;
	m_open_list.erase(minimum_cost);
	m_closed_list.emplace_back(next_node);
	g_terrain.SetColor(next_node.m_coord.r, next_node.m_coord.c, DEBUG_COLOR_YELLOW);
	return next_node;
}

bool a_star::is_flag(e_Flag f)
{
	return static_cast<bool>(m_flags & f);
}
