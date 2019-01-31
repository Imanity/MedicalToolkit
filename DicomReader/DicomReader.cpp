#include <vtkSmartPointer.h>
#include <vtkDirectory.h>

#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/djencode.h>

#define DLL_EXPORTS

#include "DicomReader.h"

#include "DicomParser/DicomDataParser.h"
#include "DicomParser/DicomHeaderParser.h"
#include "DicomParser/DicomPatientData.h"
#include "DicomParser/DicomStudyData.h"
#include "DicomParser/DicomSeriesData.h"

inline float ofstr_to_float(OFString &str) {
	return static_cast<Float32>(atof((const char *)str.c_str()));
}

inline float *ofstr_to_float_array(OFString &str, int count) {
	Float32 *l_ret_vec = new Float32[count];
	int l_ret_vec_top = 0;

	const size_t l_str_length = str.length();
	size_t l_idx = 0;
	size_t l_last_idx = 0;
	while (l_idx < l_str_length) {
		while (l_idx < l_str_length && str.at(l_idx) != '\\')
			++l_idx;
		if (str.at(l_idx - 1) != '\\') {
			OFString l_tmp_str = str.substr(l_last_idx, l_idx - l_last_idx);
			l_ret_vec[l_ret_vec_top++] = ofstr_to_float(l_tmp_str);
		}
		l_last_idx = l_idx + 1;
		l_idx = l_last_idx;
	}
	return l_ret_vec;
}

DcmData::DcmData(std::string dcm_path, bool dcm_multiFrame) {
	if (dcm_multiFrame) {
		LoadMultiFrameData(dcm_path);
	} else {
		LoadSingleFrameData(dcm_path);
	}
}

DcmData::~DcmData() {
	delete[] volume_buf;
}

void DcmData::LoadSingleFrameData(std::string file_path) {
	folder_path = file_path;

	vtkSmartPointer<vtkDirectory> directory = vtkSmartPointer<vtkDirectory>::New();
	int opened = directory->Open(folder_path.c_str());
	if (!opened) {
		std::cerr << "Invalid directory!" << std::endl;
		return;
	}
	int numberOfFiles = directory->GetNumberOfFiles();
	for (int i = 0; i < numberOfFiles; i++) {
		if (!directory->FileIsDirectory(directory->GetFile(i)))
			file_names.push_back(directory->GetFile(i));
	}

	slice_num = file_names.size();

	DicomHeaderParser header_parser;
	for (int i = 0; i < file_names.size(); ++i) {
		header_parser.parse_header_info(folder_path + "\\" + file_names[i]);
	}

	DicomDataMgr *data_mgr = DicomDataMgr::get_instance();

	instance_number_to_idx_map = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_instance_number_to_idx_map;

	is_img_inverse = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_locations[0] < data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_locations[1];
	img_pixel_spacing[0] = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_pixel_spacing[0];
	img_pixel_spacing[1] = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_pixel_spacing[1];
	data_mgr->m_patients[0]->m_studies[0]->m_series[0]->update_slice_thickness();
	img_slice_thickness = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_series_slice_thickness;
	img_width = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_resolution[0];
	img_height = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_resolution[1];
	img_bit_num = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_bits_allocated;
	img_sample_num = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_samples_per_pixel;
	img_modality = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_series_modality;
	img_rescale_slope = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_rescale_slope;
	img_rescale_intercept = data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_rescale_intercept;
	img_planar_configuration = atoi(data_mgr->m_patients[0]->m_studies[0]->m_series[0]->m_planar_configuration.c_str());

	data_mgr->clear_data();

	DicomDataParser data_parser;

	volume_buf = new short[img_width * img_height * slice_num];

	for (int i = 0; i < slice_num; ++i) {
		short *img_buf = new short[img_width * img_height];
		data_parser.get_data_slice(folder_path + "\\" + file_names[instance_number_to_idx_map[i]], img_buf, img_width, img_height, img_bit_num,
			img_sample_num, img_modality, img_rescale_slope, img_rescale_intercept, img_planar_configuration);

		for (int j = 0; j < img_width * img_height; ++j) {
			volume_buf[(is_img_inverse ? i : (slice_num - i - 1)) * img_width * img_height + j] = img_buf[j];
		}

		delete[] img_buf;
	}
}

void DcmData::LoadMultiFrameData(std::string file_path) {
	DcmFileFormat fileformat;
	OFCondition oc = fileformat.loadFile(file_path.c_str());
	if (!oc.good()) {
		std::cerr << "file Load error" << std::endl;
		return;
	}

	DcmDataset *dataset = fileformat.getDataset();
	E_TransferSyntax xfer = dataset->getOriginalXfer();

	OFString patientname;
	dataset->findAndGetOFString(DCM_PatientName, patientname);

	unsigned short bit_count(0);
	dataset->findAndGetUint16(DCM_BitsStored, bit_count);

	OFString isRGB;
	dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);

	unsigned short img_bits(0);
	dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);
	int Img_bitCount = (int)img_bits;

	OFString framecount;
	dataset->findAndGetOFString(DCM_NumberOfFrames, framecount);

	unsigned short m_width;
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);

	OFString dis_detector_str, dis_patient_str;
	dataset->findAndGetOFString(DCM_DistanceSourceToDetector, dis_detector_str);
	dataset->findAndGetOFString(DCM_DistanceSourceToPatient, dis_patient_str);
	distance_source_detector = ofstr_to_float(dis_detector_str);
	distance_source_patient = ofstr_to_float(dis_patient_str);

	OFString pixel_spacing_str;
	OFCondition l_status = dataset->findAndGetOFStringArray(DCM_PixelSpacing, pixel_spacing_str, true);
	if (!l_status.good())
		dataset->findAndGetOFStringArray(DCM_ImagerPixelSpacing, pixel_spacing_str, true);
	Float32 *l_spacing = ofstr_to_float_array(pixel_spacing_str, 2);
	img_pixel_spacing[0] = l_spacing[0];
	img_pixel_spacing[1] = l_spacing[1];

	const char* transferSyntax = NULL;
	fileformat.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);
	std::string losslessTransUID = "1.2.840.10008.1.2.4.70";
	std::string lossTransUID = "1.2.840.10008.1.2.4.51";
	std::string losslessP14 = "1.2.840.10008.1.2.4.57";
	std::string lossyP1 = "1.2.840.10008.1.2.4.50";
	std::string lossyRLE = "1.2.840.10008.1.2.5";
	if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
		transferSyntax == losslessP14 || transferSyntax == lossyP1) {
		DJDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DJDecoderRegistration::cleanup();
	}
	else if (transferSyntax == lossyRLE) {
		DcmRLEDecoderRegistration::registerCodecs();
		dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DcmRLEDecoderRegistration::cleanup();
	}
	else {
		dataset->chooseRepresentation(xfer, NULL);
	}

	DicomImage* m_dcmImage = new DicomImage((DcmObject*)dataset, xfer);

	slice_num = m_dcmImage->getFrameCount();
	img_width = m_dcmImage->getHeight();
	img_height = m_dcmImage->getWidth();

	volume_buf = new short[img_width * img_height * slice_num];

	for (int k = 0; k < this->slice_num; k++) {
		unsigned char *pixelData = (unsigned char*)(m_dcmImage->getOutputData(8, k, 0));
		if (pixelData != NULL) {
			if (3 == Img_bitCount) {
				for (int i = 0; i < m_height; i++) {
					for (int j = 0; j < m_width; j++) {
						volume_buf[k * img_width * img_height + i * img_width + j] = *(pixelData + i * m_width * 3 + j * 3);
					}
				}
			} else if (1 == Img_bitCount) {
				uchar* data = nullptr;
				for (int i = 0; i < m_height; i++) {
					for (int j = 0; j < m_width; j++) {
						volume_buf[k * img_width * img_height + i * img_width + j] = *(pixelData + i * m_width + j);
					}
				}
			}
		}
	}
}
