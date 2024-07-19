#pragma once
#include <filesystem>
#include <string>
#include <vector>

bool exec(const std::string& cmd, std::string& result);

std::wstring transformString2Wstring(const std::string& s);
std::filesystem::path getPath(const std::string& p);

std::string getMd5(const std::string& str);
std::string bigFileMd5(const std::string& filePath);
std::string fileMd5(const std::string& filePath);

bool regexMatch(const std::string& s, const std::wstring& pattern);

bool fileExisted(const std::string& file);
int makeDir(const std::string& dir);
std::string pathAppend(const std::string& p1, const std::string& p2);
std::string absolutePath(const std::string& path);

std::string genUUID();

std::string getEnv(const std::string& envName);
bool setEnv(const std::string& envName, const std::string& value);

void transformToLowerOrUpper(std::string& s, bool toLower);
std::vector<std::string> splitstr(const std::string& str, char tag);
void ltrim(std::string& s);
std::string trim(const std::string& s);

void mSleep(int milliseconds);

std::string gb2312ToUTF8(std::string strGb2312);
