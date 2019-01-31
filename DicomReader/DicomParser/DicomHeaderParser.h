#pragma once

#define GL_FLOAT 0
#define GL_UNSIGNED_BYTE 1
#define GL_SHORT 2

// dcmtk
#include <dcmtk/ofstd/ofcond.h>
// meta
/*#include <QObject>*/

#include "DicomDataMgr.h"
class DcmFileFormat;

class DicomHeaderParser/* : public QObject*/
{
	/*Q_OBJECT*/
public:
	DicomHeaderParser();
	~DicomHeaderParser();
	/*!
	* \brief 读取一个dicom文件的头信息，记录关键信息数据
	* 只会被 Dicom Header 读取线程调用
	*/

    bool parse_header_info(std::string file_path, DcmFileFormat* file_format=NULL);
    //bool parse_header_info(std::string &file_path, QByteArray* data_array=NULL);
	/*bool update_dr_window_cw();*/

protected:

private:

	inline unsigned short ofstr_to_uint16(OFString &str)
	{
		return static_cast<Uint16>(atoi((const char *)str.c_str()));
	}

	inline float ofstr_to_float(OFString &str)
	{
		return static_cast<Float32>(atof((const char *)str.c_str()));
	}

	inline float *ofstr_to_float_array(OFString &str, int count)
	{
		Float32 *l_ret_vec = new Float32[count];
		int l_ret_vec_top = 0;

		const size_t l_str_length = str.length();
		size_t l_idx = 0;
		size_t l_last_idx = 0;
		while (l_idx < l_str_length)
		{
			while (l_idx < l_str_length && str.at(l_idx) != '\\')
				++l_idx;
			if (str.at(l_idx - 1) != '\\')
			{
				OFString l_tmp_str = str.substr(l_last_idx, l_idx - l_last_idx);
				l_ret_vec[l_ret_vec_top++] = ofstr_to_float(l_tmp_str);
			}
			l_last_idx = l_idx + 1;
			l_idx = l_last_idx;
		}
		return l_ret_vec;
	}

/*
signals:
	void new_series(int patient_idx, int study_idx, int series_idx);*/
};