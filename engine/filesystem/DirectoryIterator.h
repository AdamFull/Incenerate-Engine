#pragma once

#include "interface/DirectoryIteratorInterface.h"

namespace engine
{
    namespace filesystem
    {
        /**
         * @class CDirectoryIterator
         * @brief This class provides a directory iterator implementation.
         */
        class CDirectoryIterator : public IDirectoryIteratorInterface
        {
        public:
            /**
             * @brief Default constructor.
             */
            CDirectoryIterator() = default;

            /**
             * @brief Construct a CDirectoryIterator from a unique_ptr to an IDirectoryIterator and a mountpoint string.
             *
             * @param it A unique_ptr to an IDirectoryIterator.
             * @param mountpoint A string representing the mountpoint.
             */
            CDirectoryIterator(std::unique_ptr<IDirectoryIterator>&& it, const std::string& mountpoint);

            /**
             * @brief Destructor.
             */
            ~CDirectoryIterator() override = default;

            /**
             * @brief Get the iterator pointing to the beginning of the directory.
             *
             * @return A reference to the iterator pointing to the beginning of the directory.
             */
            CDirectoryIterator& begin() override;

            /**
             * @brief Get the iterator pointing to the end of the directory.
             *
             * @return A reference to the iterator pointing to the end of the directory.
             */
            CDirectoryIterator& end() override;

            /**
             * @brief Move to the next item in the directory.
             *
             * @return A reference to the updated iterator.
             */
            CDirectoryIterator& operator++() override;

            /**
             * @brief Get the current item in the directory.
             *
             * @return A constant reference to the item's string representation.
             */
            const std::string& operator*() override;

            /**
             * @brief Compare the directory iterators for inequality.
             *
             * @param other A reference to the CDirectoryIterator to compare.
             * @return True if the iterators are not equal, false otherwise.
             */
            bool operator!=(const IDirectoryIteratorInterface& other) const override;
        private:
            std::string m_srMountPoint{};
            std::string m_srCurrent{};
        };
    }
}