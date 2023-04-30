#pragma once

#include "interface/DirectoryIteratorInterface.h"

#include <gpak_data.h>

namespace engine
{
    namespace filesystem
    {
        /**
         * @class CGPakIterator
         * @brief This class provides an iterator for GPak archives.
         *
         * CGPakIterator is a derived class from IDirectoryIterator and implements its
         * interface for iterating through the files within a gpak archive.
         */
        class CGPakIterator final : public IDirectoryIterator
        {
        public:
            /**
             * @brief Default constructor.
             */
            CGPakIterator() = default;

            /**
             * @brief Construct a CGPakIterator with a gpak archive, a starting path, and a recursion flag.
             *
             * @param _pak A pointer to the gpak archive.
             * @param start_path A string representing the starting path within the archive.
             * @param recursive A boolean flag indicating if the iterator should recurse into subdirectories.
             */
            CGPakIterator(gpak_t* _pak, const std::string& start_path, bool recursive);

            /**
             * @brief Virtual destructor.
             */
            virtual ~CGPakIterator() override;
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
            filesystem_tree_iterator_t* m_pIterator{ nullptr };
            filesystem_tree_node_t* m_pCurrentDir{ nullptr };
            std::string m_srCurrentPath{};
            bool m_bIsRecursive{ false };
        };
    }
}