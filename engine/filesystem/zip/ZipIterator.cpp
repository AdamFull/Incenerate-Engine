#include "ZipIterator.h"

using namespace engine::filesystem;

void get_entries(zip* archive, std::vector<std::string>& outresults, const std::string& start_path, bool recurse)
{
    for (zip_int64_t i = 0; i < zip_get_num_entries(archive, 0); ++i)
    {
        zip_stat_t stat;
        if (zip_stat_index(archive, i, 0, &stat) == 0) 
        {
            std::string entry_path(stat.name);

            if (entry_path == start_path)
                continue;

            if (recurse)
            {
                if (entry_path.find(start_path) == 0)
                    outresults.emplace_back(entry_path);
            }
            else
            {
                if (entry_path.find(start_path) == 0 && entry_path.find('/', start_path.size()) == std::string::npos)
                    outresults.emplace_back(entry_path);
            }
        }
        else 
            throw std::runtime_error("Error reading entry information.");
    }
}

CZipIterator::CZipIterator(zip* archive, const std::string& start_path, bool recursive)
{
    archive_ = archive;
    get_entries(archive, entries_, start_path, recursive);
}

void CZipIterator::_operator_increment()
{
    ++index_;
}

const std::string& CZipIterator::_operator_dereferencing() const
{
    return entries_.at(index_);
}

bool CZipIterator::_operator_not_equal(const IDirectoryIteratorInterface* other) const
{
    if (!other->it_)
        return index_ != entries_.size();
 
    const auto* other_it = dynamic_cast<const CZipIterator*>(other->it_.get());
    return index_ != other_it->index_;
}