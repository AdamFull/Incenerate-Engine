#include "ZipFileSystem.h"
#include "ZipInputFile.h"
#include "ZipIterator.h"

#include <zip.h>

using namespace engine::filesystem;

CZipFileSystem::CZipFileSystem(const std::string& fspath, const std::string& password)
{
	create(fspath);
	m_srPassword = password;
}

CZipFileSystem::~CZipFileSystem()
{
	if(m_pArchive)
		zip_close(m_pArchive);
}


bool CZipFileSystem::create(const std::string& fspath)
{
	if (m_pArchive)
		return false;

	int error_code = 0;
	m_pArchive = zip_open(fspath.c_str(), ZIP_RDONLY, &error_code);

	if (!m_pArchive) 
	{
		char error_message[256];
		zip_error_to_str(error_message, sizeof(error_message), error_code, errno);
		return false;
	}

	return true;
}

bool CZipFileSystem::exists(const std::string& path)
{
	if (!m_pArchive)
		return false;

	zip_stat_t stat_info;
	if (zip_stat(m_pArchive, path.c_str(), ZIP_FL_NOCASE, &stat_info) == 0)
		return stat_info.valid & ZIP_STAT_NAME;

	return false;
}

size_t CZipFileSystem::fileSize(const std::string& path)
{
	if (!m_pArchive)
		return 0;

	zip_stat_t stat_info;
	if (zip_stat(m_pArchive, path.c_str(), 0, &stat_info) == 0)
		return static_cast<size_t>(stat_info.size);

	return 0ull;
}

bool CZipFileSystem::isDirectory(const std::string& path)
{
	if (!m_pArchive)
		return false;

	zip_stat_t stat_info;
	if (zip_stat(m_pArchive, path.c_str(), ZIP_FL_NOCASE, &stat_info) == 0)
		return (stat_info.valid & ZIP_STAT_NAME) && (stat_info.name[strlen(stat_info.name) - 1] == '/');

	return false;
}

bool CZipFileSystem::isFile(const std::string& path)
{
	if (!m_pArchive)
		return false;

	zip_stat_t stat_info;
	if (zip_stat(m_pArchive, path.c_str(), ZIP_FL_NOCASE, &stat_info) == 0)
		return (stat_info.valid & ZIP_STAT_NAME) && (stat_info.name[strlen(stat_info.name) - 1] != '/');

	return false;
}

std::unique_ptr<IInputFileInterface> CZipFileSystem::openFile(const std::string& path)
{
	if (!isFile(path))
		return nullptr;

	return std::make_unique<CZipInputFile>(m_pArchive, path, m_srPassword);
}

std::unique_ptr<IDirectoryIterator>  CZipFileSystem::walk(const std::string& path, bool recursive)
 {
	return std::make_unique<CZipIterator>(m_pArchive, path, recursive);;
 }

EFilesystemModeFlags CZipFileSystem::getFilesystemMode() const
{
	return static_cast<uint32_t>(EFilesystemModeFlagBits::eRead);
}