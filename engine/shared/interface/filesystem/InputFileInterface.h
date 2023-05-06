#pragma once

#include <cstddef>
#include <cstdint>

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class IInputFileInterface
		 * @brief This class is an interface for input file operations.
		 *
		 * It provides a standard interface for operations such as opening and closing files,
		 * reading from files, checking if a file is open, and manipulating the file pointer.
		 */
		class IInputFileInterface
		{
		public:
			/**
			 * @brief Virtual destructor.
			 */
			virtual ~IInputFileInterface() = default;

			/**
			 * @brief Close the current open file.
			 */
			virtual void close() = 0;

			/**
			 * @brief Check if a file is currently open.
			 *
			 * @return True if a file is open, false otherwise.
			 */
			virtual bool is_open() const = 0;

			/**
			 * @brief Read a character from the file.
			 *
			 * @return The character read cast to an int, or EOF if the end of the file is reached.
			 */
			virtual int32_t getc() = 0;

			/**
			 * @brief Read a sequence of elements from the file into a buffer.
			 *
			 * @param buffer The buffer to store the read data.
			 * @param elemSize The size of each element to read.
			 * @param elemCount The number of elements to read.
			 * @return The total number of elements successfully read.
			 */
			virtual std::size_t read(void* buffer, std::size_t elemSize, std::size_t elemCount) = 0;

			/**
			 * @brief Unget a character from the file.
			 *
			 * @param character The character to unget.
			 * @return The ungotten character cast to an int, or EOF on error.
			 */
			virtual int32_t ungetc(int32_t character) = 0;

			/**
			 * @brief Get the current position of the file pointer.
			 *
			 * @return The current position of the file pointer.
			 */
			virtual long tell() = 0;

			/**
			 * @brief Set the position of the file pointer.
			 *
			 * @param offset The number of bytes to offset from origin.
			 * @param origin The position from where offset is added.
			 * @return 0 if successful, otherwise a non-zero value.
			 */
			virtual int32_t seek(long offset, int32_t origin) = 0;

			/**
			 * @brief Check if the end of the file has been reached.
			 *
			 * @return A non-zero value if the end of the file has been reached, otherwise 0.
			 */
			virtual int32_t eof() const = 0;
		};
	}
}