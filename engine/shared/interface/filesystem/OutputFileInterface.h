#pragma once

#include <cstddef>
#include <cstdint>

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class IOutputFileInterface
		 * @brief This class is an interface for output file operations.
		 *
		 * It provides a standard interface for operations such as opening and closing files,
		 * writing to files, checking if a file is open, and manipulating the file pointer.
		 */
		class IOutputFileInterface
		{
		public:
			/**
			 * @brief Virtual destructor.
			 */
			virtual ~IOutputFileInterface() = default;

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
			 * @brief Write a character to the file.
			 *
			 * @param character The character to write to the file.
			 * @return The written character cast to an int, or EOF on error.
			 */
			virtual int32_t putc(int32_t character) = 0;

			/**
			 * @brief Write a sequence of elements to the file from a buffer.
			 *
			 * @param buffer The buffer to write data from.
			 * @param elemSize The size of each element to write.
			 * @param elemCount The number of elements to write.
			 * @return The total number of elements successfully written.
			 */
			virtual std::size_t write(const void* buffer, std::size_t elemSize, std::size_t elemCount) = 0;

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
			 * @brief Flush the buffer of the file.
			 *
			 * @return Zero if the buffer was successfully flushed. A non-zero value indicates an error.
			 */
			virtual int32_t flush() = 0;
		};
	}
}