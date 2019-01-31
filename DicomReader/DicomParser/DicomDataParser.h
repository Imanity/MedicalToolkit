#pragma once
// Qt
/*#include <QMutex>*/
// Cpp
#include <string>
// meta
/*#include "centralmanager_global.h"*/
#include "dcmtk/dcmimgle/dcmimage.h"  
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcpxitem.h" 
class DcmFileFormat;
class /*CENTRALMANAGER_EXPORT*/ DicomDataParser
{
public:
	DicomDataParser();
	~DicomDataParser();

	bool get_data_slice(std::string file_path, short *buffer,
		int width, int height, int bit_num, std::string &sample_num,
		std::string &modality, float rescale_slope, float rescale_intercept,
        int planarConfiguration,DcmFileFormat* file_format=NULL);
	bool get_data_slice_overlay(std::string &file_path, short *buffer,
		int width, int height, int bit_num, std::string &sample_num,
		std::string &modality, float rescale_slope, float rescale_intercept,
		int overlay_row, int overlay_column,int overlay_row_ori,
		int planarConfiguration, int overlay_column_ori, int overlay_bit_allocate, DcmFileFormat* file_format = NULL);

protected:
	/*static QMutex m_jpeg_decoder_mutex;*/

private:

};