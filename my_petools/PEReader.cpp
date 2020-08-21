#include "StdAfx.h"
#include "head.h"

extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;
extern DWORD file_size;
extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor_origin;

DWORD ReadPEFile(IN LPCSTR file_in, OUT LPVOID& pFileBuffer,
				 PIMAGE_DOS_HEADER& pDosHeader, PIMAGE_NT_HEADERS32& pNTHeader,
				 PIMAGE_SECTION_HEADER& pSectionHeader)
{
	FILE* fp;
	fp = fopen(file_in, "rb");
	if (!fp)
		EXIT_ERROR("fp == NULL!");
	DWORD file_size = 0;
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	LPVOID t = malloc(file_size);
	if ((fread(t, file_size, 1, fp) != 1) || t == NULL)
		EXIT_ERROR("fread error or malloc error!");
	
	pFileBuffer = t;
	MY_ASSERT(pFileBuffer);
	
	pDosHeader = (PIMAGE_DOS_HEADER)(pFileBuffer);
	MY_ASSERT(pDosHeader);
	MY_ASSERT((pDosHeader->e_magic == IMAGE_DOS_SIGNATURE));
	
	pNTHeader =
		(PIMAGE_NT_HEADERS32)((PBYTE)pFileBuffer + pDosHeader->e_lfanew);
	if (pNTHeader->FileHeader.SizeOfOptionalHeader != 0xe0)
		EXIT_ERROR("this is not a 32-bit executable file.");
	
	pSectionHeader = (PIMAGE_SECTION_HEADER)(
		(PBYTE)pNTHeader + sizeof(IMAGE_NT_SIGNATURE) +
		sizeof(IMAGE_FILE_HEADER) + pNTHeader->FileHeader.SizeOfOptionalHeader);
	
	return file_size;
}

DWORD RVA_TO_FOA(LPVOID pFileBuffer, PIMAGE_DOS_HEADER pDosHeader,
				 PIMAGE_NT_HEADERS32 pNTHeader,
				 PIMAGE_SECTION_HEADER pSectionHeader, IN DWORD RVA)
{
	if (RVA < pNTHeader->OptionalHeader.SizeOfHeaders)
		return RVA;
	
	for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if (RVA >= pSectionHeader[i].VirtualAddress &&
			RVA < pSectionHeader[i].VirtualAddress +
			pSectionHeader[i].Misc.VirtualSize)
		{
			return (RVA - pSectionHeader[i].VirtualAddress +
				pSectionHeader[i].PointerToRawData);
		}
	}
	
	EXIT_ERROR("rva to foa failure!");
}

DWORD FOA_TO_RVA(LPVOID pFileBuffer, PIMAGE_DOS_HEADER pDosHeader,
				 PIMAGE_NT_HEADERS32 pNTHeader,
				 PIMAGE_SECTION_HEADER pSectionHeader, IN DWORD FOA)
{
	if (FOA < pNTHeader->OptionalHeader.SizeOfHeaders)
		return FOA;
	
	for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if (FOA >= pSectionHeader[i].PointerToRawData &&
			FOA < pSectionHeader[i].PointerToRawData +
			pSectionHeader[i].Misc.VirtualSize)
		{
			return (FOA - pSectionHeader[i].PointerToRawData +
				pSectionHeader[i].VirtualAddress);
		}
	}
	
	EXIT_ERROR("foa to rva error!");
}