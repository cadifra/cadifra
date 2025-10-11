module;

#include <Windows.h>
#include <unknwn.h>

export module WinUtil:types;

export namespace WinUtil
{

using ::HINSTANCE;
using ::IUnknown;
using ::STGMEDIUM;
using ::HBRUSH;
using ::METAFILEPICT;
using ::HENHMETAFILE;
using ::LOGFONT;
using ::HBITMAP;
using ::HPEN;
using ::HRGN;
using ::COLORREF;
using ::HFONT;

}
