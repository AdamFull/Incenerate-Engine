#pragma once

#include "interface/DirectoryIteratorInterface.h"

#include <zip.h>

namespace engine
{
    namespace filesystem
    {
        /**
         * @class CZipIterator
         * @brief This class provides an iterator for ZIP archives.
         *
         * CZipIterator is a derived class from IDirectoryIterator and implements its
         * interface for iterating through the files within a ZIP archive.
         */
        class CZipIterator final : public IDirectoryIterator
        {
        public:
            /**
             * @brief Default constructor.
             */
            CZipIterator() = default;

            /**
             * @brief Construct a CZipIterator with a zip archive, a starting path, and a recursion flag.
             *
             * @param archive A pointer to the zip archive.
             * @param start_path A string representing the starting path within the archive.
             * @param recursive A boolean flag indicating if the iterator should recurse into subdirectories.
             */
            CZipIterator(zip* archive, const std::string& start_path, bool recursive);

            /**
             * @brief Virtual destructor.
             */
            virtual ~CZipIterator() override = default;

        protected:
            /**
             * @brief Move to the next item in the archive.
             */
            void _operator_increment() override;

            /**
             * @brief Get the current item in the archive.
             *
             * @return A constant reference to the item's string representation.
             */
            const std::string& _operator_dereferencing() const override;

            /**
             * @brief Compare the directory iterators for inequality.
             *
             * @param other A pointer to the IDirectoryIteratorInterface to compare.
             * @return True if the iterators are not equal, false otherwise.
             */
            bool _operator_not_equal(const IDirectoryIteratorInterface*) const override;

        protected:
            /**
             * @brief A pointer to the zip archive.
             */
            zip* archive_{ nullptr };

            /**
             * @brief The index of the current entry in the archive.
             */
            size_t index_{ 0 };

            /**
             * @brief A vector of strings representing the entries in the archive.
             */
            std::vector<std::string> entries_;
        };
    }
}