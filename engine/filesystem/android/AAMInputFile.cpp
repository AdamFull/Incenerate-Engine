#include "AAMInputFile.h"

using namespace engine::filesystem;

aasset_file_t* aasset_fopen(AAssetManager* handle, const char* filename)
{
	aasset_file_t* asset_ptr = (aasset_file_t*)malloc(sizeof(aasset_file_t));
	asset_ptr->asset = AAssetManager_open(handle, filename, AASSET_MODE_STREAMING);

	if (!asset_ptr->asset)
	{
		free(asset_ptr);
		return NULL;
	}

	asset_ptr->buffer = '\0';
	asset_ptr->buffer_filled = false;
	asset_ptr->end_of_file = false;
	asset_ptr->position = 0l;

	return asset_ptr;
}

void aasset_fclose(aasset_file_t* asset_ptr)
{
	if (asset_ptr->asset)
		AAsset_close(asset_ptr->asset);

	free(asset_ptr);
}

int32_t aasset_fgetc(aasset_file_t* asset_ptr)
{
	if (asset_ptr->buffer_filled)
	{
		asset_ptr->buffer_filled = false;
		asset_ptr->position++;
		return (int32_t)asset_ptr->buffer;
	}

	char c;
	size_t bytesRead = AAsset_read(asset_ptr->asset, &c, 1ull);
	if (bytesRead == 1) 
	{
		asset_ptr->position++;
		return (int32_t)c;
	}

	asset_ptr->end_of_file = true;
	return EOF;
}

int32_t aasset_ungetc(int32_t character, aasset_file_t* asset_ptr)
{
	if (asset_ptr->buffer_filled)
		return EOF;

	asset_ptr->buffer = character;
	asset_ptr->buffer_filled = true;
	asset_ptr->end_of_file = false;
	asset_ptr->position--;
	return character;
}

size_t aasset_fread(void* buffer, size_t elemSize, size_t elemCount, aasset_file_t* asset_ptr)
{
	size_t readed = AAsset_read(asset_ptr->asset, buffer, elemSize * elemCount);
	if(readed == 0)
		asset_ptr->end_of_file = true;

	asset_ptr->position += readed;
	return readed;
}

int32_t aasset_fseek(aasset_file_t* asset_ptr, long offset, int32_t origin)
{
	if (origin == SEEK_END)
	{
		asset_ptr->position = offset + AAsset_getLength(asset_ptr->asset);
		if(offset == 0l)
			asset_ptr->end_of_file = true;
	}
	else if (origin == SEEK_CUR)
		asset_ptr->position = asset_ptr->position + offset;
	else
		asset_ptr->position = offset;

	return AAsset_seek(asset_ptr->asset, offset, origin);
}

long aasset_ftell(aasset_file_t* asset_ptr)
{
	return asset_ptr->position;
}

int32_t aasset_feof(aasset_file_t* asset_ptr)
{
	return asset_ptr->end_of_file ? 1 : 0;
}


CAAMInputFile::CAAMInputFile(AAssetManager* handle, const std::string& filename)
{
	m_pFilePtr = aasset_fopen(handle, filename.c_str());

	if (!m_pFilePtr)
		throw std::runtime_error("Failed to open aasset file.");
}

CAAMInputFile::~CAAMInputFile()
{
	close();
}

void CAAMInputFile::close()
{
	if (m_pFilePtr)
		aasset_fclose(m_pFilePtr);
}

bool CAAMInputFile::is_open() const
{
	return m_pFilePtr && m_pFilePtr->asset;
}

int32_t CAAMInputFile::getc()
{
	return m_pFilePtr ? aasset_fgetc(m_pFilePtr) : -1;
}

std::size_t CAAMInputFile::read(char* buffer, std::size_t elemSize, std::size_t elemCount)
{
	return m_pFilePtr ? aasset_fread(buffer, elemSize, elemCount, m_pFilePtr) : 0ull;
}

int32_t CAAMInputFile::ungetc(int32_t character)
{
	return m_pFilePtr ? aasset_ungetc(character, m_pFilePtr) : -1;
}

long CAAMInputFile::tell()
{
	return m_pFilePtr ? aasset_ftell(m_pFilePtr) : -1;
}

int32_t CAAMInputFile::seek(long offset, int32_t origin)
{
	return m_pFilePtr ? aasset_fseek(m_pFilePtr, offset, origin) : -1;
}

int32_t CAAMInputFile::eof() const
{
	return m_pFilePtr ? aasset_feof(m_pFilePtr) : 1;
}