#include "VFSInputFile.h"

using namespace engine::filesystem;

CVFSInFile::CVFSInFile(std::unique_ptr<IInputFileInterface>&& pFile) :
	m_pFileInterface(std::move(pFile))
{
	if (!m_pFileInterface)
		throw std::runtime_error("Failed to open file");
}

CVFSInFile::~CVFSInFile()
{
	if (m_pFileInterface)
		m_pFileInterface->close();
}

bool CVFSInFile::is_open()
{
	if (m_pFileInterface)
		return m_pFileInterface->is_open();

	return false;
}

int CVFSInFile::get()
{
	m_gcount = 0ull;

	int ch = m_pFileInterface ? m_pFileInterface->getc() : EOF;
	if (ch != EOF) 
		m_gcount = 1ull;

	return ch;
}

std::string CVFSInFile::getline()
{
	m_gcount = 0ull;

	if (!m_pFileInterface)
		return {};

	std::string line;

	int ch;
	while ((ch = m_pFileInterface->getc()) != EOF && ch != '\n')
	{
		line.push_back(static_cast<char>(ch));
		++m_gcount;
	}

	return line;
}

std::size_t CVFSInFile::gcount() const
{
	return m_gcount;
}

CVFSInFile& CVFSInFile::ignore(std::size_t count, int delimiter)
{
	if (m_pFileInterface)
	{
		for (std::size_t i = 0; i < count; ++i) 
		{
			int ch = m_pFileInterface->getc();
			if (ch == EOF || ch == delimiter)
				break;
		}
	}

	return *this;
}

int CVFSInFile::peek() const
{
	return m_pFileInterface ? m_pFileInterface->getc() : EOF;
}

CVFSInFile& CVFSInFile::read(char* buffer, std::size_t count)
{
	m_gcount = 0ull;

	if (m_pFileInterface)
		m_gcount = m_pFileInterface->read(buffer, sizeof(char), count);

	return *this;
}

CVFSInFile& CVFSInFile::unget()
{
	if (m_pFileInterface)
		m_pFileInterface->ungetc(peek());

	return *this;
}

std::streampos CVFSInFile::tellg()
{
	return m_pFileInterface ? m_pFileInterface->tell() : -1;
}

CVFSInFile& CVFSInFile::seekg(std::streampos pos)
{
	if(m_pFileInterface)
		m_pFileInterface->seek(pos, SEEK_SET);

	return *this;
}

bool CVFSInFile::eof() const
{
	if (m_pFileInterface)
		return m_pFileInterface->eof();

	return true;
}