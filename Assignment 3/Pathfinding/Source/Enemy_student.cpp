// Author: Chi-Hao Kuo
// Updated: 12/25/2015

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

void Enemy::FieldOfView(float angle, float closeDistance, float occupancyValue)
{
	// TODO: Implement this for the Occupancy Map project.

	// Parameters:
	//   angle - view cone angle in degree
	//   closeDistance - if the tile is within this distance, no need to check angle
	//   occupancyValue - if the tile is in FOV, apply this value

	// first, clear out old occupancyValue from previous frame
	// (mark m_terrainInfluenceMap[r][c] as zero if the old value is negative

	// For every square on the terrain that is within the field of view cone
	// by the enemy square, mark it with occupancyValue.

	// If the tile is close enough to the enemy (less than closeDistance),
	// you only check if it's visible to the enemy.
	// Otherwise you must also consider the direction the enemy is facing:

	// Get enemy's position and direction
	//   D3DXVECTOR3 pos = m_owner->GetBody().GetPos();
	//   D3DXVECTOR3 dir = m_owner->GetBody().GetDir();

	// Give the enemyDir a field of view a tad greater than the angle:
	//   D3DXVECTOR2 enemyDir = D3DXVECTOR2(dir.x, dir.z);

	// Two grid squares are visible to each other if a line between 
	// their centerpoints doesn't intersect the four boundary lines
	// of every walled grid square. Put this code in IsClearPath().

	if (!g_blackboard.GetTerrainAnalysisFlag() ||
		(g_blackboard.GetTerrainAnalysisType() != TerrainAnalysis_HideAndSeek))
		return;

	//for (int r = 0; r < g_terrain.GetWidth(); r++)
	//	for (int c = 0; c < g_terrain.GetWidth(); c++)
	//		if(g_terrain.GetInfluenceMapValue(r, c) < 0.0f)
	//			g_terrain.SetInfluenceMapValue(r, c, 0.0f);

	D3DXVECTOR3 enemy_pos = m_owner->GetBody().GetPos();
	int enemy[2];
	g_terrain.GetRowColumn(&enemy_pos, enemy, enemy + 1);
	D3DXVECTOR3 enemy_dir = m_owner->GetBody().GetDir();

	for (int r = 0; r < g_terrain.GetWidth(); r++)
		for (int c = 0; c < g_terrain.GetWidth(); c++)
			if (g_terrain.IsClearPath(enemy[0], enemy[1], r, c))
			{
				// Compute difference Tile/Enemy
				vec3 delta = g_terrain.GetCoordinates(r, c) - enemy_pos;
				
				// Compute Distance
				float dist = D3DXVec3Length(&delta);

				// Compute Angle
				D3DXVec3Normalize(&delta, &delta);
				float dot_p = D3DXVec3Dot(&delta, &enemy_dir);

				// Set Value
				if (dot_p > cosf(angle) || dist < closeDistance)
					g_terrain.SetInfluenceMapValue(r, c, occupancyValue);
			}

}

bool Enemy::FindPlayer(void)
{
	// TODO: Implement this for the Occupancy Map project.

	// Check if the player's tile has negative value (FOV cone)
	// Return true if player is within field cone of view of enemy
	// Return false otherwise
	
	// You need to also set new goal to player's position:
	//   ChangeGoal(row_player, col_player);

	int player[]{ g_blackboard.GetRowPlayer(), g_blackboard.GetColPlayer() };
	if (g_terrain.GetInfluenceMapValue(player[0], player[1]) < 0.0f)
	{
		ChangeGoal(player[0], player[1]);
		return true;
	}
	return false;
}

bool Enemy::SeekPlayer(void)
{
	// TODO: Implement this for the Occupancy Map project.

	// Find the tile with 1.0 occupancy value, and set it as 
	// the new goal:
	//   ChangeGoal(row, col);
	// If multiple tiles with 1.0 occupancy, pick the closest
	// tile to the enemy

	// Return false if no tile is found
	std::vector<std::pair<int, int>> tiles;

	for (int r = 0; r < g_terrain.GetWidth(); r++)
		for (int c = 0; c < g_terrain.GetWidth(); c++)
			if (g_terrain.GetInfluenceMapValue(r, c) > 1.0f - FLT_EPSILON)
				tiles.push_back({ r,c });


	D3DXVECTOR3 enemy_pos = m_owner->GetBody().GetPos();
	int enemy[2];
	g_terrain.GetRowColumn(&enemy_pos, enemy, enemy + 1);
	if (tiles.size() > 0)
	{
		// Compute difference Tile/Enemy
		vec3 delta = g_terrain.GetCoordinates(tiles[0].first, tiles[0].second) - enemy_pos;

		// Compute First Distance
		float closer_dist = D3DXVec3Length(&delta);

		// Set First Pair
		std::pair<int, int> closer = tiles[0];

		// Find any closer pair
		for (int i = 1; i < tiles.size(); i++)
		{
			vec3 delta = g_terrain.GetCoordinates(tiles[i].first, tiles[i].second) - enemy_pos;
			float dist = D3DXVec3Length(&delta);
			if (dist < closer_dist)
				closer_dist = dist, closer = tiles[i];
		}

		// Set as Target
		ChangeGoal(closer.first, closer.second);
	}
	else
		return false;

}
