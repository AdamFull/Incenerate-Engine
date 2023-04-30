#pragma once

namespace engine
{
	namespace filesystem
	{
		class IVFSOutputFileInterface
		{
		public:
			virtual ~IVFSOutputFileInterface() = default;
			/**
			 * @brief Checks if the output file is open.
			 * @return true if the output file is open, false otherwise.
			 */
			virtual bool is_open() = 0;

			/**
			 * @brief Puts a single character into the output file.
			 * @param ch The character to be written.
			 * @return A reference to the IVFSOutputFileInterface object.
			 */
			virtual IVFSOutputFileInterface& put(char ch) = 0;

			/**
			 * @brief Writes a string to the output file.
			 * @param str The string to be written.
			 * @return A reference to the IVFSOutputFileInterface object.
			 */
			virtual IVFSOutputFileInterface& write(const std::string& str) = 0;

			/**
			 * @brief Writes a buffer of characters to the output file.
			 * @param buffer The buffer containing the characters to be written.
			 * @param count The number of characters to write from the buffer.
			 * @return A reference to the IVFSOutputFileInterface object.
			 */
			virtual IVFSOutputFileInterface& write(const char* buffer, std::size_t count) = 0;

			/**
			 * @brief Retrieves the current output position indicator.
			 * @return The current position in the output file as a std::streampos.
			 */
			virtual std::streampos tellp() = 0;

			/**
			 * @brief Sets the output position indicator to the specified position.
			 * @param pos The new position in the output file as a std::streampos.
			 * @return A reference to the IVFSOutputFileInterface object.
			 */
			virtual IVFSOutputFileInterface& seekp(std::streampos pos) = 0;

			/**
			 * @brief Flushes the output buffer.
			 * @return A reference to the IVFSOutputFileInterface object.
			 */
			virtual IVFSOutputFileInterface& flush() = 0;

			/**
			 * @brief Stream output operator for writing from an input stream to a IVFSOutputFileInterface.
			 * @param cfs A reference to the IVFSOutputFileInterface object.
			 * @param is A reference to the input stream.
			 * @return A reference to the IVFSOutputFileInterface object.
			 */
			friend IVFSOutputFileInterface& operator<<(IVFSOutputFileInterface& cfs, std::istream& is)
			{
				std::string buffer;
				buffer.resize(1024ull);

				while (!is.eof())
				{
					is.read(buffer.data(), buffer.size());
					std::streamsize read_count = is.gcount();
					cfs.write(buffer);
				}

				return cfs;
			}
		};
	}
}