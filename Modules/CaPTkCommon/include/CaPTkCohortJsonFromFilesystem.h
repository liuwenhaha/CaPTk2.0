#ifndef CaPTkCohortJsonFromFilesystem_h
#define CaPTkCohortJsonFromFilesystem_h

// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.
#include <MitkCaPTkCommonExports.h>

#include <json/json.h>

#include <string>
#include <vector>

namespace captk
{
MITKCAPTKCOMMON_EXPORT Json::Value CohortMergeJsonObjects(std::vector<Json::Value> jsons);

MITKCAPTKCOMMON_EXPORT Json::Value CohortJsonFromFilesystem(std::string              directory);
MITKCAPTKCOMMON_EXPORT Json::Value CohortJsonFromFilesystem(std::vector<std::string> directoriesVector);

MITKCAPTKCOMMON_EXPORT std::vector<std::string> GetSubdirectories(std::string directory);
MITKCAPTKCOMMON_EXPORT std::vector<std::string> GetContainedFiles(std::string directory);

MITKCAPTKCOMMON_EXPORT std::string GetFileNameFromPath(std::string path);
MITKCAPTKCOMMON_EXPORT bool IsDir(std::string path);
}

#endif // ! #ifndef CaPTkCohortJsonFromFilesystem_h