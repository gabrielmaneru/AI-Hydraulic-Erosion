#include "Stdafx.h"
#include <functional>

bool a_star::initialize(vec3 start, vec3 goal, int flags, float heuristicWeight, int heuristicMethod)
{
	// Set input data
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
	
	// Insert initial node
	insert_open_list(node{ c_start, utils.c_root, 0.0f });
	return false;
}

bool a_star::iterate()
{
	do
	{
		// Check if is empty
		if (m_open_list.empty())
			return true;

		// find node minimum cost
		node next_node = find_node_minimum_cost();

		// terminating check
		if (next_node.m_coord == coord{ m_goal })
		{
			create_waypoint_list(next_node);
			return true;
		}

		// Add Next Neighboors
		insert_possible_neighboors(next_node);
	} while (!is_flag(e_singlestep));
	return false;
}

void a_star::reset()
{
	m_waypoints.clear();
	m_open_list.clear();
	m_closed_list.clear();
}

void a_star::create_waypoint_list(node next_node)
{
	m_waypoints.clear();
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

	if (is_flag(e_rubberband) && m_waypoints.size() > 2)
	{
		auto start = m_waypoints.begin();
		auto mid = start; mid++;
		auto end = mid; end++;
		while ( end != m_waypoints.end())
		{
			coord c_start{ *start };
			coord c_end{ *end };
			if (is_clear_path(c_start, c_end))
			{
				end++;
				mid = m_waypoints.erase(mid);
			}
			else
				end++, mid++, start++;
		}
	}
	if (is_flag(e_smooth))
	{
		// Perform Mid Point Subdivision
		{
			float max_size_sq = powf(1.5f / g_terrain.GetWidth(), 2.0f);
			auto start = m_waypoints.begin();
			auto end = start; end++;
			while (end != m_waypoints.end())
			{
				D3DXVECTOR3 delta = *start - *end;
				if (D3DXVec3LengthSq(&delta) > max_size_sq)
					end = m_waypoints.insert(end, .5f *(*start + *end));
				else
					start++, end++;
			}
		}

		// Perform Catmull Smoothing
		{
			if (m_waypoints.size() <2)
				return;

			auto c_0 = m_waypoints.begin();
			auto p_0 = c_0; p_0++;
			if (m_waypoints.size() == 2)
				return insert_smoothing(c_0, c_0, p_0, p_0);
			auto p_1 = p_0; p_1++;
			auto c_1 = p_1; c_1++;
			while (1)
			{
				if(c_0 == m_waypoints.begin())
					insert_smoothing(c_0, c_0, p_0, p_1);

				if (c_1 != m_waypoints.end())
				{
					insert_smoothing(c_0, p_0, p_1, c_1);
					c_0 = p_0;
					p_0 = p_1;
					p_1 = c_1;
					c_1++;
				}
				else
				{
					insert_smoothing(c_0, p_0, p_1, p_1);
					break;
				}
			}
		}
	}
}

void a_star::insert_smoothing(	const std::list<vec3>::iterator& control_0,
								const std::list<vec3>::iterator& point_0,
								const std::list<vec3>::iterator& point_1,
								const std::list<vec3>::iterator& control_1)
{
	vec3 val[3];
	D3DXVec3CatmullRom(val  , &*control_0, &*point_0, &*point_1, &*control_1, .25f);
	D3DXVec3CatmullRom(val+1, &*control_0, &*point_0, &*point_1, &*control_1, .5f);
	D3DXVec3CatmullRom(val+2, &*control_0, &*point_0, &*point_1, &*control_1, .75f);
	m_waypoints.insert(point_1, val[0]);
	m_waypoints.insert(point_1, val[1]);
	m_waypoints.insert(point_1, val[2]);
}

void a_star::insert_possible_neighboors(const node & next_node)
{
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
		float new_cost = next_node.m_movement_cost + n.cost;

		// Check same node is already in closed list
		std::list<node>::iterator copy_in_list = m_closed_list.end();
		for (auto it = m_closed_list.begin(); it != m_closed_list.end(); it++)
			if (it->m_coord == c_neighboor)
			{
				copy_in_list = it;
				break;
			}
		if (copy_in_list != m_closed_list.end()) continue;

		// Check same node is already in open list
		copy_in_list = m_open_list.end();
		for (auto it = m_open_list.begin(); it != m_open_list.end(); it++)
			if (it->m_coord == c_neighboor)
			{
				copy_in_list = it;
				break;
			}
		if (copy_in_list == m_open_list.end())
			insert_open_list(node{ c_neighboor, next_node.m_coord, new_cost });
		else if (new_cost < copy_in_list->m_movement_cost)
		{
			copy_in_list->m_parent = next_node.m_coord;
			copy_in_list->m_movement_cost = new_cost;
		}
	}
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

void a_star::insert_open_list(node && c)
{
	c.m_heuristic_cost = compute_heuristic(c.m_coord);
	g_terrain.SetColor(c.m_coord.r, c.m_coord.c, DEBUG_COLOR_BLUE);
	m_open_list.emplace_back(c);
}

a_star::node a_star::find_node_minimum_cost()
{
	std::list<node>::iterator minimum_cost = m_open_list.begin();
	float min_found = minimum_cost->m_heuristic_cost + minimum_cost->m_movement_cost;
	for (auto it = minimum_cost; it != m_open_list.end(); it++)
	{
		float local_cost = it->m_heuristic_cost + it->m_movement_cost;
		if (local_cost < min_found)
			minimum_cost = it, min_found = local_cost;
	}

	//for (auto it = m_open_list.begin(); it != m_open_list.end(); it++)
	//{
	//	if (it == minimum_cost)
	//		std::cout << "  ";
	//	std::cout << "(" << it->m_coord.r << "," << it->m_coord.c << "): " << it->m_cost << std::endl;
	//}
	//std::cout << std::endl;

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

float a_star::compute_heuristic(const coord & c)
{
	coord delta = c - coord(m_goal);
	float value;
	switch (m_heuristic_method)
	{
	case 0: // Euclidean
		value = sqrtf(delta.r * delta.r + delta.c * delta.c);
		break;
	case 1: // Octile
		value = min(fabsf(delta.r), fabsf(delta.c)) * sqrtf(2.0f)
		+ max(fabsf(delta.r), fabsf(delta.c)) - min(fabsf(delta.r), fabsf(delta.c));
		break;
	case 2: // Chebyshev
		value = max(fabsf(delta.r), fabsf(delta.c));
		break;
	case 3: // Manhattan
		value = fabsf(delta.r) + fabsf(delta.c);
		break;
	default:
		break;
	}
	return value * m_heuristic_weight;
}
