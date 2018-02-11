// 
// (C) Jan de Vaan 2007-2010, all rights reserved. See the accompanying "License.txt" for licensed use. 
// 


#ifndef JLS_INTERFACE
#define JLS_INTERFACE

#include "pubtypes.h"
#include "dcmtk/ofstd/ofstd.h"    /* for size_t */
#include "dcmtk/ofstd/ofdefine.h" /* for DCMTK_DECL_EXPORT */

#ifdef d3d_EXPORTS
#define DCMTK_D3D_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_D3D_EXPORT DCMTK_DECL_IMPORT
#endif

#ifndef D3D_IMEXPORT
#define D3D_IMEXPORT(returntype) DCMTK_D3D_EXPORT returntype
#endif


#ifdef __cplusplus
extern "C" 
{
#endif
  D3D_IMEXPORT(enum JLS_ERROR) JpegLsEncode(BYTE **ptr, size_t *size, size_t* pcbyteWritten,
		const void* uncompressedData, size_t uncompressedLength, struct JlsParameters* pparams);


  D3D_IMEXPORT(enum JLS_ERROR) JpegLsDecode(void* uncompressedData, size_t uncompressedLength, 
		const void* compressedData, size_t compressedLength, 
		struct JlsParameters* info);

D3D_IMEXPORT(enum JLS_ERROR) JpegLsReadHeader(const void* compressedData, size_t compressedLength, 
		struct JlsParameters* pparams);

/*

  D3D_IMEXPORT(enum JLS_ERROR) JpegLsDecodeRect(void* uncompressedData, size_t uncompressedLength, 
		const void* compressedData, size_t compressedLength, 
		struct JlsRect rect, struct JlsParameters* info);

  D3D_IMEXPORT(enum JLS_ERROR) JpegLsVerifyEncode(const void* uncompressedData, size_t uncompressedLength, 
		const void* compressedData, size_t compressedLength);

*/
#ifdef __cplusplus
}
#endif

#endif
