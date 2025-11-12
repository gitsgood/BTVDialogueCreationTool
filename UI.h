#pragma once

#ifndef UI_H
#define UI_H

#include "Core.h"
#include "DialogueContent.h"

class UserInterface
{
public:
	std::vector<std::unique_ptr<DialogueEdge>> DialogueEdgeContainer;
	std::vector<std::unique_ptr<DialogueNode>> DialogueNodeContainer;

	UserInterface();
};

#endif // !UI_H