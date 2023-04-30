#include "VirtualFileSystemManager.h"
#include "VFSInputFile.h"
#include "VFSOutputFile.h"
#include "DirectoryIterator.h"
#include "vfs_helper.h"

using namespace engine::filesystem;

bool is_read_only(const std::unique_ptr<IVFSInterface>& pFS)
{
	return pFS && !(pFS->getFilesystemMode() & static_cast<uint32_t>(EFilesystemModeFlagBits::eWrite));
}


CVirtualFileSystemManager::~CVirtualFileSystemManager()
{
}

void CVirtualFileSystemManager::mount(const std::string& mountpoint, std::unique_ptr<IVFSInterface>&& vfsptr)
{
	m_vMountedFS.emplace(mountpoint, std::move(vfsptr));
}

void CVirtualFileSystemManager::unmount(const std::string& mountpoint)
{
	auto found = m_vMountedFS.find(mountpoint);
	if (found != m_vMountedFS.end())
		m_vMountedFS.erase(found);
}

void CVirtualFileSystemManager::copy(const std::string& from, const std::string& to)
{
	auto& pFromFilesystem = getFilesystem(from);
	auto& pToFilesystem = getFilesystem(to);

	if (is_read_only(pToFilesystem))
		throw std::runtime_error("Failed. You trying to copy file to read only filesystem.");

	const auto& fromMountPoint = fs::get_mount_point(from);
	const auto& toMountPoint = fs::get_mount_point(to);
	
	if (fromMountPoint == toMountPoint)
	{
		pFromFilesystem->copy(fs::get_local_path(from), fs::get_local_path(to));
		return;
	}
		
	auto fromPath = fs::get_local_path(from);
	if (pFromFilesystem->isDirectory(fromPath))
	{
		auto iterator = std::make_unique<CDirectoryIterator>(pFromFilesystem->walk(fromPath, true), fromMountPoint);

		for (auto& it = iterator->begin(); it != iterator->end() ; ++it)
			processCopyFile(pFromFilesystem, pToFilesystem, fs::get_local_path(*it));
	}
	else
		processCopyFile(pFromFilesystem, pToFilesystem, fromPath);
}

bool CVirtualFileSystemManager::createDirectories(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return false;

	if (is_read_only(pFilesystem))
		throw std::runtime_error("Failed. You trying to create directories in read only filesystem.");

	return pFilesystem->createDirectories(fs::get_local_path(path));
}

bool CVirtualFileSystemManager::createDirectory(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return false;

	if (is_read_only(pFilesystem))
		throw std::runtime_error("Failed. You trying to create directories in read only filesystem.");

	return pFilesystem->createDirectory(fs::get_local_path(path));
}

bool CVirtualFileSystemManager::exists(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return false;

	return pFilesystem->exists(fs::get_local_path(path));
}

size_t CVirtualFileSystemManager::fileSize(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return 0ull;

	return pFilesystem->fileSize(fs::get_local_path(path));
}

bool CVirtualFileSystemManager::isDirectory(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return false;

	return pFilesystem->isDirectory(fs::get_local_path(path));
}

bool CVirtualFileSystemManager::isFile(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return false;

	return pFilesystem->isFile(fs::get_local_path(path));
}

bool CVirtualFileSystemManager::remove(const std::string& path)
{
	auto& pFilesystem = getFilesystem(path);

	if (!pFilesystem)
		return false;

	return pFilesystem->remove(fs::get_local_path(path));
}

void CVirtualFileSystemManager::rename(const std::string& original, const std::string& newfile)
{
	if (fs::get_mount_point(original) != fs::get_mount_point(newfile))
		return;

	auto& pFilesystem = getFilesystem(original);

	if (!pFilesystem)
		return;

	pFilesystem->rename(fs::get_local_path(original), fs::get_local_path(newfile));
}

std::unique_ptr<IVFSOutputFileInterface> CVirtualFileSystemManager::createFile(const std::string& path)
{
	const std::lock_guard<std::mutex> lock(m_SynchronizationMutex);

	auto& pFilesystem = getFilesystem(path);
	if (!pFilesystem)
		return nullptr;

	if (is_read_only(pFilesystem))
		throw std::runtime_error("Failed. You trying to create file in read only filesystem.");

	return std::make_unique<CVFSOutFile>(pFilesystem->createFile(fs::get_local_path(path)));
}

std::unique_ptr<IVFSInputFileInterface> CVirtualFileSystemManager::openFile(const std::string& path)
{
	const std::lock_guard<std::mutex> lock(m_SynchronizationMutex);

	auto& pFilesystem = getFilesystem(path);
	if (!pFilesystem)
		return nullptr;

	auto local_path = fs::get_local_path(path);

	if (pFilesystem->isDirectory(local_path))
		return nullptr;

	return std::make_unique<CVFSInFile>(pFilesystem->openFile(local_path));
}

std::unique_ptr<IDirectoryIteratorInterface> CVirtualFileSystemManager::walk(const std::string& path, bool recursive)
{
	auto& pFilesystem = getFilesystem(path);
	if (!pFilesystem)
		return nullptr;

	return std::make_unique<CDirectoryIterator>(pFilesystem->walk(fs::get_local_path(path), recursive), fs::get_mount_point(path));
}

bool CVirtualFileSystemManager::readFile(const std::string& path, std::string& data)
{
	return false;
}

bool CVirtualFileSystemManager::writeFile(const std::string& path, const std::string& data)
{
	return false;
}

bool CVirtualFileSystemManager::writeFile(const std::string& path, std::vector<uint8_t>& binary)
{
	return false;
}

const std::unique_ptr<IVFSInterface>& CVirtualFileSystemManager::getFilesystem(const std::string& path)
{
	static std::unique_ptr<IVFSInterface> pEmpty{ nullptr };

	auto fs_prefix = fs::get_mount_point(path);
	auto found = m_vMountedFS.find(fs_prefix);
	if (found != m_vMountedFS.end())
		return found->second;

	return pEmpty;
}

void CVirtualFileSystemManager::processCopyFile(const std::unique_ptr<IVFSInterface>& pFSFrom, const std::unique_ptr<IVFSInterface>& pFSTo, const std::string& path)
{
	if (pFSFrom->isDirectory(path))
	{
		pFSTo->createDirectories(path);
		return;
	}
	

	auto pInFile = std::make_unique<CVFSInFile>(pFSFrom->openFile(path));
	auto pOutFile = std::make_unique<CVFSOutFile>(pFSTo->createFile(path));

	std::string buffer(4096, '\0');

	while (!pInFile->eof())
	{
		pInFile->read(buffer.data(), buffer.size());
		pOutFile->write(buffer.data(), pInFile->gcount());
	}
}