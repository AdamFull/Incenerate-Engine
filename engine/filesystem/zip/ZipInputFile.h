#pragma once

#include "ie_vfs_exports.h"

#include <interface/filesystem/InputFileInterface.h>

#include <string>

struct zip;
struct zip_file;

namespace engine
{
    namespace filesystem
    {
        typedef struct {
            zip_file* file;
            char buffer;
            bool buffer_filled;
            bool end_of_file;
        } lzip_file_t;

        /**
         * @class CZipInputFile
         * @brief This class provides an input file interface for files within a ZIP archive.
         *
         * CZipInputFile is a derived class from IInputFileInterface and implements its
         * interface for reading files within a ZIP archive.
         */
        class INCENERATE_VFS_EXPORT CZipInputFile final : public IInputFileInterface
        {
        public:
            /**
             * @brief Construct a CZipInputFile with a zip handle, a filename, and a password.
             *
             * @param handle A pointer to the zip_t structure.
             * @param filename A string representing the filename within the archive.
             * @param password A string representing the password for encrypted files.
             */
            CZipInputFile(zip* handle, const std::string& filename, const std::string& password);

            /**
             * @brief Destructor.
             */
            ~CZipInputFile() override;

            /**
             * @brief Close the current open file.
             */
            void close() override;

            /**
             * @brief Check if a file is currently open.
             *
             * @return True if a file is open, false otherwise.
             */
            bool is_open() const override;

            /**
             * @brief Read a character from the file.
             *
             * @return The character read cast to an int, or EOF if the end of the file is reached.
             */
            int32_t getc() override;

            /**
             * @brief Read a sequence of elements from the file into a buffer.
             *
             * @param buffer The buffer to store the read data.
             * @param elemSize The size of each element to read.
             * @param elemCount The number of elements to read.
             * @return The total number of elements successfully read.
             */
            std::size_t read(void* buffer, std::size_t elemSize, std::size_t elemCount) override;

            /**
             * @brief Unget a character from the file.
             *
             * @param character The character to unget.
             * @return The ungotten character cast to an int, or EOF on error.
             */
            int32_t ungetc(int32_t character) override;

            /**
             * @brief Get the current position of the file pointer.
             *
             * @return The current position of the file pointer.
             */
            long tell() override;

            /**
             * @brief Set the position of the file pointer.
             *
             * @param offset The number of bytes to offset from origin.
             * @param origin The position from where offset is added.
             * @return 0 if successful, otherwise a non-zero value.
             */
            int32_t seek(long offset, int32_t origin) override;

            /**
             * @brief Check if the end of the file has been reached.
             *
             * @return A non-zero value if the end of the file has been reached, otherwise 0.
             */
            int32_t eof() const override;

        private:
            /**
             * @brief The password for encrypted files.
             */
            std::string m_srPassword{};

            /**
             * @brief A pointer to the lzip_file_t structure.
             */
            lzip_file_t* m_pFilePtr{ nullptr };
        };
    }
}