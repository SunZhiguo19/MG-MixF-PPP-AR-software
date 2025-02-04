#ifndef READ_ORBEX_H
#define READ_ORBEX_H
#include"pos_algorith.h"
#include"rinex_sp3.h"
struct SatOBXData
{
    double q0_scalar;
    double q1_x;
    double q2_y;
    double q3_z;
    SatOBXData(){
        q0_scalar=q1_x=q2_y=q3_z=0.0;
    }
};
struct OBXData
{/*OBX file data format*/
    gtime_t GPSTime;/*GPS week within seconds*/

    QVector< SatOBXData > GPSobxData;/*GPS [MAXPRNGPS]*/
    QVector< SatOBXData > GLOobxData;/*Glonass [MAXPRNGLO]*/
    QVector< SatOBXData > BDSobxData;/*BDS [MAXPRNBDS]*/
    QVector< SatOBXData > GLEobxData;/*Galileo [MAXPRNGLE]*/
};
class read_orbex:public rinex_sp3
{
public:
    read_orbex();
    void releaseOBXAllData();
    void readobxFilePartData(int epochNum);
    void closeOBXFile();
    void setOBXFileNames(QString obxFileNames);
    void getATT_Dat(gtime_t obs_T,char SatType,int PRN,MatrixXd &quater);
private:
    void getOBXData(OBXData *ObxData);
    void InitobxDatStruct();
    void initobxVar();
public:
//    QStringList m_obxFileNames;
    QString obxfiles;
    bool IsSigalobxFile;/*whether multiple files or a single file*/
    bool isReadobxAllData;/*whether to read the entire file data*/
private:
    OBXData Obxdat_ref;
    OBXData ObxData;
    QVector< OBXData > m_allEpochobxData;
    QFile m_readOBXFileClass;/*read the.obx file class*/
    QString tempobxLine;
    int obx_dat_pos;
    int getobx_flag;

};

#endif // READ_ORBEX_H
