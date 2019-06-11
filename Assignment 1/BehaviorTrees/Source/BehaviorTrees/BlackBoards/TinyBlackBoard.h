/******************************************************************************/
/*!
\file		TinyBlackBoard.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Local blackboard for tiny.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// store tiny info
	struct TinyBlackBoard : public AgentAbstractData
	{
		/* variable */

		GameObject *m_npc;				// agent's game object pointer

		// Mouse Click
		bool m_mouseClick;
		D3DXVECTOR3 m_mousePos;

		// Detection
		float detection_distance{ 0.5f };
		D3DXVECTOR3 detected_position;
		float detected_distance;

		// Manage Minions
		std::vector<unsigned int> minions;
		unsigned int closer_minion;

		/* constructors/destructor */

		// Default constructor
		TinyBlackBoard();

		/* methods */

		// Handle messages
		virtual void OnMessage(void) override;
	};
}