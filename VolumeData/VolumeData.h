#pragma once

#define DLL_EXPORTS

#ifdef DLL_EXPORTS
#define VOLUME_DATA_EXPORT __declspec(dllexport)
#else
#define VOLUME_DATA_EXPORT __declspec(dllimport)
#endif

#include <vector>
#include <Eigen/Dense>
#include <fstream>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkNIFTIImageReader.h>
#include <vtkNIFTIImageWriter.h>

#include "../DicomReader/DicomReader.h"

template <class T>
class VOLUME_DATA_EXPORT VolumeData {
public:
	VolumeData();
	VolumeData(int nx, int ny, int nz, float dx, float dy, float dz);
	~VolumeData();

	// Read data from Dicom file
	void readFromDicom(std::string file_path);

	// Read data from DSA Dicom file
	void readFromDSADicom(std::string file_path);

	// Read data from NII file
	void readFromNII(std::string file_path);

	// Write data To NII file
	void writeToNII(std::string file_path);

	// (x, y, z) => idx
	int idx(Eigen::Vector3i v);
	// (x, y, z) => idx
	int idx(int x, int y, int z);

	// idx => (x, y, z)
	Eigen::Vector3i coord(int idx_);

	// Value at pos (x, y, z)
	T at(Eigen::Vector3i pos);
	// Value at pos (x, y, z)
	T at(int x, int y, int z);

	// Trilineared Value at float pos (x, y, z)
	T trilinear(float x, float y, float z);

	// Set value at pos (x, y, z)
	void set(int x, int y, int z, T new_data);

public:
	T* data;
	int nx, ny, nz;
	float dx, dy, dz;
	int nvox;

	double distance_source_detector;
	double distance_source_patient;
};

template <class T>
VolumeData<T>::VolumeData() : data(nullptr) {
}

template <class T>
VolumeData<T>::VolumeData(int nx, int ny, int nz, float dx, float dy, float dz) {
	this->nx = nx, this->ny = ny, this->nz = nz;
	this->dx = dx, this->dy = dy, this->dz = dz;
	nvox = nx * ny * nz;
	data = new T[nvox];
}

template <class T>
VolumeData<T>::~VolumeData() {
	/*if (data != nullptr)
		delete[] data;*/
}

template <class T>
void VolumeData<T>::readFromDicom(std::string file_path) {
	DcmData dcmData(file_path);
	nx = dcmData.img_width;
	ny = dcmData.img_height;
	nz = dcmData.slice_num;
	dx = dcmData.img_pixel_spacing[0];
	dy = dcmData.img_pixel_spacing[1];
	dz = dcmData.img_slice_thickness;
	nvox = nx * ny * nz;
	data = new T[nvox];
	for (int i = 0; i < nvox; ++i)
		data[i] = dcmData.volume_buf[i];
}

template <class T>
void VolumeData<T>::readFromDSADicom(std::string file_path) {
	DcmData dcmData(file_path, true);
	nx = dcmData.img_width;
	ny = dcmData.img_height;
	nz = dcmData.slice_num;
	dx = dcmData.img_pixel_spacing[0];
	dy = dcmData.img_pixel_spacing[1];
	dz = dcmData.img_slice_thickness;
	nvox = nx * ny * nz;
	data = new T[nvox];
	for (int i = 0; i < nvox; ++i)
		data[i] = dcmData.volume_buf[i];
	distance_source_detector = dcmData.distance_source_detector;
	distance_source_patient = dcmData.distance_source_patient;
}

template <class T>
void VolumeData<T>::readFromNII(std::string file_path) {
	vtkSmartPointer<vtkNIFTIImageReader> niiReader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	niiReader->SetFileName(file_path.c_str());
	niiReader->Update();
	vtkSmartPointer<vtkImageData> image = niiReader->GetOutput();
	int dims[3];
	image->GetDimensions(dims);
	nx = dims[0], ny = dims[1], nz = dims[2];
	nvox = nx * ny * nz;
	double spacings[3];
	image->GetSpacing(spacings);
	dx = spacings[0], dy = spacings[1], dz = spacings[2];
	data = new T[nvox];
	short * image_buf = static_cast<short *>(image->GetScalarPointer());
	for (int i = 0; i < nvox; ++i)
		data[i] = image_buf[i];
}

template <class T>
void VolumeData<T>::writeToNII(std::string file_path) {
	vtkSmartPointer<vtkImageImport> image_import = vtkSmartPointer<vtkImageImport>::New();
	image_import->SetDataSpacing(dx, dy, dz);
	image_import->SetDataOrigin(0, 0, 0);
	image_import->SetWholeExtent(0, nx - 1, 0, ny - 1, 0, nz - 1);
	image_import->SetDataExtentToWholeExtent();
	image_import->SetDataScalarTypeToShort();
	image_import->SetNumberOfScalarComponents(1);
	image_import->SetImportVoidPointer(data);
	image_import->Update();

	vtkSmartPointer<vtkNIFTIImageWriter> niiWriter = vtkSmartPointer<vtkNIFTIImageWriter>::New();
	niiWriter->SetInputData(image_import->GetOutput());
	niiWriter->SetFileName(file_path.c_str());
	niiWriter->Write();
}

template <class T>
int VolumeData<T>::idx(Eigen::Vector3i v) {
	return idx(v(0), v(1), v(2));
}

template <class T>
int VolumeData<T>::idx(int x, int y, int z) {
	int i = x + y * nx + z * nx * ny;
	if (x < 0 || x >= nx || y < 0 || y >= ny || z < 0 || z >= nz) {
		return -1;
	}
	return i;
}

template <class T>
Eigen::Vector3i VolumeData<T>::coord(int idx_) {
	Eigen::Vector3i v;
	if (idx_ < 0 || idx_ >= nvox) {
		v << 0, 0, 0;
		return v;
	}
	int z = idx_ / (nx * ny);
	int tmp = idx_ % (nx * ny);
	int y = tmp / nx;
	int x = tmp % nx;
	v << x, y, z;
	return v;
}

template <class T>
T VolumeData<T>::at(int x, int y, int z) {
	int i = idx(x, y, z);
	if (i < 0) return -1;
	return data[i];
}

template <class T>
T VolumeData<T>::at(Eigen::Vector3i pos) {
	int i = idx(pos);
	if (i < 0) return -1;
	return data[i];
}

template <class T>
T VolumeData<T>::trilinear(float x, float y, float z) {
	int x0 = int(x), x1 = x0 + 1;
	int y0 = int(y), y1 = y0 + 1;
	int z0 = int(z), z1 = z0 + 1;

	float xd = x - x0, yd = y - y0, zd = z - z0;
	float c00 = at(x0, y0, z0) * (1 - xd) + at(x1, y0, z0) * xd;
	float c01 = at(x0, y0, z1) * (1 - xd) + at(x1, y0, z1) * xd;
	float c10 = at(x0, y1, z0) * (1 - xd) + at(x1, y1, z0) * xd;
	float c11 = at(x0, y1, z1) * (1 - xd) + at(x1, y1, z1) * xd;

	float c0 = c00 * (1 - yd) + c10 * yd;
	float c1 = c01 * (1 - yd) + c11 * yd;

	float c = c0 * (1 - zd) + c1 * zd;
	return short(c + 0.5);
}

template <class T>
void VolumeData<T>::set(int x, int y, int z, T new_data) {
	int i = idx(x, y, z);
	if (i < 0) return;
	this->data[i] = new_data;
}
