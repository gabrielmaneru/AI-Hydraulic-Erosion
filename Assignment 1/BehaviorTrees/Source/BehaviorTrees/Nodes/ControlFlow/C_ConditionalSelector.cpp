#include <Stdafx.h>

using namespace BT;

void C_ConditionalSelector::OnInitial(NodeData * nodedata_ptr)
{
	CompositeNode::OnInitial(nodedata_ptr);
}

Status C_ConditionalSelector::OnEnter(NodeData * nodedata_ptr)
{
	return CompositeNode::OnEnter(nodedata_ptr);
}

void C_ConditionalSelector::OnExit(NodeData * nodedata_ptr)
{
	CompositeNode::OnExit(nodedata_ptr);
}

Status C_ConditionalSelector::OnUpdate(float dt, NodeData * nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);
	Status condition = nodedata_ptr->GetChildStatus(0);
	switch (condition)
	{
	case BT::BT_READY:
		RunChild(0, true, false, nodedata_ptr);
		return Status::BT_RUNNING;
		break;

	case BT::BT_SUCCESS:
		if (nodedata_ptr->GetChildStatus(1) == Status::BT_READY)
		{
			RunChild(1, true, false, nodedata_ptr);
			return Status::BT_RUNNING;
		}
		else
			return condition;

		break;

	case BT::BT_FAILURE:
		if (nodedata_ptr->GetChildStatus(2) == Status::BT_READY)
		{
			RunChild(2, true, false, nodedata_ptr);
			return Status::BT_RUNNING;
		}
		else
			return condition;
		break;

	default:
		return Status::BT_FAILURE;
	}
}

Status C_ConditionalSelector::OnSuspend(NodeData * nodedata_ptr)
{
	return CompositeNode::OnSuspend(nodedata_ptr);
}
