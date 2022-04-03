#pragma warning(disable:4996)
#pragma pack(2)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;
typedef struct tagBITMAPFILEHEADER{
WORD bfType;
DWORD bfSize;
WORD bfReserved1;
WORD bfReserved2;
DWORD bfoffBits;
}BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
DWORD biSize;
LONG biWidth;
LONG biHeight;
WORD biplanes;
WORD biBitCount;
DWORD biCompression;
DWORD biSizeImage;
LONG biXPelsPerMeter;
LONG biYPelsPerMeter;
DWORD biClrUsed;
DWORD biClrImportant;
}BITMAPINFOHEADER;
typedef struct tagRGBQUAD{
BYTE rgbBlue;
BYTE rgbGreen;
BYTE rgbRed;
BYTE rgbReserved1;
}RGBQUAD;

void Binarization(BYTE * Img, BYTE * Out, int ImgSize, double Threshold)
{
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < (int)Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int ImgSize)
{
	double Temp[256] = { 0 };
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
	/*for (int i = 0; i < 256; i++) {
		Temp[i] = (double)Histo[i] / ImgSize;
	}
	FILE* fp = fopen("histogram.txt", "wt");
	for(int i=0; i<256; i++) fprintf(fp, "%lf\n", Temp[i]);
	fclose(fp);*/
}

void GetMinMax(BYTE* High, BYTE* Low, int* Histo){
	
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			*Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			*High = i;
			break;
		}
	}
}

double GonzalezBinThresh(BYTE* Img,int ImgSize, double Threshold, double epsilon) {
	double ThresNext;
	double diff;
	double low_sum=0, high_sum=0;
	double low_pixel_num=0, high_pixel_num=0;
	double G2Avg, G1Avg;
	printf("Threshold %f\n", Threshold);
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] > Threshold) {
			//printf("GonzelzBinThresh Threshold value %d", Threshold);
			high_sum += Img[i];
			high_pixel_num++;
		}
		else if (Img[i] < Threshold){
			low_sum += Img[i];
			low_pixel_num++;
		}
	}
	printf("high_sum %f, low_sum %f, high_pixel_sum %f, low_pixel_sum %f\n", high_sum, low_sum, high_pixel_num, low_pixel_num);

	G2Avg = low_sum / low_pixel_num;
	G1Avg = high_sum / high_pixel_num;
	ThresNext = (G2Avg + G1Avg) / 2;
	diff = fabs(Threshold - ThresNext);
	printf("diff %f\n", diff);
	printf("Threshold %f\n", Threshold);
	printf("ThresNext %f\n", ThresNext);
	if (diff < epsilon) {
		printf("%f\n", diff);
		return Threshold;
	}
	GonzalezBinThresh(Img, ImgSize, ThresNext, epsilon);
	return Threshold;
}

double GetInitThres(BYTE* Low, BYTE* High) {
	double Thres;
	Thres = (double)(*Low + *High) / 2;
	return Thres;
}

int main(){
	BITMAPFILEHEADER hf; // 14BYTES
	BITMAPINFOHEADER hInfo; // 40BYTES
	RGBQUAD hRGB[256]; // 1024BYTES
	FILE* fp;

	char ImgFilePath[200];
	char OutputPath[200];

	printf("ImgFilePath:    ");
	scanf("%200s", ImgFilePath);
	printf("OutputPath:    ");
	scanf("%200s", OutputPath);

	fp = fopen(ImgFilePath, "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	
	int Histo[256] = { 0 };
	BYTE * High = (BYTE*)malloc(1);
	BYTE * Low = (BYTE*)malloc(1);
	double Threshold;
	double ThresholdInit;
	int epsilon=3; //diff value limit

	ObtainHistogram(Image, Histo, ImgSize); //Obtain Histogram to get color data of each pixel
	GetMinMax(High, Low, Histo); //To calculate threshold initial value 
	
	// Threshold initial value is the average of Lowest and Highest value
	ThresholdInit = GetInitThres(Low, High); 
	//GonzalezBinThresh calls itself recursively until it finds Threshold having diff smaller than epsilon 
	Threshold = GonzalezBinThresh(Image, ImgSize, ThresholdInit, epsilon); 

	Binarization(Image, Output,ImgSize, Threshold);

	fp = fopen(OutputPath, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}