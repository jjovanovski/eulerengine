#include "Utils.h"

using namespace Euler;

#include <iostream>
#include <fstream>

std::vector<char> Euler::ReadFile(const char* filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return std::vector<char>();
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
