#ifndef RINEX_OBS_H
#define RINEX_OBS_H
#include "pos_algorith.h"
#include "read_cnt_bia.h"
/*Read satellite observation files, able to read versions RINEX 2.0 and 3.0*/
struct  obsVarNamesVer3
{/*version 3 is used*/
    int obsNum3ver;/*version 3 is used*/
    QString SatType;/*G R C S E: system number*/
    QVector< QString > obsNames3ver;/*version 3 is used*/
};

/*store carrier and pseudo-range dual-frequency types and positions
 * (which can be extended to three frequencies)*/
struct CLPos
{
  QString SatType;/*G R C S E: system number*/

  QVector< QString > C1Type;
  QVector< int > C1Pos;
  QVector< QString > C2Type;
  QVector< int > C2Pos;
  QVector< QString > C3Type;
  QVector< int > C3Pos;
  QVector< QString > C4Type;
  QVector< int > C4Pos;
  QVector< QString > C5Type;
  QVector< int > C5Pos;

  QVector< QString > L1Type;
  QVector< int > L1Pos;
  QVector< QString > L2Type;
  QVector< int > L2Pos;
  QVector< QString > L3Type;
  QVector< int > L3Pos;
  QVector< QString > L4Type;
  QVector< int > L4Pos;
  QVector< QString > L5Type;
  QVector< int > L5Pos;

  QVector< QString > S1Type;
  QVector< int > S1Pos;
  QVector< QString > S2Type;
  QVector< int > S2Pos;
  QVector< QString > S3Type;
  QVector< int > S3Pos;
  QVector< QString > S4Type;
  QVector< int > S4Pos;
  QVector< QString > S5Type;
  QVector< int > S5Pos;
};


class rinex_obs:public read_cnt_bia
{
public:
    rinex_obs();
    void getAntHEN(double* m_AntHEM);
    void getApproXYZ(double* AppXYZ);
    void getMultEpochData(QVector< QVector< SatlitData > >&multEpochData,int epochNum);
    void setObsFileName(QStringList OfileName);
    void setWangObsType(QVector<QStringList> wantObsTypeVct);
    void getFistObsTime(int* m_YMDHM,double &Seconds);
    double getInterval();
    bool isObsEnd();
    QString getMakerName();
    QString getAntType();
    void closeobsFile();
private:
    void initObsVar();
    void getHeadInf();
    void getEpochData(QVector< SatlitData > &epochData);
    bool getOneSatlitData(QString &dataString,SatlitData &oneSatlite);
    void getFrequency(SatlitData &oneSatlite);
    void getFrequencyVer2(SatlitData &oneSatlite);
    void getFrequencyVer3(SatlitData &oneSatlite);
    void getObsType(SatlitData &oneSatlite);
    void readEpochVer3(QVector< SatlitData > &epochData);
    void readEpochVer2(QVector< SatlitData > &epochData);
    QString getObsTypeData(SatlitData &oneSatlit, QString obsType, double &obsValue);
    void ProcessCLPos(obsVarNamesVer3 epochSystem);
    void PriorityRanking();
    void getWantData_2(SatlitData &oneSatlit);
    void getWantData_3(SatlitData &oneSatlit);

    QString getComment();
    void arrange_obs();
    QString getReciveType();

//public:
//    QString m_OfileName;/*save the O file name*/
//    QStringList mult_OfileName;/*save the mult O file name*/

//    QString The_same_ant;
private:
    int baseYear;/*Version 2.x is used to get the baseYear (for example, 1989 baseYear = 1900; 2010 baseYear = 2000)*/
    /*MARKER NAME*/
    QString MarkerName;
    double ApproxXYZ[3];
    /*ANTENNA: DELTA H/E/N*/
    double AntHEN[3];
    /*WAVELENGTH FACT L1/2*/
    /*INTERVAL*/
    double IntervalSeconds;
    /*TIME OF FIRST OBS*/
    int YMDHM[5];
    double ObsSeconds;
    QString SateSystemOTime;/*GPS UTC*/
    QVector<QStringList> m_wantObsTypeVct;
    /*internal variables*/
    QFile m_readOFileClass;// reads the O file class
    bool m_IsCloseFile;
    int Serial_number;
    bool isReadHead;
    QString tempLine;/*one-line string buffering*/
    QVector< CLPos > m_allObsTypePos;
    QVector< obsVarNamesVer3 > m_obsVarNamesVer3;/*Version is larger than 3 storage multiple systems SYS / # / OBS TYPES*/
    QVector< QString > m_ObservVarsNames_2;/*Version less than 3 is used //# / TYPES OF OBSERV*/
    int m_TypeObservNum_2;/*Version less than 3 is used //# / TYPES OF OBSERV*/
    QRegExp matchHead;/*Version 2.x USES a matching header file (just initialize it once)*/
    /*header file information*/
    /*RINEX VERSION / TYPE*/
    double RinexVersion;
    QChar FileIdType;
    QChar SatelliteSys;
    /*PGM / RUN BY / DATE*/
    QString PGM;
    QString RUNBY;
    QString CreatFileDate;
    /*COMMENT*/
    QString CommentInfo;

    /*MARKER NUMBER*/
    QString MarkerNumber;
    /*OBSERVER / AGENCY*/
    QString ObserverNames;
    QString Agency;
    /*REC # / TYPE / VERS*/
    QString ReciverREC;
    QString ReciverType;
    QString ReciverVers;
    /*ANT # / TYPE*/
    QString AntNumber;
    QString AntType;
    /*APPROX POSITION XYZ*/

    int FactL12[2];
};


#endif // RINEX_OBS_H
