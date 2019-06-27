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
