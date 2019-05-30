#include "Stdafx.h"
using namespace BT;
void L_RunAwayFromTarget::OnInitial(NodeData * nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);
}

Status L_RunAwayFromTarget::OnEnter(NodeData * nodedata_ptr)
{
	LeafNode::OnEnter(nodedata_ptr);

	GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();
	D3DXVECTOR3 myPos = self->GetBody().GetPos();
	TinyBlackBoard *tinybb = nodedata_ptr->GetAgentData().GetLocalBlackBoard<TinyBlackBoard>();
	D3DXVECTOR3 dir = -(tinybb->detected_position - myPos);
	D3DXVec3Normalize(&dir, &dir);
	self->SetTargetPOS(myPos + dir * (0.1f + tinybb->detection_distance - tinybb->detected_distance));
	self->SetSpeedStatus(TinySpeedStatus::TS_JOG);
	SetTinySpeed(self);
	return Status::BT_RUNNING;
}

void L_RunAwayFromTarget::OnExit(NodeData * nodedata_ptr)
{
	LeafNode::OnExit(nodedata_ptr);
}

Status L_RunAwayFromTarget::OnUpdate(float dt, NodeData * nodedata_ptr)
{
	LeafNode::OnUpdate(dt, nodedata_ptr);

	GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();

	if (IsNear(self->GetBody().GetPos(), self->GetTargetPOS()))
		return Status::BT_SUCCESS;

	return Status::BT_RUNNING;
}

Status L_RunAwayFromTarget::OnSuspend(NodeData * nodedata_ptr)
{
	return LeafNode::OnSuspend(nodedata_ptr);
}
