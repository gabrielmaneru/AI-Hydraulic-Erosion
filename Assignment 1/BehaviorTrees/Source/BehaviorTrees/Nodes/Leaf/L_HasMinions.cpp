#include "Stdafx.h"
using namespace BT;
void L_HasMinions::OnInitial(NodeData * nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);
}

Status L_HasMinions::OnEnter(NodeData * nodedata_ptr)
{
	LeafNode::OnEnter(nodedata_ptr);
	TinyBlackBoard *tinybb = nodedata_ptr->GetAgentData().GetLocalBlackBoard<TinyBlackBoard>();

	D3DXVECTOR3 myPos = nodedata_ptr->GetAgentData().GetGameObject()->GetBody().GetPos();
	if (tinybb->minions.size() > 0)
	{
		D3DXVECTOR3 myPos = nodedata_ptr->GetAgentData().GetGameObject()->GetBody().GetPos();
		float nearest{ FLT_MAX };
		unsigned nearest_id{0};
		for (auto id : tinybb->minions)
		{
			D3DXVECTOR3 otherPos = g_database.Find(id)->GetBody().GetPos();
			D3DXVECTOR3 diff = otherPos - myPos;
			float distance = D3DXVec3Length(&diff);
			if (distance < nearest)
			{
				nearest = distance;
				nearest_id = id;
			}
		}
		tinybb->closer_minion = nearest_id;
		return Status::BT_SUCCESS;
	}

	return Status::BT_FAILURE;
}

void L_HasMinions::OnExit(NodeData * nodedata_ptr)
{
	LeafNode::OnExit(nodedata_ptr);
}

Status L_HasMinions::OnUpdate(float dt, NodeData * nodedata_ptr)
{
	return LeafNode::OnUpdate(dt, nodedata_ptr);
}

Status L_HasMinions::OnSuspend(NodeData * nodedata_ptr)
{
	return LeafNode::OnSuspend(nodedata_ptr);
}
