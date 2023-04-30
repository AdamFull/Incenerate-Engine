#include "NativeInputFile.h"

using namespace engine::filesystem;

CNativeInputFile::CNativeInputFile(const std::string& filename)
{
	m_pFilePtr = std::fopen(filename.c_str(), "rb");

	if (!m_pFilePtr)
		throw std::runtime_error("Failed to open native file.");
}

CNativeInputFile::~CNativeInputFile()
{
	close();
}

void CNativeInputFile::close()
{
	if (m_pFilePtr)
	{
		fclose(m_pFilePtr);
		m_pFilePtr = nullptr;
	}
}

bool CNativeInputFile::is_open() const
{
	return m_pFilePtr;
}

int32_t CNativeInputFile::getc()
{
	return m_pFilePtr ? std::fgetc(m_pFilePtr) : -1;
}

std::size_t CNativeInputFile::read(char* buffer, std::size_t elemSize, std::size_t elemCount)
{
	return m_pFilePtr ? std::fread(buffer, elemSize, elemCount, m_pFilePtr) : 0ull;
}

int32_t CNativeInputFile::ungetc(int32_t character)
{
	return m_pFilePtr ? std::ungetc(character, m_pFilePtr) : -1;
}

long CNativeInputFile::tell()
{
	return m_pFilePtr ? std::ftell(m_pFilePtr) : -1;
}

int32_t CNativeInputFile::seek(long offset, int32_t origin)
{
	return m_pFilePtr ? std::fseek(m_pFilePtr, offset, origin) : -1;
}

int32_t CNativeInputFile::eof() const
{
	return m_pFilePtr ? std::feof(m_pFilePtr) : 1;
}