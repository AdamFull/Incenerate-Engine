#include "ZipInputFile.h"

using namespace engine::filesystem;

lzip_file_t* lzip_fopen(zip_t* handle, const char* filename, const char* password)
{
	lzip_file_t* zfile = (lzip_file_t*)malloc(sizeof(lzip_file_t));
	zfile->file = password ? zip_fopen(handle, filename, 0) : zip_fopen_encrypted(handle, filename, 0, password);
	zfile->buffer = '\0';
	zfile->buffer_filled = false;
	zfile->end_of_file = false;

	return zfile;
}

void lzip_fclose(lzip_file_t* zfile)
{
	if (zfile->file)
		zip_fclose(zfile->file);
	
	free(zfile);
}

int lzip_fgetc(lzip_file_t* zfile) 
{
	if (zfile->buffer_filled) 
	{
		zfile->buffer_filled = false;
		return zfile->buffer;
	}

	char c;
	if (zip_fread(zfile->file, &c, 1) == 1)
		return c;

	zfile->end_of_file = true;
	return EOF;
}

int32_t lzip_ungetc(int32_t c, lzip_file_t* zfile)
{
	if (zfile->buffer_filled) 
		return EOF;

	zfile->buffer = c;
	zfile->buffer_filled = true;
	zfile->end_of_file = false;
	return c;
}

size_t lzip_fread(void* buffer, size_t elemSize, size_t elemCount, lzip_file_t* zfile)
{
	size_t readed = zip_fread(zfile->file, buffer, elemSize * elemCount);
	if (readed == 0)
		zfile->end_of_file = true;

	return readed;
}

int32_t lzip_fseek(lzip_file_t* zfile, long offset, int32_t origin)
{
	if (origin == SEEK_END && offset == 0)
		zfile->end_of_file = true;
	
	return zip_fseek(zfile->file, offset, origin);
}

long lzip_ftell(lzip_file_t* zfile)
{
	return zip_ftell(zfile->file);
}

int32_t lzip_feof(lzip_file_t* zfile)
{
	return zfile->file ? zfile->end_of_file : 1;
}



CZipInputFile::CZipInputFile(zip_t* handle, const std::string& filename, const std::string& password) :
	m_srPassword{ password }
{

	if (!handle)
		return;

	m_pFilePtr = lzip_fopen(handle, filename.c_str(), m_srPassword.empty() ? NULL : m_srPassword.c_str());

	if (!m_pFilePtr)
		throw std::runtime_error("Failed to open zip file.");
}

CZipInputFile::~CZipInputFile()
{
	close();
}

void CZipInputFile::close()
{
	if (m_pFilePtr)
	{
		lzip_fclose(m_pFilePtr);
		m_pFilePtr = nullptr;
	}
}

bool CZipInputFile::is_open() const
{
	return m_pFilePtr && m_pFilePtr->file;
}

int32_t CZipInputFile::getc()
{
	return m_pFilePtr ? lzip_fgetc(m_pFilePtr) : -1;
}

std::size_t CZipInputFile::read(void* buffer, std::size_t elemSize, std::size_t elemCount)
{
	return m_pFilePtr ? lzip_fread(buffer, elemSize, elemCount, m_pFilePtr) : 0ull;
}

int32_t CZipInputFile::ungetc(int32_t character)
{
	return m_pFilePtr ? lzip_ungetc(character, m_pFilePtr) : -1;
}

long CZipInputFile::tell()
{
	return m_pFilePtr ? lzip_ftell(m_pFilePtr) : -1;
}

int32_t CZipInputFile::seek(long offset, int32_t origin)
{
	return m_pFilePtr ? lzip_fseek(m_pFilePtr, offset, origin) : -1;
}

int32_t CZipInputFile::eof() const
{
	return m_pFilePtr ? lzip_feof(m_pFilePtr) : 1;
}