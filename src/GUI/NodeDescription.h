#pragma once

#include <string>
#include <vector>

struct NodeDescriptor
{
	NodeDescriptor(void);
	bool loadFromFile(const std::string & filename);

	enum NodeType
	{
		UnknownNodeType,
		FinalNode,
		Renderer,
		FullscreenEffect
	};

	enum DataType
	{
		UnknownDataType,
		Texture,
		Constant
	};

	struct Input
	{
		std::string name;
		DataType type;
	};

	struct Output
	{
		std::string name;
		DataType type;
	};

	bool operator == (const char * strId) const
	{
		return(identifier == strId);
	}

	std::string identifier;

	std::string name;
	NodeType type;

	std::vector<Input> inputs;
	std::vector<Output> outputs;
};