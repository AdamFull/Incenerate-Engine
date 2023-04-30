#pragma once

namespace engine
{
	namespace filesystem
	{
		class IVFSInputFileInterface
		{
		public:
			virtual ~IVFSInputFileInterface() = default;

			/**
			 * @brief Checks if the input file is open.
			 * @return true if the input file is open, false otherwise.
			 */
			virtual bool is_open() = 0;

			/**
			 * @brief Reads a single character from the input file.
			 * @return The character read or EOF if the end of the file is reached.
			 */
			virtual int get() = 0;

			/**
			 * @brief Reads a line of characters from the input file.
			 * @return The string containing the line read.
			 */
			virtual std::string getline() = 0;

			/**
			 * @brief Retrieves the number of characters read during the last unformatted input operation.
			 * @return The number of characters read.
			 */
			virtual std::size_t gcount() const = 0;

			/**
			 * @brief Ignores a specified number of characters or until a specified delimiter is encountered.
			 * @param count The number of characters to ignore (default is 1).
			 * @param delimiter The delimiter character (default is EOF).
			 * @return A reference to the IVFSInputFileInterface object.
			 */
			virtual IVFSInputFileInterface& ignore(std::size_t count, int delimiter) = 0;

			/**
			 * @brief Peeks at the next character in the input file without extracting it.
			 * @return The next character in the input file or EOF if the end of the file is reached.
			 */
			virtual int peek() const = 0;

			/**
			 * @brief Reads a specified number of characters from the input file into a buffer.
			 * @param buffer The buffer to store the characters read.
			 * @param count The number of characters to read.
			 * @return A reference to the IVFSInputFileInterface object.
			 */
			virtual IVFSInputFileInterface& read(char* buffer, std::size_t count) = 0;

			/**
			 * @brief Reads characters from the input file into a buffer.
			 * @param buffer The buffer to store the characters read.
			 * @return A reference to the IVFSInputFileInterface object.
			 */
			virtual IVFSInputFileInterface& read(std::string& buffer) = 0;

			/**
			 * @brief Puts the last character read back into the input file.
			 * @return A reference to the IVFSInputFileInterface object.
			 */
			virtual IVFSInputFileInterface& unget() = 0;

			/**
			 * @brief Retrieves the current input position indicator.
			 * @return The current position in the input file as a std::streampos.
			 */
			virtual std::streampos tellg() = 0;

			/**
			 * @brief Sets the input position indicator to the specified position.
			 * @param pos The new position in the input file as a std::streampos.
			 * @return A reference to the IVFSInputFileInterface object.
			 */
			virtual IVFSInputFileInterface& seekg(std::streampos pos) = 0;

			/**
			 * @brief Sets the input position indicator to the specified position.
			 * @param pos The new position in the input file as a std::streampos.
			 * @param origin The origin in the input file as a C FILE seek pos.
			 * @return A reference to the IVFSInputFileInterface object.
			 */
			virtual IVFSInputFileInterface& seekg(std::streampos pos, int32_t origin) = 0;

			/**
			 * @brief Checks if the end-of-file (EOF) has been reached.
			 * @return true if the end-of-file is reached, false otherwise.
			 */
			virtual bool eof() const = 0;

			/**
			 * @brief Stream input operator for reading from a CVFSInFile to an output stream.
			 * @param cfs A reference to the CVFSInFile object.
			 * @param os A reference to the output stream.
			 * @return A reference to the output stream.
			 */
			friend std::ostream& operator>>(IVFSInputFileInterface& cfs, std::ostream& os)
			{
				const size_t buffer_size = 1024ull;
				char buffer[buffer_size];

				while (!cfs.eof())
				{
					cfs.read(buffer, buffer_size);
					os.write(buffer, cfs.gcount());
				}

				return os;
			}
		};
	}
}