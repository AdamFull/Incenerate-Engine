#pragma once

#include "../ie_vfs_exports.h"

#include "VFSInterface.h"
#include "VFSInputFileInterface.h"
#include "VFSOutputFileInterface.h"
#include "DirectoryIteratorInterface.h"
#include <json.hpp>

namespace engine
{
    namespace filesystem
    {
        class INCENERATE_VFS_EXPORT IVirtualFileSystemInterface
        {
        public:
            virtual ~IVirtualFileSystemInterface() = default;

            virtual void mount(const std::string& mountpoint, std::unique_ptr<IVFSInterface>&& vfsptr) = 0;
            virtual void unmount(const std::string& mountpoint) = 0;

            virtual void copy(const std::string& from, const std::string& to) = 0;

            virtual bool createDirectories(const std::string& path) = 0;
            virtual bool createDirectory(const std::string& path) = 0;

            virtual bool exists(const std::string& path) = 0;

            virtual size_t fileSize(const std::string& path) = 0;

            virtual bool isDirectory(const std::string& path) = 0;
            virtual bool isFile(const std::string& path) = 0;

            virtual bool remove(const std::string& path) = 0;
            virtual void rename(const std::string& original, const std::string& newfile) = 0;

            virtual std::unique_ptr<IVFSOutputFileInterface> createFile(const std::string& path) = 0;
            virtual std::unique_ptr<IVFSInputFileInterface> openFile(const std::string& path) = 0;

            virtual std::unique_ptr<IDirectoryIteratorInterface> walk(const std::string& path, bool recursive) = 0;

            virtual bool readFile(const std::string& path, std::string& data) = 0;
            virtual bool readFile(const std::string& path, std::vector<uint8_t>& data) = 0;

            virtual bool writeFile(const std::string& path, const std::string& data) = 0;
            virtual bool writeFile(const std::string& path, const std::vector<uint8_t>& binary) = 0;

            template<class _Ty>
            bool readJson(const std::string& path, _Ty& type)
            {
                std::string data{};
                if (readFile(path, data))
                {
                    if (!data.empty())
                    {
                        auto json = nlohmann::json::parse(data);
                        if (!json.empty())
                        {
                            json.get_to(type);
                            return true;
                        }
                    }
                }

                return false;
            }

            template<class _Ty>
            bool writeJson(const std::string& path, const _Ty& type, int32_t indent = -1)
            {
                auto json = nlohmann::json(type);
                if (!json.empty())
                {
                    auto data = json.dump(indent);
                    if (!data.empty())
                        return writeFile(path, data);
                }

                return false;
            }

            template<class _Ty>
            bool readBson(const std::string& path, _Ty& type)
            {
                std::string data{};
                if (readFile(path, data))
                {
                    auto json = nlohmann::json::from_bson(data);
                    if (!json.empty())
                    {
                        json.get_to(type);
                        return true;
                    }
                }

                return false;
            }

            template<class _Ty>
            bool writeBson(const std::string& path, const _Ty& type)
            {
                auto json = nlohmann::json(type);
                if (!json.empty())
                {
                    auto data = nlohmann::json::to_bson(json);
                    if (!data.empty())
                        return writeFile(path, data);
                }

                return false;
            }
        };
    }
}