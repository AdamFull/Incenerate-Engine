#pragma once

#include <ie_vfs_exports.h>

#include <interface/filesystem/InputFileInterface.h>
#include <interface/filesystem/VfsInputFileInterface.h>

namespace engine
{
	namespace filesystem
	{
		/**
		 * @class CVFSInFile
		 * @brief A final class representing an input file in the virtual file system.
		 *
		 * CVFSInFile provides file input operations, such as reading data, seeking positions,
		 * and checking the end of the file.
		 */
		class INCENERATE_VFS_EXPORT CVFSInFile final : public IVFSInputFileInterface
		{
		public:
			/**
			 * @brief Default constructor.
			 */
			CVFSInFile() = default;

			/**
			 * @brief Constructs a CVFSInFile object with a given IInputFileInterface.
			 * @param pFile A unique_ptr to an IInputFileInterface object.
			 */
			explicit CVFSInFile(std::unique_ptr<IInputFileInterface>&& pFile);

			/**
			 * @brief Destructor.
			 */
			~CVFSInFile() override;

			/**
			 * @brief Checks if the input file is open.
			 * @return true if the input file is open, false otherwise.
			 */
			bool is_open() override;

			/**
			 * @brief Reads a single character from the input file.
			 * @return The character read or EOF if the end of the file is reached.
			 */
			int get() override;

			/**
			 * @brief Reads a line of characters from the input file.
			 * @return The string containing the line read.
			 */
			std::string getline() override;

			/**
			 * @brief Retrieves the number of characters read during the last unformatted input operation.
			 * @return The number of characters read.
			 */
			std::size_t gcount() const override;

			/**
			 * @brief Ignores a specified number of characters or until a specified delimiter is encountered.
			 * @param count The number of characters to ignore (default is 1).
			 * @param delimiter The delimiter character (default is EOF).
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& ignore(std::size_t count = 1, int delimiter = EOF) override;

			/**
			 * @brief Peeks at the next character in the input file without extracting it.
			 * @return The next character in the input file or EOF if the end of the file is reached.
			 */
			int peek() const override;

			/**
			 * @brief Reads a specified number of characters from the input file into a buffer.
			 * @param buffer The buffer to store the characters read.
			 * @param count The number of characters to read.
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& read(char* buffer, std::size_t count) override;

			/**
			 * @brief Reads characters from the input file into a buffer.
			 * @param buffer The buffer to store the characters read.
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& read(std::string& buffer) override;

			/**
			 * @brief Reads characters from the input file into a buffer.
			 * @param buffer The buffer to store the characters read.
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& read(std::vector<uint8_t>& buffer) override;

			/**
			 * @brief Puts the last character read back into the input file.
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& unget() override;

			/**
			 * @brief Retrieves the current input position indicator.
			 * @return The current position in the input file as a std::streampos.
			 */
			std::streampos tellg() override;

			/**
			 * @brief Sets the input position indicator to the specified position.
			 * @param pos The new position in the input file as a std::streampos.
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& seekg(std::streampos pos) override;

			/**
			 * @brief Sets the input position indicator to the specified position.
			 * @param pos The new position in the input file as a std::streampos.
			 * @param origin The origin in the input file as a C FILE seek pos.
			 * @return A reference to the CVFSInFile object.
			 */
			CVFSInFile& seekg(std::streampos pos, int32_t origin) override;

			/**
			 * @brief Checks if the end-of-file (EOF) has been reached.
			 * @return true if the end-of-file is reached, false otherwise.
			 */
			bool eof() const override;

		private:
			std::unique_ptr<IInputFileInterface> m_pFileInterface{ nullptr };
			std::size_t m_gcount{ 0ull };
		};
	}
}