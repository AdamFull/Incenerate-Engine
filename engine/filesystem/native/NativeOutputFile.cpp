#include "NativeOutputFile.h"

using namespace engine::filesystem;

CNativeOutputFile::CNativeOutputFile(const std::string& filename)
{
	m_pFilePtr = std::fopen(filename.c_str(), "wb");

	if (!m_pFilePtr)
		throw std::runtime_error("Failed to open native file.");
}

CNativeOutputFile::~CNativeOutputFile()
{
	close();
}

void CNativeOutputFile::close()
{
	if (m_pFilePtr)
	{
		fclose(m_pFilePtr);
		m_pFilePtr = nullptr;
	}
}

bool CNativeOutputFile::is_open() const
{
	return m_pFilePtr;
}

int32_t CNativeOutputFile::putc(int32_t character)
{
	return m_pFilePtr ? std::fputc(character, m_pFilePtr) : -1;
}

std::size_t CNativeOutputFile::write(const void* buffer, std::size_t elemSize, std::size_t elemCount)
{
	return m_pFilePtr ? std::fwrite(buffer, elemSize, elemCount, m_pFilePtr) : 0ull;
}

long CNativeOutputFile::tell()
{
	return m_pFilePtr ? std::ftell(m_pFilePtr) : -1;
}

int32_t CNativeOutputFile::seek(long offset, int32_t origin)
{
	return m_pFilePtr ? std::fseek(m_pFilePtr, offset, origin) : -1;
}

int32_t CNativeOutputFile::flush()
{
	return m_pFilePtr ? std::fflush(m_pFilePtr) : -1;
}