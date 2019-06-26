/* Copyright Steve Rabin, 2012.
* All rights reserved worldwide.
*
* This software is provided "as is" without express or implied
* warranties. You may freely copy and compile this source into
* applications you distribute provided that the copyright text
* below is included in the resulting source code, for example:
* "Portions Copyright Steve Rabin, 2012"
*/

#include <Stdafx.h>


float Terrain::ClosestWall(int row, int col)
{
	float min_dist{ FLT_MAX };
	vec3 position = GetCoordinates(row, col);

	// Check Closest Border
	vec3 edges_projections[4] = { GetCoordinates(-1,col), GetCoordinates(m_width,col), GetCoordinates(row,-1), GetCoordinates(row,m_width) };
	for (int edge = 0; edge < 4; edge++)
	{
		D3DXVECTOR3 delta = edges_projections[edge] - position;
		float dist = D3DXVec3Length(&delta);
		if (dist < min_dist)
			min_dist = dist;
	}

	// Check Closest Wall
	for (int r = 0; r < m_width; r++)
		for(int c = 0; c < m_width; c++)
			if (IsWall(r, c))
			{
				vec3 wall_pos = GetCoordinates(r, c);
				D3DXVECTOR3 delta = wall_pos - position;
				float dist = D3DXVec3Length(&delta);
				if (dist < min_dist)
					min_dist = dist;
			}
	return min_dist;
}

void Terrain::AnalyzeOpennessClosestWall(void)
{
	// TODO: Implement this for the Terrain Analysis project.

	// Mark every square on the terrain (m_terrainInfluenceMap[r][c]) with
	// the value 1/(d*d), where d is the distance to the closest wall in 
	// row/column grid space.
	// Edges of the map count as walls!

	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
			if (IsWall(r, c))
				m_terrainInfluenceMap[r][c] = 1.0f;
			else
				m_terrainInfluenceMap[r][c] = 1.0f / pow(ClosestWall(r, c)*m_width, 2.0f);
}

void Terrain::AnalyzeVisibility(void)
{
	// TODO: Implement this for the Terrain Analysis project.

	// Mark every square on the terrain (m_terrainInfluenceMap[r][c]) with
	// the number of grid squares (that are visible to the square in question)
	// divided by 160 (a magic number that looks good). Cap the value at 1.0.

	// Two grid squares are visible to each other if a line between 
	// their centerpoints doesn't intersect the four boundary lines
	// of every walled grid square. Put this code in IsClearPath().


	for (int r0 = 0; r0 < m_width; r0++)
		for (int c0 = 0; c0 < m_width; c0++)
		{
			if (g_terrain.IsWall(r0, c0))
			{
				m_terrainInfluenceMap[r0][c0] = 0.0f;
				continue;
			}

			int cell_count{ 0 };
			for (int r1 = 0; r1 < m_width; r1++)
				for (int c1 = 0; c1 < m_width; c1++)
					if (!g_terrain.IsWall(r1, c1) && IsClearPath(r0, c0, r1, c1))
						cell_count++;
			m_terrainInfluenceMap[r0][c0] = min(static_cast<float>(cell_count)/160.0f,1.0f);
		}

}

void Terrain::AnalyzeVisibleToPlayer(void)
{
	// TODO: Implement this for the Terrain Analysis project.

	// For every square on the terrain (m_terrainInfluenceMap[r][c])
	// that is visible to the player square, mark it with 1.0.
	// For all non 1.0 squares that are visible to, and next to 1.0 squares,
	// mark them with 0.5. Otherwise, the square should be marked with 0.0.

	// Two grid squares are visible to each other if a line between 
	// their centerpoints doesn't intersect the four boundary lines
	// of every walled grid square. Put this code in IsClearPath().
	
	// Reset
	ResetInfluenceMap();

	int player[]{ g_blackboard.GetRowPlayer(), g_blackboard.GetColPlayer() };
	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
			if (IsClearPath(player[0], player[1], r, c))
			{
				m_terrainInfluenceMap[r][c] = 1.0f;
				
				if (r > 0 && m_terrainInfluenceMap[r - 1][c] < 0.5f)
					m_terrainInfluenceMap[r - 1][c] = 0.5f;
				if (r < m_width - 1 && m_terrainInfluenceMap[r + 1][c] < 0.5f)
					m_terrainInfluenceMap[r + 1][c] = 0.5f;
				if (c > 0 && m_terrainInfluenceMap[r][c - 1] < 0.5f)
					m_terrainInfluenceMap[r][c - 1] = 0.5f;
				if (c < m_width - 1 && m_terrainInfluenceMap[r][c + 1] < 0.5f)
					m_terrainInfluenceMap[r][c + 1] = 0.5f;
			}

}

void Terrain::AnalyzeSearch(void)
{
	// TODO: Implement this for the Terrain Analysis project.

	// For every square on the terrain (m_terrainInfluenceMap[r][c])
	// that is visible by the player square, mark it with 1.0.
	// Otherwise, don't change the value (because it will get
	// decremented with time in the update call).
	// You must consider the direction the player is facing:
	// D3DXVECTOR2 playerDir = D3DXVECTOR2(m_dirPlayer.x, m_dirPlayer.z)
	// Give the player a field of view a tad greater than 180 degrees.

	// Two grid squares are visible to each other if a line between 
	// their centerpoints doesn't intersect the four boundary lines
	// of every walled grid square. Put this code in IsClearPath().

	int player[]{ g_blackboard.GetRowPlayer(), g_blackboard.GetColPlayer() };
	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
			if (IsClearPath(player[0], player[1], r, c))
			{
				vec3 line_of_sight[2]{ GetCoordinates(player[0],player[1]), GetCoordinates(r, c) };
				vec3 dir = line_of_sight[1] - line_of_sight[0];
				D3DXVec3Normalize(&dir, &dir);
				float dot_p = D3DXVec3Dot(&dir, &m_dirPlayer);
				if (dot_p > 0.0f)
					m_terrainInfluenceMap[r][c] = 1.0f;
			}
}

bool Terrain::IsClearPath(int r0, int c0, int r1, int c1)
{
	// TODO: Implement this for the Terrain Analysis project.

	// Two grid squares (r0,c0) and (r1,c1) are visible to each other 
	// if a line between their centerpoints doesn't intersect the four 
	// boundary lines of every walled grid square. Make use of the
	// function LineIntersect(). You should also puff out the four
	// boundary lines by a very tiny bit so that a diagonal line passing
	// by the corner will intersect it.

	if (g_terrain.IsWall(r0, c0) || g_terrain.IsWall(r1, c1))
		return false;

	int r_min = (r0 < r1) ? r0 : r1;
	int r_max = (r0 > r1) ? r0 : r1;
	int c_min = (c0 < c1) ? c0 : c1;
	int c_max = (c0 > c1) ? c0 : c1;
	const float half_square_size = 0.5f / static_cast<float>(m_width);
	const float epsi = half_square_size*0.01f;
	vec3 line_of_sight[2]{ GetCoordinates(r0,c0), GetCoordinates(r1,c1) };

	for (int r = r_min; r <= r_max; r++)
		for (int c = c_min; c <= c_max; c++)
			if (g_terrain.IsWall(r, c))
			{
				vec3 square_center = GetCoordinates(r, c);
				vec3 square_vertices[4]{	square_center + vec3(-half_square_size,0.0f,-half_square_size),
											square_center + vec3(-half_square_size,0.0f,half_square_size),
											square_center + vec3(half_square_size,0.0f,half_square_size),
											square_center + vec3(half_square_size,0.0f,-half_square_size) };

				if (LineIntersect(line_of_sight[0].x, line_of_sight[0].z, line_of_sight[1].x, line_of_sight[1].z, square_vertices[0].x, square_vertices[0].z-epsi, square_vertices[1].x, square_vertices[1].z+epsi))
					return false;
				if (LineIntersect(line_of_sight[0].x, line_of_sight[0].z, line_of_sight[1].x, line_of_sight[1].z, square_vertices[1].x-epsi, square_vertices[1].z, square_vertices[2].x+epsi, square_vertices[2].z))
					return false;
				if (LineIntersect(line_of_sight[0].x, line_of_sight[0].z, line_of_sight[1].x, line_of_sight[1].z, square_vertices[2].x, square_vertices[2].z+epsi, square_vertices[3].x, square_vertices[3].z-epsi))
					return false;
				if (LineIntersect(line_of_sight[0].x, line_of_sight[0].z, line_of_sight[1].x, line_of_sight[1].z, square_vertices[3].x+epsi, square_vertices[3].z, square_vertices[0].x-epsi, square_vertices[0].z))
					return false;
			}
	return true;
}

void Terrain::Propagation(float decay, float growing, bool computeNegativeInfluence)
{
	// TODO: Implement this for the Occupancy Map project.

	// computeNegativeInfluence flag is true if we need to handle two agents
	// (have both positive and negative influence)
	// computeNegativeInfluence flag is false if we only deal with positive
	// influence (we should ignore all negative influence)

	// Pseudo code:
	//
	// For each tile on the map
	//
	//   Get the influence value of each neighbor after decay
	//   Then keep the decayed influence WITH THE HIGHEST ABSOLUTE.
	//
	//   Apply linear interpolation to the influence value of the tile, 
	//   and the maximum decayed influence value from all neighbors, with growing 
	//   factor as coefficient
	//
	//   Store the result to the temp layer
	//
	// Store influence value from temp layer
	std::vector<float> temp_layer(static_cast<size_t>(m_width*m_width));
	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
		{
			float highest_value{0.0f};
			for (int rn = max(r - 1, 0); rn < min(r + 2, m_width); rn++)
				for (int cn = max(c - 1, 0); cn < min(c + 2, m_width); cn++)
					if (rn != r || cn != c)
					{
						vec3 orig = GetCoordinates(r, c);
						vec3 neig = GetCoordinates(rn,cn);
						D3DXVECTOR3 delta = neig - orig;
						float dist = D3DXVec3Length(&delta);
						float decayed_val = m_terrainInfluenceMap[rn][cn] * expf(-1.0f*dist*decay);
						if (computeNegativeInfluence)
						{
							if (fabsf(decayed_val) > fabsf(highest_value))
								highest_value = decayed_val;
						}
						else
						{
							if (decayed_val > highest_value)
								highest_value = decayed_val;
						}
					}
			temp_layer[r * m_width + c] = Lerp(m_terrainInfluenceMap[r][c], highest_value, growing);
		}

	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
			m_terrainInfluenceMap[r][c] = temp_layer[r * m_width + c];
}

void Terrain::NormalizeOccupancyMap(bool computeNegativeInfluence)
{
	// TODO: Implement this for the Occupancy Map project.

	// divide all tiles with maximum influence value, so the range of the
	// influence is kept in [0,1]
	// if we need to handle negative influence value, divide all positive
	// tiles with maximum influence value, and all negative tiles with
	// minimum influence value * -1, so the range of the influence is kept
	// at [-1,1]
	
	// computeNegativeInfluence flag is true if we need to handle two agents
	// (have both positive and negative influence)
	// computeNegativeInfluence flag is false if we only deal with positive
	// influence, ignore negative influence 

	float min_value{ 0.0f }, max_value{ 0.0f };
	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
		{
			min_value = min(min_value, m_terrainInfluenceMap[r][c]);
			max_value = max(max_value, m_terrainInfluenceMap[r][c]);
		}
	for (int r = 0; r < m_width; r++)
		for (int c = 0; c < m_width; c++)
		{
			float value = m_terrainInfluenceMap[r][c];
			if (value < 0.0f)
			{
				if (computeNegativeInfluence)
					m_terrainInfluenceMap[r][c] /= fabsf(min_value);
			}
			else
				m_terrainInfluenceMap[r][c] /= max_value;
		}


}
