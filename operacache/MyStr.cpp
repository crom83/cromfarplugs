#include "StdAfx.h"
#include "MyStr.h"

CString ExtractName(CString Path, bool bRev)
{
  if (Path.IsEmpty())
    return _T("");

	bool endslash = false;
	if (bRev)
		endslash = Path.Right(1) == _T('/');
	else
		endslash = Path.Right(1) == _T('\\');

	if (endslash)
		Path = Path.Left(Path.GetLength()-1);

  int pos = -1;
	if (bRev)
		pos = Path.ReverseFind(_T('/'));
	else
		pos = Path.ReverseFind(_T('\\'));

  if (pos != -1)
    return Path.Right(Path.GetLength()-pos-1);
  return Path;
}
CString ExtractPath( CString Path )
{
  if (Path.IsEmpty())
    return _T("");

  if (Path.Right(1) == _T('\\'))
    Path = Path.Left(Path.GetLength()-1);

  int pos = Path.ReverseFind(_T('\\'));
  if (pos != -1)
    return Path.Left(pos);
  return _T("");
}
CString ExtractExt( CString Path )
{
  if (Path.IsEmpty())
    return _T("");

  if (Path.Right(1) == _T('\\'))
    Path = Path.Left(Path.GetLength()-1);

  int pos = Path.ReverseFind(_T('.'));
  if (pos != -1)
    return Path.Right(Path.GetLength()-pos-1);
  return Path;
}
bool TestExt( CString ext, CString extList )
{
  ext.MakeLower();
  ext.Insert(0, _T("*."));
  ext += _T(";");
  return extList.Find(ext) != -1;
}
void Tokenize(CString str,  strvec& tokens, const CString& sep /*= " "*/)
{
  str.TrimLeft();
  str.TrimRight();
  int i = 0;
  int j = 0;
  tokens.RemoveAll();
  while (i != -1)
  {
    j = str.Find(sep, i + sep.GetLength());
    CString mid;
    if (j != -1)  mid = str.Mid(i, j-i);
    else          mid = str.Mid(i);
    mid.TrimLeft();
    mid.TrimRight();
    if (!mid.IsEmpty()) tokens.Add(mid);
    i = j;
  }
}
void TokensToParams( strvec& tokens, strmap& params )
{
  params.RemoveAll();
  int from = 0;
  if(tokens[0].Left(1) != _T("/"))
  {
    params.Add(_T("filename"), Unquote(tokens[0]));
    from++;
  }
  for (int i = from; i < tokens.GetSize(); i++)
  {
    CString token = tokens[i];
    if (token.Left(1) == _T("/")) token.Delete(0);
    int j = token.Find(_T(":"));
    if (j == -1)
    {
      CString opt = token;
      opt.TrimLeft();
      opt.TrimRight();
      opt.MakeLower();
      params.Add(opt, _T(""));
    } else {
      CString opt = token.Left(j);
      opt.TrimLeft();
      opt.TrimRight();
      opt.MakeLower();
      CString param = token.Mid(j+1);
      param.TrimLeft();
      param.TrimRight();
      param.MakeLower();
      params.Add(opt, Unquote(param));
    }
  }
}
CString & Unquote( CString &str )
{
  if (!str.IsEmpty() && (str.Left(1)  == _T("\""))) str.Delete(0);
  if (!str.IsEmpty() && (str.Right(1) == _T("\""))) str.Delete(str.GetLength()-1);
  return str;
}
CString GetParam( strmap& params, CString opt  )
{
  int i = params.FindKey(opt);
  if (i == -1)
    return "";

  return params.GetValueAt(i);
}
bool ExistsParam( strmap& params, CString opt )
{
  int i = params.FindKey(opt);
  if (i == -1)
    return false;

  return true;
}
int utf8ToUnicode ( const char* lpMultiByteStr, WCHAR* lpWideCharStr, int cmbChars )
{
  const unsigned char*    pmb = (unsigned char  *)lpMultiByteStr;
  unsigned short*         pwc = (unsigned short *)lpWideCharStr;
  const unsigned char*    pmbe;
  int  cwChars = 0;

  if ( cmbChars >= 0 )  pmbe = pmb + cmbChars;
  else                  pmbe = NULL;

  while ( (pmbe == NULL) || (pmb < pmbe) )
  {
    char            mb = *pmb++;
    unsigned int    cc = 0;
    unsigned int    wc;

    while ( (cc < 7) && (mb & (1 << (7 - cc)))) cc++;
    if ( cc == 1 || cc > 6 ) continue; // illegal character combination for UTF-8
    if ( cc == 0 ) wc = mb;
    else {
      wc = (mb & ((1 << (7 - cc)) - 1)) << ((cc - 1) * 6);
      while ( --cc > 0 )
      {
        if ( pmb == pmbe ) return cwChars;// reached end of the buffer
        mb = *pmb++;
        if ( ((mb >> 6) & 0x03) != 2 ) return cwChars;// not part of multibyte character
        wc |= (mb & 0x3F) << ((cc - 1) * 6);
      }
    }

    if ( wc & 0xFFFF0000 )  wc = L'?';
    *pwc++ = wc;
    cwChars++;
    if ( wc == L'\0' )  return cwChars;
  }

  return cwChars;
}
/*bool UnicodeToutf8(const wchar_t* wcs, size_t length, byte *outbuf, size_t outlength)
{
	//WideCharToMultiByte(CP_UTF8, 0, wcs, length, (LPSTR)outbuf, outlength, NULL, NULL);
	const wchar_t *pc = wcs;
	const wchar_t *end = pc + length;
	UINT  num_errors = 0;
	int pos = 0;
	for(unsigned int c = *pc; pc < end ; c = *(++pc)) 
	{
		if (c < (1 << 7)) 
		{
			//outbuf.push(byte(c));
			outbuf[pos++] = byte(c);
		} 
		else if (c < (1 << 11)) 
		{
			//outbuf.push(byte((c >> 6) | 0xc0));
			//outbuf.push(byte((c & 0x3f) | 0x80));
			outbuf[pos++] = byte((c >> 6) | 0xc0);
			outbuf[pos++] = byte((c & 0x3f) | 0x80);
		} 
		else if (c < (1 << 16)) 
		{
			//outbuf.push(byte((c >> 12) | 0xe0));
			//outbuf.push(byte(((c >> 6) & 0x3f) | 0x80));
			//outbuf.push(byte((c & 0x3f) | 0x80));
			outbuf[pos++] = byte((c >> 12) | 0xe0);
			outbuf[pos++] = byte(((c >> 6) & 0x3f) | 0x80);
			outbuf[pos++] = byte((c & 0x3f) | 0x80);
		} 
		else if (c < (1 << 21)) 
		{
			//outbuf.push(byte((c >> 18) | 0xf0));
			//outbuf.push(byte(((c >> 12) & 0x3f) | 0x80));
			//outbuf.push(byte(((c >> 6) & 0x3f) | 0x80));
			//outbuf.push(byte((c & 0x3f) | 0x80));
			outbuf[pos++] = byte((c >> 18) | 0xf0);
			outbuf[pos++] = byte(((c >> 12) & 0x3f) | 0x80);
			outbuf[pos++] = byte(((c >> 6) & 0x3f) | 0x80);
			outbuf[pos++] = byte((c & 0x3f) | 0x80);
		}
		else 
			++num_errors;
	}
	return num_errors == 0;
	//return 0;
}*/
void AddEndSlash( CString & str )
{
  if (str.IsEmpty()) str = _T("\\");
  if (str.Right(1) != _T("\\")) str += _T("\\");
}
int __cdecl my_wcstombsz(char* mbstr, const wchar_t* wcstr, size_t count, UINT CodePage)
{
  if (count == 0 && mbstr != NULL)
    return 0;

  int result = ::WideCharToMultiByte(CodePage, 0, wcstr, -1, mbstr, (int)count, NULL, NULL);
  ATLASSERT(mbstr == NULL || result <= (int)count);
  if (result > 0)
    mbstr[result - 1] = 0;
  return result;
}
int __cdecl my_mbstowcsz(wchar_t* wcstr, const char* mbstr, size_t count, UINT CodePage)
{
  if (count == 0 && wcstr != NULL)
    return 0;

  int result = ::MultiByteToWideChar(CodePage, 0, mbstr, -1, wcstr, (int)count);
  ATLASSERT(wcstr == NULL || result <= (int)count);
  if (result > 0)
    wcstr[result - 1] = 0;
  return result;
}
char * getAnsiString( const CString& val )
{
  LPCTSTR str = val;
  int len = val.GetLength();
#ifdef _UNICODE
  wchar_t * wc = (wchar_t *)(LPCTSTR)val;
  char * ansi = (char*)my_malloc(len + 1);
  my_wcstombsz(ansi, wc, len);
  return ansi;
#else
  #error "TODO!!!"
#endif
}
char * getOemString( const CString& val )
{
  LPCTSTR str = val;
  int len = val.GetLength();
#ifdef _UNICODE
  wchar_t * wc = (wchar_t *)(LPCTSTR)val;
  char * ansi = (char*)my_malloc(len + 1);
  my_wcstombsz(ansi, wc, len, CP_ACP);
  ::AnsiToOem(ansi, ansi);
  return ansi;
#else
  #error "TODO!!!"
#endif
}
void strcpyC( char * dst, const CString& src, bool Ansi )
{
  char * data;
  if (Ansi)
    data = getAnsiString(src);
  else
    data = getOemString(src);
  lstrcpyA(dst, data);
  my_free(data);
}
CString ExtractUrl(const CString &Path )
{
	int i = Path.Find(_T("://"));
	if (i != -1)
		return Path.Left(i);
	else
		return _T("");
}
void ExtractURLParts(const CString &url, CString *proto, CString *serv, CString *port, CString *path)
{
	int i = url.Find(_T("://"));
	if (i != -1 && proto)
		*proto = url.Left(i);

	if (i == -1)	i = 0;
	if (i != 0) i += 3;
	int j = url.Find(_T("/"), i);
	if (j != -1)
	{
		if (path)
			*path = url.Mid(j+1);
		CString firstpart = url.Mid(i, j-i);
		int k = firstpart.Find(_T(":"));
		if (k != -1)
		{
			if (port)
				*port = firstpart.Mid(k+1);
			if (serv)
				*serv = firstpart.Left(k);
		}
		else if (serv)
			*serv = firstpart;
	}
}
CString URLEncode(char * str)
{
	CString res;
	int len = lstrlenA(str);
	for (int i = 0; i < len; i++)
	{
		if ((str[i] >= 'A' && str[i] <= 'Z')||
				(str[i] >= 'a' && str[i] <= 'z')||
				(str[i] >= '0' && str[i] <= '9'))
			res += str[i];
		else
		{
			CString enc;
			if (str[i] == ' ')
				enc = _T("+");
			else
			{
				enc.Format(_T("%%%X"), str[i]);
				if (enc.GetLength() > 3)
					enc.Delete(1, 6);
			}
			res += enc;
		}
	}
	return res;
}
CString GetExtensionByMIME(const CString &MIME)
{
	CString path;
	if (MIME == _T("application/x-javascript")||
			MIME == _T("text/javascript") ||
			MIME == _T("application/javascript"))
		return _T(".js");

	if (MIME == _T("application/json"))
		return _T(".json");

	if (MIME == _T("video/flv") ||
			MIME == _T("video/x-flv"))
		return _T(".flv");

	path.Format(_T("MIME\\Database\\Content Type\\%s"), MIME);
	CString res;
	GetRegKey(HKEY_CLASSES_ROOT, TRUE, path, _T("Extension"), res, _T(""));
	return res;
}

CString GenShortName(const CString &url, BOOL AddExt, BOOL NameFromCache, const CString &MIME)
{
	CString path, serv;

	if (!NameFromCache)
	{
		ExtractURLParts(url, NULL, &serv, NULL, &path);
		if (path.IsEmpty())
			path = serv;
		else
			path = ExtractName(path, true);

		if (AddExt)
		{
			CString ext = _T(".");
			ext	+= ExtractExt(path);
			CString mime = GetExtensionByMIME(MIME);
			if (ext.CompareNoCase(mime) != 0)
				path += mime;
		}
	}
	else
	{
		path = ExtractName(url);
		path += GetExtensionByMIME(MIME);
	}

	PrepareShortName(path);

	return path;
}

void PrepareShortName( CString &name )
{
	name.Replace(_T("?"), _T("_"));
	name.Replace(_T("\\"), _T("_"));
	name.Replace(_T(":"), _T("_"));
	name.Replace(_T("/"), _T("_"));
	name.Replace(_T("%"), _T("_"));
	name.Replace(_T("*"), _T("_"));
	name.Replace(_T("\""), _T("_"));
	name.Replace(_T("|"), _T("_"));
	name.Replace(_T("<"), _T("_"));
	name.Replace(_T(">"), _T("_"));
	name.Replace(_T("+"), _T("_"));
	name.Replace(_T("["), _T("_"));
	name.Replace(_T("]"), _T("_"));
	name.Replace(_T("="), _T("_"));
	CString ext = ExtractExt(name);
	int len = ext.GetLength();
	if (len > 0)
		name.Delete(name.GetLength() - len - 1, len + 1);
	name = name.Left(64);
	if (len > 0)
	{
		name += _T(".");
		name += ext;
	}
	
}