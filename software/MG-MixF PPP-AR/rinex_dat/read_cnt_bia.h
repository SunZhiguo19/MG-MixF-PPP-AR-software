#ifndef READ_CNT_BIA_H
#define READ_CNT_BIA_H
#include "pos_algorith.h"
#include "read_orbex.h"
struct Sat_G_BIAData
{
    double C1C; double L1C;
    double C1P;
    double C1W;
    double C2S;
    double C2L;
    double C2X;
    double C2W; double L2W;
    double C5Q;
    double C5X; double L5I;
    Sat_G_BIAData(){
        C1C=C1P=C1W=C2S=C2L=C2X=C2W=C5Q=C5X=L1C=L2W=L5I=0.0;
    }
};
struct Sat_C_BIAData
{
    double C1P;
    double C1X; double L1X;
    double C7I; double L7I;
    double C6I; double L6I;
    double C2I; double L2I;
    double C5P;
    double C5X; double L5X;
    double C8X; double L8X;
    double C7Z; double L7Z;double C7D;
    Sat_C_BIAData(){
        C1P=C1X=C7I=C6I=C2I=C5P=C8X=C7Z=C7D=C5X=L7I=L6I=L2I=L8X=L7Z=L1X=L5X=0.0;
    }
};
struct Sat_R_BIAData
{
    double C1C;
    double C1P;
    double C2C;
    double C2P;
    Sat_R_BIAData(){
        C1P=C1C=C2C=C2P=0.0;
    }
};
struct Sat_E_BIAData
{
    double C1C; double L1C;
    double C5Q; double L5Q;
    double C7Q; double L7Q;
    double C6C; double L6C;
    double C8Q; double L8Q;
    double C1X; double L1X;
    double C5X; double L5X;
    double C6X; double L6X;
    double C7X; double L7X;
    double C8X; double L8X;
    Sat_E_BIAData(){
        C1C=C5Q=C7Q=C6C=L1C=L5Q=L7Q=L6C=C8Q=L8Q=0.0;
        C1X=C5X=C7X=C8X=L1X=L5X=L7X=L8X=C6X=L6X=0.0;
    }
};
struct BIAData
{/*OBX file data format*/
    int day_Y;
    int day_Seconds;
    QVector< Sat_G_BIAData > GPSbiaData;/*GPS [MAXPRNGPS]*/
    QVector< Sat_R_BIAData > GLObiaData;/*Glonass [MAXPRNGLO]*/
    QVector< Sat_C_BIAData > BDSbiaData;/*BDS [MAXPRNBDS]*/
    QVector< Sat_E_BIAData > GLEbiaData;/*Galileo [MAXPRNGLE]*/
    BIAData(){
        day_Y=0;day_Seconds=0;
    }
};
class read_cnt_bia:public read_orbex
{
public:
    read_cnt_bia();
    void releaseBiaAllData();
    void readbiaFilePartData(int epochNum);
    void closeBIAFile();
    void setBIAFileNames(QString biaFileNames);
    void getbia_Dat(int obs_day,double Seconds_day,char SatType,int PRN,
                    double *CODE_BIA,double *PHASE_BIA,QVector< QString > wantObserType);
    void getBIAData_WHU(BIAData &BiaData);
private:
    void getBIAData(BIAData *BiaData);

    void InitbiaDatStruct();
    void initbiaVar();
public:
    QString biafiles;
    bool IsSigalbiaFile;/*whether multiple files or a single file*/
    bool isReadbiaAllData;/*whether to read the entire file data*/
private:
    BIAData Biadat_ref;
    BIAData BiaData;
    QVector< BIAData > m_allEpochbiaData;
    QFile m_readBIAFileClass;/*read the.obx file class*/
    QString tempbiaLine;
    int ref_Second;
    int code_ref_Second;
    bool file_type;
    int dat_pos;
    int getbiaFlag;
    bool biadat_flag;
    int ref_flag;
};

#endif // READ_ORBEX_H
