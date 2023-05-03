#pragma once

#include "../ie_vfs_exports.h"

#include "DirectoryIteratorInterface.h"
#include "InputFileInterface.h"
#include "OutputFileInterface.h"

namespace engine
{
	namespace filesystem
	{
		/**
		 * @enum EFilesystemModeFlagBits
		 * @brief Enumerator for file system mode flag bits.
		 */
		enum class EFilesystemModeFlagBits
		{
			eNone = 1 << 0, ///< No file system mode flag.
			eRead = 1 << 1, ///< Read mode flag.
			eWrite = 1 << 2, ///< Write mode flag.
		};

		/**
		 * @typedef EFilesystemModeFlags
		 * @brief An alias for uint32_t representing a combination of file system mode flag bits.
		 */
		using EFilesystemModeFlags = uint32_t;

		/**
		 * @class IVFSInterface
		 * @brief Interface for virtual file system implementations.
		 *
		 * IVFSInterface provides a common interface for file system operations, such as
		 * create, copy, check existence, etc. It must be implemented by any file system
		 * to be supported by the CFilesystemWrapper.
		 */
		class INCENERATE_VFS_EXPORT IVFSInterface
		{
		public:
			/**
			 * @brief Virtual destructor.
			 */
			virtual ~IVFSInterface() = default;

			/**
			 * @brief Initializes the file system.
			 * @param fspath The file system path.
			 * @return true if the file system is initialized successfully, false otherwise.
			 */
			virtual bool create(const std::string& fspath) = 0;

			/**
			 * @brief Copies a file or directory from the source to the destination path.
			 * @param src The source path.
			 * @param dst The destination path.
			 */
			virtual void copy(const std::string& src, const std::string& dst) = 0;

			/**
			 * @brief Creates directories recursively at the specified path, if they do not exist.
			 * @param path The directory path to be created.
			 * @return true if the directories were created or already exist, false otherwise.
			 */
			virtual bool createDirectories(const std::string& path) = 0;

			/**
			 * @brief Creates a single directory at the specified path, if it does not exist.
			 * @param path The directory path to be created.
			 * @return true if the directory was created or already exists, false otherwise.
			 */
			virtual bool createDirectory(const std::string& path) = 0;

			/**
			 * @brief Checks whether the specified path exists.
			 * @param path The path to be checked.
			 * @return true if the path exists, false otherwise.
			 */
			virtual bool exists(const std::string& path) = 0;

			/**
			 * @brief Retrieves the size of a file at the specified path.
			 * @param path The file path.
			 * @return The size of the file in bytes.
			 */
			virtual size_t fileSize(const std::string& path) = 0;

			/**
			 * @brief Checks whether the specified path is a directory.
			 * @param path The path to be checked.
			 * @return true if the path is a directory, false otherwise.
			 */
			virtual bool isDirectory(const std::string& path) = 0;

			/**
			 * @brief Checks whether the specified path is a file.
			 * @param path The path to be checked.
			 * @return true if the path is a file, false otherwise.
			 */
			virtual bool isFile(const std::string& path) = 0;

			/**
			 * @brief Removes a file or an empty directory at the specified path.
			 * @param path The path to be removed.
			 * @return true if the path was successfully removed, false otherwise.
			 */
			virtual bool remove(const std::string& path) = 0;

			/**
			 * @brief Renames a file or a directory at the specified original path to a new path.
			 * @param original The original file or directory path.
			 * @param newfile The new file or directory path.
			 */
			virtual void rename(const std::string& original, const std::string& newfile) = 0;

			/**
			 * @brief Creates a new file with write mode (wb) at the specified path.
			 * @param path The file path.
			 * @return A unique_ptr to the IOutputFileInterface object representing the created file.
			 */
			virtual std::unique_ptr<IOutputFileInterface> createFile(const std::string& path) = 0;

			/**
			 * @brief Opens an existing file with read mode (rb) at the specified path.
			 * @param path The file path.
			 * @return A unique_ptr to the IInputFileInterface object representing the opened file.
			 */
			virtual std::unique_ptr<IInputFileInterface> openFile(const std::string& path) = 0;

			/**
			 * @brief Creates a recursive directory iterator for the specified path.
			 * @param path The path to start the iteration from.
			 * @param recursive true if the iteration should be recursive, false otherwise.
			 * @return A unique_ptr to the IDirectory
			 */
			virtual std::unique_ptr<IDirectoryIterator> walk(const std::string& path, bool recursive) = 0;

			/**
			 * @brief Gets the file system mode flags.
			 * @return A combination of EFilesystemModeFlagBits as EFilesystemModeFlags.
			 */
			virtual EFilesystemModeFlags getFilesystemMode() const = 0;
		};
	}
}