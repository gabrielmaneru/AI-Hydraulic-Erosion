#include <Stdafx.h>
using namespace BT;

void L_DetectEnemy::OnInitial(NodeData * nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);
}

Status L_DetectEnemy::OnEnter(NodeData * nodedata_ptr)
{
	LeafNode::OnEnter(nodedata_ptr);
	GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();
	GameObject * nearest = GetNearestAgent(self);
	if (nearest)
	{
		D3DXVECTOR3 myPos = self->GetBody().GetPos();
		D3DXVECTOR3 nearestPos = nearest->GetBody().GetPos();
		D3DXVECTOR3 diff = myPos - nearestPos;
		float distance = D3DXVec3Length(&diff);

		TinyBlackBoard *tinybb = nodedata_ptr->GetAgentData().GetLocalBlackBoard<TinyBlackBoard>();
		if (distance < tinybb->detection_distance)
		{
			tinybb->detected_position = nearestPos;
			tinybb->detected_distance = distance;
			return Status::BT_SUCCESS;
		}
	}
	return Status::BT_FAILURE;
}

void L_DetectEnemy::OnExit(NodeData * nodedata_ptr)
{
	LeafNode::OnExit(nodedata_ptr);
}

Status L_DetectEnemy::OnUpdate(float dt, NodeData * nodedata_ptr)
{
	return LeafNode::OnUpdate(dt, nodedata_ptr);
}

Status L_DetectEnemy::OnSuspend(NodeData * nodedata_ptr)
{
	return LeafNode::OnSuspend(nodedata_ptr);
}
