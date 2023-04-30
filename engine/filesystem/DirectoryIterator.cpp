#include "DirectoryIterator.h"

using namespace engine::filesystem;

CDirectoryIterator::CDirectoryIterator(std::unique_ptr<IDirectoryIterator>&& it, const std::string& mountpoint) :
    m_srMountPoint(mountpoint)
{
    it_ = std::move(it);
}

CDirectoryIterator& CDirectoryIterator::begin()
{ 
    return *this; 
}

CDirectoryIterator& CDirectoryIterator::end()
{
    static CDirectoryIterator end_{};
    return end_;
}

CDirectoryIterator& CDirectoryIterator::operator++()
{
    it_->_operator_increment();
    return *this;
}

const std::string& CDirectoryIterator::operator*()
{
    m_srCurrent = m_srMountPoint + "/" + it_->_operator_dereferencing();
    return m_srCurrent;
}

bool CDirectoryIterator::operator!=(const IDirectoryIteratorInterface& other) const
{
    return it_->_operator_not_equal(&other); 
}