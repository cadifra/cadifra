/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module WinUtil.Gdi;


namespace WinUtil
{

#define STATUS_ENTRY(a) \
    case Gdiplus::a:  \
        return "Gdiplus::Status = " #a

const char* GdiplusException::what() const
{
    switch (status_)
    {
        STATUS_ENTRY(Ok);
        STATUS_ENTRY(GenericError);
        STATUS_ENTRY(InvalidParameter);
        STATUS_ENTRY(OutOfMemory);
        STATUS_ENTRY(ObjectBusy);
        STATUS_ENTRY(InsufficientBuffer);
        STATUS_ENTRY(NotImplemented);
        STATUS_ENTRY(Win32Error);
        STATUS_ENTRY(WrongState);
        STATUS_ENTRY(Aborted);
        STATUS_ENTRY(FileNotFound);
        STATUS_ENTRY(ValueOverflow);
        STATUS_ENTRY(AccessDenied);
        STATUS_ENTRY(UnknownImageFormat);
        STATUS_ENTRY(FontFamilyNotFound);
        STATUS_ENTRY(FontStyleNotFound);
        STATUS_ENTRY(NotTrueTypeFont);
        STATUS_ENTRY(UnsupportedGdiplusVersion);
        STATUS_ENTRY(GdiplusNotInitialized);
        STATUS_ENTRY(PropertyNotFound);
        STATUS_ENTRY(PropertyNotSupported);

    default:
        return "Unknown Gdiplus Error";
    }
}

}

