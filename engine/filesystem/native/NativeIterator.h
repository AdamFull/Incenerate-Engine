#pragma once

#include <ie_vfs_exports.h>

#include <interface/filesystem/DirectoryIteratorInterface.h>
#include "vfs_helper.h"

#include <string>

namespace engine
{
    namespace filesystem
    {
        /**
         * @class CNativeIterator
         * @brief A template class providing an interface for native directory iteration.
         *
         * CNativeIterator is a derived class from IDirectoryIterator that provides
         * a platform-specific implementation for iterating through directories using the
         * native filesystem functions. The class is templated to allow for different types
         * of directory iterators.
         *
         * @tparam _Ty The type of native directory iterator to be used.
         */
        template<class _Ty>
        class INCENERATE_VFS_EXPORT CNativeIterator final : public IDirectoryIterator
        {
        public:
            /**
             * @brief Default constructor.
             */
            CNativeIterator() = default;

            /**
             * @brief Construct a CNativeIterator for the specified path and root.
             *
             * @param path A string representing the path to the directory to be iterated.
             * @param root A string representing the root directory for relative paths.
             */
            CNativeIterator(const std::string& path, const std::string& root) : it_(_Ty(path))
            {
                root_ = root;
                path_ = fs::to_posix_path(std::filesystem::relative(it_->path(), root_).string());
            }
        protected:
            /**
             * @brief Increment the iterator to the next entry in the directory.
             */
            void _operator_increment() override
            {
                ++it_;
                if (it_ != _Ty())
                    path_ = fs::to_posix_path(fs::from_unicode(std::filesystem::relative(it_->path(), root_).u16string()));
            }

            /**
             * @brief Dereference the iterator to get the current entry's relative path.
             *
             * @return A reference to a string representing the current entry's relative path.
             */
            const std::string& _operator_dereferencing() const override { return path_; }

            /**
             * @brief Compare this iterator with another IDirectoryIteratorInterface.
             *
             * @param other A pointer to the IDirectoryIteratorInterface to be compared with.
             * @return True if the iterators are not equal, false otherwise.
             */
            bool _operator_not_equal(const IDirectoryIteratorInterface* other) const override
            {
                if (!other->it_)
                    return it_ != _Ty();

                const auto* other_it = dynamic_cast<const CNativeIterator<_Ty>*>(other->it_.get());
                return it_ != other_it->it_;
            }
        private:
            /**
             * @brief The native directory iterator.
             */
            _Ty it_;

            /**
             * @brief The relative path of the current file or directory.
             */
            std::string path_;

            /**
             * @brief The root directory for relative paths.
             */
            std::string root_;
        };
    }
}