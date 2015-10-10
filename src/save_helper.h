#pragma once

#include <memory>
#include <string>
#include <vector>


class SaveData;


// When user interacts with save files, the extension is excluded.
class SaveHelper {
public:
	static bool userSaveExists(const std::string&);
	static std::vector<std::string> getUserSaveNames(void);
	static std::string userNameToFileName(const std::string&);
	static void save(const std::string&, const SaveData&);
	static std::shared_ptr<SaveData> getData(const std::string&);
private:
	static std::string getTempFileName(const std::string&);
	static std::string getPath(const std::string&);
	static void doSave(const std::string&, const SaveData&);
};
