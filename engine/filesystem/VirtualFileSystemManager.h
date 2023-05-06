#pragma once

#include <ie_vfs_exports.h>

#include <interface/filesystem/VirtualFileSystemInterface.h>

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class CFilesystemWrapper
		 * @brief A final wrapper class that provides a unified interface for file system operations.
		 *
		 * CFilesystemWrapper allows mounting different file systems with IVFSInterface implementations,
		 * and transparently performing operations on them, such as copy, create, delete, etc.
		 */
		class INCENERATE_VFS_EXPORT CVirtualFileSystemManager final : public IVirtualFileSystemInterface
		{
		public:
			/**
			 * @brief Destructor.
			 */
			~CVirtualFileSystemManager() override;

			/**
			 * @brief Mounts a filesystem at a specified mount point.
			 * @param mountpoint The mount point as a string.
			 * @param vfsptr A unique_ptr to the filesystem implementation (IVFSInterface).
			 */
			void mount(const std::string& mountpoint, std::unique_ptr<IVFSInterface>&& vfsptr);

			/**
			 * @brief Unmounts a filesystem from a specified mount point.
			 * @param mountpoint The mount point as a string.
			 */
			void unmount(const std::string& mountpoint);

			/**
			 * @brief Copies a file from a source to a destination path or filesystem.
			 * @param from The source file path.
			 * @param to The destination file path.
			 */
			void copy(const std::string& from, const std::string& to);

			/**
			 * @brief Creates directories recursively, if they do not exist.
			 * @param path The directory path to be created.
			 * @return true if the directories were created or already exist, false otherwise.
			 */
			bool createDirectories(const std::string& path);

			/**
			 * @brief Creates a single directory if it does not exist.
			 * @param path The directory path to be created.
			 * @return true if the directory was created or already exists, false otherwise.
			 */
			bool createDirectory(const std::string& path);

			/**
			 * @brief Checks whether the specified path exists.
			 * @param path The path to be checked.
			 * @return true if the path exists, false otherwise.
			 */
			bool exists(const std::string& path);

			/**
			 * @brief Retrieves the size of a file at the specified path.
			 * @param path The file path.
			 * @return The size of the file in bytes.
			 */
			size_t fileSize(const std::string& path);

			/**
			 * @brief Checks whether the specified path is a directory.
			 * @param path The path to be checked.
			 * @return true if the path is a directory, false otherwise.
			 */
			bool isDirectory(const std::string& path);

			/**
			 * @brief Checks whether the specified path is a file.
			 * @param path The path to be checked.
			 * @return true if the path is a file, false otherwise.
			 */
			bool isFile(const std::string& path);

			/**
			 * @brief Removes a file or an empty directory at the specified path.
			 * @param path The path to be removed.
			 * @return true if the path was successfully removed, false otherwise.
			 */
			bool remove(const std::string& path);

			/**
			 * @brief Renames a file or a directory at the specified original path to a new path.
			 * @param original The original file or directory path.
			 * @param newfile The new file or directory path.
			 */
			void rename(const std::string& original, const std::string& newfile);

			/**
			 * @brief Creates a new file at the specified path.
			 * @param path The file path.
			 * @return A unique_ptr to the created CVFSOutFile object.
			 */
			std::unique_ptr<IVFSOutputFileInterface> createFile(const std::string& path);

			/**
			 * @brief Opens an existing file at the specified path.
			 * @param path The file path.
			 * @return A unique_ptr to the opened CVFSInFile object.
			 */
			std::unique_ptr<IVFSInputFileInterface> openFile(const std::string& path);

			/**
			 * @brief Walks through the directories and files at the specified path.
			 * @param path The path to start walking from.
			 * @param recursive true if the walk should be recursive, false otherwise (default: true).
			 * @return A unique_ptr to the CDirectoryIterator object.
			 */
			std::unique_ptr<IDirectoryIteratorInterface> walk(const std::string& path, bool recursive = true);

			bool readFile(const std::string& path, std::string& data) override;
			bool readFile(const std::string& path, std::vector<uint8_t>& data) override;

			bool writeFile(const std::string& path, const std::string& data) override;
			bool writeFile(const std::string& path, const std::vector<uint8_t>& binary) override;
		private:
			/**
			 * @brief Retrieves the filesystem implementation for the given path.
			 * @param path The file path.
			 * @return A const reference to the unique_ptr of the filesystem implementation (IVFSInterface).
			 */
			const std::unique_ptr<IVFSInterface>& getFilesystem(const std::string& path);

			/**
			 * @brief Processes the file copy operation between two filesystems.
			 * @param pFSFrom A unique_ptr to the source filesystem implementation (IVFSInterface).
			 * @param pFSTo A unique_ptr to the destination filesystem implementation (IVFSInterface).
			 * @param path The file path as a string.
			 */
			static void processCopyFile(const std::unique_ptr<IVFSInterface>& pFSFrom, const std::unique_ptr<IVFSInterface>& pFSTo, const std::string& path);
		private:
			std::unordered_map<std::string, std::unique_ptr<IVFSInterface>> m_vMountedFS{};
			std::mutex m_SynchronizationMutex;
		};
	}
}