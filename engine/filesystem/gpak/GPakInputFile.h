#pragma once

#include "ie_vfs_exports.h"

#include "interface/InputFileInterface.h"

#include <string>

struct gpak;
struct gpak_file;

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class CGPakInputFile
		 * @brief A class for handling gpak input file operations.
		 *
		 * CGPakInputFile is a derived class from IInputFileInterface that provides
		 * a implementation for reading from files using the gpak
		 * file handling functions.
		 */
		class INCENERATE_VFS_EXPORT CGPakInputFile final : public IInputFileInterface
		{
		public:
			/**
			 * @brief Construct a CGPakInputFile for the specified file.
			 *
			 * @param handle A pointer to the gpak archive.
			 * @param filename A string representing the path to the file to be opened.
			 */
			CGPakInputFile(gpak* handle, const std::string& filename);

			/**
			 * @brief Destructor that closes the file if it is open.
			 */
			~CGPakInputFile() override;

			/**
			 * @brief Close the currently opened file.
			 */
			void close() override;

			/**
			 * @brief Check if the file is open.
			 *
			 * @return True if the file is open, false otherwise.
			 */
			bool is_open() const override;

			/**
			 * @brief Read a single character from the file.
			 *
			 * @return The character read or EOF on end of file or error.
			 */
			int32_t getc() override;

			/**
			 * @brief Read a specified number of elements of a specified size from the file.
			 *
			 * @param buffer A pointer to a buffer to store the read data.
			 * @param elemSize The size of each element in bytes.
			 * @param elemCount The number of elements to read.
			 * @return The number of elements successfully read.
			 */
			std::size_t read(void* buffer, std::size_t elemSize, std::size_t elemCount) override;

			/**
			 * @brief Unget a character, pushing it back onto the input stream.
			 *
			 * @param character The character to push back.
			 * @return The character pushed back or EOF on error.
			 */
			int32_t ungetc(int32_t character) override;

			/**
			 * @brief Get the current position in the file.
			 *
			 * @return The current position in the file or -1L on error.
			 */
			long tell() override;

			/**
			 * @brief Set the file position indicator to a new position.
			 *
			 * @param offset The number of bytes to offset from the origin.
			 * @param origin The position used as a reference for the offset (SEEK_SET, SEEK_CUR, or SEEK_END).
			 * @return Zero on success, nonzero on error.
			 */
			int32_t seek(long offset, int32_t origin) override;

			/**
			 * @brief Check if the end of file has been reached.
			 *
			 * @return Nonzero if the end of file has been reached, zero otherwise.
			 */
			int32_t eof() const override;
		private:
			gpak_file* m_pFilePtr{ nullptr };
		};
	}
}