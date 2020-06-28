#pragma once

struct fileStackEntry_t
{
	FILE* file;
	char filename[_MAX_PATH];
};

int splitPath(const char* fullpath, int& filename);
void getFileName(const char* fullpath, std::string& result);
void getDirectory(const char* fullpath, std::string& result);
bool ensureDir(const char* path);
bool readFile(const char* filename, std::string& result);
bool getLine(char* line, std::vector<fileStackEntry_t>& stack);