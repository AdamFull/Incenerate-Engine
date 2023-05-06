#pragma once

#include "ie_vfs_exports.h"

#include <interface/filesystem/VFSInterface.h>

#include <filesystem>

namespace engine
{
    namespace filesystem
    {
        /**
         * @class CNativeFileSystem
         * @brief A class for handling native file system operations.
         *
         * CNativeFileSystem is a derived class from IVFSInterface that provides
         * a platform-specific implementation for native file system operations.
         */
        class INCENERATE_VFS_EXPORT CNativeFileSystem final : public IVFSInterface
        {
        public:
            /**
             * @brief Construct a CNativeFileSystem for the specified file system path.
             *
             * @param fspath A string representing the file system path to be used.
             */
            CNativeFileSystem(const std::string& fspath);

            /**
             * @brief Destructor.
             */
            virtual ~CNativeFileSystem() override = default;

            /**
             * @brief Create a new file or directory at the specified path.
             *
             * @param fspath A string representing the path to create the new file or directory.
             * @return True on success, false on failure.
             */
            bool create(const std::string& fspath) override;

            /**
             * @brief Copy a file from one path to another.
             *
             * @param from A string representing the source file path.
             * @param to A string representing the destination file path.
             */
            void copy(const std::string& from, const std::string& to) override;

            /**
             * @brief Create a directory hierarchy at the specified path.
             *
             * @param path A string representing the directory hierarchy to create.
             * @return True on success, false on failure.
             */
            bool createDirectories(const std::string& path) override;

            /**
             * @brief Create a directory at the specified path.
             *
             * @param path A string representing the directory to create.
             * @return True on success, false on failure.
             */
            bool createDirectory(const std::string& path) override;

            /**
             * @brief Check if a file or directory exists at the specified path.
             *
             * @param path A string representing the path to check for existence.
             * @return True if the path exists, false otherwise.
             */
            bool exists(const std::string& path) override;

            /**
             * @brief Get the size of a file at the specified path.
             *
             * @param path A string representing the path to the file.
             * @return The size of the file in bytes.
             */
            size_t fileSize(const std::string& path) override;

            /**
             * @brief Check if a path points to a directory.
             *
             * @param path A string representing the path to check.
             * @return True if the path points to a directory, false otherwise.
             */
            bool isDirectory(const std::string& path) override;

            /**
             * @brief Check if a path points to a file.
             *
             * @param path A string representing the path to check.
             * @return True if the path points to a file, false otherwise.
             */
            bool isFile(const std::string& path) override;

            /**
             * @brief Remove a file or directory at the specified path.
             *
             * @param path A string representing the path to remove.
             * @return True on success, false on failure.
             */
            bool remove(const std::string& path) override;

            /**
             * @brief Rename a file or directory.
             *
             * @brief Rename a file or directory.
             * @param original A string representing the original path of the file or directory.
             * @param newfile A string representing the new path for the file or directory.
             */
            void rename(const std::string& original, const std::string& newfile) override;

            /**
             * @brief Create a new file at the specified path.
             *
             * @param path A string representing the path to create the new file.
             * @return A unique_ptr to an IOutputFileInterface object on success, nullptr on failure.
             */
            std::unique_ptr<IOutputFileInterface> createFile(const std::string& path) override;

            /**
             * @brief Open a file at the specified path for reading.
             *
             * @param path A string representing the path to the file to open.
             * @return A unique_ptr to an IInputFileInterface object on success, nullptr on failure.
            */
            std::unique_ptr<IInputFileInterface> openFile(const std::string& path) override;

            /**
             * @brief Create a directory iterator for walking the file system.
             *
             * @param path A string representing the starting path for the iterator.
             * @param recursive A boolean flag indicating whether the iterator should recurse into subdirectories.
             * @return A unique_ptr to an IDirectoryIterator object on success, nullptr on failure.
             */
            std::unique_ptr<IDirectoryIterator> walk(const std::string& path, bool recursive) override;

        protected:
            /**
             * @brief Get the file system mode flags for the ZIP file system.
             *
             * @return The file system mode flags.
             */
            EFilesystemModeFlags getFilesystemMode() const override;

        private:
            std::filesystem::path m_fsRoot{};
        };
    }
}