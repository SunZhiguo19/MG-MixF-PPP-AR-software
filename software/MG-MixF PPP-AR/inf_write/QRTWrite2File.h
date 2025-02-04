#ifndef QRTWRITE2FILE_H
#define QRTWRITE2FILE_H

#include "pos_algorith.h"
#include "AGNSS_APPData.h"

#include "pos_alg/mat_cal.h"
class QRTWrite2File
{
// Functional part
public:
    QRTWrite2File(void);
    ~QRTWrite2File(void);
    bool setSaveFloder(QString fload_path);
    void closeSaveFile();
    bool writeRecivePos2Txt(const RecivePos oneRecivePos, MatrixXd *Qmat=NULL);// The result of calculating ENU direction is written to TXT
    bool writePPP2Txt(const QVector<SatlitData> epochSatlite, int out_Comb_mode);// Corrections to errors are written to. PPP file
    bool writeClockZTDW2Txt(const ClockData epochZTDWClock, QString write_sys_str);// Write zenith wet delay and clock difference to TXT first column wet delay second column clock difference
    bool writeAmbiguity2Txt(QString fload_path);// Write the satellite stored in all Ambiguity variable into TXT file named "G32.txt", "C02.txt", "R08.txt" and so on. The first column is ambiguity.
    bool WriteEpochPRN(QString fload_path, QString tempfileName);// Write the satellite stored in variable allAmbiguity to the file, the first column epoch, the second column satellite number
    bool writeRecivePosKML(QString fload_path, QString tempfileName);// generate KML format
    bool writeBadSatliteData(const SatlitData oneSatAmb);// Write the bad satellite stored in variable allBadSatlitData to the file
    bool writeAmbiguity_information(const QVector < SatlitData > epochSatlite, int out_Comb_mode, VectorXd X, MatrixXd *Xq_mat);
    bool writeGNSS_SDNLAmbiguity2Txt(QVector < SatlitData > epochSatlite, int *system_flag);
    bool write_dops(const RecivePos oneRecivePos);
    bool writeAzEL2Txt(const QVector < SatlitData > epochSatlite);
    bool writeQdebug2Txt(QString qdebug_log);
    bool writeambupd(const QVector < SatlitData > epochSatlite);
private:
    bool isDirExist(QString fullPath);
    bool WriteAmbPRN(QString temp_floder, int PRN,char SatType);// Fuzziness of Writing to PRN Satellite
    int findFilenameFlag(QString Filename);
    double WriteYMD2GPSTime(int Year, int Month, int Day, int HoursInt, int Minutes, double Seconds, int *GPSWeek=NULL, int *day=NULL);
    void WriteXYZ2BLH(double *pXYZ,double *m_pBLH);
// Variable part
public:
    QVector< RecivePos > allReciverPos;// Save the position result to TXT KML
    QVector< QVector < SatlitData > > allPPPSatlitData;// Save the data calculated by PPP to write to. PPP file
    QVector< Ambiguity> allAmbiguity;// Ambiguity of Storage Satellite
    QVector< ClockData > allClock;// Store Clock Data
    QVector< VectorXd > allSolverX;// Storage Solution X
    QVector< MatrixXd > allSloverQ;// Storage Covariance Matrix
    QVector< SatlitData > allBadSatlitData;// Storage Elimination Satellites
//    int out_Comb_mode;
//    QString write_sys_str;
private:
    mat_cal mat_cal_M;
};

#endif
