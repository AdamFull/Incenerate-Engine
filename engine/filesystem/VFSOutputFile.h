#pragma once

#include "ie_vfs_exports.h"

#include <interface/filesystem/OutputFileInterface.h>
#include <interface/filesystem/VFSOutputFileInterface.h>

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class CVFSOutFile
		 * @brief A final class representing an output file in the virtual file system.
		 *
		 * CVFSOutFile provides file output operations, such as writing data, seeking positions,
		 * and flushing the output buffer.
		 */
		class INCENERATE_VFS_EXPORT CVFSOutFile final : public IVFSOutputFileInterface
		{
		public:
			/**
			 * @brief Default constructor.
			 */
			CVFSOutFile() = default;

			/**
			 * @brief Constructs a CVFSOutFile object with a given IOutputFileInterface.
			 * @param pFile A unique_ptr to an IOutputFileInterface object.
			 */
			explicit CVFSOutFile(std::unique_ptr<IOutputFileInterface>&& pFile);

			/**
			 * @brief Destructor.
			 */
			~CVFSOutFile() override;

			/**
			 * @brief Checks if the output file is open.
			 * @return true if the output file is open, false otherwise.
			 */
			bool is_open() override;

			/**
			 * @brief Puts a single character into the output file.
			 * @param ch The character to be written.
			 * @return A reference to the CVFSOutFile object.
			 */
			CVFSOutFile& put(char ch) override;

			/**
			 * @brief Writes a string to the output file.
			 * @param data The string to be written.
			 * @return A reference to the CVFSOutFile object.
			 */
			CVFSOutFile& write(const std::string& data) override;

			/**
			 * @brief Writes a string to the output file.
			 * @param data The data vector to be written.
			 * @return A reference to the CVFSOutFile object.
			 */
			CVFSOutFile& write(const std::vector<uint8_t>& data) override;

			/**
			 * @brief Writes a buffer of characters to the output file.
			 * @param buffer The buffer containing the characters to be written.
			 * @param count The number of characters to write from the buffer.
			 * @return A reference to the CVFSOutFile object.
			 */
			CVFSOutFile& write(const char* buffer, std::size_t count) override;

			/**
			 * @brief Retrieves the current output position indicator.
			 * @return The current position in the output file as a std::streampos.
			 */
			std::streampos tellp() override;

			/**
			 * @brief Sets the output position indicator to the specified position.
			 * @param pos The new position in the output file as a std::streampos.
			 * @return A reference to the CVFSOutFile object.
			 */
			CVFSOutFile& seekp(std::streampos pos) override;

			/**
			 * @brief Flushes the output buffer.
			 * @return A reference to the CVFSOutFile object.
			 */
			CVFSOutFile& flush() override;
		private:
			std::unique_ptr<IOutputFileInterface> m_pFileInterface;
		};
	}
}