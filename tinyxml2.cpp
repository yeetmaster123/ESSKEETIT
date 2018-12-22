/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "tinyxml2.h"

#include <new>		// yes, this one new style header, is in the Android SDK.
#if defined(ANDROID_NDK) || defined(__QNXNTO__)
#   include <stddef.h>
#   include <stdarg.h>
#else
#   include <cstddef>
#   include <cstdarg>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
	// Microsoft Visual Studio, version 2005 and higher. Not WinCE.
	/*int _snprintf_s(
	   char *buffer,
	   size_t sizeOfBuffer,
	   size_t count,
	   const char *format [,
		  argument] ...
	);*/
	static inline int TIXML_SNPRINTF( char* buffer, size_t size, const char* format, ... )
	{
		va_list va;
		va_start( va, format );
		int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
		va_end( va );
		return result;
	}

	static inline int TIXML_VSNPRINTF( char* buffer, size_t size, const char* format, va_list va )
	{
		int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
		return result;
	}

	#define TIXML_VSCPRINTF	_vscprintf
	#define TIXML_SSCANF	sscanf_s
#elif defined _MSC_VER
	// Microsoft Visual Studio 2003 and earlier or WinCE
	#define TIXML_SNPRINTF	_snprintf
	#define TIXML_VSNPRINTF _vsnprintf
	#define TIXML_SSCANF	sscanf
	#if (_MSC_VER < 1400 ) && (!defined WINCE)
		// Microsoft Visual Studio 2003 and not WinCE.
		#define TIXML_VSCPRINTF   _vscprintf // VS2003's C runtime has this, but VC6 C runtime or WinCE SDK doesn't have.
	#else
		// Microsoft Visual Studio 2003 and earlier or WinCE.
		static inline int TIXML_VSCPRINTF( const char* format, va_list va )
		{
			int len = 512;
			for (;;) {
				len = len*2;
				char* str = new char[len]();
				const int required = _vsnprintf(str, len, format, va);
				delete[] str;
				if ( required != -1 ) {
					TIXMLASSERT( required >= 0 );
					len = required;
					break;
				}
			}
			TIXMLASSERT( len >= 0 );
			return len;
		}
	#endif
#else
	// GCC version 3 and higher
	//#warning( "Using sn* functions." )
	#define TIXML_SNPRINTF	snprintf
	#define TIXML_VSNPRINTF	vsnprintf
	static inline int TIXML_VSCPRINTF( const char* format, va_list va )
	{
		int len = vsnprintf( 0, 0, format, va );
		TIXMLASSERT( len >= 0 );
		return len;
	}
	#define TIXML_SSCANF   sscanf
#endif


static const char LINE_FEED				= (char)0x0a;			// all line endings are normalized to LF
static const char LF = LINE_FEED;
static const char CARRIAGE_RETURN		= (char)0x0d;			// CR gets filtered out
static const char CR = CARRIAGE_RETURN;
static const char SINGLE_QUOTE			= '\'';
static const char DOUBLE_QUOTE			= '\"';

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
//	ef bb bf (Microsoft "lead bytes") - designates UTF-8

static const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
static const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
static const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

namespace tinyxml2
{

struct Entity {
    const char* pattern;
    int length;
    char value;
};

static const int NUM_ENTITIES = 5;
static const Entity entities[NUM_ENTITIES] = {
    { "quot", 4,	DOUBLE_QUOTE },
    { "amp", 3,		'&'  },
    { "apos", 4,	SINGLE_QUOTE },
    { "lt",	2, 		'<'	 },
    { "gt",	2,		'>'	 }
};


StrPair::~StrPair()
{
    Reset();
}


void StrPair::TransferTo( StrPair* other )
{
    if ( this == other ) {
        return;
    }
    // This in effect implements the assignment operator by "moving"
    // ownership (as in auto_ptr).

    TIXMLASSERT( other->_flags == 0 );
    TIXMLASSERT( other->_start == 0 );
    TIXMLASSERT( other->_end == 0 );

    other->Reset();

    other->_flags = _flags;
    other->_start = _start;
    other->_end = _end;

    _flags = 0;
    _start = 0;
    _end = 0;
}

void StrPair::Reset()
{
    if ( _flags & NEEDS_DELETE ) {
        delete [] _start;
    }
    _flags = 0;
    _start = 0;
    _end = 0;
}


void StrPair::SetStr( const char* str, int flags )
{
    Reset();
    size_t len = strlen( str );
    TIXMLASSERT( _start == 0 );
    _start = new char[ len+1 ];
    memcpy( _start, str, len+1 );
    _end = _start + len;
    _flags = flags | NEEDS_DELETE;
}


char* StrPair::ParseText( char* p, const char* endTag, int strFlags )
{
    TIXMLASSERT( endTag && *endTag );

    char* start = p;
    char  endChar = *endTag;
    size_t length = strlen( endTag );

    // Inner loop of text parsing.
    while ( *p ) {
        if ( *p == endChar && strncmp( p, endTag, length ) == 0 ) {
            Set( start, p, strFlags );
            return p + length;
        }
        ++p;
    }
    return 0;
}


char* StrPair::ParseName( char* p )
{
    if ( !p || !(*p) ) {
        return 0;
    }
    if ( !XMLUtil::IsNameStartChar( *p ) ) {
        return 0;
    }

    char* const start = p;
    ++p;
    while ( *p && XMLUtil::IsNameChar( *p ) ) {
        ++p;
    }

    Set( start, p, 0 );
    return p;
}


void StrPair::CollapseWhitespace()
{
    // Adjusting _start would cause undefined behavior on delete[]
    TIXMLASSERT( ( _flags & NEEDS_DELETE ) == 0 );
    // Trim leading space.
    _start = XMLUtil::SkipWhiteSpace( _start );

    if ( *_start ) {
        char* p = _start;	// the read pointer
        char* q = _start;	// the write pointer

        while( *p ) {
            if ( XMLUtil::IsWhiteSpace( *p )) {
                p = XMLUtil::SkipWhiteSpace( p );
                if ( *p == 0 ) {
                    break;    // don't write to q; this trims the trailing space.
                }
                *q = ' ';
                ++q;
            }
            *q = *p;
            ++q;
            ++p;
        }
        *q = 0;
    }
}


const char* StrPair::GetStr()
{
    TIXMLASSERT( _start );
    TIXMLASSERT( _end );
    if ( _flags & NEEDS_FLUSH ) {
        *_end = 0;
        _flags ^= NEEDS_FLUSH;

        if ( _flags ) {
            char* p = _start;	// the read pointer
            char* q = _start;	// the write pointer

            while( p < _end ) {
                if ( (_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == CR ) {
                    // CR-LF pair becomes LF
                    // CR alone becomes LF
                    // LF-CR becomes LF
                    if ( *(p+1) == LF ) {
                        p += 2;
                    }
                    else {
                        ++p;
                    }
                    *q++ = LF;
                }
                else if ( (_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == LF ) {
                    if ( *(p+1) == CR ) {
                        p += 2;
                    }
                    else {
                        ++p;
                    }
                    *q++ = LF;
                }
                else if ( (_flags & NEEDS_ENTITY_PROCESSING) && *p == '&' ) {
                    // Entities handled by tinyXML2:
                    // - special entities in the entity table [in/out]
                    // - numeric character reference [in]
                    //   &#20013; or &#x4e2d;

                    if ( *(p+1) == '#' ) {
                        const int buflen = 10;
                        char buf[buflen] = { 0 };
                        int len = 0;
                        char* adjusted = const_cast<char*>( XMLUtil::GetCharacterRef( p, buf, &len ) );
                        if ( adjusted == 0 ) {
                            *q = *p;
                            ++p;
                            ++q;
                        }
                        else {
                            TIXMLASSERT( 0 <= len && len <= buflen );
                            TIXMLASSERT( q + len <= adjusted );
                            p = adjusted;
                            memcpy( q, buf, len );
                            q += len;
                        }
                    }
                    else {
                        bool entityFound = false;
                        for( int i = 0; i < NUM_ENTITIES; ++i ) {
                            const Entity& entity = entities[i];
                            if ( strncmp( p + 1, entity.pattern, entity.length ) == 0
                                    && *( p + entity.length + 1 ) == ';' ) {
                                // Found an entity - convert.
                                *q = entity.value;
                                ++q;
                                p += entity.length + 2;
                                entityFound = true;
                                break;
                            }
                        }
                        if ( !entityFound ) {
                            // fixme: treat as error?
                            ++p;
                            ++q;
                        }
                    }
                }
                else {
                    *q = *p;
                    ++p;
                    ++q;
                }
            }
            *q = 0;
        }
        // The loop below has plenty going on, and this
        // is a less useful mode. Break it out.
        if ( _flags & NEEDS_WHITESPACE_COLLAPSING ) {
            CollapseWhitespace();
        }
        _flags = (_flags & NEEDS_DELETE);
    }
    TIXMLASSERT( _start );
    return _start;
}




// --------- XMLUtil ----------- //

const char* XMLUtil::ReadBOM( const char* p, bool* bom )
{
    TIXMLASSERT( p );
    TIXMLASSERT( bom );
    *bom = false;
    const unsigned char* pu = reinterpret_cast<const unsigned char*>(p);
    // Check for BOM:
    if (    *(pu+0) == TIXML_UTF_LEAD_0
            && *(pu+1) == TIXML_UTF_LEAD_1
            && *(pu+2) == TIXML_UTF_LEAD_2 ) {
        *bom = true;
        p += 3;
    }
    TIXMLASSERT( p );
    return p;
}


void XMLUtil::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (input < 0x80) {
        *length = 1;
    }
    else if ( input < 0x800 ) {
        *length = 2;
    }
    else if ( input < 0x10000 ) {
        *length = 3;
    }
    else if ( input < 0x200000 ) {
        *length = 4;
    }
    else {
        *length = 0;    // This code won't convert this correctly anyway.
        return;
    }

    output += *length;

    // Scary scary fall throughs.
    switch (*length) {
        case 4:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 3:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 2:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 1:
            --output;
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
            break;
        default:
            TIXMLASSERT( false );
    }
}


const char* XMLUtil::GetCharacterRef( const char* p, char* value, int* length )
{
    // Presume an entity, and pull it out.
    *length = 0;

    if ( *(p+1) == '#' && *(p+2) ) {
        unsigned long ucs = 0;
        TIXMLASSERT( sizeof( ucs ) >= 4 );
        ptrdiff_t delta = 0;
        unsigned mult = 1;
        static const char SEMICOLON = ';';

        if ( *(p+2) == 'x' ) {
            // Hexadecimal.
            const char* q = p+3;
            if ( !(*q) ) {
                return 0;
            }

            q = strchr( q, SEMICOLON );

            if ( !q ) {
                return 0;
            }
            TIXMLASSERT( *q == SEMICOLON );

            delta = q-p;
            --q;

            while ( *q != 'x' ) {
                unsigned int digit = 0;

                if ( *q >= '0' && *q <= '9' ) {
                    digit = *q - '0';
                }
                else if ( *q >= 'a' && *q <= 'f' ) {
                    digit = *q - 'a' + 10;
                }
                else if ( *q >= 'A' && *q <= 'F' ) {
                    digit = *q - 'A' + 10;
                }
                else {
                    return 0;
                }
                TIXMLASSERT( digit >= 0 && digit < 16);
                TIXMLASSERT( digit == 0 || mult <= UINT_MAX / digit );
                const unsigned int digitScaled = mult * digit;
                TIXMLASSERT( ucs <= ULONG_MAX - digitScaled );
                ucs += digitScaled;
                TIXMLASSERT( mult <= UINT_MAX / 16 );
                mult *= 16;
                --q;
            }
        }
        else {
            // Decimal.
            const char* q = p+2;
            if ( !(*q) ) {
                return 0;
            }

            q = strchr( q, SEMICOLON );

            if ( !q ) {
                return 0;
            }
            TIXMLASSERT( *q == SEMICOLON );

            delta = q-p;
            --q;

            while ( *q != '#' ) {
                if ( *q >= '0' && *q <= '9' ) {
                    const unsigned int digit = *q - '0';
                    TIXMLASSERT( digit >= 0 && digit < 10);
                    TIXMLASSERT( digit == 0 || mult <= UINT_MAX / digit );
                    const unsigned int digitScaled = mult * digit;
                    TIXMLASSERT( ucs <= ULONG_MAX - digitScaled );
                    ucs += digitScaled;
                }
                else {
                    return 0;
                }
                TIXMLASSERT( mult <= UINT_MAX / 10 );
                mult *= 10;
                --q;
            }
        }
        // convert the UCS to UTF-8
        ConvertUTF32ToUTF8( ucs, value, length );
        return p + delta + 1;
    }
    return p+1;
}


void XMLUtil::ToStr( int v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%d", v );
}


void XMLUtil::ToStr( unsigned v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%u", v );
}


void XMLUtil::ToStr( bool v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%d", v ? 1 : 0 );
}

/*
	ToStr() of a number is a very tricky topic.
	https://github.com/leethomason/tinyxml2/issues/106
*/
void XMLUtil::ToStr( float v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%.8g", v );
}


void XMLUtil::ToStr( double v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%.17g", v );
}


bool XMLUtil::ToInt( const char* str, int* value )
{
    if ( TIXML_SSCANF( str, "%d", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToUnsigned( const char* str, unsigned *value )
{
    if ( TIXML_SSCANF( str, "%u", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToBool( const char* str, bool* value )
{
    int ival = 0;
    if ( ToInt( str, &ival )) {
        *value = (ival==0) ? false : true;
        return true;
    }
    if ( StringEqual( str, "true" ) ) {
        *value = true;
        return true;
    }
    else if ( StringEqual( str, "false" ) ) {
        *value = false;
        return true;
    }
    return false;
}


bool XMLUtil::ToFloat( const char* str, float* value )
{
    if ( TIXML_SSCANF( str, "%f", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToDouble( const char* str, double* value )
{
    if ( TIXML_SSCANF( str, "%lf", value ) == 1 ) {
        return true;
    }
    return false;
}


char* XMLDocument::Identify( char* p, XMLNode** node )
{
    TIXMLASSERT( node );
    TIXMLASSERT( p );
    char* const start = p;
    p = XMLUtil::SkipWhiteSpace( p );
    if( !*p ) {
        *node = 0;
        TIXMLASSERT( p );
        return p;
    }

    // These strings define the matching patterns:
    static const char* xmlHeader		= { "<?" };
    static const char* commentHeader	= { "<!--" };
    static const char* cdataHeader		= { "<![CDATA[" };
    static const char* dtdHeader		= { "<!" };
    static const char* elementHeader	= { "<" };	// and a header for everything else; check last.

    static const int xmlHeaderLen		= 2;
    static const int commentHeaderLen	= 4;
    static const int cdataHeaderLen		= 9;
    static const int dtdHeaderLen		= 2;
    static const int elementHeaderLen	= 1;

    TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLUnknown ) );		// use same memory pool
    TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLDeclaration ) );	// use same memory pool
    XMLNode* returnNode = 0;
    if ( XMLUtil::StringEqual( p, xmlHeader, xmlHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLDeclaration ) == _commentPool.ItemSize() );
        returnNode = new (_commentPool.Alloc()) XMLDeclaration( this );
        returnNode->_memPool = &_commentPool;
        p += xmlHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, commentHeader, commentHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLComment ) == _commentPool.ItemSize() );
        returnNode = new (_commentPool.Alloc()) XMLComment( this );
        returnNode->_memPool = &_commentPool;
        p += commentHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, cdataHeader, cdataHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
        XMLText* text = new (_textPool.Alloc()) XMLText( this );
        returnNode = text;
        returnNode->_memPool = &_textPool;
        p += cdataHeaderLen;
        text->SetCData( true );
    }
    else if ( XMLUtil::StringEqual( p, dtdHeader, dtdHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLUnknown ) == _commentPool.ItemSize() );
        returnNode = new (_commentPool.Alloc()) XMLUnknown( this );
        returnNode->_memPool = &_commentPool;
        p += dtdHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, elementHeader, elementHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLElement ) == _elementPool.ItemSize() );
        returnNode = new (_elementPool.Alloc()) XMLElement( this );
        returnNode->_memPool = &_elementPool;
        p += elementHeaderLen;
    }
    else {
        TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
        returnNode = new (_textPool.Alloc()) XMLText( this );
        returnNode->_memPool = &_textPool;
        p = start;	// Back it up, all the text counts.
    }

    TIXMLASSERT( returnNode );
    TIXMLASSERT( p );
    *node = returnNode;
    return p;
}


bool XMLDocument::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    if ( visitor->VisitEnter( *this ) ) {
        for ( const XMLNode* node=FirstChild(); node; node=node->NextSibling() ) {
            if ( !node->Accept( visitor ) ) {
                break;
            }
        }
    }
    return visitor->VisitExit( *this );
}


// --------- XMLNode ----------- //

XMLNode::XMLNode( XMLDocument* doc ) :
    _document( doc ),
    _parent( 0 ),
    _firstChild( 0 ), _lastChild( 0 ),
    _prev( 0 ), _next( 0 ),
    _memPool( 0 )
{
}


XMLNode::~XMLNode()
{
    DeleteChildren();
    if ( _parent ) {
        _parent->Unlink( this );
    }
}

const char* XMLNode::Value() const 
{
    // Catch an edge case: XMLDocuments don't have a a Value. Carefully return nullptr.
    if ( this->ToDocument() )
        return 0;
    return _value.GetStr();
}

void XMLNode::SetValue( const char* str, bool staticMem )
{
    if ( staticMem ) {
        _value.SetInternedStr( str );
    }
    else {
        _value.SetStr( str );
    }
}


void XMLNode::DeleteChildren()
{
    while( _firstChild ) {
        TIXMLASSERT( _lastChild );
        TIXMLASSERT( _firstChild->_document == _document );
        XMLNode* node = _firstChild;
        Unlink( node );

        DeleteNode( node );
    }
    _firstChild = _lastChild = 0;
}


void XMLNode::Unlink( XMLNode* child )
{
    TIXMLASSERT( child );
    TIXMLASSERT( child->_document == _document );
    TIXMLASSERT( child->_parent == this );
    if ( child == _firstChild ) {
        _firstChild = _firstChild->_next;
    }
    if ( child == _lastChild ) {
        _lastChild = _lastChild->_prev;
    }

    if ( child->_prev ) {
        child->_prev->_next = child->_next;
    }
    if ( child->_next ) {
        child->_next->_prev = child->_prev;
    }
	child->_parent = 0;
}


void XMLNode::DeleteChild( XMLNode* node )
{
    TIXMLASSERT( node );
    TIXMLASSERT( node->_document == _document );
    TIXMLASSERT( node->_parent == this );
    DeleteNode( node );
}


XMLNode* XMLNode::InsertEndChild( XMLNode* addThis )
{
    TIXMLASSERT( addThis );
    if ( addThis->_document != _document ) {
        TIXMLASSERT( false );
        return 0;
    }
    InsertChildPreamble( addThis );

    if ( _lastChild ) {
        TIXMLASSERT( _firstChild );
        TIXMLASSERT( _lastChild->_next == 0 );
        _lastChild->_next = addThis;
        addThis->_prev = _lastChild;
        _lastChild = addThis;

        addThis->_next = 0;
    }
    else {
        TIXMLASSERT( _firstChild == 0 );
        _firstChild = _lastChild = addThis;

        addThis->_prev = 0;
        addThis->_next = 0;
    }
    addThis->_parent = this;
    return addThis;
}


XMLNode* XMLNode::InsertFirstChild( XMLNode* addThis )
{
    TIXMLASSERT( addThis );
    if ( addThis->_document != _document ) {
        TIXMLASSERT( false );
        return 0;
    }
    InsertChildPreamble( addThis );

    if ( _firstChild ) {
        TIXMLASSERT( _lastChild );
        TIXMLASSERT( _firstChild->_prev == 0 );

        _firstChild->_prev = addThis;
        addThis->_next = _firstChild;
        _firstChild = addThis;

        addThis->_prev = 0;
    }
    else {
        TIXMLASSERT( _lastChild == 0 );
        _firstChild = _lastChild = addThis;

        addThis->_prev = 0;
        addThis->_next = 0;
    }
    addThis->_parent = this;
    return addThis;
}


XMLNode* XMLNode::InsertAfterChild( XMLNode* afterThis, XMLNode* addThis )
{
    TIXMLASSERT( addThis );
    if ( addThis->_document != _document ) {
        TIXMLASSERT( false );
        return 0;
    }

    TIXMLASSERT( afterThis );

    if ( afterThis->_parent != this ) {
        TIXMLASSERT( false );
        return 0;
    }

    if ( afterThis->_next == 0 ) {
        // The last node or the only node.
        return InsertEndChild( addThis );
    }
    InsertChildPreamble( addThis );
    addThis->_prev = afterThis;
    addThis->_next = afterThis->_next;
    afterThis->_next->_prev = addThis;
    afterThis->_next = addThis;
    addThis->_parent = this;
    return addThis;
}




const XMLElement* XMLNode::FirstChildElement( const char* name ) const
{
    for( const XMLNode* node = _firstChild; node; node = node->_next ) {
        const XMLElement* element = node->ToElement();
        if ( element ) {
            if ( !name || XMLUtil::StringEqual( element->Name(), name ) ) {
                return element;
            }
        }
    }
    return 0;
}


const XMLElement* XMLNode::LastChildElement( const char* name ) const
{
    for( const XMLNode* node = _lastChild; node; node = node->_prev ) {
        const XMLElement* element = node->ToElement();
        if ( element ) {
            if ( !name || XMLUtil::StringEqual( element->Name(), name ) ) {
                return element;
            }
        }
    }
    return 0;
}


const XMLElement* XMLNode::NextSiblingElement( const char* name ) const
{
    for( const XMLNode* node = _next; node; node = node->_next ) {
        const XMLElement* element = node->ToElement();
        if ( element
                && (!name || XMLUtil::StringEqual( name, element->Name() ))) {
            return element;
        }
    }
    return 0;
}


const XMLElement* XMLNode::PreviousSiblingElement( const char* name ) const
{
    for( const XMLNode* node = _prev; node; node = node->_prev ) {
        const XMLElement* element = node->ToElement();
        if ( element
                && (!name || XMLUtil::StringEqual( name, element->Name() ))) {
            return element;
        }
    }
    return 0;
}


char* XMLNode::ParseDeep( char* p, StrPair* parentEnd )
{
    // This is a recursive method, but thinking about it "at the current level"
    // it is a pretty simple flat list:
    //		<foo/>
    //		<!-- comment -->
    //
    // With a special case:
    //		<foo>
    //		</foo>
    //		<!-- comment -->
    //
    // Where the closing element (/foo) *must* be the next thing after the opening
    // element, and the names must match. BUT the tricky bit is that the closing
    // element will be read by the child.
    //
    // 'endTag' is the end tag for this node, it is returned by a call to a child.
    // 'parentEnd' is the end tag for the parent, which is filled in and returned.

    while( p && *p ) {
        XMLNode* node = 0;

        p = _document->Identify( p, &node );
        if ( node == 0 ) {
            break;
        }

        StrPair endTag;
        p = node->ParseDeep( p, &endTag );
        if ( !p ) {
            DeleteNode( node );
            if ( !_document->Error() ) {
                _document->SetError( XML_ERROR_PARSING, 0, 0 );
            }
            break;
        }

        XMLDeclaration* decl = node->ToDeclaration();
        if ( decl ) {
                // A declaration can only be the first child of a document.
                // Set error, if document already has children.
                if ( !_document->NoChildren() ) {
                        _document->SetError( XML_ERROR_PARSING_DECLARATION, decl->Value(), 0);
                        DeleteNode( decl );
                        break;
                }
        }

        XMLElement* ele = node->ToElement();
        if ( ele ) {
            // We read the end tag. Return it to the parent.
            if ( ele->ClosingType() == XMLElement::CLOSING ) {
                if ( parentEnd ) {
                    ele->_value.TransferTo( parentEnd );
                }
                node->_memPool->SetTracked();   // created and then immediately deleted.
                DeleteNode( node );
                return p;
            }

            // Handle an end tag returned to this level.
            // And handle a bunch of annoying errors.
            bool mismatch = false;
            if ( endTag.Empty() ) {
                if ( ele->ClosingType() == XMLElement::OPEN ) {
                    mismatch = true;
                }
            }
            else {
                if ( ele->ClosingType() != XMLElement::OPEN ) {
                    mismatch = true;
                }
                else if ( !XMLUtil::StringEqual( endTag.GetStr(), ele->Name() ) ) {
                    mismatch = true;
                }
            }
            if ( mismatch ) {
                _document->SetError( XML_ERROR_MISMATCHED_ELEMENT, ele->Name(), 0 );
                DeleteNode( node );
                break;
            }
        }
        InsertEndChild( node );
    }
    return 0;
}

void XMLNode::DeleteNode( XMLNode* node )
{
    if ( node == 0 ) {
        return;
    }
    MemPool* pool = node->_memPool;
    node->~XMLNode();
    pool->Free( node );
}

void XMLNode::InsertChildPreamble( XMLNode* insertThis ) const
{
    TIXMLASSERT( insertThis );
    TIXMLASSERT( insertThis->_document == _document );

    if ( insertThis->_parent )
        insertThis->_parent->Unlink( insertThis );
    else
        insertThis->_memPool->SetTracked();
}

// --------- XMLText ---------- //
char* XMLText::ParseDeep( char* p, StrPair* )
{
    const char* start = p;
    if ( this->CData() ) {
        p = _value.ParseText( p, "]]>", StrPair::NEEDS_NEWLINE_NORMALIZATION );
        if ( !p ) {
            _document->SetError( XML_ERROR_PARSING_CDATA, start, 0 );
        }
        return p;
    }
    else {
        int flags = _document->ProcessEntities() ? StrPair::TEXT_ELEMENT : StrPair::TEXT_ELEMENT_LEAVE_ENTITIES;
        if ( _document->WhitespaceMode() == COLLAPSE_WHITESPACE ) {
            flags |= StrPair::NEEDS_WHITESPACE_COLLAPSING;
        }

        p = _value.ParseText( p, "<", flags );
        if ( p && *p ) {
            return p-1;
        }
        if ( !p ) {
            _document->SetError( XML_ERROR_PARSING_TEXT, start, 0 );
        }
    }
    return 0;
}


XMLNode* XMLText::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLText* text = doc->NewText( Value() );	// fixme: this will always allocate memory. Intern?
    text->SetCData( this->CData() );
    return text;
}


bool XMLText::ShallowEqual( const XMLNode* compare ) const
{
    const XMLText* text = compare->ToText();
    return ( text && XMLUtil::StringEqual( text->Value(), Value() ) );
}


bool XMLText::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}


// --------- XMLComment ---------- //

XMLComment::XMLComment( XMLDocument* doc ) : XMLNode( doc )
{
}


XMLComment::~XMLComment()
{
}


char* XMLComment::ParseDeep( char* p, StrPair* )
{
    // Comment parses as text.
    const char* start = p;
    p = _value.ParseText( p, "-->", StrPair::COMMENT );
    if ( p == 0 ) {
        _document->SetError( XML_ERROR_PARSING_COMMENT, start, 0 );
    }
    return p;
}


XMLNode* XMLComment::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLComment* comment = doc->NewComment( Value() );	// fixme: this will always allocate memory. Intern?
    return comment;
}


bool XMLComment::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLComment* comment = compare->ToComment();
    return ( comment && XMLUtil::StringEqual( comment->Value(), Value() ));
}


bool XMLComment::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}


// --------- XMLDeclaration ---------- //

XMLDeclaration::XMLDeclaration( XMLDocument* doc ) : XMLNode( doc )
{
}


XMLDeclaration::~XMLDeclaration()
{
    //printf( "~XMLDeclaration\n" );
}


char* XMLDeclaration::ParseDeep( char* p, StrPair* )
{
    // Declaration parses as text.
    const char* start = p;
    p = _value.ParseText( p, "?>", StrPair::NEEDS_NEWLINE_NORMALIZATION );
    if ( p == 0 ) {
        _document->SetError( XML_ERROR_PARSING_DECLARATION, start, 0 );
    }
    return p;
}


XMLNode* XMLDeclaration::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLDeclaration* dec = doc->NewDeclaration( Value() );	// fixme: this will always allocate memory. Intern?
    return dec;
}


bool XMLDeclaration::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLDeclaration* declaration = compare->ToDeclaration();
    return ( declaration && XMLUtil::StringEqual( declaration->Value(), Value() ));
}



bool XMLDeclaration::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}

// --------- XMLUnknown ---------- //

XMLUnknown::XMLUnknown( XMLDocument* doc ) : XMLNode( doc )
{
}


XMLUnknown::~XMLUnknown()
{
}


char* XMLUnknown::ParseDeep( char* p, StrPair* )
{
    // Unknown parses as text.
    const char* start = p;

    p = _value.ParseText( p, ">", StrPair::NEEDS_NEWLINE_NORMALIZATION );
    if ( !p ) {
        _document->SetError( XML_ERROR_PARSING_UNKNOWN, start, 0 );
    }
    return p;
}


XMLNode* XMLUnknown::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLUnknown* text = doc->NewUnknown( Value() );	// fixme: this will always allocate memory. Intern?
    return text;
}


bool XMLUnknown::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLUnknown* unknown = compare->ToUnknown();
    return ( unknown && XMLUtil::StringEqual( unknown->Value(), Value() ));
}


bool XMLUnknown::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}

// --------- XMLAttribute ---------- //

const char* XMLAttribute::Name() const 
{
    return _name.GetStr();
}

const char* XMLAttribute::Value() const 
{
    return _value.GetStr();
}

char* XMLAttribute::ParseDeep( char* p, bool processEntities )
{
    // Parse using the name rules: bug fix, was using ParseText before
    p = _name.ParseName( p );
    if ( !p || !*p ) {
        return 0;
    }

    // Skip white space before =
    p = XMLUtil::SkipWhiteSpace( p );
    if ( *p != '=' ) {
        return 0;
    }

    ++p;	// move up to opening quote
    p = XMLUtil::SkipWhiteSpace( p );
    if ( *p != '\"' && *p != '\'' ) {
        return 0;
    }

    char endTag[2] = { *p, 0 };
    ++p;	// move past opening quote

    p = _value.ParseText( p, endTag, processEntities ? StrPair::ATTRIBUTE_VALUE : StrPair::ATTRIBUTE_VALUE_LEAVE_ENTITIES );
    return p;
}


void XMLAttribute::SetName( const char* n )
{
    _name.SetStr( n );
}


XMLError XMLAttribute::QueryIntValue( int* value ) const
{
    if ( XMLUtil::ToInt( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryUnsignedValue( unsigned int* value ) const
{
    if ( XMLUtil::ToUnsigned( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryBoolValue( bool* value ) const
{
    if ( XMLUtil::ToBool( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryFloatValue( float* value ) const
{
    if ( XMLUtil::ToFloat( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryDoubleValue( double* value ) const
{
    if ( XMLUtil::ToDouble( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


void XMLAttribute::SetAttribute( const char* v )
{
    _value.SetStr( v );
}


void XMLAttribute::SetAttribute( int v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


void XMLAttribute::SetAttribute( unsigned v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


void XMLAttribute::SetAttribute( bool v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}

void XMLAttribute::SetAttribute( double v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}

void XMLAttribute::SetAttribute( float v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


// --------- XMLElement ---------- //
XMLElement::XMLElement( XMLDocument* doc ) : XMLNode( doc ),
    _closingType( 0 ),
    _rootAttribute( 0 )
{
}


XMLElement::~XMLElement()
{
    while( _rootAttribute ) {
        XMLAttribute* next = _rootAttribute->_next;
        DeleteAttribute( _rootAttribute );
        _rootAttribute = next;
    }
}


const XMLAttribute* XMLElement::FindAttribute( const char* name ) const
{
    for( XMLAttribute* a = _rootAttribute; a; a = a->_next ) {
        if ( XMLUtil::StringEqual( a->Name(), name ) ) {
            return a;
        }
    }
    return 0;
}


const char* XMLElement::Attribute( const char* name, const char* value ) const
{
    const XMLAttribute* a = FindAttribute( name );
    if ( !a ) {
        return 0;
    }
    if ( !value || XMLUtil::StringEqual( a->Value(), value )) {
        return a->Value();
    }
    return 0;
}


const char* XMLElement::GetText() const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        return FirstChild()->Value();
    }
    return 0;
}


void	XMLElement::SetText( const char* inText )
{
	if ( FirstChild() && FirstChild()->ToText() )
		FirstChild()->SetValue( inText );
	else {
		XMLText*	theText = GetDocument()->NewText( inText );
		InsertFirstChild( theText );
	}
}


void XMLElement::SetText( int v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( unsigned v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( bool v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( float v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( double v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


XMLError XMLElement::QueryIntText( int* ival ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToInt( t, ival ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryUnsignedText( unsigned* uval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToUnsigned( t, uval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryBoolText( bool* bval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToBool( t, bval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryDoubleText( double* dval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToDouble( t, dval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryFloatText( float* fval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToFloat( t, fval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}



XMLAttribute* XMLElement::FindOrCreateAttribute( const char* name )
{
    XMLAttribute* last = 0;
    XMLAttribute* attrib = 0;
    for( attrib = _rootAttribute;
            attrib;
            last = attrib, attrib = attrib->_next ) {
        if ( XMLUtil::StringEqual( attrib->Name(), name ) ) {
            break;
        }
    }
    if ( !attrib ) {
        TIXMLASSERT( sizeof( XMLAttribute ) == _document->_attributePool.ItemSize() );
        attrib = new (_document->_attributePool.Alloc() ) XMLAttribute();
        attrib->_memPool = &_document->_attributePool;
        if ( last ) {
            last->_next = attrib;
        }
        else {
            _rootAttribute = attrib;
        }
        attrib->SetName( name );
        attrib->_memPool->SetTracked(); // always created and linked.
    }
    return attrib;
}


void XMLElement::DeleteAttribute( const char* name )
{
    XMLAttribute* prev = 0;
    for( XMLAttribute* a=_rootAttribute; a; a=a->_next ) {
        if ( XMLUtil::StringEqual( name, a->Name() ) ) {
            if ( prev ) {
                prev->_next = a->_next;
            }
            else {
                _rootAttribute = a->_next;
            }
            DeleteAttribute( a );
            break;
        }
        prev = a;
    }
}


char* XMLElement::ParseAttributes( char* p )
{
    const char* start = p;
    XMLAttribute* prevAttribute = 0;

    // Read the attributes.
    while( p ) {
        p = XMLUtil::SkipWhiteSpace( p );
        if ( !(*p) ) {
            _document->SetError( XML_ERROR_PARSING_ELEMENT, start, Name() );
            return 0;
        }

        // attribute.
        if (XMLUtil::IsNameStartChar( *p ) ) {
            TIXMLASSERT( sizeof( XMLAttribute ) == _document->_attributePool.ItemSize() );
            XMLAttribute* attrib = new (_document->_attributePool.Alloc() ) XMLAttribute();
            attrib->_memPool = &_document->_attributePool;
			attrib->_memPool->SetTracked();

            p = attrib->ParseDeep( p, _document->ProcessEntities() );
            if ( !p || Attribute( attrib->Name() ) ) {
                DeleteAttribute( attrib );
                _document->SetError( XML_ERROR_PARSING_ATTRIBUTE, start, p );
                return 0;
            }
            // There is a minor bug here: if the attribute in the source xml
            // document is duplicated, it will not be detected and the
            // attribute will be doubly added. However, tracking the 'prevAttribute'
            // avoids re-scanning the attribute list. Preferring performance for
            // now, may reconsider in the future.
            if ( prevAttribute ) {
                prevAttribute->_next = attrib;
            }
            else {
                _rootAttribute = attrib;
            }
            prevAttribute = attrib;
        }
        // end of the tag
        else if ( *p == '>' ) {
            ++p;
            break;
        }
        // end of the tag
        else if ( *p == '/' && *(p+1) == '>' ) {
            _closingType = CLOSED;
            return p+2;	// done; sealed element.
        }
        else {
            _document->SetError( XML_ERROR_PARSING_ELEMENT, start, p );
            return 0;
        }
    }
    return p;
}

void XMLElement::DeleteAttribute( XMLAttribute* attribute )
{
    if ( attribute == 0 ) {
        return;
    }
    MemPool* pool = attribute->_memPool;
    attribute->~XMLAttribute();
    pool->Free( attribute );
}

//
//	<ele></ele>
//	<ele>foo<b>bar</b></ele>
//
char* XMLElement::ParseDeep( char* p, StrPair* strPair )
{
    // Read the element name.
    p = XMLUtil::SkipWhiteSpace( p );

    // The closing element is the </element> form. It is
    // parsed just like a regular element then deleted from
    // the DOM.
    if ( *p == '/' ) {
        _closingType = CLOSING;
        ++p;
    }

    p = _value.ParseName( p );
    if ( _value.Empty() ) {
        return 0;
    }

    p = ParseAttributes( p );
    if ( !p || !*p || _closingType ) {
        return p;
    }

    p = XMLNode::ParseDeep( p, strPair );
    return p;
}



XMLNode* XMLElement::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLElement* element = doc->NewElement( Value() );					// fixme: this will always allocate memory. Intern?
    for( const XMLAttribute* a=FirstAttribute(); a; a=a->Next() ) {
        element->SetAttribute( a->Name(), a->Value() );					// fixme: this will always allocate memory. Intern?
    }
    return element;
}


bool XMLElement::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLElement* other = compare->ToElement();
    if ( other && XMLUtil::StringEqual( other->Name(), Name() )) {

        const XMLAttribute* a=FirstAttribute();
        const XMLAttribute* b=other->FirstAttribute();

        while ( a && b ) {
            if ( !XMLUtil::StringEqual( a->Value(), b->Value() ) ) {
                return false;
            }
            a = a->Next();
            b = b->Next();
        }
        if ( a || b ) {
            // different count
            return false;
        }
        return true;
    }
    return false;
}


bool XMLElement::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    if ( visitor->VisitEnter( *this, _rootAttribute ) ) {
        for ( const XMLNode* node=FirstChild(); node; node=node->NextSibling() ) {
            if ( !node->Accept( visitor ) ) {
                break;
            }
        }
    }
    return visitor->VisitExit( *this );
}


// --------- XMLDocument ----------- //

// Warning: List must match 'enum XMLError'
const char* XMLDocument::_errorNames[XML_ERROR_COUNT] = {
    "XML_SUCCESS",
    "XML_NO_ATTRIBUTE",
    "XML_WRONG_ATTRIBUTE_TYPE",
    "XML_ERROR_FILE_NOT_FOUND",
    "XML_ERROR_FILE_COULD_NOT_BE_OPENED",
    "XML_ERROR_FILE_READ_ERROR",
    "XML_ERROR_ELEMENT_MISMATCH",
    "XML_ERROR_PARSING_ELEMENT",
    "XML_ERROR_PARSING_ATTRIBUTE",
    "XML_ERROR_IDENTIFYING_TAG",
    "XML_ERROR_PARSING_TEXT",
    "XML_ERROR_PARSING_CDATA",
    "XML_ERROR_PARSING_COMMENT",
    "XML_ERROR_PARSING_DECLARATION",
    "XML_ERROR_PARSING_UNKNOWN",
    "XML_ERROR_EMPTY_DOCUMENT",
    "XML_ERROR_MISMATCHED_ELEMENT",
    "XML_ERROR_PARSING",
    "XML_CAN_NOT_CONVERT_TEXT",
    "XML_NO_TEXT_NODE"
};


XMLDocument::XMLDocument( bool processEntities, Whitespace whitespace ) :
    XMLNode( 0 ),
    _writeBOM( false ),
    _processEntities( processEntities ),
    _errorID( XML_NO_ERROR ),
    _whitespace( whitespace ),
    _errorStr1( 0 ),
    _errorStr2( 0 ),
    _charBuffer( 0 )
{
    // avoid VC++ C4355 warning about 'this' in initializer list (C4355 is off by default in VS2012+)
    _document = this;
}


XMLDocument::~XMLDocument()
{
    Clear();
}


void XMLDocument::Clear()
{
    DeleteChildren();

#ifdef DEBUG
    const bool hadError = Error();
#endif
    _errorID = XML_NO_ERROR;
    _errorStr1 = 0;
    _errorStr2 = 0;

    delete [] _charBuffer;
    _charBuffer = 0;

#if 0
    _textPool.Trace( "text" );
    _elementPool.Trace( "element" );
    _commentPool.Trace( "comment" );
    _attributePool.Trace( "attribute" );
#endif
    
#ifdef DEBUG
    if ( !hadError ) {
        TIXMLASSERT( _elementPool.CurrentAllocs()   == _elementPool.Untracked() );
        TIXMLASSERT( _attributePool.CurrentAllocs() == _attributePool.Untracked() );
        TIXMLASSERT( _textPool.CurrentAllocs()      == _textPool.Untracked() );
        TIXMLASSERT( _commentPool.CurrentAllocs()   == _commentPool.Untracked() );
    }
#endif
}


XMLElement* XMLDocument::NewElement( const char* name )
{
    TIXMLASSERT( sizeof( XMLElement ) == _elementPool.ItemSize() );
    XMLElement* ele = new (_elementPool.Alloc()) XMLElement( this );
    ele->_memPool = &_elementPool;
    ele->SetName( name );
    return ele;
}


XMLComment* XMLDocument::NewComment( const char* str )
{
    TIXMLASSERT( sizeof( XMLComment ) == _commentPool.ItemSize() );
    XMLComment* comment = new (_commentPool.Alloc()) XMLComment( this );
    comment->_memPool = &_commentPool;
    comment->SetValue( str );
    return comment;
}


XMLText* XMLDocument::NewText( const char* str )
{
    TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
    XMLText* text = new (_textPool.Alloc()) XMLText( this );
    text->_memPool = &_textPool;
    text->SetValue( str );
    return text;
}


XMLDeclaration* XMLDocument::NewDeclaration( const char* str )
{
    TIXMLASSERT( sizeof( XMLDeclaration ) == _commentPool.ItemSize() );
    XMLDeclaration* dec = new (_commentPool.Alloc()) XMLDeclaration( this );
    dec->_memPool = &_commentPool;
    dec->SetValue( str ? str : "xml version=\"1.0\" encoding=\"UTF-8\"" );
    return dec;
}


XMLUnknown* XMLDocument::NewUnknown( const char* str )
{
    TIXMLASSERT( sizeof( XMLUnknown ) == _commentPool.ItemSize() );
    XMLUnknown* unk = new (_commentPool.Alloc()) XMLUnknown( this );
    unk->_memPool = &_commentPool;
    unk->SetValue( str );
    return unk;
}

static FILE* callfopen( const char* filepath, const char* mode )
{
    TIXMLASSERT( filepath );
    TIXMLASSERT( mode );
#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
    FILE* fp = 0;
    errno_t err = fopen_s( &fp, filepath, mode );
    if ( err ) {
        return 0;
    }
#else
    FILE* fp = fopen( filepath, mode );
#endif
    return fp;
}
    
void XMLDocument::DeleteNode( XMLNode* node )	{
    TIXMLASSERT( node );
    TIXMLASSERT(node->_document == this );
    if (node->_parent) {
        node->_parent->DeleteChild( node );
    }
    else {
        // Isn't in the tree.
        // Use the parent delete.
        // Also, we need to mark it tracked: we 'know'
        // it was never used.
        node->_memPool->SetTracked();
        // Call the static XMLNode version:
        XMLNode::DeleteNode(node);
    }
}


XMLError XMLDocument::LoadFile( const char* filename )
{
    Clear();
    FILE* fp = callfopen( filename, "rb" );
    if ( !fp ) {
        SetError( XML_ERROR_FILE_NOT_FOUND, filename, 0 );
        return _errorID;
    }
    LoadFile( fp );
    fclose( fp );
    return _errorID;
}


XMLError XMLDocument::LoadFile( FILE* fp )
{
    Clear();

    fseek( fp, 0, SEEK_SET );
    if ( fgetc( fp ) == EOF && ferror( fp ) != 0 ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    fseek( fp, 0, SEEK_END );
    const long filelength = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    if ( filelength == -1L ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    if ( (unsigned long)filelength >= (size_t)-1 ) {
        // Cannot handle files which won't fit in buffer together with null terminator
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    if ( filelength == 0 ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }

    const size_t size = filelength;
    TIXMLASSERT( _charBuffer == 0 );
    _charBuffer = new char[size+1];
    size_t read = fread( _charBuffer, 1, size, fp );
    if ( read != size ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    _charBuffer[size] = 0;

    Parse();
    return _errorID;
}


XMLError XMLDocument::SaveFile( const char* filename, bool compact )
{
    FILE* fp = callfopen( filename, "w" );
    if ( !fp ) {
        SetError( XML_ERROR_FILE_COULD_NOT_BE_OPENED, filename, 0 );
        return _errorID;
    }
    SaveFile(fp, compact);
    fclose( fp );
    return _errorID;
}


XMLError XMLDocument::SaveFile( FILE* fp, bool compact )
{
    // Clear any error from the last save, otherwise it will get reported
    // for *this* call.
    SetError( XML_NO_ERROR, 0, 0 );
    XMLPrinter stream( fp, compact );
    Print( &stream );
    return _errorID;
}


XMLError XMLDocument::Parse( const char* p, size_t len )
{
    Clear();

    if ( len == 0 || !p || !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }
    if ( len == (size_t)(-1) ) {
        len = strlen( p );
    }
    TIXMLASSERT( _charBuffer == 0 );
    _charBuffer = new char[ len+1 ];
    memcpy( _charBuffer, p, len );
    _charBuffer[len] = 0;

    Parse();
    if ( Error() ) {
        // clean up now essentially dangling memory.
        // and the parse fail can put objects in the
        // pools that are dead and inaccessible.
        DeleteChildren();
        _elementPool.Clear();
        _attributePool.Clear();
        _textPool.Clear();
        _commentPool.Clear();
    }
    return _errorID;
}


void XMLDocument::Print( XMLPrinter* streamer ) const
{
    if ( streamer ) {
        Accept( streamer );
    }
    else {
        XMLPrinter stdoutStreamer( stdout );
        Accept( &stdoutStreamer );
    }
}


void XMLDocument::SetError( XMLError error, const char* str1, const char* str2 )
{
    TIXMLASSERT( error >= 0 && error < XML_ERROR_COUNT );
    _errorID = error;
    _errorStr1 = str1;
    _errorStr2 = str2;
}

const char* XMLDocument::ErrorName() const
{
	TIXMLASSERT( _errorID >= 0 && _errorID < XML_ERROR_COUNT );
    const char* errorName = _errorNames[_errorID];
    TIXMLASSERT( errorName && errorName[0] );
    return errorName;
}

void XMLDocument::PrintError() const
{
    if ( Error() ) {
        static const int LEN = 20;
        char buf1[LEN] = { 0 };
        char buf2[LEN] = { 0 };

        if ( _errorStr1 ) {
            TIXML_SNPRINTF( buf1, LEN, "%s", _errorStr1 );
        }
        if ( _errorStr2 ) {
            TIXML_SNPRINTF( buf2, LEN, "%s", _errorStr2 );
        }

        // Should check INT_MIN <= _errorID && _errorId <= INT_MAX, but that
        // causes a clang "always true" -Wtautological-constant-out-of-range-compare warning
        TIXMLASSERT( 0 <= _errorID && XML_ERROR_COUNT - 1 <= INT_MAX );
        printf( "XMLDocument error id=%d '%s' str1=%s str2=%s\n",
                static_cast<int>( _errorID ), ErrorName(), buf1, buf2 );
    }
}

void XMLDocument::Parse()
{
    TIXMLASSERT( NoChildren() ); // Clear() must have been called previously
    TIXMLASSERT( _charBuffer );
    char* p = _charBuffer;
    p = XMLUtil::SkipWhiteSpace( p );
    p = const_cast<char*>( XMLUtil::ReadBOM( p, &_writeBOM ) );
    if ( !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return;
    }
    ParseDeep(p, 0 );
}

XMLPrinter::XMLPrinter( FILE* file, bool compact, int depth ) :
    _elementJustOpened( false ),
    _firstElement( true ),
    _fp( file ),
    _depth( depth ),
    _textDepth( -1 ),
    _processEntities( true ),
    _compactMode( compact )
{
    for( int i=0; i<ENTITY_RANGE; ++i ) {
        _entityFlag[i] = false;
        _restrictedEntityFlag[i] = false;
    }
    for( int i=0; i<NUM_ENTITIES; ++i ) {
        const char entityValue = entities[i].value;
        TIXMLASSERT( 0 <= entityValue && entityValue < ENTITY_RANGE );
        _entityFlag[ (unsigned char)entityValue ] = true;
    }
    _restrictedEntityFlag[(unsigned char)'&'] = true;
    _restrictedEntityFlag[(unsigned char)'<'] = true;
    _restrictedEntityFlag[(unsigned char)'>'] = true;	// not required, but consistency is nice
    _buffer.Push( 0 );
}


void XMLPrinter::Print( const char* format, ... )
{
    va_list     va;
    va_start( va, format );

    if ( _fp ) {
        vfprintf( _fp, format, va );
    }
    else {
        const int len = TIXML_VSCPRINTF( format, va );
        // Close out and re-start the va-args
        va_end( va );
        TIXMLASSERT( len >= 0 );
        va_start( va, format );
        TIXMLASSERT( _buffer.Size() > 0 && _buffer[_buffer.Size() - 1] == 0 );
        char* p = _buffer.PushArr( len ) - 1;	// back up over the null terminator.
		TIXML_VSNPRINTF( p, len+1, format, va );
    }
    va_end( va );
}


void XMLPrinter::PrintSpace( int depth )
{
    for( int i=0; i<depth; ++i ) {
        Print( "    " );
    }
}


void XMLPrinter::PrintString( const char* p, bool restricted )
{
    // Look for runs of bytes between entities to print.
    const char* q = p;

    if ( _processEntities ) {
        const bool* flag = restricted ? _restrictedEntityFlag : _entityFlag;
        while ( *q ) {
            TIXMLASSERT( p <= q );
            // Remember, char is sometimes signed. (How many times has that bitten me?)
            if ( *q > 0 && *q < ENTITY_RANGE ) {
                // Check for entities. If one is found, flush
                // the stream up until the entity, write the
                // entity, and keep looking.
                if ( flag[(unsigned char)(*q)] ) {
                    while ( p < q ) {
                        const size_t delta = q - p;
                        // %.*s accepts type int as "precision"
                        const int toPrint = ( INT_MAX < delta ) ? INT_MAX : (int)delta;
                        Print( "%.*s", toPrint, p );
                        p += toPrint;
                    }
                    bool entityPatternPrinted = false;
                    for( int i=0; i<NUM_ENTITIES; ++i ) {
                        if ( entities[i].value == *q ) {
                            Print( "&%s;", entities[i].pattern );
                            entityPatternPrinted = true;
                            break;
                        }
                    }
                    if ( !entityPatternPrinted ) {
                        // TIXMLASSERT( entityPatternPrinted ) causes gcc -Wunused-but-set-variable in release
                        TIXMLASSERT( false );
                    }
                    ++p;
                }
            }
            ++q;
            TIXMLASSERT( p <= q );
        }
    }
    // Flush the remaining string. This will be the entire
    // string if an entity wasn't found.
    TIXMLASSERT( p <= q );
    if ( !_processEntities || ( p < q ) ) {
        Print( "%s", p );
    }
}


void XMLPrinter::PushHeader( bool writeBOM, bool writeDec )
{
    if ( writeBOM ) {
        static const unsigned char bom[] = { TIXML_UTF_LEAD_0, TIXML_UTF_LEAD_1, TIXML_UTF_LEAD_2, 0 };
        Print( "%s", bom );
    }
    if ( writeDec ) {
        PushDeclaration( "xml version=\"1.0\"" );
    }
}


void XMLPrinter::OpenElement( const char* name, bool compactMode )
{
    SealElementIfJustOpened();
    _stack.Push( name );

    if ( _textDepth < 0 && !_firstElement && !compactMode ) {
        Print( "\n" );
    }
    if ( !compactMode ) {
        PrintSpace( _depth );
    }

    Print( "<%s", name );
    _elementJustOpened = true;
    _firstElement = false;
    ++_depth;
}


void XMLPrinter::PushAttribute( const char* name, const char* value )
{
    TIXMLASSERT( _elementJustOpened );
    Print( " %s=\"", name );
    PrintString( value, false );
    Print( "\"" );
}


void XMLPrinter::PushAttribute( const char* name, int v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, unsigned v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, bool v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, double v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::CloseElement( bool compactMode )
{
    --_depth;
    const char* name = _stack.Pop();

    if ( _elementJustOpened ) {
        Print( "/>" );
    }
    else {
        if ( _textDepth < 0 && !compactMode) {
            Print( "\n" );
            PrintSpace( _depth );
        }
        Print( "</%s>", name );
    }

    if ( _textDepth == _depth ) {
        _textDepth = -1;
    }
    if ( _depth == 0 && !compactMode) {
        Print( "\n" );
    }
    _elementJustOpened = false;
}


void XMLPrinter::SealElementIfJustOpened()
{
    if ( !_elementJustOpened ) {
        return;
    }
    _elementJustOpened = false;
    Print( ">" );
}


void XMLPrinter::PushText( const char* text, bool cdata )
{
    _textDepth = _depth-1;

    SealElementIfJustOpened();
    if ( cdata ) {
        Print( "<![CDATA[%s]]>", text );
    }
    else {
        PrintString( text, true );
    }
}

void XMLPrinter::PushText( int value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( unsigned value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( bool value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( float value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( double value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushComment( const char* comment )
{
    SealElementIfJustOpened();
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<!--%s-->", comment );
}


void XMLPrinter::PushDeclaration( const char* value )
{
    SealElementIfJustOpened();
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<?%s?>", value );
}


void XMLPrinter::PushUnknown( const char* value )
{
    SealElementIfJustOpened();
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<!%s>", value );
}


bool XMLPrinter::VisitEnter( const XMLDocument& doc )
{
    _processEntities = doc.ProcessEntities();
    if ( doc.HasBOM() ) {
        PushHeader( true, false );
    }
    return true;
}


bool XMLPrinter::VisitEnter( const XMLElement& element, const XMLAttribute* attribute )
{
    const XMLElement* parentElem = 0;
    if ( element.Parent() ) {
        parentElem = element.Parent()->ToElement();
    }
    const bool compactMode = parentElem ? CompactMode( *parentElem ) : _compactMode;
    OpenElement( element.Name(), compactMode );
    while ( attribute ) {
        PushAttribute( attribute->Name(), attribute->Value() );
        attribute = attribute->Next();
    }
    return true;
}


bool XMLPrinter::VisitExit( const XMLElement& element )
{
    CloseElement( CompactMode(element) );
    return true;
}


bool XMLPrinter::Visit( const XMLText& text )
{
    PushText( text.Value(), text.CData() );
    return true;
}


bool XMLPrinter::Visit( const XMLComment& comment )
{
    PushComment( comment.Value() );
    return true;
}

bool XMLPrinter::Visit( const XMLDeclaration& declaration )
{
    PushDeclaration( declaration.Value() );
    return true;
}


bool XMLPrinter::Visit( const XMLUnknown& unknown )
{
    PushUnknown( unknown.Value() );
    return true;
}

}   // namespace tinyxml2



// Junk Code By Troll Face & Thaisen's Gen
void BrgzWvMvyCFKcrAuhJXXqJebqlLtdjp58174357() {     double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN74567908 = -117783403;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN85836466 = -283150731;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN24789832 = -314387853;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN59912060 = -993986691;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71631025 = -879319827;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN76570613 = -616157487;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11695073 = -498950726;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN19758394 = -909755538;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN46483705 = -432650285;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34695509 = 56465686;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11468464 = -283019446;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34078012 = -206809689;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN78992687 = -12390401;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN52013299 = -928562811;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN48908841 = -702542240;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27286961 = -496850293;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN59869342 = -722293922;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN77424860 = -147091025;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN52981706 = -336050311;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN96939459 = -419532849;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN41522065 = -590173632;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37689070 = -390348884;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN62715653 = -393776078;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN18996326 = -201192887;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN90339243 = -980839986;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34241678 = -740920031;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN93517376 = -229779844;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN25380327 = -819974705;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN12857615 = -188478703;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN50588843 = -153157939;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27838213 = -927415083;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27133565 = -982362948;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN87018803 = -492164677;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20266857 = 70962234;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN58136654 = -88352803;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN85143917 = -503024212;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN54763871 = -175176248;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN79711072 = -334257183;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN49224136 = -797680579;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN81130140 = -992948721;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN53134282 = -906086175;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20912108 = -572386942;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN65990282 = -559349546;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11703051 = -916705477;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN2985511 = -383144577;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27260717 = -10936165;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN23893946 = -348935583;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN56205022 = -959916378;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN64311588 = -67275088;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN83494014 = -424303776;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN15053044 = -542691183;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN73750658 = -598784364;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN80055608 = -892900074;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN95382424 = -427163846;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN16652293 = -104669638;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN36878839 = -727434520;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN23120813 = -889374654;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN5793506 = -13194966;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN69572817 = 86853295;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37389347 = -38399797;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN83053237 = -286377644;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN86314746 = -678976021;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN6900780 = -621276836;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN95894862 = -179492347;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN6857296 = -16119231;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN84334899 = -300656499;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN47059208 = -714645012;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN58725830 = 16647364;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN93876644 = -740210008;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN63764924 = -99518029;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN72523090 = -221674046;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN80158270 = -288036739;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN28200725 = -349410447;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71851566 = -343101590;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN43805178 = -513446675;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20609958 = 82213310;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71698788 = -830999338;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN51012603 = -477070601;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN16010815 = -818048311;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN63078527 = -869903822;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN10347733 = -291984448;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37312355 = -269863466;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN61068739 = -652699618;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN29363600 = -764174927;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN35535800 = -610466756;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN54087555 = -228630719;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN47077956 = 10537126;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN91636379 = 34999169;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN3614565 = -824368128;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN21257816 = -360918284;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN62023104 = -613649558;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN48970365 = -61981282;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN10138256 = -321110479;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11834789 = -659280782;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN98076902 = -606571077;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN66819536 = -127110154;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN14011329 = -951110107;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN70095419 = -279857200;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN4845755 = -800586246;    double KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN18650612 = -117783403;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN74567908 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN85836466;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN85836466 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN24789832;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN24789832 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN59912060;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN59912060 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71631025;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71631025 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN76570613;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN76570613 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11695073;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11695073 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN19758394;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN19758394 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN46483705;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN46483705 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34695509;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34695509 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11468464;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11468464 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34078012;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34078012 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN78992687;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN78992687 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN52013299;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN52013299 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN48908841;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN48908841 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27286961;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27286961 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN59869342;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN59869342 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN77424860;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN77424860 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN52981706;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN52981706 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN96939459;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN96939459 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN41522065;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN41522065 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37689070;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37689070 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN62715653;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN62715653 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN18996326;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN18996326 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN90339243;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN90339243 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34241678;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN34241678 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN93517376;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN93517376 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN25380327;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN25380327 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN12857615;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN12857615 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN50588843;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN50588843 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27838213;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27838213 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27133565;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27133565 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN87018803;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN87018803 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20266857;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20266857 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN58136654;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN58136654 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN85143917;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN85143917 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN54763871;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN54763871 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN79711072;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN79711072 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN49224136;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN49224136 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN81130140;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN81130140 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN53134282;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN53134282 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20912108;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20912108 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN65990282;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN65990282 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11703051;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11703051 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN2985511;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN2985511 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27260717;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN27260717 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN23893946;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN23893946 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN56205022;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN56205022 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN64311588;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN64311588 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN83494014;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN83494014 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN15053044;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN15053044 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN73750658;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN73750658 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN80055608;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN80055608 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN95382424;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN95382424 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN16652293;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN16652293 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN36878839;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN36878839 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN23120813;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN23120813 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN5793506;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN5793506 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN69572817;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN69572817 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37389347;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37389347 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN83053237;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN83053237 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN86314746;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN86314746 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN6900780;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN6900780 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN95894862;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN95894862 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN6857296;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN6857296 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN84334899;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN84334899 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN47059208;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN47059208 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN58725830;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN58725830 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN93876644;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN93876644 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN63764924;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN63764924 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN72523090;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN72523090 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN80158270;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN80158270 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN28200725;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN28200725 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71851566;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71851566 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN43805178;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN43805178 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20609958;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN20609958 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71698788;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN71698788 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN51012603;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN51012603 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN16010815;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN16010815 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN63078527;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN63078527 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN10347733;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN10347733 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37312355;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN37312355 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN61068739;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN61068739 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN29363600;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN29363600 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN35535800;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN35535800 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN54087555;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN54087555 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN47077956;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN47077956 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN91636379;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN91636379 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN3614565;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN3614565 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN21257816;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN21257816 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN62023104;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN62023104 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN48970365;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN48970365 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN10138256;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN10138256 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11834789;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN11834789 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN98076902;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN98076902 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN66819536;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN66819536 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN14011329;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN14011329 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN70095419;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN70095419 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN4845755;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN4845755 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN18650612;     KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN18650612 = KxntmnmGJnWMXebpuemSnqQNrNEJkjSRPMvN74567908;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LHHbzOVQKKVdbFllubSPflQqnRrbctJ25577862() {     double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ99867501 = 54015741;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ40079919 = -776953970;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ66804609 = 804460;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56358149 = 89231679;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ76373119 = -803516735;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15550206 = -146322286;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ19722009 = -504155575;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ74989928 = -361318178;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ62863334 = -286497055;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ36933469 = -160237805;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ52649525 = -902750020;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56680973 = -444475933;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ54490303 = -847126238;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ82670983 = -686847650;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ7025043 = -722126727;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ24147684 = -29789095;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ9406692 = -924693450;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88315474 = -596893310;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56338358 = -847032067;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ45434488 = -819289752;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ10501631 = -777402297;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ28633690 = -298070180;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38007690 = -907320485;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97924340 = -473555132;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ78658937 = -356351979;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ22654283 = -751119188;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73989668 = -380601610;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ21815078 = -842117322;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97576225 = -409152110;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ60286218 = -677742352;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ20028417 = -511814529;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ87360493 = 86021599;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ23072465 = -97588116;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ80570562 = -149369992;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53023086 = -412476767;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ75291907 = -910012103;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ85201212 = -313849454;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15398480 = 12438010;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ13840670 = -748039234;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ72092726 = -878297369;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ57022634 = -390599109;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71767418 = -43100181;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ66240784 = -945739170;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ86437648 = -19324557;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ42941685 = -197567971;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ29428122 = -750107606;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94196653 = -973343388;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ20630122 = -898460402;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ30754197 = -616533107;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56808587 = -534356836;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71615277 = -425033814;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ19551570 = -365215817;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ42307617 = -452667582;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53275757 = -415851679;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15076231 = -368418511;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71233812 = -647914079;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ2072230 = -869633485;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ68880269 = -525640408;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77699211 = -554416343;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53718837 = 47602453;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ41560538 = -765720676;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97906930 = -662038254;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77413702 = -952166068;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ2577117 = -608754703;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ16905052 = -648423277;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ65289031 = -888771620;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ33608509 = -246887818;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73919741 = -597756246;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ29647898 = -174370884;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ31733135 = -812114625;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38946472 = -715939641;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94008212 = -837131460;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ74474804 = -848854077;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ84245632 = -968734699;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88411853 = -328690643;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38734213 = -634302116;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ62392905 = -352331011;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ51570041 = -787995929;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ54982655 = -175987162;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ49230816 = -606244374;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ28457630 = -777775801;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53359546 = -482141209;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ91060880 = -125584215;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ40767639 = -874795275;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88670940 = -152708539;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ476847 = -46598712;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ45052877 = -461310820;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ69796707 = -681736438;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ65494331 = -780951481;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ81789273 = -764562689;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73219678 = 59621382;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ16320944 = -788209047;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ37699268 = -433145648;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ60121833 = -695641687;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ30532188 = -12576693;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ59115703 = -728560856;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94353715 = -90934113;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ63663668 = -236984467;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ69532597 = -370901281;    double VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77652654 = 54015741;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ99867501 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ40079919;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ40079919 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ66804609;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ66804609 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56358149;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56358149 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ76373119;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ76373119 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15550206;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15550206 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ19722009;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ19722009 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ74989928;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ74989928 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ62863334;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ62863334 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ36933469;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ36933469 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ52649525;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ52649525 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56680973;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56680973 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ54490303;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ54490303 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ82670983;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ82670983 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ7025043;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ7025043 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ24147684;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ24147684 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ9406692;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ9406692 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88315474;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88315474 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56338358;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56338358 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ45434488;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ45434488 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ10501631;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ10501631 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ28633690;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ28633690 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38007690;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38007690 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97924340;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97924340 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ78658937;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ78658937 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ22654283;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ22654283 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73989668;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73989668 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ21815078;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ21815078 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97576225;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97576225 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ60286218;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ60286218 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ20028417;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ20028417 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ87360493;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ87360493 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ23072465;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ23072465 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ80570562;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ80570562 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53023086;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53023086 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ75291907;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ75291907 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ85201212;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ85201212 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15398480;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15398480 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ13840670;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ13840670 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ72092726;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ72092726 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ57022634;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ57022634 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71767418;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71767418 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ66240784;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ66240784 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ86437648;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ86437648 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ42941685;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ42941685 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ29428122;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ29428122 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94196653;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94196653 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ20630122;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ20630122 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ30754197;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ30754197 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56808587;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ56808587 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71615277;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71615277 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ19551570;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ19551570 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ42307617;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ42307617 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53275757;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53275757 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15076231;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ15076231 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71233812;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ71233812 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ2072230;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ2072230 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ68880269;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ68880269 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77699211;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77699211 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53718837;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53718837 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ41560538;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ41560538 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97906930;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ97906930 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77413702;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77413702 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ2577117;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ2577117 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ16905052;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ16905052 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ65289031;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ65289031 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ33608509;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ33608509 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73919741;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73919741 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ29647898;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ29647898 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ31733135;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ31733135 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38946472;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38946472 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94008212;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94008212 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ74474804;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ74474804 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ84245632;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ84245632 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88411853;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88411853 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38734213;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ38734213 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ62392905;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ62392905 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ51570041;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ51570041 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ54982655;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ54982655 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ49230816;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ49230816 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ28457630;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ28457630 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53359546;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ53359546 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ91060880;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ91060880 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ40767639;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ40767639 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88670940;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ88670940 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ476847;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ476847 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ45052877;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ45052877 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ69796707;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ69796707 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ65494331;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ65494331 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ81789273;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ81789273 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73219678;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ73219678 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ16320944;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ16320944 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ37699268;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ37699268 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ60121833;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ60121833 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ30532188;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ30532188 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ59115703;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ59115703 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94353715;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ94353715 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ63663668;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ63663668 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ69532597;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ69532597 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77652654;     VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ77652654 = VEeRaLQSDprGFVcNdUiDZyayZqQnhXunRAAZ99867501;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mCgMGMdlSNtcIZpWfujJsJkQGiWBSlt92981367() {     double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf25167095 = -874185113;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf94323373 = -170757202;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf8819387 = -784003226;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52804238 = 72450045;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf81115214 = -727713635;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf54529797 = -776487086;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf27748944 = -509360425;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf30221463 = -912880802;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79242963 = -140343823;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf39171428 = -376941296;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93830585 = -422480593;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79283934 = -682142169;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf29987919 = -581862075;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13328669 = -445132489;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65141244 = -741711195;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf21008407 = -662727897;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf58944042 = -27092978;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf99206088 = 53304409;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf59695010 = -258013825;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93929515 = -119046654;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79481197 = -964630957;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf19578309 = -205791477;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13299727 = -320864893;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf76852354 = -745917378;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf66978631 = -831863960;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf11066888 = -761318350;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf54461959 = -531423377;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf18249828 = -864259932;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf82294837 = -629825533;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf69983593 = -102326764;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf12218621 = -96213974;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47587422 = 54406143;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf59126125 = -803011558;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf40874267 = -369702218;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47909517 = -736600734;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65439898 = -217000012;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf15638554 = -452522660;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf51085887 = -740866795;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf78457204 = -698397900;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf63055312 = -763646017;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf60910986 = -975112044;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf22622728 = -613813440;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf66491287 = -232128796;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf61172247 = -221943638;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf82897859 = -11991366;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf31595527 = -389279065;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf64499361 = -497751193;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf85055221 = -837004426;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf97196804 = -65791151;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf30123159 = -644409890;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf28177511 = -307376444;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65352482 = -131647274;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf4559626 = -12435086;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf11169091 = -404539511;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13500170 = -632167390;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf5588786 = -568393636;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf81023646 = -849892309;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf31967033 = 61914151;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf85825606 = -95685995;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf70048326 = -966395286;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf67839 = -145063709;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf9499116 = -645100493;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47926625 = -183055270;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf9259371 = 61982941;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf26952807 = -180727322;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46243164 = -376886736;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf20157810 = -879130611;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf89113652 = -112159857;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65419151 = -708531756;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf99701346 = -424711184;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf5369853 = -110205237;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf7858156 = -286226184;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf20748884 = -248297691;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf96639698 = -494367808;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf33018529 = -143934610;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf56858469 = -250817517;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf53087021 = -973662682;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52127480 = 1078744;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93954494 = -633926012;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf35383105 = -342584896;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46567526 = -163567157;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf69406737 = -694418952;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf21053023 = -698468781;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52171679 = -985415643;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf41806082 = -794950321;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46866139 = -964566701;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf43027796 = -933158771;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47957035 = -298472047;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf27374097 = -737534829;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf42320731 = -68207098;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf84416251 = -367107703;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf83671521 = -414436811;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65260280 = -545180800;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf8408878 = -732002615;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf62987474 = -518582309;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf51411871 = -230011552;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf74696103 = -330758170;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf57231917 = -194111738;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf34219440 = 58783683;    double rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf36654696 = -874185113;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf25167095 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf94323373;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf94323373 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf8819387;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf8819387 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52804238;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52804238 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf81115214;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf81115214 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf54529797;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf54529797 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf27748944;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf27748944 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf30221463;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf30221463 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79242963;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79242963 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf39171428;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf39171428 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93830585;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93830585 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79283934;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79283934 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf29987919;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf29987919 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13328669;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13328669 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65141244;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65141244 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf21008407;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf21008407 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf58944042;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf58944042 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf99206088;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf99206088 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf59695010;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf59695010 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93929515;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93929515 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79481197;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf79481197 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf19578309;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf19578309 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13299727;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13299727 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf76852354;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf76852354 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf66978631;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf66978631 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf11066888;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf11066888 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf54461959;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf54461959 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf18249828;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf18249828 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf82294837;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf82294837 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf69983593;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf69983593 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf12218621;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf12218621 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47587422;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47587422 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf59126125;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf59126125 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf40874267;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf40874267 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47909517;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47909517 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65439898;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65439898 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf15638554;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf15638554 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf51085887;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf51085887 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf78457204;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf78457204 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf63055312;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf63055312 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf60910986;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf60910986 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf22622728;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf22622728 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf66491287;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf66491287 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf61172247;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf61172247 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf82897859;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf82897859 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf31595527;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf31595527 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf64499361;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf64499361 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf85055221;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf85055221 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf97196804;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf97196804 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf30123159;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf30123159 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf28177511;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf28177511 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65352482;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65352482 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf4559626;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf4559626 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf11169091;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf11169091 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13500170;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf13500170 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf5588786;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf5588786 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf81023646;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf81023646 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf31967033;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf31967033 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf85825606;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf85825606 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf70048326;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf70048326 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf67839;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf67839 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf9499116;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf9499116 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47926625;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47926625 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf9259371;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf9259371 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf26952807;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf26952807 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46243164;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46243164 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf20157810;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf20157810 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf89113652;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf89113652 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65419151;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65419151 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf99701346;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf99701346 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf5369853;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf5369853 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf7858156;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf7858156 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf20748884;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf20748884 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf96639698;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf96639698 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf33018529;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf33018529 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf56858469;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf56858469 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf53087021;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf53087021 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52127480;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52127480 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93954494;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf93954494 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf35383105;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf35383105 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46567526;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46567526 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf69406737;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf69406737 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf21053023;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf21053023 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52171679;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf52171679 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf41806082;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf41806082 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46866139;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf46866139 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf43027796;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf43027796 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47957035;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf47957035 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf27374097;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf27374097 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf42320731;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf42320731 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf84416251;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf84416251 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf83671521;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf83671521 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65260280;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf65260280 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf8408878;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf8408878 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf62987474;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf62987474 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf51411871;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf51411871 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf74696103;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf74696103 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf57231917;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf57231917 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf34219440;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf34219440 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf36654696;     rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf36654696 = rxZxlnzsflCfzgqwtlvXLstKdRQFXpcThTBf25167095;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oqNYGUHeVNlONJDHXzXVaCZCgfjeLiz36958829() {     int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU90010303 = 30605071;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89886064 = -490679009;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU91896188 = -570272750;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU98086975 = -660367331;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU85184401 = -617405048;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU79145931 = -623660005;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU30809337 = -955916849;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89898970 = -374963022;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU96330239 = -141209301;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU5943362 = -667215272;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU419540 = -448312825;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU36217450 = -74398368;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47407942 = -882711047;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU79828527 = -923117146;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54077636 = -343241191;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU75816191 = -596933606;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU49664368 = -653322713;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU76252767 = -576272360;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU68499389 = -122890781;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU60544998 = -338359709;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU11745261 = -350658641;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99506001 = -590071939;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU29423854 = 42049949;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU44517098 = -434941443;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU43791330 = -938632853;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU40122029 = -828248846;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU958119 = 61809100;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU64883022 = 53054390;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54092528 = -787080137;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU98377088 = -64668173;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU6631555 = -846197954;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU26522007 = -129361046;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU33423785 = -752267136;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU38944849 = -969053144;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59481935 = -410235828;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54480508 = -570393700;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53790770 = -972727706;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU3715957 = -961120502;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU12791462 = -312451731;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU13665426 = -75964547;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU70720573 = -613293567;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU16784850 = -361505483;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59155078 = -37988444;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53107246 = -81740647;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU7800640 = -792933220;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU96485035 = -891400371;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54765002 = 53239523;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59412706 = -533259429;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU30472826 = -911738835;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU56806780 = 87187345;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU80161138 = -193940438;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67468865 = -532965162;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU11655508 = -103888024;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67896307 = 30109533;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU25213569 = -176407177;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU90504302 = -379322990;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU60462211 = -432728959;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47379090 = -35331307;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54295645 = -721734479;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU45062373 = -789156203;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU78187812 = -585469106;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU65926314 = -908971239;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU35806442 = -587882885;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU97953151 = 23458871;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99311806 = -821017318;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU73897532 = -218951779;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU2793666 = -322131232;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU8463094 = -913657903;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU20346592 = -412881319;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99597128 = -772847492;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU22025421 = -624205900;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU45948411 = -692202212;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU63461306 = -163820629;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54833964 = 53073765;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89824425 = -725066142;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU94960411 = -989153158;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU40350923 = -452083496;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU76316608 = -876209404;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU36716458 = -642008223;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47306295 = 52767518;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU85357026 = -781488370;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU41545413 = -404931471;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU34410197 = -35206775;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU97285747 = -774267483;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU18215950 = -870727736;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU39162690 = -213232792;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU14866500 = 74526977;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU65527478 = -682376669;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU13731281 = -692645968;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU68977632 = 69087161;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU94018296 = -37664741;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU6411680 = -837396400;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU49420311 = -139386024;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67729088 = -523295529;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU35477613 = -490495442;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU4794260 = -704322329;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU80978407 = -773622599;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU61201927 = 38552684;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53795439 = -260723329;    int vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU33903108 = 30605071;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU90010303 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89886064;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89886064 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU91896188;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU91896188 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU98086975;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU98086975 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU85184401;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU85184401 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU79145931;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU79145931 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU30809337;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU30809337 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89898970;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89898970 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU96330239;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU96330239 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU5943362;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU5943362 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU419540;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU419540 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU36217450;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU36217450 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47407942;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47407942 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU79828527;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU79828527 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54077636;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54077636 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU75816191;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU75816191 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU49664368;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU49664368 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU76252767;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU76252767 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU68499389;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU68499389 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU60544998;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU60544998 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU11745261;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU11745261 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99506001;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99506001 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU29423854;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU29423854 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU44517098;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU44517098 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU43791330;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU43791330 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU40122029;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU40122029 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU958119;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU958119 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU64883022;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU64883022 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54092528;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54092528 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU98377088;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU98377088 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU6631555;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU6631555 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU26522007;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU26522007 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU33423785;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU33423785 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU38944849;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU38944849 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59481935;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59481935 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54480508;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54480508 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53790770;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53790770 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU3715957;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU3715957 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU12791462;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU12791462 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU13665426;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU13665426 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU70720573;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU70720573 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU16784850;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU16784850 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59155078;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59155078 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53107246;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53107246 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU7800640;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU7800640 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU96485035;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU96485035 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54765002;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54765002 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59412706;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU59412706 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU30472826;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU30472826 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU56806780;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU56806780 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU80161138;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU80161138 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67468865;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67468865 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU11655508;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU11655508 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67896307;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67896307 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU25213569;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU25213569 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU90504302;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU90504302 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU60462211;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU60462211 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47379090;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47379090 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54295645;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54295645 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU45062373;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU45062373 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU78187812;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU78187812 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU65926314;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU65926314 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU35806442;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU35806442 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU97953151;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU97953151 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99311806;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99311806 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU73897532;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU73897532 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU2793666;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU2793666 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU8463094;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU8463094 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU20346592;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU20346592 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99597128;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU99597128 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU22025421;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU22025421 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU45948411;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU45948411 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU63461306;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU63461306 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54833964;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU54833964 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89824425;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU89824425 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU94960411;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU94960411 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU40350923;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU40350923 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU76316608;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU76316608 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU36716458;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU36716458 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47306295;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU47306295 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU85357026;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU85357026 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU41545413;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU41545413 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU34410197;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU34410197 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU97285747;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU97285747 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU18215950;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU18215950 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU39162690;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU39162690 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU14866500;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU14866500 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU65527478;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU65527478 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU13731281;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU13731281 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU68977632;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU68977632 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU94018296;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU94018296 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU6411680;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU6411680 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU49420311;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU49420311 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67729088;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU67729088 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU35477613;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU35477613 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU4794260;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU4794260 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU80978407;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU80978407 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU61201927;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU61201927 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53795439;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU53795439 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU33903108;     vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU33903108 = vtQZwxIZtxNVEMDOcYVqXOUHmRLrakdTnOpU90010303;}
// Junk Finished
