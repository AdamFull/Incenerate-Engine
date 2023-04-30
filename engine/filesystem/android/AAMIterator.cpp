#include "AAMIterator.h"

using namespace engine::filesystem;

CAAMDirectoryIterator::CAAMDirectoryIterator(AAssetManager* handle, const std::string& start_path) :
	m_pAssetMgr(handle)
{
	m_pCurrentDir = AAssetManager_openDir(handle, start_path.c_str());
	_operator_increment();
}

CAAMDirectoryIterator::~CAAMDirectoryIterator()
{
	if (m_pCurrentDir != nullptr)
		AAssetDir_close(m_pCurrentDir);
}

void CAAMDirectoryIterator::_operator_increment()
{
	const char* filename = AAssetDir_getNextFileName(m_pCurrentDir);
	if (filename)
		m_srCurrentPath = filename;
	else 
	{
		m_srCurrentPath.clear();
		AAssetDir_close(m_pCurrentDir);
		m_pCurrentDir = nullptr;
	}
}

const std::string& CAAMDirectoryIterator::_operator_dereferencing() const
{
	return m_srCurrentPath;
}

bool CAAMDirectoryIterator::_operator_not_equal(const CDirectoryIterator* other) const
{
	if (!other->it_)
		return m_pCurrentDir != nullptr;

	const auto* other_it = dynamic_cast<const CAAMDirectoryIterator*>(other->it_.get());
	return m_pCurrentDir != other_it->m_pCurrentDir;
}


CAAMRecursiveDirectoryIterator::CAAMRecursiveDirectoryIterator(AAssetManager* handle, const std::string& start_path) :
	m_pAssetMgr(handle)
{
	//m_iterStack.emplace(CAAMDirectoryIterator(handle, start_path));
	_operator_increment();
}

CAAMRecursiveDirectoryIterator::~CAAMRecursiveDirectoryIterator()
{

}

void CAAMRecursiveDirectoryIterator::_operator_increment()
{
	/*while (!m_iterStack.empty())
	{
		auto& iter = m_iterStack.top();
		if (iter._operator_dereferencing() != "")
		{
			m_srCurrentPath = iter._operator_dereferencing();
			iter._operator_increment();

			AAsset* asset = AAssetManager_open(m_pAssetMgr, m_srCurrentPath.c_str(), AASSET_MODE_UNKNOWN);
			if (asset) 
			{
				off_t length = AAsset_getLength(asset);
				if (length == 0)
					m_iterStack.emplace(CAAMDirectoryIterator(m_pAssetMgr, m_srCurrentPath));

				AAsset_close(asset);
			}
			return;
		}
		else
			m_iterStack.pop();
	}*/

	m_srCurrentPath.clear();
}

const std::string& CAAMRecursiveDirectoryIterator::_operator_dereferencing() const
{
	return m_srCurrentPath;
}

bool CAAMRecursiveDirectoryIterator::_operator_not_equal(const CDirectoryIterator* other) const
{
	if (!other->it_)
		return !m_iterStack.empty();

	//const auto* other_it = dynamic_cast<const CAAMRecursiveDirectoryIterator*>(other->it_.get());
	//return m_iterStack != other_it->m_iterStack;
}