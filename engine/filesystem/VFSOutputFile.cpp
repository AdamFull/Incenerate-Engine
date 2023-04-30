#include "VFSOutputFile.h"

using namespace engine::filesystem;

CVFSOutFile::CVFSOutFile(std::unique_ptr<IOutputFileInterface>&& pFile) :
	m_pFileInterface(std::move(pFile))
{
	if (!m_pFileInterface)
		throw std::runtime_error("Failed to create file");
}

CVFSOutFile::~CVFSOutFile()
{
	if (m_pFileInterface)
		m_pFileInterface->close();
}

bool CVFSOutFile::is_open()
{
	if (m_pFileInterface)
		return m_pFileInterface->is_open();

	return false;
}

CVFSOutFile& CVFSOutFile::put(char ch)
{
	if (m_pFileInterface)
		m_pFileInterface->putc(ch);

	return *this;
}

CVFSOutFile& CVFSOutFile::write(const std::string& str)
{
	if (m_pFileInterface)
		m_pFileInterface->write(str.c_str(), sizeof(char), str.size());

	return *this;
}

CVFSOutFile& CVFSOutFile::write(const char* buffer, std::size_t count)
{
	if (m_pFileInterface)
		m_pFileInterface->write(buffer, sizeof(char), count);

	return *this;
}

std::streampos CVFSOutFile::tellp()
{
	return m_pFileInterface ? m_pFileInterface->tell() : -1;
}

CVFSOutFile& CVFSOutFile::seekp(std::streampos pos)
{
	if (m_pFileInterface)
		m_pFileInterface->seek(pos, SEEK_SET);

	return *this;
}

CVFSOutFile& CVFSOutFile::flush()
{
	if (m_pFileInterface)
		m_pFileInterface->flush();

	return *this;
}