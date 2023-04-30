#include "GPakIterator.h"

#include <gpak.h>
#include <filesystem_tree.h>

using namespace engine::filesystem;

CGPakIterator::CGPakIterator(gpak_t* _pak, const std::string& start_path, bool recursive)
{
	m_bIsRecursive = recursive;

	if (auto* _root = gpak_get_root(_pak))
	{
		if (auto* _start_dir = filesystem_tree_find_directory(_root, start_path.c_str()))
		{
			m_pIterator = filesystem_iterator_create(_start_dir);
			m_pCurrentDir = _start_dir;
		}
	}
}

CGPakIterator::~CGPakIterator()
{
	if (m_pIterator)
		filesystem_iterator_free(m_pIterator);
}

void CGPakIterator::_operator_increment()
{
	char* pathBuf{ nullptr };

	if (auto* currentFile = filesystem_iterator_next_file(m_pIterator))
		pathBuf = filesystem_tree_file_path(m_pCurrentDir, currentFile);
	else
	{
		m_pCurrentDir = m_bIsRecursive ? filesystem_iterator_next_directory(m_pIterator) : filesystem_iterator_next_subling_directory(m_pIterator);
		pathBuf = filesystem_tree_directory_path(m_pCurrentDir);
	}

	if (pathBuf)
	{
		m_srCurrentPath = std::string(pathBuf);
		free(pathBuf);
	}
}

const std::string& CGPakIterator::_operator_dereferencing() const
{
	return m_srCurrentPath;
}

bool CGPakIterator::_operator_not_equal(const IDirectoryIteratorInterface* other) const
{
	if (!other->it_)
		return m_pCurrentDir != nullptr;

	const auto* other_it = dynamic_cast<const CGPakIterator*>(other->it_.get());
	return m_pCurrentDir != other_it->m_pCurrentDir;
}