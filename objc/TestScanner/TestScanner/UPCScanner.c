//
//  UPCScanner.c
//
//  Created by Ivan Klimchuk on 3/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "UPCScanner.h"
#include "UPCScannerInt.h"

const int codeUPC[10][4] = {{3,2,1,1},{2,2,2,1},{2,1,2,2},{1,4,1,1},{1,1,3,2},{1,2,3,1},{1,1,1,4},{1,3,1,2},{1,2,1,3},{3,1,1,2}};

void scanHorizLine(int *array, unsigned char *imgBytes, size_t imgWidth, size_t imgHeight, int scanThick)
{
    for (int y = imgWidth/2 - scanThick; y <= imgWidth/2 + scanThick; y++) {
        for (int x = 0; x < imgHeight; x++) {
            int i = (x * imgWidth + y) << 2;
            int c = imgBytes[i+0] + imgBytes[i+1] + imgBytes[i+2];
            array[x] += c;
        }
    }
}

void drawPixel(unsigned char *imgBytes, size_t imgWidth, size_t imgHeight, int x, int y)
{
    if ((x>=0)&&(x<imgHeight)&&(y>=0)&&(y<imgWidth)) {
        int i = (x * imgWidth + y) << 2;
        imgBytes[i+0] += 64;
        imgBytes[i+1] += 64;
        imgBytes[i+2] += 64;
    }
}

void drawLine(unsigned char *imgBytes, size_t imgWidth, size_t imgHeight, int x0, int y0, int x1, int y1)
{
    const int deltaX = abs(x0 - x1);
    const int deltaY = abs(y0 - y1);
    const int signX = x0 < x1 ? 1 : -1;
    const int signY = y0 < y1 ? 1 : -1;
    
    int error = deltaX - deltaY;
    int x = x0;
    int y = y0;
    
    while(x != x1 || y != y1) {
        drawPixel(imgBytes, imgWidth, imgHeight, x, y);
        const int error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x += signX;
        }
        if(error2 < deltaX) {
            error += deltaX;
            y += signY;
        }
    }
    drawPixel(imgBytes, imgWidth, imgHeight, x1, y1);
}

void drawHorizScanLine(unsigned char *imgBytes, size_t imgWidth, size_t imgHeight, int scanThick)
{
    for (int y = imgWidth/2 - scanThick; y <= imgWidth/2 + scanThick; y++)
        drawLine(imgBytes, imgWidth, imgHeight, 0, y, imgHeight, y);
}

void drawHorizHistogram(unsigned char *imgBytes, size_t imgWidth, int *scanDataArr, size_t imgHeight)
{
    for (int x = 2; x < imgHeight-2; x++) {
        drawLine(imgBytes, imgWidth, imgHeight, x, imgWidth/2 + imgWidth/8, x, imgWidth/2 + imgWidth/8 - (scanDataArr[x] >> 5));
    }
}

void drawHorizOffsHistogram(unsigned char *imgBytes, size_t imgWidth, int *scanDataArr, size_t imgHeight, int offset)
{
    for (int x = 2; x < imgHeight-2; x++) {
        drawLine(imgBytes, imgWidth, imgHeight, x, offset, x, offset - (scanDataArr[x] >> 5));
    }
}

void drawHorizLineHistogram(unsigned char *imageBytes, size_t imgWidth, size_t imgHeight, int *dataArray)
{
    for (int x = 0; x < imgHeight-1; x++) {
        drawLine(imageBytes, imgWidth, imgHeight, x, imgWidth/2, x, imgWidth/2 + (dataArray[x]>>4));
    }
}

void derivative(int *array, int *derivArray, size_t length)
{
    for (int i = 0; i < length-1; i++) {
        derivArray[i] = array[i+1] - array[i];
    }
}

void underivative(int *array, int *underivArray, size_t length)
{
    underivArray[0] = 0;
    for (int i = 0; i < length-1; i++) {
        underivArray[i+1] = underivArray[i] + array[i];
    }
}

void noiseReduction(int *array, int *filteredArray, size_t length, int noiseThreshold)
{
    int trSum = 0;
    for (int i = 0; i < length-1; i++) {
        if (abs(array[i]+trSum)<=noiseThreshold)
        {
            filteredArray[i]=0;
            trSum += array[i];
        } else {
            filteredArray[i]=array[i]+trSum;
            trSum = 0;
        }
            
    }
}

int concentrateData(int *dataArray, size_t length)
{
    int maxMinCount;
    int maxMinCnt = 0;
    int start = -1;
    int end = -1;
    int sign = 0;
    
    for (int i = 0; i < length-1; i++) {
        int curSign = (dataArray[i]>0)?1:(dataArray[i]<0)?-1:0;
        
        if (curSign!=0)
        {
            if (start<0)
            {
                sign = curSign;
                start = i;
                end = i;
            }
            
            if (sign == curSign) {
                end = i;
            } else {
                int scanDerivSum = 0;
                for (int j = start; j<=end; j++)
                {
                    scanDerivSum += dataArray[j];
                    dataArray[j] = 0;
                }
                
                int dsumMax = 0;
                int dsumMaxIndex = (start + end)/2;
                
                for (int jj = start; jj<=end; jj++) {
                    if (dsumMax<dataArray[jj]) {
                        dsumMax = dataArray[jj];
                        dsumMaxIndex = jj;
                    }
                }
                
                dataArray[dsumMaxIndex] = scanDerivSum;
                    
                maxMinCnt++;
                
                sign = curSign;
                start = i;
                end = i;
            }
        }
    }
    
    int scanDerivSum = 0;
    for (int j = start; j<=end; j++)
    {
        scanDerivSum += dataArray[j];
        dataArray[j] = 0;
    }
    dataArray[(start + end)/2] = scanDerivSum;
    maxMinCnt++;
    
    
    maxMinCount = maxMinCnt;
    return maxMinCount;
}

void extractChangepoints(int *extractedDataArray, int *dataArray, size_t lenght)
{
    int j = 0;
    for (int x = 0; x < lenght - 1; x++) {
        if (dataArray[x] != 0)
            extractedDataArray[j++] = x;
    }
}

//void changepointsToIntervals(int *dataArray, int *maxMinCount_p)
//{
//    for (int j = 1; j < *maxMinCount_p; j++) {
//        dataArray[j-1] = dataArray[j] - dataArray[j-1]; 
//    }
//    (*maxMinCount_p)--;
//}

void changepointsToIntervals(int *dataArray, int *intDataArray, int maxMinCount)
{
    for (int j = 1; j < maxMinCount; j++) {
        intDataArray[j-1] = dataArray[j] - dataArray[j-1]; 
    }
}


void calcAbsMaxMin(int *extrDataArr, int maxMinCount, int *start_p, int *end_p, int *absMin_p, int *absMax_p, float maxMinThreshold)
{
    *start_p = maxMinCount/2;
    *end_p = maxMinCount/2;
    
    *absMin_p = extrDataArr[maxMinCount/2];
    *absMax_p = extrDataArr[maxMinCount/2];
    
    for (int j = maxMinCount/2; j<maxMinCount; j++) {
        if (*start_p>0)
        {
            int newAbsMin = (*absMin_p<extrDataArr[*start_p-1])?*absMin_p:extrDataArr[*start_p-1];
            int newAbsMax = (*absMax_p>extrDataArr[*start_p-1])?*absMax_p:extrDataArr[*start_p-1];
            if (newAbsMax/newAbsMin < maxMinThreshold)
            {
                *absMin_p = newAbsMin;
                *absMax_p = newAbsMax;
                (*start_p)--;
            }
        }
        
        if (*end_p<maxMinCount-1)
        {
            int newAbsMin = (*absMin_p<extrDataArr[*end_p+1])?*absMin_p:extrDataArr[*end_p+1];
            int newAbsMax = (*absMax_p>extrDataArr[*end_p+1])?*absMax_p:extrDataArr[*end_p+1];
            if (newAbsMax/newAbsMin < maxMinThreshold)
            {
                *absMin_p = newAbsMin;
                *absMax_p = newAbsMax;
                (*end_p)++;
            }
        }
        
    }
}

float calcAvgMin(int absMin, int *extrDataArr, int je, int js)
{
    float avgMin;
    avgMin = absMin;
    int nAvgMin = 1;
    for (int i = js; i <= je; i++) {
        if (extrDataArr[i]/avgMin<0.5) {
            continue;
        }
        if (extrDataArr[i]/avgMin<1.5) {
            avgMin = (nAvgMin*avgMin + extrDataArr[i]/1.0) / (nAvgMin + 1);
            nAvgMin++;
            continue;
        }
        if (extrDataArr[i]/avgMin<2.5) {
            avgMin = (nAvgMin*avgMin + extrDataArr[i]/2.0) / (nAvgMin + 1);
            nAvgMin++;
            continue;
        }
        if (extrDataArr[i]/avgMin<3.5) {
            avgMin = (nAvgMin*avgMin + extrDataArr[i]/3.0) / (nAvgMin + 1);
            nAvgMin++;
            continue;
        }
        if (extrDataArr[i]/avgMin<4.5) {
            avgMin = (nAvgMin*avgMin + extrDataArr[i]/4.0) / (nAvgMin + 1);
            nAvgMin++;
            continue;
        }
        
    }
    return avgMin;
}

void normalize(float avgMin, int end, int *inputDataArray, int start, int *outputDataArray)
{
    for (int i = start; i <= end; i++) {
        outputDataArray[i-start] = (inputDataArray[end-i]/avgMin+0.5);
    }
}

int findLeftDataStart(int *normDataArr, int normDataLength)
{
    int startLeft=-1;
    for (int i = 0; i < normDataLength-2; i++)
        if ((normDataArr[(i-1<0)?0:i-1]>5) && (normDataArr[(i+3>normDataLength)?normDataLength:i+3]<8)) {
            
        if ((normDataArr[i]==1) && (normDataArr[i+1]==1) && (normDataArr[i+2]==1)) {
            startLeft = i + 3;
            break;
        }
        }

    return startLeft;
}

int findLeftDataEnd(int *normDataArr, int normDataLength, int startLeft)
{
    int endLeft=-1;
    if (startLeft>=0)
        for (int i = startLeft; i < normDataLength-4; i+=4)
            if ((normDataArr[i]==1) && (normDataArr[i+1]==1) && (normDataArr[i+2]==1) && (normDataArr[i+3]==1) && (normDataArr[i+4]==1)) {
                endLeft = i;
                break;
            }
    return endLeft;
}

unsigned char testDigit(int *arrayPart, int digit)
{
    unsigned char okf = 1;
    for (int k = 0; k < 4; k++) {
        okf = okf && ((arrayPart[k] == codeUPC[digit][k]));
    }
    return okf;
}

unsigned char testDigitCorr(int *arrayPart, int digit, int corrDigit, int corr)
{
    unsigned char ok = 1;
    for (int k = 0; k < 4; k++) {
        ok = ok && ((arrayPart[k] + ((corrDigit==k)?corr:0)) == codeUPC[digit][k]);
    }
    return ok;
}

int digitControlSum(int *arrayPart)
{
    int s = 0;
    for (int k = 0; k < 4; k++)
        s+=arrayPart[k];
    s = 7 - s;
    return s;
}

void decodeData(int *resultLength_p, char *resultString, int resultMaxLength, int *normalizedDataArray, int end, int start)
{
    if ((start>0) && (end>0))
        for (int i = start; i<end; i+=4) {
            int *arrayPart = &(normalizedDataArray[i]);
            
            int s = digitControlSum(arrayPart);
            
            int res = -1;
            for (int j = 0; j<10; j++) {
                unsigned char ok = 1;
                
                if (s == 0) {
                    
                    ok = ok && testDigit(&(normalizedDataArray[i]), j);;
                    
                } else {
                    if (abs(s) == 1) {
                        for (int l = 0; l < 4; l++) {
                            ok = 1;
                            for (int k = 0; k < 4; k++) {
                                ok = ok && ((normalizedDataArray[i+k] + ((l==k)?s:0) == codeUPC[j][k]));
                            }
                            if (ok)
                                break;
                        }
                    }
                }
                
                if (ok) {
                    res = j;
                    break;
                }
            }
            if (res>=0) {
                if (*resultLength_p<resultMaxLength) {
                    resultString[(*resultLength_p)++]=res+'0';
                }
            } else {
                if (*resultLength_p<resultMaxLength) {
            //        resultString[(*resultLength_p)++]='?';
                }
            }
        }
}

char checkControlDigit(char *resultCString, int resultStringLength)
{
    char correctResult;
    correctResult = 1;
    
    if (resultStringLength>0) {
        int s = 0;
        for (int i = 0; i < resultStringLength; i++) {
            int b = ((i+1)%2)*2+1;
            int c = resultCString[i]-'0';
            s += b * c;
        }
        if (s%10!=0) {
            correctResult = 0;
        }
    }
    return correctResult;
}

int scanAndReadUPC(uint8_t *imgAddress, size_t imgHeight, size_t imgWidth, char *resultCString, int resultMaxLength)
{
    int resCStrLength = 0;
    
    unsigned char *imageBytes = imgAddress;
    
    int *scanDataArr = malloc(imgHeight*sizeof(int));
    
    for (int jj = 0; jj < imgHeight; jj++) {
        if (scanDataArr[jj]!=0) {
            scanDataArr[jj]=0;
        }
    }
    
    int scanThick = 2;
    scanHorizLine(scanDataArr, imageBytes, imgWidth, imgHeight, scanThick);

    drawHorizScanLine(imageBytes, imgWidth, imgHeight, scanThick);
    
    drawHorizHistogram(imageBytes, imgWidth, scanDataArr, imgHeight);
    
    int *scanDataDerivArr = malloc((imgHeight-1)*sizeof(int));
    
    int noiseThreshold = 150;

    derivative(scanDataArr, scanDataDerivArr, imgHeight);

    int *scanDataFilteredArr = malloc((imgHeight-1)*sizeof(int));

    noiseReduction(scanDataDerivArr, scanDataFilteredArr, imgHeight, noiseThreshold);
    free(scanDataDerivArr);

//    free(scanDataArr);
    
    int maxMinCount = concentrateData(scanDataFilteredArr, imgHeight);
    
    underivative(scanDataFilteredArr, scanDataArr, imgHeight);
    drawHorizHistogram(imageBytes, imgWidth, scanDataArr, imgHeight);
    
    free(scanDataArr);
    
    
    drawHorizLineHistogram(imageBytes, imgWidth, imgHeight, scanDataFilteredArr);
    
    int *extrDataArr = malloc(maxMinCount*sizeof(int));
    int *intrDataArr = malloc(maxMinCount*sizeof(int));
    
    extractChangepoints(extrDataArr, scanDataFilteredArr, imgHeight);
    free(scanDataFilteredArr);
    
    changepointsToIntervals(extrDataArr, intrDataArr, maxMinCount);
    maxMinCount--;
    
    //float maxMinThreshold = 4.625;
    float maxMinThreshold = 4.2;
    
    if (maxMinCount>2)
    {
        int js;
        int je;
        
        int absMin;
        int absMax;

        calcAbsMaxMin(intrDataArr, maxMinCount, &js, &je, &absMin, &absMax, maxMinThreshold);
        
        float avgMin = calcAvgMin(absMin, intrDataArr, je, js);
                
        int normDataLength = je - js + 1;
        int *normDataArr = malloc((normDataLength)*sizeof(int));
        
        normalize(avgMin, je, intrDataArr, js, normDataArr);
        
        
        int startLeft = -1;
        int endLeft = -1;
        int startRight = -1;
        int endRight = -1;
        startLeft = findLeftDataStart(normDataArr, normDataLength);
        
        endLeft = findLeftDataEnd(normDataArr, normDataLength, startLeft);
        
        if (endLeft+5 < normDataLength)
        {
            startRight = endLeft+5;
            if ((endLeft - startLeft) + startRight < normDataLength)
                endRight = (endLeft - startLeft) + startRight;
        }
        
        if (startLeft>=0)
            drawLine(imageBytes, imgWidth, imgHeight, extrDataArr[startLeft], 0, extrDataArr[startLeft], imgWidth);
        if (endLeft>=0)
            drawLine(imageBytes, imgWidth, imgHeight, extrDataArr[endLeft], 0, extrDataArr[endLeft], imgWidth);
        if (startRight>=0)
            drawLine(imageBytes, imgWidth, imgHeight, extrDataArr[startRight], 0, extrDataArr[startRight], imgWidth);
        if (endRight>=0)
            drawLine(imageBytes, imgWidth, imgHeight, extrDataArr[endRight], 0, extrDataArr[endRight], imgWidth);


        decodeData(&resCStrLength, resultCString, resultMaxLength, normDataArr, endLeft, startLeft);
        decodeData(&resCStrLength, resultCString, resultMaxLength, normDataArr, endRight, startRight);        
        free(normDataArr);

        
        char correctResult;
        
        correctResult = checkControlDigit(resultCString, resCStrLength);
        
        if (correctResult == 0) {
            resCStrLength = 0;
        }
    }

    free(extrDataArr);
    free(intrDataArr);
    return resCStrLength;
}


