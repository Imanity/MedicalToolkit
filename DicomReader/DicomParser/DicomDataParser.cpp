// dcmtk
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmjpeg/djdecode.h>
// meta
#include "DicomDataParser.h"
/*#include "../DICOMReader/DcmHandler.h"*/

#include<vector>
#include<iostream>
using namespace std;

// public
DicomDataParser::DicomDataParser()
{

}

DicomDataParser::~DicomDataParser()
{
}

bool DicomDataParser::get_data_slice(std::string file_path, short *buffer,
	int width, int height, int bit_num, std::string &sample_num,
	std::string &modality, float rescale_slope, float rescale_intercept, int planarConfiguration,DcmFileFormat* file_format)//file_format:dcmtk
{
	bool l_ret = false;
	// 1、打开文件
	OFCondition l_status;
	DcmFileFormat l_file_format;// dcmtk
    /*DcmHandler handler(&l_file_format);//self*/
    if (!file_format) {
        /*l_status = handler.loadDicomFile(file_path.c_str());//根据路径打开dcm数据*/
		l_status = l_file_format.loadFile(file_path.c_str());
        if (l_status.bad())
            return false;
		file_format = &l_file_format;
    } else {
        //l_status = handler.loadDicomFile(*byte_array);
		l_file_format = *file_format;
    }


	// 2、处理压缩图片文件
	DcmDataset *l_dataset;//dcmtk
    if (!file_format) {
        l_dataset = l_file_format.getDataset();// excute
    } else {
        l_dataset = file_format->getDataset();
    }

	Uint16 l_pixel_representation;
	l_status = l_dataset->findAndGetUint16(DCM_PixelRepresentation, l_pixel_representation);
	if (l_pixel_representation != EXS_LittleEndianExplicit)
	{
		// REMARK 下述过程可能是线程不安全的，这里改为全局互斥操作
		/*m_jpeg_decoder_mutex.lock();*/
		DJDecoderRegistration::registerCodecs(); // register JPEG codecs
		

		E_TransferSyntax xfer = l_dataset->getOriginalXfer(); 
		l_status = l_dataset->chooseRepresentation(xfer, NULL);
		if (l_dataset->canWriteXfer(xfer))
		{
			DicomImage* dcmImage = new DicomImage(l_dataset, xfer); //生成DicomImage
			dcmImage->setMinMaxWindow();        //自动调窗
			//dcmImage->writeBMP("myoutput.bmp");//存为bmp格式 
		}


		// decompress dataset if compressed
		l_status = l_dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);

		// check if everything went well
		if (l_dataset->canWriteXfer(EXS_LittleEndianExplicit))
		{
			//file_format->saveFile("test_decompressed.dcm", EXS_LittleEndianExplicit);
		}

		if(l_status.bad())
		{
			cout<<"解压错误！"<<endl;
		    return false;
		}


		// check if everything went well
		DJDecoderRegistration::cleanup(); // deregister JPEG codecs
		/*m_jpeg_decoder_mutex.unlock();*/
	}

	/*
	DcmElement* element=NULL;  
	l_status = l_dataset->findAndGetElement(DCM_PixelData,element);//获取存放图像数据的那个data element  
	if (l_status.bad() || element == NULL)
	{
		std::cout<<"Error element!"<<std::endl;
	}

	l_dataset->removeAllButCurrentRepresentations(); 

	DcmPixelData *dpix = NULL;//得到pixel data
	dpix = OFstatic_cast(DcmPixelData*, element);

	dpix->removeAllButOriginalRepresentations();

	E_TransferSyntax xferSyntax = EXS_Unknown;
	const DcmRepresentationParameter *rep = NULL;
	dpix->getOriginalRepresentationKey(xferSyntax, rep);


	DcmPixelSequence *dseq = NULL;//处理像素数据元素中的像素序列
	// Access original data representation and get result within pixel sequence
	l_status = dpix->getEncapsulatedRepresentation(xferSyntax, rep, dseq);//正常的为false

	unsigned short *l_temp_16bit_buffer2 = nullptr;
	if ( l_status == EC_Normal )
	{
	

		DcmPixelItem* pixitem = NULL;
		// Access first frame (skipping offset table)
		dseq->getItem(pixitem, 1);
		if (pixitem == NULL)
		{
			std::cout<<"Error DcmPixelItem!"<<std::endl;
		}
		Uint8* pixData = NULL;
		// Get the length of this pixel item (i.e. fragment, i.e. most of the time, the lenght of the frame)
		Uint32 length = pixitem->getLength();
		std::cout<<"Length:"<<length<<std::endl;
		if (length == 0)
		{
			std::cout<<"Error DcmPixelItem length!"<<std::endl;
		}
		// Finally, get the compressed data for this pixel item
		l_status = pixitem->getUint8Array(pixData);
	
		if (l_status.bad()){
			std::cout<<"Error element!"<<std::endl;
		}

		
	}// end normal
	*/

	// 3、读取数据
	unsigned long l_data_read_count = 0;
	const unsigned char *l_temp_8bit_buffer = nullptr;
	const unsigned short *l_temp_16bit_buffer = nullptr;

	switch (bit_num)
	{
	case 8:
		l_dataset->findAndGetUint8Array(DCM_PixelData, l_temp_8bit_buffer, &l_data_read_count);
		break;
	case 16:
		l_dataset->findAndGetUint16Array(DCM_PixelData, l_temp_16bit_buffer, &l_data_read_count);// no data
		break;
	default:
		std::cout << "Slice bit num err: " << bit_num << " | " << file_path << std::endl;
		return false;
		break;
	}
	// 4、检查数据长度
	int l_assumed_pixel_num = width * height;
	if ((sample_num == "1" && l_assumed_pixel_num != l_data_read_count) ||
		(sample_num == "3" && l_assumed_pixel_num * 3 != l_data_read_count &&
		 l_assumed_pixel_num * 3 != l_data_read_count - 1))
	{
		std::cout << "Slice data size err: " << l_assumed_pixel_num << "(" <<  //error 
			l_data_read_count << ") | " << file_path << std::endl;
		return false;
	}

	// 5、数据后处理
	switch (bit_num)
	{
	case 8:
		if (sample_num == "1")
		{
			for (int i = 0; i < l_assumed_pixel_num; ++i)
			{
				short l_value = l_temp_8bit_buffer[i];
				buffer[i] = (short)(l_value * rescale_slope + rescale_intercept);
			}
		}
		else if (sample_num == "3")
		{
			int count = 0;
			for (int i = 0; i < l_assumed_pixel_num; ++i)
			{
				if (planarConfiguration == 1)
				{
					buffer[count] = (short)(l_temp_8bit_buffer[i]);
					buffer[count + 1] = (short)(l_temp_8bit_buffer[l_assumed_pixel_num + i]);
					buffer[count + 2] = (short)(l_temp_8bit_buffer[l_assumed_pixel_num * 2 + i]);
				}
				else
				{
					buffer[count] = (short)(l_temp_8bit_buffer[3*i]);
					buffer[count + 1] = (short)(l_temp_8bit_buffer[3*i + 1]);
					buffer[count + 2] = (short)(l_temp_8bit_buffer[3*i + 2]);
				}
				count += 3;
			}
		}
		else
		{
			std::cout << "Slice sample num err: " << sample_num << " | " << file_path << std::endl;
			return false;
		}
		break;
	case 16:
		if (sample_num == "1")
		{
			if (modality == "DR")
			{
				if (rescale_slope < 0.0001)
				{
					for (int i = 0; i < l_assumed_pixel_num; ++i)
					{
						short l_value = l_temp_16bit_buffer[i] ;
						buffer[i] = (short)(l_value + rescale_intercept);
					}
				}
				else
				{
					for (int i = 0; i < l_assumed_pixel_num; ++i)
					{
						short l_value = l_temp_16bit_buffer[i] ;
						buffer[i] = (short)(l_value * rescale_slope + rescale_intercept);
					}
				}
			}
			else
			{
				int l_label_flag = 0;
				for (int i = 0; i < l_assumed_pixel_num; ++i)
				{
					short l_value = l_temp_16bit_buffer[i];
					l_value = (short)(l_value * rescale_slope + rescale_intercept);
					buffer[i] = l_value;
					if (l_value == -1024 || l_value == -1025)
						++l_label_flag;
				}
				if (l_label_flag == l_assumed_pixel_num)
				{
					if (l_assumed_pixel_num > 0)
					{
						for (int i = 0; i < l_assumed_pixel_num; ++i)
							if(buffer[i] == -1024)
								buffer[i] = -2048;
							else
								buffer[i] = 2048;
					}
				}
			}
		}
		else
		{
			std::cout << "Slice sample num err: " << sample_num << " | " << file_path << std::endl;
			return false;
		}

		break;
	}//end switch
	if (l_temp_16bit_buffer != nullptr)
		l_temp_16bit_buffer = nullptr;
	if (l_temp_8bit_buffer != nullptr)
		l_temp_8bit_buffer = nullptr;

	return true;
}

bool DicomDataParser::get_data_slice_overlay(std::string &file_path, short *buffer,
	int width, int height, int bit_num, std::string &sample_num,
	std::string &modality, float rescale_slope, float rescale_intercept,
	int overlay_row, int overlay_column, int overlay_row_ori,
	int overlay_column_ori, int overlay_bit_allocate, int planarConfiguration,DcmFileFormat* file_format)
{

	bool l_ret = false;
	// 1、打开文件
	OFCondition l_status;//dcmtk
	DcmFileFormat l_file_format;// self
	/*DcmHandler handler(&l_file_format);*/
	if (!file_format) {
		/*l_status = handler.loadDicomFile(file_path.c_str());*/
		l_status = l_file_format.loadFile(file_path.c_str());
        if (l_status.bad())
            return false;
	}
	else {
		//l_status = handler.loadDicomFile(*byte_array);
	}
	

	// 2、处理压缩图片文件
    DcmDataset *l_dataset;
    if (!file_format) {
	    l_dataset = l_file_format.getDataset();
    } else {
        l_dataset = file_format->getDataset();
    }
	Uint16 l_pixel_representation;
	l_status = l_dataset->findAndGetUint16(DCM_PixelRepresentation, l_pixel_representation);
	if (l_pixel_representation != EXS_LittleEndianExplicit)
	{
		// REMARK 下述过程可能是线程不安全的，这里改为全局互斥操作
		/*m_jpeg_decoder_mutex.lock();*/
		DJDecoderRegistration::registerCodecs(); // register JPEG codecs
		// decompress dataset if compressed
		l_dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);
		// check if everything went well
		DJDecoderRegistration::cleanup(); // deregister JPEG codecs
		/*m_jpeg_decoder_mutex.unlock();*/
	}


	// 3、读取数据
	unsigned long l_data_read_count = 0;
	unsigned long l_data_overlay_count = 0;
	const unsigned char *l_temp_8bit_buffer = nullptr;
	const unsigned short *l_temp_16bit_buffer = nullptr;
	const unsigned char *l_overlay_8bit_buffer = nullptr;
	const unsigned short *l_overlay_16bit_buffer = nullptr;
	switch (bit_num)
	{
	case 8:
		l_dataset->findAndGetUint8Array(DCM_PixelData, l_temp_8bit_buffer, &l_data_read_count);
		l_dataset->findAndGetUint8Array(DCM_OverlayData, l_overlay_8bit_buffer);
		break;
	case 16:
		l_dataset->findAndGetUint16Array(DCM_PixelData, l_temp_16bit_buffer, &l_data_read_count);
		l_dataset->findAndGetUint8Array(DCM_OverlayData, l_overlay_8bit_buffer, &l_data_overlay_count);
		break;
	default:
		std::cout << "Slice bit num err: " << bit_num << " | " << file_path << std::endl;
		return false;
		break;
	}

	// 4、检查数据长度
	int l_assumed_pixel_num = width * height;
	if ((sample_num == "1" && l_assumed_pixel_num != l_data_read_count) ||
		(sample_num == "3" && l_assumed_pixel_num * 3 != l_data_read_count))
	{
		std::cout << "Slice data size err: " << l_assumed_pixel_num << "(" <<
			l_data_read_count << ") | " << file_path << std::endl;
		return false;
	}

	// 5、数据后处理
	int start = overlay_row_ori * width + overlay_column;
	int overlay_num = overlay_row * overlay_column;
	int count_overlay = 0;
	int index_overlay = 0;
	switch (bit_num)
	{
	case 8:
		if (sample_num == "1")
		{
			for (int i = 0; i < l_assumed_pixel_num; ++i)
			{
				short l_value = l_temp_8bit_buffer[i];
				if (i > start &&
					i < (start + overlay_num))
				{
					l_value = l_value + (short)l_overlay_8bit_buffer[count_overlay];
					count_overlay++;
				}
				buffer[i] = (short)(l_value * rescale_slope + rescale_intercept);
			}
		}
		else if (sample_num == "3")
		{
			int count = 0;
			for (int i = 0; i < l_assumed_pixel_num; ++i)
			{

				if (planarConfiguration == 1)
				{
					buffer[count] = (short)(l_temp_8bit_buffer[i]);
					buffer[count + 1] = (short)(l_temp_8bit_buffer[l_assumed_pixel_num + i]);
					buffer[count + 2] = (short)(l_temp_8bit_buffer[l_assumed_pixel_num * 2 + i]);
				}
				else
				{
					buffer[count] = (short)(l_temp_8bit_buffer[3 * i]);
					buffer[count + 1] = (short)(l_temp_8bit_buffer[3 * i + 1]);
					buffer[count + 2] = (short)(l_temp_8bit_buffer[3 * i + 2]);
				}

				count += 3;
			}
		}
		else
		{
			std::cout << "Slice sample num err: " << sample_num << " | " << file_path << std::endl;
			return false;
		}
		break;
	case 16:
		if (sample_num == "1")
		{
			if (modality == "DR")
			{
				if (rescale_slope < 0.0001)
				{
					for (int i = 0; i < l_assumed_pixel_num; ++i)
					{
						short l_value = l_temp_16bit_buffer[i];
						if (i > start &&
							i < (start + overlay_num))
						{
							l_value = l_value + (short)l_overlay_16bit_buffer[count_overlay];
							count_overlay++;
						}
						buffer[i] = (short)(l_value + rescale_intercept);
					}
				}
				else
				{
					for (int i = 0; i < l_assumed_pixel_num; ++i)
					{
						short l_value = l_temp_16bit_buffer[i];
						if (i > start &&
							i < (start + overlay_num))
						{
							l_value = l_value + (short)l_overlay_16bit_buffer[count_overlay];
							count_overlay++;
						}
						buffer[i] = (short)(l_value * rescale_slope + rescale_intercept);
					}
				}
			}
			else
			{
				int l_label_flag = 0;
				for (int i = 0; i < width; ++i)
				{
					for (int j = 0; j < height; j++)
					{
						short l_value = l_temp_16bit_buffer[i*height + j];
						l_value = (short)(l_value * rescale_slope + rescale_intercept);

						if (count_overlay < 8)
						{
							if (l_overlay_8bit_buffer[index_overlay] >> count_overlay & 0x01 == 1)
								l_value = -32767;   //-32767作为一种识别信号
							count_overlay++;
						}
						else
						{
							count_overlay = 0;
							index_overlay++;
							if (l_overlay_8bit_buffer[index_overlay] & 0x01 == 1)
								l_value = -32767;
							count_overlay++;
						}
						
						buffer[i*height + j] = l_value;
						if (l_value == -1024 || l_value == -1025)
							++l_label_flag;
					}
				}
				if (l_label_flag == l_assumed_pixel_num)
				{
					if (l_assumed_pixel_num > 0)
					{
						for (int i = 0; i < l_assumed_pixel_num; ++i)
							if (buffer[i] == -1024)
								buffer[i] = -2048;
							else
								buffer[i] = 2048;
					}
				}
			}
		}
		else
		{
			std::cout << "Slice sample num err: " << sample_num << " | " << file_path << std::endl;
			return false;
		}

		break;
	}

	if (l_temp_16bit_buffer != nullptr)
		l_temp_16bit_buffer = nullptr;
	if (l_temp_8bit_buffer != nullptr)
		l_temp_8bit_buffer = nullptr;

	return true;


}

// protected
/*QMutex DicomDataParser::m_jpeg_decoder_mutex;*/

// private