#pragma once

using vec3 = D3DXVECTOR3;
class a_star
{
public:
	enum e_Flag {
		e_smooth = 1 << 0,
		e_rubberband = 1 << 1,
		e_straightline = 1 << 2,
		e_singlestep = 1 << 3,
	};
	bool initialize(vec3 start, vec3 goal, int flags, float heuristicWeight, int heuristicMethod);
	bool iterate();

	std::list<vec3> m_waypoints;

private:
	struct coord
	{
		coord(vec3& p) { g_terrain.GetRowColumn(&p, &r, &c); }
		coord(int r_, int c_) : r(r_), c(c_) {}
		bool operator==(const coord& other) const { return r == other.r && c == other.c; }
		coord operator+(const coord& other) const { return coord{ r + other.r, c + other.c }; }
		coord operator-(const coord& other) const { return coord{ r - other.r, c - other.c }; }
		bool is_wall() { return g_terrain.IsWall(r, c); }

		int r,c;
	};
	struct node
	{
		node(const coord& c, const coord& p, float cost)
			: m_coord(c), m_parent(p), m_movement_cost(cost) {}

		coord m_coord;
		coord m_parent;
		float m_heuristic_cost{0.0f};
		float m_movement_cost;
	};
	const struct
	{
		coord c_root{ -1,-1 };
		struct neighboor
		{
			neighboor(const coord& c, float f)
				:offset(c), cost(f) {}
			coord offset;
			float cost;
		};
		std::array<neighboor, 8> neighborhood{
			neighboor{ { 1,0 },	 1.0f },
			neighboor{ { 0,1 },	 1.0f },
			neighboor{ { -1,0 }, 1.0f },
			neighboor{ { 0,-1 }, 1.0f },
			neighboor{ { 1,1 },  sqrtf(2.0f) },
			neighboor{ { 1,-1 }, sqrtf(2.0f) },
			neighboor{ { -1,1 }, sqrtf(2.0f) },
			neighboor{ { -1,-1 },sqrtf(2.0f) }
		};
	} utils;
	
	void reset();
	void create_waypoint_list(node next_node);
	void insert_smoothing(	const std::list<vec3>::iterator& control_0,
							const std::list<vec3>::iterator& point_0,
							const std::list<vec3>::iterator& point_1,
							const std::list<vec3>::iterator& control_1);
	void insert_possible_neighboors(const node& next_node);
	void insert_open_list(node && c);
	node find_node_minimum_cost();
	bool is_clear_path(coord a, coord b);
	bool is_flag(e_Flag f);
	float compute_heuristic(const coord& c);

	float m_heuristic_weight;
	int m_heuristic_method;
	int m_flags;
	vec3 m_goal;
	std::list<node> m_open_list;
	std::list<node> m_closed_list;
};