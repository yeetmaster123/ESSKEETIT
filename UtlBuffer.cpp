//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// $Header: $
// $NoKeywords: $
//
// Serialization buffer
//===========================================================================//

#pragma warning (disable : 4514)

#include "UtlBuffer.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "characterset.hpp"
#include <string>

const char* V_strnchr(const char* pStr, char c, int n)
{
    char const* pLetter = pStr;
    char const* pLast = pStr + n;

    // Check the entire string
    while((pLetter < pLast) && (*pLetter != 0)) {
        if(*pLetter == c)
            return pLetter;
        ++pLetter;
    }
    return NULL;
}
//-----------------------------------------------------------------------------
// Finds a string in another string with a case insensitive test w/ length validation
//-----------------------------------------------------------------------------
char const* V_strnistr(char const* pStr, char const* pSearch, int n)
{
    if(!pStr || !pSearch)
        return 0;

    char const* pLetter = pStr;

    // Check the entire string
    while(*pLetter != 0) {
        if(n <= 0)
            return 0;

        // Skip over non-matches
        if(tolower(*pLetter) == tolower(*pSearch)) {
            int n1 = n - 1;

            // Check for match
            char const* pMatch = pLetter + 1;
            char const* pTest = pSearch + 1;
            while(*pTest != 0) {
                if(n1 <= 0)
                    return 0;

                // We've run off the end; don't bother.
                if(*pMatch == 0)
                    return 0;

                if(tolower(*pMatch) != tolower(*pTest))
                    break;

                ++pMatch;
                ++pTest;
                --n1;
            }

            // Found a match!
            if(*pTest == 0)
                return pLetter;
        }

        ++pLetter;
        --n;
    }

    return 0;
}
//-----------------------------------------------------------------------------
// Character conversions for C strings
//-----------------------------------------------------------------------------
class CUtlCStringConversion : public CUtlCharConversion
{
public:
    CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray);

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength);

private:
    char m_pConversion[255];
};


//-----------------------------------------------------------------------------
// Character conversions for no-escape sequence strings
//-----------------------------------------------------------------------------
class CUtlNoEscConversion : public CUtlCharConversion
{
public:
    CUtlNoEscConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
        CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
    {
    }

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength) { *pLength = 0; return 0; }
};


//-----------------------------------------------------------------------------
// List of character conversions
//-----------------------------------------------------------------------------
BEGIN_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\')
{
    '\n', "n"
},
{ '\t', "t" },
{ '\v', "v" },
{ '\b', "b" },
{ '\r', "r" },
{ '\f', "f" },
{ '\a', "a" },
{ '\\', "\\" },
{ '\?', "\?" },
{ '\'', "\'" },
{ '\"', "\"" },
END_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\');

    CUtlCharConversion *GetCStringCharConversion()
    {
        return &s_StringCharConversion;
    }

    BEGIN_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F)
    {
        0x7F, ""
    },
        END_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F);

        CUtlCharConversion *GetNoEscCharConversion()
        {
            return &s_NoEscConversion;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCStringConversion::CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
            CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
        {
            memset(m_pConversion, 0x0, sizeof(m_pConversion));
            for(int i = 0; i < nCount; ++i) {
                m_pConversion[pArray[i].m_pReplacementString[0]] = pArray[i].m_nActualChar;
            }
        }

        // Finds a conversion for the passed-in string, returns length
        char CUtlCStringConversion::FindConversion(const char *pString, int *pLength)
        {
            char c = m_pConversion[pString[0]];
            *pLength = (c != '\0') ? 1 : 0;
            return c;
        }



        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCharConversion::CUtlCharConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray)
        {
            m_nEscapeChar = nEscapeChar;
            m_pDelimiter = pDelimiter;
            m_nCount = nCount;
            m_nDelimiterLength = strlen(pDelimiter);
            m_nMaxConversionLength = 0;

            memset(m_pReplacements, 0, sizeof(m_pReplacements));

            for(int i = 0; i < nCount; ++i) {
                m_pList[i] = pArray[i].m_nActualChar;
                ConversionInfo_t &info = m_pReplacements[m_pList[i]];
                assert(info.m_pReplacementString == 0);
                info.m_pReplacementString = pArray[i].m_pReplacementString;
                info.m_nLength = strlen(info.m_pReplacementString);
                if(info.m_nLength > m_nMaxConversionLength) {
                    m_nMaxConversionLength = info.m_nLength;
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Escape character + delimiter
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::GetEscapeChar() const
        {
            return m_nEscapeChar;
        }

        const char *CUtlCharConversion::GetDelimiter() const
        {
            return m_pDelimiter;
        }

        int CUtlCharConversion::GetDelimiterLength() const
        {
            return m_nDelimiterLength;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        const char *CUtlCharConversion::GetConversionString(char c) const
        {
            return m_pReplacements[c].m_pReplacementString;
        }

        int CUtlCharConversion::GetConversionLength(char c) const
        {
            return m_pReplacements[c].m_nLength;
        }

        int CUtlCharConversion::MaxConversionLength() const
        {
            return m_nMaxConversionLength;
        }


        //-----------------------------------------------------------------------------
        // Finds a conversion for the passed-in string, returns length
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::FindConversion(const char *pString, int *pLength)
        {
            for(int i = 0; i < m_nCount; ++i) {
                if(!strcmp(pString, m_pReplacements[m_pList[i]].m_pReplacementString)) {
                    *pLength = m_pReplacements[m_pList[i]].m_nLength;
                    return m_pList[i];
                }
            }

            *pLength = 0;
            return '\0';
        }


        //-----------------------------------------------------------------------------
        // constructors
        //-----------------------------------------------------------------------------
        CUtlBuffer::CUtlBuffer(int growSize, int initSize, int nFlags) :
            m_Memory(growSize, initSize), m_Error(0)
        {
            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if((initSize != 0) && !IsReadOnly()) {
                m_nMaxPut = -1;
                AddNullTermination();
            } else {
                m_nMaxPut = 0;
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }

        CUtlBuffer::CUtlBuffer(const void *pBuffer, int nSize, int nFlags) :
            m_Memory((unsigned char*)pBuffer, nSize), m_Error(0)
        {
            assert(nSize != 0);

            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if(IsReadOnly()) {
                m_nMaxPut = nSize;
            } else {
                m_nMaxPut = -1;
                AddNullTermination();
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }


        //-----------------------------------------------------------------------------
        // Modifies the buffer to be binary or text; Blows away the buffer and the CONTAINS_CRLF value. 
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetBufferType(bool bIsText, bool bContainsCRLF)
        {
#ifdef _DEBUG
            // If the buffer is empty, there is no opportunity for this stuff to fail
            if(TellMaxPut() != 0) {
                if(IsText()) {
                    if(bIsText) {
                        assert(ContainsCRLF() == bContainsCRLF);
                    } else {
                        assert(ContainsCRLF());
                    }
                } else {
                    if(bIsText) {
                        assert(bContainsCRLF);
                    }
                }
            }
#endif

            if(bIsText) {
                m_Flags |= TEXT_BUFFER;
            } else {
                m_Flags &= ~TEXT_BUFFER;
            }
            if(bContainsCRLF) {
                m_Flags |= CONTAINS_CRLF;
            } else {
                m_Flags &= ~CONTAINS_CRLF;
            }
        }


        //-----------------------------------------------------------------------------
        // Attaches the buffer to external memory....
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.SetExternalBuffer((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Assumes an external buffer but manages its deletion
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AssumeMemory(void *pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.AssumeMemory((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Makes sure we've got at least this much memory
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EnsureCapacity(int num)
        {
            // Add one extra for the null termination
            num += 1;
            if(m_Memory.IsExternallyAllocated()) {
                if(IsGrowable() && (m_Memory.NumAllocated() < num)) {
                    m_Memory.ConvertToGrowableMemory(0);
                } else {
                    num -= 1;
                }
            }

            m_Memory.EnsureCapacity(num);
        }


        //-----------------------------------------------------------------------------
        // Base Get method from which all others derive
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Get(void* pMem, int size)
        {
            if(CheckGet(size)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], size);
                m_Get += size;
            }
        }


        //-----------------------------------------------------------------------------
        // This will Get at least 1 uint8_t and up to nSize bytes. 
        // It will return the number of bytes actually read.
        //-----------------------------------------------------------------------------
        int CUtlBuffer::GetUpTo(void *pMem, int nSize)
        {
            if(CheckArbitraryPeekGet(0, nSize)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], nSize);
                m_Get += nSize;
                return nSize;
            }
            return 0;
        }


        //-----------------------------------------------------------------------------
        // Eats whitespace
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EatWhiteSpace()
        {
            if(IsText() && IsValid()) {
                while(CheckGet(sizeof(char))) {
                    if(!isspace(*(const unsigned char*)PeekGet()))
                        break;
                    m_Get += sizeof(char);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Eats C++ style comments
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::EatCPPComment()
        {
            if(IsText() && IsValid()) {
                // If we don't have a a c++ style comment next, we're done
                const char *pPeek = (const char *)PeekGet(2 * sizeof(char), 0);
                if(!pPeek || (pPeek[0] != '/') || (pPeek[1] != '/'))
                    return false;

                // Deal with c++ style comments
                m_Get += 2;

                // read complete line
                for(char c = GetChar(); IsValid(); c = GetChar()) {
                    if(c == '\n')
                        break;
                }
                return true;
            }
            return false;
        }


        //-----------------------------------------------------------------------------
        // Peeks how much whitespace to eat
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekWhiteSpace(int nOffset)
        {
            if(!IsText() || !IsValid())
                return 0;

            while(CheckPeekGet(nOffset, sizeof(char))) {
                if(!isspace(*(unsigned char*)PeekGet(nOffset)))
                    break;
                nOffset += sizeof(char);
            }

            return nOffset;
        }


        //-----------------------------------------------------------------------------
        // Peek size of sting to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekStringLength()
        {
            if(!IsValid())
                return 0;

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                if(!IsText()) {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(pTest[i] == 0)
                            return (i + nOffset - nStartingOffset + 1);
                    }
                } else {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(isspace((unsigned char)pTest[i]) || (pTest[i] == 0))
                            return (i + nOffset - nStartingOffset + 1);
                    }
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Peek size of line to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekLineLength()
        {
            if(!IsValid())
                return 0;

            int nOffset = 0;
            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                for(int i = 0; i < nPeekAmount; ++i) {
                    // The +2 here is so we eat the terminating '\n' and 0
                    if(pTest[i] == '\n' || pTest[i] == '\r')
                        return (i + nOffset - nStartingOffset + 2);
                    // The +1 here is so we eat the terminating 0
                    if(pTest[i] == 0)
                        return (i + nOffset - nStartingOffset + 1);
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Does the next bytes of the buffer match a pattern?
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PeekStringMatch(int nOffset, const char *pString, int nLen)
        {
            if(!CheckPeekGet(nOffset, nLen))
                return false;
            return !strncmp((const char*)PeekGet(nOffset), pString, nLen);
        }


        //-----------------------------------------------------------------------------
        // This version of PeekStringLength converts \" to \\ and " to \, etc.
        // It also reads a " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekDelimitedStringLength(CUtlCharConversion *pConv, bool bActualSize)
        {
            if(!IsText() || !pConv)
                return PeekStringLength();

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            if(!PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return 0;

            // Try to read ending ", but don't accept \"
            int nActualStart = nOffset;
            nOffset += pConv->GetDelimiterLength();
            int nLen = 1;	// Starts at 1 for the '\0' termination

            do {
                if(PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                    break;

                if(!CheckPeekGet(nOffset, 1))
                    break;

                char c = *(const char*)PeekGet(nOffset);
                ++nLen;
                ++nOffset;
                if(c == pConv->GetEscapeChar()) {
                    int nLength = pConv->MaxConversionLength();
                    if(!CheckArbitraryPeekGet(nOffset, nLength))
                        break;

                    pConv->FindConversion((const char*)PeekGet(nOffset), &nLength);
                    nOffset += nLength;
                }
            } while(true);

            return bActualSize ? nLen : nOffset - nActualStart + pConv->GetDelimiterLength() + 1;
        }


        //-----------------------------------------------------------------------------
        // Reads a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetString(char* pString, int nMaxChars)
        {
            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekStringLength();

            if(IsText()) {
                EatWhiteSpace();
            }

            if(nLen == 0) {
                *pString = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pString, nLen - 1);
                pString[nLen - 1] = 0;
            } else {
                Get(pString, nMaxChars - 1);
                pString[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }

            // Read the terminating NULL in binary formats
            if(!IsText()) {
                assert(GetChar() == 0);
            }
        }


        //-----------------------------------------------------------------------------
        // Reads up to and including the first \n
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetLine(char* pLine, int nMaxChars)
        {
            assert(IsText() && !ContainsCRLF());

            if(!IsValid()) {
                *pLine = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekLineLength();
            if(nLen == 0) {
                *pLine = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pLine, nLen - 1);
                pLine[nLen - 1] = 0;
            } else {
                Get(pLine, nMaxChars - 1);
                pLine[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }
        }


        //-----------------------------------------------------------------------------
        // This version of GetString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        char CUtlBuffer::GetDelimitedCharInternal(CUtlCharConversion *pConv)
        {
            char c = GetChar();
            if(c == pConv->GetEscapeChar()) {
                int nLength = pConv->MaxConversionLength();
                if(!CheckArbitraryPeekGet(0, nLength))
                    return '\0';

                c = pConv->FindConversion((const char *)PeekGet(), &nLength);
                SeekGet(SEEK_CURRENT, nLength);
            }

            return c;
        }

        char CUtlBuffer::GetDelimitedChar(CUtlCharConversion *pConv)
        {
            if(!IsText() || !pConv)
                return GetChar();
            return GetDelimitedCharInternal(pConv);
        }

        void CUtlBuffer::GetDelimitedString(CUtlCharConversion *pConv, char *pString, int nMaxChars)
        {
            if(!IsText() || !pConv) {
                GetString(pString, nMaxChars);
                return;
            }

            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            EatWhiteSpace();
            if(!PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return;

            // Pull off the starting delimiter
            SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());

            int nRead = 0;
            while(IsValid()) {
                if(PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength())) {
                    SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());
                    break;
                }

                char c = GetDelimitedCharInternal(pConv);

                if(nRead < nMaxChars) {
                    pString[nRead] = c;
                    ++nRead;
                }
            }

            if(nRead >= nMaxChars) {
                nRead = nMaxChars - 1;
            }
            pString[nRead] = '\0';
        }


        //-----------------------------------------------------------------------------
        // Checks if a Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckGet(int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            if(TellMaxPut() < m_Get + nSize) {
                m_Error |= GET_OVERFLOW;
                return false;
            }

            if((m_Get < m_nOffset) || (m_Memory.NumAllocated() < m_Get - m_nOffset + nSize)) {
                if(!OnGetOverflow(nSize)) {
                    m_Error |= GET_OVERFLOW;
                    return false;
                }
            }

            return true;
        }


        //-----------------------------------------------------------------------------
        // Checks if a peek Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPeekGet(int nOffset, int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            // Checking for peek can't Set the overflow flag
            bool bOk = CheckGet(nOffset + nSize);
            m_Error &= ~GET_OVERFLOW;
            return bOk;
        }


        //-----------------------------------------------------------------------------
        // Call this to peek arbitrarily long into memory. It doesn't fail unless
        // it can't read *anything* new
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckArbitraryPeekGet(int nOffset, int &nIncrement)
        {
            if(TellGet() + nOffset >= TellMaxPut()) {
                nIncrement = 0;
                return false;
            }

            if(TellGet() + nOffset + nIncrement > TellMaxPut()) {
                nIncrement = TellMaxPut() - TellGet() - nOffset;
            }

            // NOTE: CheckPeekGet could modify TellMaxPut for streaming files
            // We have to call TellMaxPut again here
            CheckPeekGet(nOffset, nIncrement);
            int nMaxGet = TellMaxPut() - TellGet();
            if(nMaxGet < nIncrement) {
                nIncrement = nMaxGet;
            }
            return (nIncrement != 0);
        }


        //-----------------------------------------------------------------------------
        // Peek part of the butt
        //-----------------------------------------------------------------------------
        const void* CUtlBuffer::PeekGet(int nMaxSize, int nOffset)
        {
            if(!CheckPeekGet(nOffset, nMaxSize))
                return NULL;
            return &m_Memory[m_Get + nOffset - m_nOffset];
        }


        //-----------------------------------------------------------------------------
        // Change where I'm reading
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SeekGet(SeekType_t type, int offset)
        {
            switch(type) {
                case SEEK_HEAD:
                    m_Get = offset;
                    break;

                case SEEK_CURRENT:
                    m_Get += offset;
                    break;

                case SEEK_TAIL:
                    m_Get = m_nMaxPut - offset;
                    break;
            }

            if(m_Get > m_nMaxPut) {
                m_Error |= GET_OVERFLOW;
            } else {
                m_Error &= ~GET_OVERFLOW;
                if(m_Get < m_nOffset || m_Get >= m_nOffset + Size()) {
                    OnGetOverflow(-1);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Parse...
        //-----------------------------------------------------------------------------

#pragma warning ( disable : 4706 )

        int CUtlBuffer::VaScanf(const char* pFmt, va_list list)
        {
            assert(pFmt);
            if(m_Error || !IsText())
                return 0;

            int numScanned = 0;
            int nLength;
            char c;
            char* pEnd;
            while(c = *pFmt++) {
                // Stop if we hit the end of the buffer
                if(m_Get >= TellMaxPut()) {
                    m_Error |= GET_OVERFLOW;
                    break;
                }

                switch(c) {
                    case ' ':
                        // eat all whitespace
                        EatWhiteSpace();
                        break;

                    case '%':
                    {
                        // Conversion character... try to convert baby!
                        char type = *pFmt++;
                        if(type == 0)
                            return numScanned;

                        switch(type) {
                            case 'c':
                            {
                                char* ch = va_arg(list, char *);
                                if(CheckPeekGet(0, sizeof(char))) {
                                    *ch = *(const char*)PeekGet();
                                    ++m_Get;
                                } else {
                                    *ch = 0;
                                    return numScanned;
                                }
                            }
                            break;

                            case 'i':
                            case 'd':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'x':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 16);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'u':
                            {
                                unsigned int* u = va_arg(list, unsigned int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *u = 0;
                                    return numScanned;
                                }

                                *u = strtoul((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'f':
                            {
                                float* f = va_arg(list, float *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *f = 0.0f;
                                    return numScanned;
                                }

                                *f = (float)strtod((char*)PeekGet(), &pEnd);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 's':
                            {
                                char* s = va_arg(list, char *);
                                GetString(s);
                            }
                            break;

                            default:
                            {
                                // unimplemented scanf type
                                assert(0);
                                return numScanned;
                            }
                            break;
                        }

                        ++numScanned;
                    }
                    break;

                    default:
                    {
                        // Here we have to match the format string character
                        // against what's in the buffer or we're done.
                        if(!CheckPeekGet(0, sizeof(char)))
                            return numScanned;

                        if(c != *(const char*)PeekGet())
                            return numScanned;

                        ++m_Get;
                    }
                }
            }
            return numScanned;
        }

#pragma warning ( default : 4706 )

        int CUtlBuffer::Scanf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            int count = VaScanf(pFmt, args);
            va_end(args);

            return count;
        }


        //-----------------------------------------------------------------------------
        // Advance the Get index until after the particular string is found
        // Do not eat whitespace before starting. Return false if it failed
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::GetToken(const char *pToken)
        {
            assert(pToken);

            // Look for the token
            int nLen = strlen(pToken);

            int nSizeToCheck = Size() - TellGet() - m_nOffset;

            int nGet = TellGet();
            do {
                int nMaxSize = TellMaxPut() - TellGet();
                if(nMaxSize < nSizeToCheck) {
                    nSizeToCheck = nMaxSize;
                }
                if(nLen > nSizeToCheck)
                    break;

                if(!CheckPeekGet(0, nSizeToCheck))
                    break;

                const char *pBufStart = (const char*)PeekGet();
                const char *pFoundEnd = V_strnistr(pBufStart, pToken, nSizeToCheck);
                if(pFoundEnd) {
                    size_t nOffset = (size_t)pFoundEnd - (size_t)pBufStart;
                    SeekGet(CUtlBuffer::SEEK_CURRENT, nOffset + nLen);
                    return true;
                }

                SeekGet(CUtlBuffer::SEEK_CURRENT, nSizeToCheck - nLen - 1);
                nSizeToCheck = Size() - (nLen - 1);

            } while(true);

            SeekGet(CUtlBuffer::SEEK_HEAD, nGet);
            return false;
        }


        //-----------------------------------------------------------------------------
        // (For text buffers only)
        // Parse a token from the buffer:
        // Grab all text that lies between a starting delimiter + ending delimiter
        // (skipping whitespace that leads + trails both delimiters).
        // Note the delimiter checks are case-insensitive.
        // If successful, the Get index is advanced and the function returns true,
        // otherwise the index is not advanced and the function returns false.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ParseToken(const char *pStartingDelim, const char *pEndingDelim, char* pString, int nMaxLen)
        {
            int nCharsToCopy = 0;
            int nCurrentGet = 0;

            size_t nEndingDelimLen;

            // Starting delimiter is optional
            char emptyBuf = '\0';
            if(!pStartingDelim) {
                pStartingDelim = &emptyBuf;
            }

            // Ending delimiter is not
            assert(pEndingDelim && pEndingDelim[0]);
            nEndingDelimLen = strlen(pEndingDelim);

            int nStartGet = TellGet();
            char nCurrChar;
            int nTokenStart = -1;
            EatWhiteSpace();
            while(*pStartingDelim) {
                nCurrChar = *pStartingDelim++;
                if(!isspace((unsigned char)nCurrChar)) {
                    if(tolower(GetChar()) != tolower(nCurrChar))
                        goto parseFailed;
                } else {
                    EatWhiteSpace();
                }
            }

            EatWhiteSpace();
            nTokenStart = TellGet();
            if(!GetToken(pEndingDelim))
                goto parseFailed;

            nCurrentGet = TellGet();
            nCharsToCopy = (nCurrentGet - nEndingDelimLen) - nTokenStart;
            if(nCharsToCopy >= nMaxLen) {
                nCharsToCopy = nMaxLen - 1;
            }

            if(nCharsToCopy > 0) {
                SeekGet(CUtlBuffer::SEEK_HEAD, nTokenStart);
                Get(pString, nCharsToCopy);
                if(!IsValid())
                    goto parseFailed;

                // Eat trailing whitespace
                for(; nCharsToCopy > 0; --nCharsToCopy) {
                    if(!isspace((unsigned char)pString[nCharsToCopy - 1]))
                        break;
                }
            }
            pString[nCharsToCopy] = '\0';

            // Advance the Get index
            SeekGet(CUtlBuffer::SEEK_HEAD, nCurrentGet);
            return true;

        parseFailed:
            // Revert the Get index
            SeekGet(SEEK_HEAD, nStartGet);
            pString[0] = '\0';
            return false;
        }


        //-----------------------------------------------------------------------------
        // Parses the next token, given a Set of character breaks to stop at
        //-----------------------------------------------------------------------------
        int CUtlBuffer::ParseToken(characterset_t *pBreaks, char *pTokenBuf, int nMaxLen, bool bParseComments)
        {
            assert(nMaxLen > 0);
            pTokenBuf[0] = 0;

            // skip whitespace + comments
            while(true) {
                if(!IsValid())
                    return -1;
                EatWhiteSpace();
                if(bParseComments) {
                    if(!EatCPPComment())
                        break;
                } else {
                    break;
                }
            }

            char c = GetChar();

            // End of buffer
            if(c == 0)
                return -1;

            // handle quoted strings specially
            if(c == '\"') {
                int nLen = 0;
                while(IsValid()) {
                    c = GetChar();
                    if(c == '\"' || !c) {
                        pTokenBuf[nLen] = 0;
                        return nLen;
                    }
                    pTokenBuf[nLen] = c;
                    if(++nLen == nMaxLen) {
                        pTokenBuf[nLen - 1] = 0;
                        return nMaxLen;
                    }
                }

                // In this case, we hit the end of the buffer before hitting the end qoute
                pTokenBuf[nLen] = 0;
                return nLen;
            }

            // parse single characters
            if(IN_CHARACTERSET(*pBreaks, c)) {
                pTokenBuf[0] = c;
                pTokenBuf[1] = 0;
                return 1;
            }

            // parse a regular word
            int nLen = 0;
            while(true) {
                pTokenBuf[nLen] = c;
                if(++nLen == nMaxLen) {
                    pTokenBuf[nLen - 1] = 0;
                    return nMaxLen;
                }
                c = GetChar();
                if(!IsValid())
                    break;

                if(IN_CHARACTERSET(*pBreaks, c) || c == '\"' || c <= ' ') {
                    SeekGet(SEEK_CURRENT, -1);
                    break;
                }
            }

            pTokenBuf[nLen] = 0;
            return nLen;
        }



        //-----------------------------------------------------------------------------
        // Serialization
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Put(const void *pMem, int size)
        {
            if(size && CheckPut(size)) {
                memcpy(&m_Memory[m_Put - m_nOffset], pMem, size);
                m_Put += size;

                AddNullTermination();
            }
        }


        //-----------------------------------------------------------------------------
        // Writes a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::PutString(const char* pString)
        {
            if(!IsText()) {
                if(pString) {
                    // Not text? append a null at the end.
                    size_t nLen = strlen(pString) + 1;
                    Put(pString, nLen * sizeof(char));
                    return;
                } else {
                    PutTypeBin<char>(0);
                }
            } else if(pString) {
                int nTabCount = (m_Flags & AUTO_TABS_DISABLED) ? 0 : m_nTab;
                if(nTabCount > 0) {
                    if(WasLastCharacterCR()) {
                        PutTabs();
                    }

                    const char* pEndl = strchr(pString, '\n');
                    while(pEndl) {
                        size_t nSize = (size_t)pEndl - (size_t)pString + sizeof(char);
                        Put(pString, nSize);
                        pString = pEndl + 1;
                        if(*pString) {
                            PutTabs();
                            pEndl = strchr(pString, '\n');
                        } else {
                            pEndl = NULL;
                        }
                    }
                }
                size_t nLen = strlen(pString);
                if(nLen) {
                    Put(pString, nLen * sizeof(char));
                }
            }
        }


        //-----------------------------------------------------------------------------
        // This version of PutString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        inline void CUtlBuffer::PutDelimitedCharInternal(CUtlCharConversion *pConv, char c)
        {
            int l = pConv->GetConversionLength(c);
            if(l == 0) {
                PutChar(c);
            } else {
                PutChar(pConv->GetEscapeChar());
                Put(pConv->GetConversionString(c), l);
            }
        }

        void CUtlBuffer::PutDelimitedChar(CUtlCharConversion *pConv, char c)
        {
            if(!IsText() || !pConv) {
                PutChar(c);
                return;
            }

            PutDelimitedCharInternal(pConv, c);
        }

        void CUtlBuffer::PutDelimitedString(CUtlCharConversion *pConv, const char *pString)
        {
            if(!IsText() || !pConv) {
                PutString(pString);
                return;
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());

            int nLen = pString ? strlen(pString) : 0;
            for(int i = 0; i < nLen; ++i) {
                PutDelimitedCharInternal(pConv, pString[i]);
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());
        }


        void CUtlBuffer::VaPrintf(const char* pFmt, va_list list)
        {
            char temp[2048];
            int nLen = vsnprintf(temp, sizeof(temp), pFmt, list);
            assert(nLen < 2048);
            PutString(temp);
        }

        void CUtlBuffer::Printf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            VaPrintf(pFmt, args);
            va_end(args);
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc)
        {
            m_GetOverflowFunc = getFunc;
            m_PutOverflowFunc = putFunc;
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::OnPutOverflow(int nSize)
        {
            return (this->*m_PutOverflowFunc)(nSize);
        }

        bool CUtlBuffer::OnGetOverflow(int nSize)
        {
            return (this->*m_GetOverflowFunc)(nSize);
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PutOverflow(int nSize)
        {
            if(m_Memory.IsExternallyAllocated()) {
                if(!IsGrowable())
                    return false;

                m_Memory.ConvertToGrowableMemory(0);
            }

            while(Size() < m_Put - m_nOffset + nSize) {
                m_Memory.Grow();
            }

            return true;
        }

        bool CUtlBuffer::GetOverflow(int nSize)
        {
            return false;
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPut(int nSize)
        {
            if((m_Error & PUT_OVERFLOW) || IsReadOnly())
                return false;

            if((m_Put < m_nOffset) || (m_Memory.NumAllocated() < m_Put - m_nOffset + nSize)) {
                if(!OnPutOverflow(nSize)) {
                    m_Error |= PUT_OVERFLOW;
                    return false;
                }
            }
            return true;
        }

        void CUtlBuffer::SeekPut(SeekType_t type, int offset)
        {
            int nNextPut = m_Put;
            switch(type) {
                case SEEK_HEAD:
                    nNextPut = offset;
                    break;

                case SEEK_CURRENT:
                    nNextPut += offset;
                    break;

                case SEEK_TAIL:
                    nNextPut = m_nMaxPut - offset;
                    break;
            }

            // Force a write of the data
            // FIXME: We could make this more optimal potentially by writing out
            // the entire buffer if you seek outside the current range

            // NOTE: This call will write and will also seek the file to nNextPut.
            OnPutOverflow(-nNextPut - 1);
            m_Put = nNextPut;

            AddNullTermination();
        }


        void CUtlBuffer::ActivateByteSwapping(bool bActivate)
        {
            m_Byteswap.ActivateByteSwapping(bActivate);
        }

        void CUtlBuffer::SetBigEndian(bool bigEndian)
        {
            m_Byteswap.SetTargetBigEndian(bigEndian);
        }

        bool CUtlBuffer::IsBigEndian(void)
        {
            return m_Byteswap.IsTargetBigEndian();
        }


        //-----------------------------------------------------------------------------
        // null terminate the buffer
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AddNullTermination(void)
        {
            if(m_Put > m_nMaxPut) {
                if(!IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)) {
                    // Add null termination value
                    if(CheckPut(1)) {
                        m_Memory[m_Put - m_nOffset] = 0;
                    } else {
                        // Restore the overflow state, it was valid before...
                        m_Error &= ~PUT_OVERFLOW;
                    }
                }
                m_nMaxPut = m_Put;
            }
        }


        //-----------------------------------------------------------------------------
        // Converts a buffer from a CRLF buffer to a CR buffer (and back)
        // Returns false if no conversion was necessary (and outBuf is left untouched)
        // If the conversion occurs, outBuf will be cleared.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ConvertCRLF(CUtlBuffer &outBuf)
        {
            if(!IsText() || !outBuf.IsText())
                return false;

            if(ContainsCRLF() == outBuf.ContainsCRLF())
                return false;

            int nInCount = TellMaxPut();

            outBuf.Purge();
            outBuf.EnsureCapacity(nInCount);

            bool bFromCRLF = ContainsCRLF();

            // Start reading from the beginning
            int nGet = TellGet();
            int nPut = TellPut();
            int nGetDelta = 0;
            int nPutDelta = 0;

            const char *pBase = (const char*)Base();
            int nCurrGet = 0;
            while(nCurrGet < nInCount) {
                const char *pCurr = &pBase[nCurrGet];
                if(bFromCRLF) {
                    const char *pNext = V_strnistr(pCurr, "\r\n", nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 2;
                    if(nGet >= nCurrGet - 1) {
                        --nGetDelta;
                    }
                    if(nPut >= nCurrGet - 1) {
                        --nPutDelta;
                    }
                } else {
                    const char *pNext = V_strnchr(pCurr, '\n', nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\r');
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 1;
                    if(nGet >= nCurrGet) {
                        ++nGetDelta;
                    }
                    if(nPut >= nCurrGet) {
                        ++nPutDelta;
                    }
                }
            }

            assert(nPut + nPutDelta <= outBuf.TellMaxPut());

            outBuf.SeekGet(SEEK_HEAD, nGet + nGetDelta);
            outBuf.SeekPut(SEEK_HEAD, nPut + nPutDelta);

            return true;
        }


        //---------------------------------------------------------------------------
        // Implementation of CUtlInplaceBuffer
        //---------------------------------------------------------------------------

        CUtlInplaceBuffer::CUtlInplaceBuffer(int growSize /* = 0 */, int initSize /* = 0 */, int nFlags /* = 0 */) :
            CUtlBuffer(growSize, initSize, nFlags)
        {
            NULL;
        }

        bool CUtlInplaceBuffer::InplaceGetLinePtr(char **ppszInBufferPtr, int *pnLineLength)
        {
            assert(IsText() && !ContainsCRLF());

            int nLineLen = PeekLineLength();
            if(nLineLen <= 1) {
                SeekGet(SEEK_TAIL, 0);
                return false;
            }

            --nLineLen; // because it accounts for putting a terminating null-character

            char *pszLine = (char *) const_cast< void * >(PeekGet());
            SeekGet(SEEK_CURRENT, nLineLen);

            // Set the out args
            if(ppszInBufferPtr)
                *ppszInBufferPtr = pszLine;

            if(pnLineLength)
                *pnLineLength = nLineLen;

            return true;
        }

        char * CUtlInplaceBuffer::InplaceGetLinePtr(void)
        {
            char *pszLine = NULL;
            int nLineLen = 0;

            if(InplaceGetLinePtr(&pszLine, &nLineLen)) {
                assert(nLineLen >= 1);

                switch(pszLine[nLineLen - 1]) {
                    case '\n':
                    case '\r':
                        pszLine[nLineLen - 1] = 0;
                        if(--nLineLen) {
                            switch(pszLine[nLineLen - 1]) {
                                case '\n':
                                case '\r':
                                    pszLine[nLineLen - 1] = 0;
                                    break;
                            }
                        }
                        break;

                    default:
                        assert(pszLine[nLineLen] == 0);
                        break;
                }
            }
            return pszLine;
        }



// Junk Code By Troll Face & Thaisen's Gen
void AvVxKsSrBfIRbhhIQdwBHhjzbJFDoPe88490784() {     double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF42300628 = -414659782;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF23186347 = -752128972;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF92826464 = -168657083;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF3483808 = -994291811;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF95353608 = -697941589;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77279333 = -347615029;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF82750108 = -19045360;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF66217148 = -879783950;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF55872426 = -269992954;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF43827108 = -507474377;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF70399028 = -754287275;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27216248 = -271130893;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF82183553 = -827567416;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF1661621 = -744167990;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF55420044 = -422898322;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF81775337 = -148358272;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF9860931 = -525973913;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15804690 = -535269248;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF93042736 = -185340888;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF57821188 = -326801157;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF8230784 = -13577789;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF50251699 = -728671089;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF29539145 = 16886751;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF44067745 = 33855072;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF30126874 = -229485659;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF81303725 = -81105470;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38616873 = -532522058;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF78042777 = -600377298;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF79852498 = -152490946;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF65310614 = -882695838;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF20423490 = -459858709;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF53683145 = -122937775;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF401598 = -464990558;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF84999651 = -573043806;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF99861862 = 65754034;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF39510244 = -570423992;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF73499095 = -57697579;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF20359935 = -327953634;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF46762618 = -816778009;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77329459 = -370864151;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF25932252 = -196713682;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF72745840 = -822763547;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80540291 = -606374812;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF2152771 = -340389460;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF10984714 = -59770457;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80027396 = -124375646;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF19717632 = -900288452;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77376387 = -738798997;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16428727 = -117261597;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF35736098 = -326304741;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF48808721 = -140551959;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF58219766 = -514537663;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF26642009 = -824895847;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF98253212 = -806958171;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80260000 = -949465072;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF92048928 = -685988694;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF93647202 = -669015724;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF48758720 = -102512156;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF73356933 = -664806153;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF14049884 = -516836120;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38662461 = -815092972;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF4707332 = -418668062;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF86364650 = -627293004;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF90561812 = -387297117;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF23403619 = 52384332;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16715884 = -531349501;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF26814650 = -806140336;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF97183901 = -154523610;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF1799759 = -709922024;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15909801 = -852474330;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF8276243 = 9339307;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF89500996 = -98020280;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF69042071 = -718491240;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15713277 = -814476738;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF31888936 = -30087475;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF35484944 = -190814243;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF69711408 = -22296278;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27386374 = -642723789;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF33083031 = -906374472;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF50099477 = -5110013;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61586094 = -180817018;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61240485 = -793723061;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61614050 = -383115701;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF44116400 = -826186206;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16501893 = -642143880;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF62203723 = -945321046;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27041137 = -298041928;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF2148385 = -658032388;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF4739652 = -623578734;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF7812934 = -248257273;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF45863042 = -901408268;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF24740376 = -955185424;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF47003001 = -663147482;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF32712735 = -199941890;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38666999 = -555771179;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF21224921 = -778045621;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF86381190 = -95470543;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF89978478 = -119077695;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF78749152 = -292773792;    double rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF94268830 = -414659782;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF42300628 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF23186347;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF23186347 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF92826464;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF92826464 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF3483808;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF3483808 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF95353608;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF95353608 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77279333;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77279333 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF82750108;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF82750108 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF66217148;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF66217148 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF55872426;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF55872426 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF43827108;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF43827108 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF70399028;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF70399028 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27216248;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27216248 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF82183553;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF82183553 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF1661621;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF1661621 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF55420044;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF55420044 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF81775337;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF81775337 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF9860931;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF9860931 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15804690;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15804690 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF93042736;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF93042736 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF57821188;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF57821188 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF8230784;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF8230784 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF50251699;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF50251699 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF29539145;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF29539145 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF44067745;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF44067745 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF30126874;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF30126874 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF81303725;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF81303725 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38616873;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38616873 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF78042777;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF78042777 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF79852498;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF79852498 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF65310614;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF65310614 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF20423490;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF20423490 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF53683145;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF53683145 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF401598;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF401598 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF84999651;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF84999651 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF99861862;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF99861862 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF39510244;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF39510244 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF73499095;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF73499095 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF20359935;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF20359935 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF46762618;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF46762618 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77329459;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77329459 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF25932252;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF25932252 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF72745840;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF72745840 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80540291;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80540291 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF2152771;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF2152771 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF10984714;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF10984714 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80027396;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80027396 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF19717632;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF19717632 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77376387;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF77376387 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16428727;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16428727 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF35736098;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF35736098 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF48808721;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF48808721 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF58219766;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF58219766 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF26642009;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF26642009 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF98253212;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF98253212 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80260000;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF80260000 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF92048928;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF92048928 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF93647202;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF93647202 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF48758720;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF48758720 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF73356933;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF73356933 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF14049884;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF14049884 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38662461;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38662461 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF4707332;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF4707332 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF86364650;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF86364650 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF90561812;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF90561812 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF23403619;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF23403619 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16715884;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16715884 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF26814650;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF26814650 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF97183901;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF97183901 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF1799759;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF1799759 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15909801;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15909801 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF8276243;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF8276243 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF89500996;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF89500996 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF69042071;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF69042071 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15713277;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF15713277 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF31888936;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF31888936 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF35484944;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF35484944 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF69711408;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF69711408 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27386374;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27386374 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF33083031;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF33083031 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF50099477;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF50099477 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61586094;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61586094 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61240485;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61240485 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61614050;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF61614050 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF44116400;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF44116400 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16501893;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF16501893 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF62203723;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF62203723 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27041137;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF27041137 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF2148385;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF2148385 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF4739652;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF4739652 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF7812934;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF7812934 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF45863042;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF45863042 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF24740376;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF24740376 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF47003001;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF47003001 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF32712735;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF32712735 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38666999;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF38666999 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF21224921;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF21224921 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF86381190;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF86381190 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF89978478;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF89978478 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF78749152;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF78749152 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF94268830;     rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF94268830 = rMtvtwGQHrjeknWvlzfzlFtGSLWWBnPDlAQF42300628;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IoXxmRUmeUwDzAHWHKNpqHxthbXsAUc55894289() {     double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd67600221 = -242860638;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77429800 = -145932210;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd34841242 = -953464770;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd99929895 = 88926558;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd95704 = -622138497;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd16258926 = -977779828;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd90777043 = -24250209;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd21448683 = -331346589;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72252055 = -123839723;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd46065068 = -724177868;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11580090 = -274017849;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd49819209 = -508797138;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd57681169 = -562303253;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd32319305 = -502452829;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13536246 = -442482808;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd78636061 = -781297073;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd59398280 = -728373442;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd26695304 = -985071534;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd96399388 = -696322645;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd6316216 = -726558059;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77210350 = -200806454;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd41196319 = -636392386;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd4831182 = -496657656;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd22995759 = -238507173;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd18446569 = -704997652;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd69716330 = -91304628;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd19089164 = -683343824;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74477527 = -622519915;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd64571110 = -373164354;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd75007988 = -307280250;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd12613694 = -44258155;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13910074 = -154553227;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd36455258 = -70413997;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd45303357 = -793376032;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd94748293 = -258369930;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd29658235 = -977411882;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd3936438 = -196370785;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56047341 = 18741559;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11379153 = -767136664;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd68292045 = -256212799;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd29820605 = -781226617;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd23601151 = -293476785;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80790793 = -992764436;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd76887368 = -543008541;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd50940889 = -974193851;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd82194801 = -863547087;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd90020338 = -424696257;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd41801487 = -677343020;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd82871335 = -666519617;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd9050670 = -436357800;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd5370955 = -22894589;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd4020678 = -280969116;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd88894016 = -384663355;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56146545 = -795646003;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd78683939 = -113213945;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd26403903 = -606468253;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72598618 = -649274554;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11845484 = -614957598;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd81483327 = -206075790;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd30379373 = -430833870;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97169761 = -194436004;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd16299516 = -401730295;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56877573 = -958182236;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97244066 = -816559473;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd33451374 = -579919714;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97670015 = -19464622;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13363951 = -338383141;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd12377813 = -768927221;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd37571012 = -144082900;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd83878011 = -465070926;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74699624 = -484926289;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd3350939 = -647115001;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd15316151 = -117934870;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd28107343 = -340109846;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd76495611 = -945331442;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd53609200 = -907329670;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd60405525 = -643627951;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd27943813 = -953649116;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72054870 = -264313323;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd36251767 = -841450566;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd79695991 = -666608371;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77287677 = 93999195;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd91606192 = -956000299;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd55520440 = -936806554;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd69637034 = -184385662;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd8593016 = -763289039;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd25016057 = -769889873;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80308713 = -274767994;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd66619417 = -580162087;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd68344391 = -651901678;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd57059616 = -228137328;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd92090953 = -581413188;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74564014 = -775182651;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80999779 = -236302794;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd71122284 = 38223205;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13521089 = -279496323;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd66723577 = -335294549;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd83546727 = -76204963;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd43435995 = -963088827;    double FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd53270873 = -242860638;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd67600221 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77429800;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77429800 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd34841242;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd34841242 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd99929895;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd99929895 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd95704;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd95704 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd16258926;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd16258926 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd90777043;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd90777043 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd21448683;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd21448683 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72252055;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72252055 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd46065068;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd46065068 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11580090;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11580090 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd49819209;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd49819209 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd57681169;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd57681169 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd32319305;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd32319305 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13536246;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13536246 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd78636061;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd78636061 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd59398280;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd59398280 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd26695304;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd26695304 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd96399388;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd96399388 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd6316216;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd6316216 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77210350;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77210350 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd41196319;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd41196319 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd4831182;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd4831182 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd22995759;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd22995759 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd18446569;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd18446569 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd69716330;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd69716330 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd19089164;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd19089164 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74477527;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74477527 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd64571110;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd64571110 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd75007988;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd75007988 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd12613694;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd12613694 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13910074;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13910074 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd36455258;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd36455258 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd45303357;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd45303357 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd94748293;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd94748293 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd29658235;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd29658235 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd3936438;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd3936438 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56047341;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56047341 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11379153;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11379153 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd68292045;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd68292045 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd29820605;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd29820605 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd23601151;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd23601151 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80790793;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80790793 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd76887368;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd76887368 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd50940889;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd50940889 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd82194801;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd82194801 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd90020338;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd90020338 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd41801487;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd41801487 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd82871335;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd82871335 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd9050670;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd9050670 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd5370955;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd5370955 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd4020678;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd4020678 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd88894016;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd88894016 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56146545;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56146545 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd78683939;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd78683939 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd26403903;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd26403903 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72598618;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72598618 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11845484;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd11845484 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd81483327;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd81483327 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd30379373;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd30379373 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97169761;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97169761 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd16299516;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd16299516 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56877573;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd56877573 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97244066;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97244066 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd33451374;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd33451374 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97670015;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd97670015 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13363951;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13363951 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd12377813;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd12377813 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd37571012;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd37571012 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd83878011;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd83878011 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74699624;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74699624 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd3350939;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd3350939 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd15316151;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd15316151 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd28107343;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd28107343 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd76495611;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd76495611 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd53609200;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd53609200 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd60405525;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd60405525 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd27943813;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd27943813 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72054870;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd72054870 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd36251767;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd36251767 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd79695991;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd79695991 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77287677;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd77287677 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd91606192;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd91606192 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd55520440;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd55520440 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd69637034;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd69637034 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd8593016;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd8593016 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd25016057;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd25016057 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80308713;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80308713 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd66619417;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd66619417 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd68344391;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd68344391 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd57059616;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd57059616 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd92090953;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd92090953 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74564014;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd74564014 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80999779;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd80999779 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd71122284;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd71122284 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13521089;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd13521089 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd66723577;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd66723577 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd83546727;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd83546727 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd43435995;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd43435995 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd53270873;     FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd53270873 = FauTolqHJvrQxzXgAQMXZbQKBqePKucmKrrd67600221;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void FYMiJVYKiuTmrEjcwmetbjtIxBFcZGN23297795() {     double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ92899814 = -71061492;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ31673254 = -639735443;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ76856019 = -638272457;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ96375984 = 72144925;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ4837798 = -546335397;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ55238517 = -507944627;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ98803978 = -29455058;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ76680217 = -882909214;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ88631683 = 22313509;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ48303027 = -940881359;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ52761150 = -893748421;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ72422170 = -746463373;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ33178785 = -297039090;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ62976990 = -260737668;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ71652448 = -462067277;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ75496784 = -314235875;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ8935630 = -930772970;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ37585918 = -334873815;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99756040 = -107304402;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ54811243 = -26314961;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ46189917 = -388035114;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ32140938 = -544113682;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ80123218 = 89797936;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ1923773 = -510869418;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ6766263 = -80509632;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ58128935 = -101503789;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99561455 = -834165591;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ70912278 = -644662525;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49289721 = -593837776;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ84705363 = -831864663;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ4803898 = -728657601;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74137002 = -186168683;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ72508919 = -775837439;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ5607062 = 86291741;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ89634724 = -582493897;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ19806225 = -284399792;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ34373779 = -335043991;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ91734748 = -734563246;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ75995687 = -717495330;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ59254632 = -141561447;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ33708957 = -265739552;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74456461 = -864190044;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ81041296 = -279154062;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51621967 = -745627621;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ90897063 = -788617246;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ84362206 = -502718545;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ60323047 = 50895938;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ6226588 = -615887045;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49313943 = -115777661;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ82365242 = -546410854;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ61933188 = 94762781;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49821589 = -47400573;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51146026 = 55569141;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ14039878 = -784333835;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ77107877 = -376962824;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ60758876 = -526947810;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51550036 = -629533379;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74932247 = -27403039;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ89609722 = -847345443;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ46708863 = -344831608;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ55677062 = -673779037;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27891701 = -384792534;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27390497 = -189071438;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ3926321 = -145821829;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ43499130 = -112223759;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ78624148 = -607579739;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99913251 = -970625935;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27571724 = -283330832;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ73342265 = -678243772;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51846223 = -77667485;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ41123005 = -979191884;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ17200882 = -96209724;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ61590231 = -617378485;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ40501409 = -965742956;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ21102287 = -760575409;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ71733456 = -523845071;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51099642 = -164959621;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28501252 = -164574443;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ11026710 = -722252173;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22404056 = -577791088;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ97805887 = -52399727;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ93334868 = -118278547;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ21598335 = -428884865;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ66924479 = 52573078;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22772175 = -826627444;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ54982308 = -581257028;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22990977 = -141737824;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ58469041 = -991503604;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28499184 = -536745436;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28875849 = 44453912;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ68256189 = -654866413;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ59441532 = -207640953;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ2125027 = -887217803;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ29286824 = -272663723;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ3577571 = -467782411;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ5817256 = -880947019;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ47065965 = -575118607;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ77114976 = -33332234;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ8122837 = -533403863;    double VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ12272915 = -71061492;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ92899814 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ31673254;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ31673254 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ76856019;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ76856019 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ96375984;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ96375984 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ4837798;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ4837798 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ55238517;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ55238517 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ98803978;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ98803978 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ76680217;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ76680217 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ88631683;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ88631683 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ48303027;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ48303027 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ52761150;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ52761150 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ72422170;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ72422170 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ33178785;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ33178785 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ62976990;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ62976990 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ71652448;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ71652448 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ75496784;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ75496784 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ8935630;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ8935630 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ37585918;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ37585918 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99756040;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99756040 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ54811243;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ54811243 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ46189917;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ46189917 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ32140938;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ32140938 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ80123218;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ80123218 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ1923773;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ1923773 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ6766263;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ6766263 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ58128935;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ58128935 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99561455;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99561455 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ70912278;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ70912278 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49289721;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49289721 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ84705363;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ84705363 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ4803898;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ4803898 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74137002;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74137002 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ72508919;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ72508919 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ5607062;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ5607062 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ89634724;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ89634724 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ19806225;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ19806225 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ34373779;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ34373779 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ91734748;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ91734748 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ75995687;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ75995687 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ59254632;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ59254632 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ33708957;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ33708957 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74456461;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74456461 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ81041296;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ81041296 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51621967;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51621967 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ90897063;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ90897063 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ84362206;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ84362206 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ60323047;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ60323047 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ6226588;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ6226588 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49313943;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49313943 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ82365242;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ82365242 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ61933188;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ61933188 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49821589;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ49821589 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51146026;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51146026 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ14039878;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ14039878 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ77107877;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ77107877 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ60758876;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ60758876 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51550036;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51550036 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74932247;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ74932247 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ89609722;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ89609722 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ46708863;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ46708863 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ55677062;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ55677062 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27891701;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27891701 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27390497;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27390497 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ3926321;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ3926321 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ43499130;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ43499130 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ78624148;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ78624148 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99913251;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ99913251 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27571724;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ27571724 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ73342265;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ73342265 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51846223;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51846223 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ41123005;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ41123005 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ17200882;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ17200882 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ61590231;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ61590231 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ40501409;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ40501409 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ21102287;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ21102287 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ71733456;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ71733456 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51099642;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ51099642 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28501252;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28501252 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ11026710;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ11026710 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22404056;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22404056 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ97805887;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ97805887 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ93334868;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ93334868 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ21598335;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ21598335 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ66924479;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ66924479 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22772175;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22772175 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ54982308;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ54982308 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22990977;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ22990977 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ58469041;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ58469041 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28499184;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28499184 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28875849;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ28875849 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ68256189;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ68256189 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ59441532;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ59441532 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ2125027;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ2125027 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ29286824;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ29286824 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ3577571;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ3577571 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ5817256;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ5817256 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ47065965;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ47065965 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ77114976;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ77114976 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ8122837;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ8122837 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ12272915;     VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ12272915 = VcZjNzemyOssZoxThxmSTzLeVfDkIQvlcgyJ92899814;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void uxPpePdvbOhkRCofxQmCNmfupndWQub67275256() {     int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV57743024 = -266271308;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV27235946 = -959657250;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59932821 = -424541981;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV41658723 = -660672452;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8906985 = -436026810;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV79854651 = -355117547;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV1864372 = -476011482;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV36357725 = -344991433;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV5718961 = 21448030;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15074961 = -131155336;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59350104 = -919580653;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV29355686 = -138719572;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV50598808 = -597888062;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV29476849 = -738722325;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV60588839 = -63597273;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV30304568 = -248441584;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99655956 = -457002705;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV14632597 = -964450583;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8560420 = 27818641;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21426726 = -245628016;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV78453980 = -874062798;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV12068631 = -928394145;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV96247345 = -647287222;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV69588516 = -199893484;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV83578961 = -187278526;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV87184076 = -168434285;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46057615 = -240933114;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV17545473 = -827348203;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21087412 = -751092381;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV13098859 = -794206072;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99216831 = -378641580;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53071588 = -369935872;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46806579 = -725093016;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV3677644 = -513059185;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV1207143 = -256128991;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8846835 = -637793479;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV72525994 = -855249037;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV44364819 = -954816953;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV10329944 = -331549162;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9864746 = -553879977;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV43518543 = 96078925;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV68618582 = -611882088;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV73705087 = -85013710;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV43556966 = -605424630;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15799843 = -469559100;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV49251715 = 95160148;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV50588688 = -498113346;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV80584071 = -312142048;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82589963 = -961725345;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9048864 = -914813619;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV13916816 = -891801213;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV51937973 = -448718462;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV58241908 = -35883797;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV70767095 = -349684791;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV88821276 = 78797389;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV45674393 = -337877164;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV30988601 = -212370029;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV90344304 = -124648497;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV58079762 = -373393927;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21722909 = -167592525;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV33797036 = -14184434;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV84318899 = -648663280;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15270314 = -593899053;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV92620101 = -184345899;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15858130 = -752513756;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV6278517 = -449644781;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82549107 = -413626556;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46921165 = 15171122;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV28269706 = -382593335;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV51742005 = -425803794;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV57778573 = -393192548;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV55291138 = -502185752;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV4302653 = -532901422;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV98695674 = -418301382;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV77908183 = -241706942;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9835398 = -162180711;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV38363543 = -743380436;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV52690380 = 58137408;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53788673 = -730334384;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV34327246 = -182438674;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV36595388 = -670320940;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV65473544 = -928791067;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV34955509 = -865622859;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV12038549 = -836278762;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99182043 = -902404860;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV47278859 = -929923119;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV94829680 = -234052076;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV76039483 = -275408225;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV14856367 = -491856574;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV55532750 = -918251828;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV77858234 = -325423451;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82181690 = -630600541;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV86285057 = -481423027;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV88607034 = -63956637;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV76067709 = -439695544;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59199644 = -255257796;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53348269 = 82016965;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV81084986 = -900667811;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV27698836 = -852910875;    int GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9521327 = -266271308;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV57743024 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV27235946;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV27235946 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59932821;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59932821 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV41658723;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV41658723 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8906985;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8906985 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV79854651;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV79854651 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV1864372;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV1864372 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV36357725;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV36357725 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV5718961;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV5718961 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15074961;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15074961 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59350104;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59350104 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV29355686;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV29355686 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV50598808;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV50598808 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV29476849;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV29476849 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV60588839;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV60588839 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV30304568;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV30304568 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99655956;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99655956 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV14632597;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV14632597 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8560420;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8560420 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21426726;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21426726 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV78453980;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV78453980 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV12068631;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV12068631 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV96247345;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV96247345 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV69588516;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV69588516 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV83578961;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV83578961 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV87184076;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV87184076 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46057615;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46057615 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV17545473;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV17545473 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21087412;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21087412 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV13098859;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV13098859 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99216831;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99216831 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53071588;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53071588 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46806579;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46806579 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV3677644;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV3677644 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV1207143;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV1207143 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8846835;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV8846835 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV72525994;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV72525994 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV44364819;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV44364819 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV10329944;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV10329944 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9864746;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9864746 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV43518543;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV43518543 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV68618582;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV68618582 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV73705087;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV73705087 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV43556966;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV43556966 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15799843;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15799843 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV49251715;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV49251715 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV50588688;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV50588688 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV80584071;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV80584071 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82589963;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82589963 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9048864;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9048864 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV13916816;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV13916816 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV51937973;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV51937973 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV58241908;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV58241908 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV70767095;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV70767095 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV88821276;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV88821276 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV45674393;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV45674393 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV30988601;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV30988601 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV90344304;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV90344304 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV58079762;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV58079762 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21722909;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV21722909 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV33797036;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV33797036 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV84318899;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV84318899 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15270314;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15270314 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV92620101;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV92620101 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15858130;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV15858130 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV6278517;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV6278517 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82549107;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82549107 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46921165;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV46921165 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV28269706;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV28269706 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV51742005;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV51742005 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV57778573;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV57778573 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV55291138;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV55291138 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV4302653;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV4302653 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV98695674;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV98695674 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV77908183;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV77908183 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9835398;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9835398 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV38363543;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV38363543 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV52690380;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV52690380 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53788673;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53788673 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV34327246;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV34327246 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV36595388;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV36595388 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV65473544;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV65473544 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV34955509;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV34955509 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV12038549;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV12038549 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99182043;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV99182043 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV47278859;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV47278859 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV94829680;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV94829680 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV76039483;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV76039483 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV14856367;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV14856367 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV55532750;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV55532750 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV77858234;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV77858234 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82181690;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV82181690 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV86285057;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV86285057 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV88607034;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV88607034 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV76067709;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV76067709 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59199644;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV59199644 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53348269;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV53348269 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV81084986;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV81084986 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV27698836;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV27698836 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9521327;     GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV9521327 = GPBTMOhygJNUYQrGvnpEBNdGISUArmHegnaV57743024;}
// Junk Finished
