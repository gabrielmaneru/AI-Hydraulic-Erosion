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

bool Movement::ComputePath( int r, int c, bool newRequest )
{
	m_goal = g_terrain.GetCoordinates( r, c );
	m_movementMode = MOVEMENT_WAYPOINT_LIST;

	// project 2: change this flag to true
	bool useAStar = true;

	if( useAStar )
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////
		//INSERT YOUR A* CODE HERE
		//1. You should probably make your own A* class.
		//2. You will need to make this function remember the current progress if it preemptively exits.
		//3. Return "true" if the path is complete, otherwise "false".
		///////////////////////////////////////////////////////////////////////////////////////////////////
		if (newRequest)
		{
			int flags{0};
			if (m_smooth)		flags |= a_star::e_smooth;
			if (m_rubberband)	flags |= a_star::e_rubberband;
			if (m_straightline)	flags |= a_star::e_straightline;
			if (m_singleStep)	flags |= a_star::e_singlestep;
			if (m_debugDraw)	flags |= a_star::e_debug_draw;

			if (m_pathfinder.initialize(m_owner->GetBody().GetPos(), m_goal, flags,m_heuristicWeight, m_heuristicCalc ))
			{
				// If start == goal or straight path found
				m_waypointList = m_pathfinder.m_waypoints;
				return true;
			}
			m_waypointList.clear();
			return false;
		}
		else
		{
			if (m_pathfinder.iterate())
			{
				m_waypointList = m_pathfinder.m_waypoints;
				return true;
			}
			return false;
		}
	}
	else
	{	
		//Randomly meander toward goal (might get stuck at wall)
		int curR, curC;
		D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
		g_terrain.GetRowColumn( &cur, &curR, &curC );

		m_waypointList.clear();
		m_waypointList.push_back( cur );

		int countdown = 100;
		while( curR != r || curC != c )
		{
			if( countdown-- < 0 ) { break; }

			if( curC == c || (curR != r && rand()%2 == 0) )
			{	//Go in row direction
				int last = curR;
				if( curR < r ) { curR++; }
				else { curR--; }

				if( g_terrain.IsWall( curR, curC ) )
				{
					curR = last;
					continue;
				}
			}
			else
			{	//Go in column direction
				int last = curC;
				if( curC < c ) { curC++; }
				else { curC--; }

				if( g_terrain.IsWall( curR, curC ) )
				{
					curC = last;
					continue;
				}
			}

			D3DXVECTOR3 spot = g_terrain.GetCoordinates( curR, curC );
			m_waypointList.push_back( spot );
			g_terrain.SetColor( curR, curC, DEBUG_COLOR_YELLOW );
		}
		return true;
	}
}
