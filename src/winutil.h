//
// Created by MarufN on 26/12/2021.
//

#ifndef REGIONOFINTEREST_WINUTIL_H
#define REGIONOFINTEREST_WINUTIL_H
#include <bits/stdc++.h>
#include <dirent.h>
#include <fileapi.h>
#include <sys/stat.h>
using namespace std;

class WinUtil {
public:
	static vector<string> DirContents(const string& pathString) {
		DIR* dir;
		struct dirent *diread;
		vector<string> files;
		if ((dir = opendir(pathString.c_str())) != nullptr) {
			while ((diread = readdir(dir)) != nullptr) {
				files.emplace_back(diread -> d_name);
			}
			closedir(dir);
		} else {
			perror("opendir");
			EXIT_FAILURE;
		}
		return files;
	}

	static bool IsExist(const string& pathString) {
		struct stat buffer{};
		return (stat(pathString.c_str(), &buffer) == 0);
	}

	static bool IsDir(const string& pathString) {
		DWORD attrib = GetFileAttributes(reinterpret_cast<LPCSTR>(pathString.c_str()));
		return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	static bool IsFile(const string& pathString) {
		DWORD attrib = GetFileAttributes(reinterpret_cast<LPCSTR>(pathString.c_str()));
		return (attrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	static string MakeDir(const string& pathString) {
		if (mkdir(pathString.c_str()) == -1) {}
		return pathString;
	}

	static bool Remove(const string& pathString) {
		if (IsExist(pathString)) {
			filesystem::remove_all(pathString);
			cerr << pathString << " dir/files Deleted";
		} else {
			cerr << "Remove Error: dir/files not exists";
		}
		return false;
	}

};

#endif //REGIONOFINTEREST_WINUTIL_H
