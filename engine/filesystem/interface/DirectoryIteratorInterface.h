#pragma once

namespace engine
{
	namespace filesystem
	{
        class IDirectoryIteratorInterface;

        /**
        * @class IDirectoryIterator
        * @brief This class is an interface for iterating over directories.
        */
        class IDirectoryIterator
        {
        public:
            /**
             * @brief Virtual destructor.
             */
            virtual ~IDirectoryIterator() = default;

            /**
             * @brief Move to the next item in the directory.
             */
            virtual void _operator_increment() = 0;

            /**
             * @brief Get the current item in the directory.
             *
             * @return A constant reference to the item's string representation.
             */
            virtual const std::string& _operator_dereferencing() const = 0;

            /**
             * @brief Compare the directory iterators for inequality.
             *
             * @param other A pointer to the IDirectoryIteratorInterface to compare.
             * @return True if the iterators are not equal, false otherwise.
             */
            virtual bool _operator_not_equal(const IDirectoryIteratorInterface*) const = 0;
        };


		class IDirectoryIteratorInterface
		{
		public:
			virtual ~IDirectoryIteratorInterface() = default;

            /**
            * @brief Get the iterator pointing to the beginning of the directory.
            *
            * @return A reference to the iterator pointing to the beginning of the directory.
            */
            virtual IDirectoryIteratorInterface& begin() = 0;

            /**
             * @brief Get the iterator pointing to the end of the directory.
             *
             * @return A reference to the iterator pointing to the end of the directory.
             */
            virtual IDirectoryIteratorInterface& end() = 0;

            /**
             * @brief Move to the next item in the directory.
             *
             * @return A reference to the updated iterator.
             */
            virtual IDirectoryIteratorInterface& operator++() = 0;

            /**
             * @brief Get the current item in the directory.
             *
             * @return A constant reference to the item's string representation.
             */
            virtual const std::string& operator*() = 0;

            /**
             * @brief Compare the directory iterators for inequality.
             *
             * @param other A reference to the CDirectoryIterator to compare.
             * @return True if the iterators are not equal, false otherwise.
             */
            virtual bool operator!=(const IDirectoryIteratorInterface& other) const = 0;

            /**
             * @brief Unique_ptr to an IDirectoryIterator object.
             */
            std::unique_ptr<IDirectoryIterator> it_{ nullptr };
		};
	}
}