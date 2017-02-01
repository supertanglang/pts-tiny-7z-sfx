/* 7zFile.c -- File IO
2009-11-24 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7zFile.h"

static WRes File_Open(CSzFile *p, const char *name, int writeMode)
{
  p->file = fopen(name, writeMode ? "wb+" : "rb");
  return (p->file != 0) ? 0 : errno;
}

STATIC WRes InFile_Open(CSzFile *p, const char *name) { return File_Open(p, name, 0); }
STATIC WRes OutFile_Open(CSzFile *p, const char *name) { return File_Open(p, name, 1); }

STATIC WRes File_Close(CSzFile *p)
{
  if (p->file != NULL)
  {
    int res = fclose(p->file);
    if (res != 0)
      return res;
    p->file = NULL;
  }
  return 0;
}

STATIC WRes File_Write(CSzFile *p, const void *data, size_t *size)
{
  size_t originalSize = *size;
  if (originalSize == 0)
    return 0;

  *size = fwrite(data, 1, originalSize, p->file);
  if (*size == originalSize)
    return 0;
  return 1;
}

/* ---------- FileSeqInStream ---------- */

/* ---------- FileInStream ---------- */

STATIC SRes FileInStream_Read(CFileInStream *p, void *data, size_t *size)
{
  size_t originalSize = *size;
  if (originalSize == 0)
    return SZ_OK;
  *size = fread(data, 1, originalSize, p->file.file);  /* 0 on error */
  if (*size == originalSize)
    return SZ_OK;
  return SZ_ERROR_READ;
}

STATIC SRes FileInStream_Seek(CFileInStream *p, Int64 *pos)
{
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK FileInStream_Seek pos=%lld, origin=0, from=%ld\n", *pos, ftell(p->file.file));
#endif
  /* TODO(pts): Use fseeko for 64-bit offset. */
  Int64 pos0 = *pos;
  int res = fseek(p->file.file, (long)pos0, SEEK_SET);
  *pos = ftell(p->file.file);
  return res == 0 && *pos == pos0 ? SZ_OK : SZ_ERROR_READ;
}

#if 0
  p->s.Read = FileInStream_Read;
  p->s.Seek = FileInStream_Seek;
#endif

/* ---------- FileOutStream ---------- */
