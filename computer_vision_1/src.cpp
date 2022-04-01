#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
void InverseImage(BYTE* Img, BYTE *Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] =Img[i] + Val;
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
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

void ObtainAHistogram(int* Histo, int* AHisto)
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] = AHisto[i]  + Histo[j];
		}
	}
	/*FILE* fp = fopen("Ahistogram.txt", "wt");
	for (int i = 0; i < 256; i++) fprintf(fp, "%d\n", AHisto[i]);
	fclose(fp);*/
}

void HistogramStretching(BYTE* Img, BYTE* Out, int * Histo, int W, int H)
{
	int ImgSize = W * H;
	BYTE Low, High;

	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}

}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	/*FILE* fp = fopen("NormSum.txt", "wt");
	for (int i = 0; i < 256; i++) fprintf(fp, "%d\n", NormSum[i]);
	fclose(fp);*/
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}

void Binarization(BYTE * Img, BYTE * Out, int W, int H, double Threshold)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < (int)Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}
//4주차 과제

double GozalezBinThresh(double Threshold, int* Histo, int epsilon, BYTE Low, BYTE High)
{
	//오차
	double low_sum=0, low_pixel_num=0, high_sum=0, high_pixel_num=0, G2Avg, G1Avg;
	int ThresNext;
	double diff;
	double ThresDouble;
	ThresDouble = (double)Threshold;

	for (int i = (double)Low; i < Threshold; i++) {
		if (Histo[i] != 0) {
			low_sum += i;
			low_pixel_num += Histo[i];
		}
	}
	for (int i = (double)High; i > Threshold; i--) {
		if (Histo[i] != 0) {
			high_sum += i;
			high_pixel_num += Histo[i];
		}
	}

	G2Avg = low_sum / low_pixel_num;
	G1Avg = high_sum / high_pixel_num;
	ThresNext= (G2Avg + G1Avg) / 2;
	return ThresNext;
}

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("coin.bmp", "rb");
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
	int AHisto[256] = { 0 };
	int epsilon=3;
	BYTE High;
	BYTE Low;
	double Threshold;
	double diff;
	double ThresNext;
	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	Threshold = (double)(Low + High) / 2;
	do {
		ThresNext = GozalezBinThresh(Threshold, Histo, epsilon, Low, High);
		diff = Threshold - ThresNext;
		Threshold = ThresNext;
	} while (diff > epsilon);

	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Threshold);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, -120);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	fp = fopen("output.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}