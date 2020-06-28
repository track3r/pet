#include "pch.h"

#include "FsUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

int splitPath(const char* fullpath, int& filename)
{
	size_t l = strlen(fullpath);
	if (l == 0)
	{
		return -1;
	}

	do
	{
		char c = fullpath[l - 1];
		if (c == '\\' || c == '/')
		{
			return (int)l;
		}
	} while (--l > 0);

	return (int)l;
}

void getFileName(const char* fullpath, std::string& result)
{
	size_t l = strlen(fullpath);
	if (l == 0)
	{
		result = "";
		return;
	}
	do
	{
		char c = fullpath[l - 1];
		if (c == '\\' || c == '/')
		{
			result.assign(fullpath + l);
			return;
		}
	} while (--l > 0);

	result = fullpath;
}

void getDirectory(const char* fullpath, std::string& result)
{
	size_t l = strlen(fullpath);
	if (l == 0)
	{
		result = "";
		return;
	}

	do
	{
		char c = fullpath[l - 1];
		if (c == '\\' || c == '/')
		{
			result.assign(fullpath, l);
			return;
		}
	} while (--l > 0);

	result = fullpath;
}

bool ensureDir(const char* path)
{
	char temp[512] = { 0 };
	size_t l = strlen(path);
	if (l == 0)
	{
		return true;
	}

	int i = 0;
	int begin = 0;
	bool foundFile = false;
	do
	{

		if (path[i] == '/' || path[i] == '\\')
		{
			if (foundFile)
			{
				LOG("%s: dir in the middle", path);
				return false;
			}
			struct stat info;

			if (stat(temp, &info) != 0)
			{
				if (_mkdir(temp) != 0)
				{
					LOG("failed to make dir %s", temp);
					return false;
				}
			}
			else if (info.st_mode & S_IFDIR)
			{

			}
			else
			{
				foundFile = true;
			}
		}

		temp[i] = path[i];
	} while (++i < l);

	return true;
}

bool readFile(const char* filename, std::string& result)
{
	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		return false;
	}

	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	result.resize(len);
	fseek(f, 0, SEEK_SET);
	fread(&result.front(), 1, len, f);

	return true;
}

bool getLine(char* line, std::vector<fileStackEntry_t>& stack)
{
	if (stack.empty())
	{
		return false;
	}

	while (fgets(line, 512, stack.back().file) == NULL)
	{
		fclose(stack.back().file);
		fileStackEntry_t last = stack.back();
		stack.pop_back();

		if (stack.empty())
		{
			return false;
		}
		else
		{
			sprintf(line, "//end %s\n", last.filename);
			return true;
		}
	}

	return true;
}