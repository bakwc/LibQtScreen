#include <string>

int GetProcessID(const std::wstring& exeFileName);
bool InjectDll(int pid, const std::string& dll);
