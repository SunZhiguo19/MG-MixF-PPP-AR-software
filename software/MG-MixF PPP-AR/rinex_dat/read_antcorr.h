#ifndef READ_ANTCORR_H
#define READ_ANTCORR_H
#include "pos_algorith.h"
#include "read_dcb.h"
/*define the storage antenna data structure.*/
struct FrqData
{/*define a PCO and PCV data format for the frequency band*/
    QString FrqFlag;
    double PCO[5];
    /*PCV data*/
    short int Row;
    short int Column;/*number of PCVAZI and PCVNoAZI columns*/
    QVector< double > PCVNoAZI;
    QVector< double > PCVAZI;/*the matrix is converted to a 1-dimensional vector*/
    FrqData(){
        for(int i=0;i<5;i++)    PCO[i]=0;
    }

};

struct AntDataType
{
    QString StrAntType;/*antenna type*/
    QString SatliCNN;/*CNN satellite Numbers G,R,E,C..*/
    double DAZI;/*store azimuth interval 360/DAZI*/
    double ZEN_12N[3];/* store high Angle range and interval (ZEN_12N [1] - ZEN_12N [0])/ZEN_12N [2]*/
    short NumFrqu;/*number of frequency bands of satellites*/
    double ValidJD;/*valid start time*/
    double EndJD;/*valid end time*/
    QVector <FrqData> PCOPCV;/*PCO and PCV data for multiple frequency bands are stored*/
    bool IsSat;/*true represents satellite false represents receiver data*/
    bool isFrqData;/*if all antenna files are not found, mark false*/
    AntDataType(){
        StrAntType=SatliCNN=' ';NumFrqu=0;
        DAZI=ZEN_12N[0]=ZEN_12N[1]=ZEN_12N[2]=ValidJD=0.0;
        EndJD=999999999;IsSat=isFrqData=false;
    }
};
/* global variables ----------------------------------------------------------*/



class read_antcorr:public read_dcb
{
public:
    read_antcorr();
    void setObsJD(QString AnTypeName,double ObsJD);
    void setAntFileName(QString AntFileName = "",QString AnTypeName = "",double ObvJD = 0);
    bool getRecvL12(double E, double A, char SatType, double *LxOffset, QVector<QString> FrqFlag);
    bool getSatOffSet(double *timeV, int PRN, char SatType, double *StaPos, double *RecPos, double *L12Offset,
                      QVector<QString> FrqFlag);
    bool getSatOffSet_quater(double *timeV,int PRN,char SatType,double *StaPos,double *RecPos,
                            double *L12Offset, QVector<QString> FrqFlag,MatrixXd quater);
    bool getSatOffSet_RTyaw(double *timeV,int PRN,char SatType,double *StaPos,double *RecPos,
                            double *sat_Ve,double *L12Offset, QVector<QString> FrqFlag,double yaw);
    bool getAntAllData();
private:
    bool readFileAntHead();
    void initANTVar();
    bool readRecvData();
    bool readSatliData();
    bool openFiles(QString AntFileName);
    bool getPCV(const AntDataType &tempAntData, char SatType, double *PCV12,
                double Ztop, double AZI, QVector<QString> FrqFlag);
    void findPCO(double *pco, QVector<FrqData> &PCOPCV, char SatType, QString frqStr);

public:
    QString m_AntFileName;/*save the antenna data file name*/
    QString m_AntType;/*save the receiver antenna type name*/
    QFile m_ReadAntFileClass;/*read the file class*/
    bool isReadANTAllData;/*whether to read all data flags*/
    double m_ObeservTime;/*Julian days to store observation O files to match the effective time of the satellite antenna*/
    bool isReadAntHead;/*whether to read header file*/
    bool isReadSatAntData;/*whether to read satellite antenna data*/
    bool isReadRecvData;/*whether to read receiver antenna data*/
    QString m_tempAntLine;/*cache read row data*/
    bool ant_wind_up;
    AntDataType m_RecvData;/*data of receiver antenna PCO and PCV are saved*/
    QVector< AntDataType > m_SatData;/*data of multiple satellite antenna PCO and PCV are saved*/
    double m_sunSecFlag;/*if the reference epoch changes, the solar coordinates need to be recalculated to store the reference epoch in seconds*/

private:

};

#endif // READ_ANTCORR_H
