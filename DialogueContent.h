#pragma once

#ifndef DIALOGUECONTENT_H
#define DIALOGUECONTENT_H

#include "Core.h"

struct DialogueParent
{
	/**
	* @brief Makes sure the input string doesn't hold any characters that mess with JSON formatting.
	* 
	* @param[in] inStringToParse The string that needs double checking.
	* 
	* @return A string with any singular " or \ characters preceded with a backslash( \ ).
	*/
	std::string ParserForJson(const std::string& inStringToParse);
};

struct DialogueNode : public DialogueParent
{
	using string = std::string;

	string NodeId;
	string NPCDialogue;
	std::vector<string> OutgoingEdgeIds;

	DialogueNode(const string& newNPCDialogue, const std::vector<string>& newOutgoingEdgeIds, const string& newNodeId = "START") :
		NodeId(newNodeId),
		NPCDialogue(newNPCDialogue),
		OutgoingEdgeIds(newOutgoingEdgeIds)
	{};

	string StringifyNodeForJson();
};

struct DialogueEdge : public DialogueParent
{
	using string = std::string;

	string EdgeId;
	string PlayerDialogue;
	string NextNodeId;

	DialogueEdge(const string& newEdgeId = "END", const string& newPlayerDialogue = "*end*", const string& newNextNodeId = "") :
		EdgeId(newEdgeId),
		PlayerDialogue(newPlayerDialogue),
		NextNodeId(newNextNodeId)
	{};

	string StringifyEdgeForJson();
};

#endif // !DIALOGUECONTENT_H