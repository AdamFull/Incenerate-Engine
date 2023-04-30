#pragma once

#include "interface/VFSInterface.h"

struct zip;

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class CZipFileSystem
		 * @brief This class provides an interface for virtual file systems within a ZIP archive.
		 *
		 * CZipFileSystem is a derived class from IVFSInterface and implements its
		 * interface for working with files and directories within a ZIP archive.
		 */
		class CZipFileSystem final : public IVFSInterface
		{
		public:
			/**
			 * @brief Construct a CZipFileSystem with a file system path and a password.
			 *
			 * @param fspath A string representing the file system path of the ZIP archive.
			 * @param password A string representing the password for encrypted files.
			 */
			CZipFileSystem(const std::string& fspath, const std::string& password);

			/**
			 * @brief Destructor.
			 */
			virtual ~CZipFileSystem() override;

			/**
			 * @brief Create a new ZIP archive at the specified file system path.
			 *
			 * @param fspath A string representing the file system path of the new ZIP archive.
			 * @return True if the archive is successfully created, false otherwise.
			 */
			bool create(const std::string& fspath) override;

			/**
			 * @brief This function is not supported by the CZipFileSystem.
			 */
			void copy(const std::string& from, const std::string& to) override {}

			/**
			 * @brief This function is not supported by the CZipFileSystem.
			 */
			bool createDirectories(const std::string& path) override { return false; }

			/**
			 * @brief This function is not supported by the CZipFileSystem.
			 */
			bool createDirectory(const std::string& path) override { return false; }

			/**
		 * @brief Check if an entry exists at the specified path within the ZIP archive.
		 *
		 * @param path A string representing the path of the entry.
		 * @return True if the entry exists, false otherwise.
		 */
			bool exists(const std::string& path) override;

			/**
			 * @brief Get the size of a file at the specified path within the ZIP archive.
			 *
			 * @param path A string representing the path of the file.
			 * @return The size of the file in bytes.
			 */
			size_t fileSize(const std::string& path) override;

			/**
			 * @brief Check if a directory exists at the specified path within the ZIP archive.
			 *
			 * @param path A string representing the path of the directory.
			 * @return True if the directory exists, false otherwise.
			 */
			bool isDirectory(const std::string& path) override;

			/**
			 * @brief Check if a file exists at the specified path within the ZIP archive.
			 *
			 * @param path A string representing the path of the file.
			 * @return True if the file exists, false otherwise.
			 */
			bool isFile(const std::string& path) override;

			/**
			 * @brief This function is not supported by the CZipFileSystem.
			 */
			bool remove(const std::string& path) override { return false; }

			/**
			 * @brief This function is not supported by the CZipFileSystem.
			 */
			void rename(const std::string& original, const std::string& newfile) override { }

			/**
			 * @brief This function is not supported by the CZipFileSystem.
			 */
			std::unique_ptr<IOutputFileInterface> createFile(const std::string& path) override { return nullptr; }

			/**
			 * @brief Open a file at the specified path within the ZIP archive.
			 *
			 * @param path A string representing the path of the file.
			 * @return A unique_ptr to an IInputFileInterface if the file is successfully opened, nullptr otherwise.
			 */
			std::unique_ptr<IInputFileInterface> openFile(const std::string& path) override;

			/**
			 * @brief Create a directory iterator for the specified path within the ZIP archive.
			 *
			 * This method creates an IDirectoryIterator to traverse the contents of a directory
			 * within the ZIP archive. If recursive is set to true, the iterator will also
			 * traverse subdirectories.
			 *
			 * @param path A string representing the path of the directory.
			 * @param recursive A boolean flag indicating whether to traverse subdirectories.
			 * @return A unique_ptr to an IDirectoryIterator for the specified path.
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
			/**
			 * @brief A pointer to the zip structure.
			 */
			zip* m_pArchive{ nullptr };

			/**
			 * @brief The password for encrypted files.
			 */
			std::string m_srPassword{};
		};
	}
}