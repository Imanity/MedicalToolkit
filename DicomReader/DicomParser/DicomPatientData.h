#pragma once
// Qt
/*#include <QReadWriteLock>*/
// Cpp
#include<string>
#include <vector>
#include <map>
#include <set>
// local
class DicomStudyData;
class DicomPatientPlateImage;

class DicomPatientData
{
public:
	std::string m_patient_ID;
	std::string m_patient_name;
	std::string m_patient_gender;
	std::string m_patient_birthday;
	std::string m_patient_age;
	std::string m_patient_position;
	std::string m_patient_orientation;

	std::vector<DicomStudyData *> m_studies;
	std::map<std::string, int> m_study_ID_to_idx_map;
	/*QReadWriteLock m_study_lock;*/

	DicomPatientPlateImage *m_patient_plate_image;
	/*QReadWriteLock m_patient_plate_lock;*/

	DicomPatientData();

	void clear_data();
	bool check_study_available(std::string &study_ID, int &idx);
	bool check_series_available(int study_idx, std::string &series_number, int &idx);
};