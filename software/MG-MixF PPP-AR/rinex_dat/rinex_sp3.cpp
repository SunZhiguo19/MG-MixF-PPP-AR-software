#include "rinex_sp3.h"
const int rinex_sp3::lagrangeSp3Fact = 8;//Constant initialization
rinex_sp3::rinex_sp3()
{

}

void rinex_sp3::initSp3Var()
{
    tempSp3Line = "";
    isReadSp3Head = false;
    isReadAllSp3Data = false;
    IsSigalSp3File = false;
    m_allEpochSp3Data.clear();
    SP3Data epochSp3Data_init;
    epochSp3Data_ref = epochSp3Data_init;
    InitSp3DatStruct();
    /*Contains only a total of 32 satellites in the GPS system*/
    m_lastGPSTime_Sp3Flag = 0;/*As a starting marker*/
    m_lastCmpGPS_Sp3Time = 999999999;/*Record the GPS time of the initial calculation*/
    m_SP3FileName = "";
    m_WeekSp3Order = 0;
    ACCELERATE = true;

}

/*init class Set SP3 file name*/
void rinex_sp3::setSP3FileNames(QStringList SP3FileNames)
{
    initSp3Var();
    if (SP3FileNames.length() > 0)
    {
        m_SP3FileNames =SP3FileNames;
    }
    else
    {
        isReadAllSp3Data = true;
        return ;
    }
}
void rinex_sp3::InitSp3DatStruct()
{
    if (isInSystem('G'))
    {
        epochSp3Data_ref.GPSSP3_data.resize(MAXPRNGPS);
    }
    if (isInSystem('R'))
    {
        epochSp3Data_ref.GLOSP3_data.resize(MAXPRNGLO);
    }
    if (isInSystem('C'))
    {
        epochSp3Data_ref.BDSSP3_data.resize(MAXPRNBDS);
    }
    if (isInSystem('E'))
    {
        epochSp3Data_ref.GLESP3_data.resize(MAXPRNGLE);
    }
}
/*Read multiple file data*/
void rinex_sp3::readFileData2Vec(QStringList SP3FileNames)
{
    if (SP3FileNames.length() == 0) isReadAllSp3Data = true;
    if (isReadAllSp3Data)
        return;
    m_allEpochSp3Data.clear();
/*First read the header file to read the file from small to large according to the time*/
    int minGPSWeek = 999999999;/*Save the minimum week and reduce the int out of bounds (weeks are converted to seconds)*/
    QVector< int > tempGPSWeek,tempGPSSeconds,fileFlagSeconds;/*Save the file to observe the actual time*/
    for (int i = 0;i < SP3FileNames.length();i++)
    {
        int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,GPSWeek = 0,GPSSeconds = 0;
        double Seconds = 0;
        QString Sp3FileName = SP3FileNames.at(i);
        QFile sp3file(Sp3FileName);
        if (!sp3file.open(QFile::ReadOnly))
        {
            QString erroInfo = "Open " + Sp3FileName + "faild!";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
        }
        /*Read header file*/
        tempSp3Line = sp3file.readLine();/*Read the first line*/
        Year = tempSp3Line.mid(3,4).toInt();
        Month = tempSp3Line.mid(8,2).toInt();
        Day = tempSp3Line.mid(11,2).toInt();
        Hours = tempSp3Line.mid(14,2).toInt();
        Minutes = tempSp3Line.mid(17,2).toInt();
        Seconds = tempSp3Line.mid(20,11).toDouble();
        GPSSeconds = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&GPSWeek);
        if (GPSWeek <= minGPSWeek)
            minGPSWeek = GPSWeek;
        tempGPSWeek.append(GPSWeek);
        tempGPSSeconds.append(GPSSeconds);
        sp3file.close();
    }
    /*Convert to seconds*/
    QVector< int > WeekOrder;/*Save the data, mark whether it is cross-week,
                              *Cross: 1 does not span: 0  read multiple files need to use*/
    for (int i = 0;i < tempGPSWeek.length();i++)
    {
        int Seconds = (tempGPSWeek.at(i) - minGPSWeek)*604800 + tempGPSSeconds.at(i);
        WeekOrder.append(tempGPSWeek.at(i) - minGPSWeek);
        fileFlagSeconds.append(Seconds);
    }
/*Sort file names by time*/
    for (int i = 0; i < fileFlagSeconds.length();i++)
    {
        for (int j = i+1;j < fileFlagSeconds.length();j++)
        {
            if (fileFlagSeconds.at(j) < fileFlagSeconds.at(i))
            {
                /*Exchange file name*/
                QString tempFileName = SP3FileNames.at(i);
                SP3FileNames[i] = SP3FileNames.at(j);
                SP3FileNames[j] = tempFileName;
                /*Exchange week sign*/
                int tempWeek = WeekOrder.at(i);
                WeekOrder[i] = WeekOrder.at(j);
                WeekOrder[j] = tempWeek;
            }
        }
    }
    m_WeekSp3Order = 0;
    for (int i = 0;i < WeekOrder.length();i++)
    {
        m_WeekSp3Order += WeekOrder[i];/*Save cross-week mark*/
    }
/*Read all files by time*/
    for (int i = 0;i < SP3FileNames.length();i++)
    {
        QString Sp3FileName = SP3FileNames.at(i);
        QFile sp3file(Sp3FileName);
        if (!sp3file.open(QFile::ReadOnly))
        {
            QString erroInfo = "Open " + Sp3FileName + "faild!";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
        }
/*Read header file*/
        tempSp3Line = sp3file.readLine();/*Read the first line*/
        QString flagHeadEnd = "*";
        QString pre3Char = tempSp3Line.mid(0,1);
        while (pre3Char != flagHeadEnd)
        {
            tempSp3Line = sp3file.readLine();/*Read the next line*/
            pre3Char = tempSp3Line.mid(0,1);
        }

/*Read data part*/
        /*When entering tempLin for the first time, the first epoch is reached. * YYYY-MM.......*/
        int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
        double Seconds = 0;
        while (!sp3file.atEnd())
        {
            /*Judge whether to end*/
            if (tempSp3Line.mid(0,3) == "EOF")
                break;
            SP3Data epochSp3Data = epochSp3Data_ref;
            /*Read header calendar metadata*/
            /*It is best to clear before use,
             * although it increases the amount of calculation but is safe.*/
            Year = tempSp3Line.mid(3,4).toInt();
            Month = tempSp3Line.mid(8,2).toInt();
            Day = tempSp3Line.mid(11,2).toInt();
            Hours = tempSp3Line.mid(14,2).toInt();
            Minutes = tempSp3Line.mid(17,2).toInt();
            Seconds = tempSp3Line.mid(20,11).toDouble();
            epochSp3Data.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//Add 604800s across the week
            epochSp3Data.GPSWeek = Week;
            /*Read satellite coordinates */
            tempSp3Line = sp3file.readLine();/*Read a row of coordinate data*/
            while (tempSp3Line.mid(0,3) != "EOF"&&tempSp3Line.mid(0,1) == "P")
            {
                int PRN = 0;
                char tempSatType = 'G';
                /*GPS system*/
                tempSatType = *(tempSp3Line.mid(1,1).toLatin1().data());
                if(tempSatType == 32) tempSatType = 'G';
                PRN = tempSp3Line.mid(2,2).toInt();
                if (tempSatType == 'G'&&isInSystem(tempSatType))
                {
                    epochSp3Data.GPSSP3_data[PRN-1].SP3_x = 1000*tempSp3Line.mid(5,13).toDouble();/*X*/
                    epochSp3Data.GPSSP3_data[PRN-1].SP3_y = 1000*tempSp3Line.mid(19,13).toDouble();/*Y*/
                    epochSp3Data.GPSSP3_data[PRN-1].SP3_z = 1000*tempSp3Line.mid(33,13).toDouble();/*Z*/
                    epochSp3Data.GPSSP3_data[PRN-1].SP3_clk = 1000*tempSp3Line.mid(47,13).toDouble()*M_C;/*sp3 Clk*/
                }
                /*Glonass system*/
                else if (tempSatType == 'R'&&isInSystem(tempSatType))
                {
                    epochSp3Data.GLOSP3_data[PRN-1].SP3_x = 1000*tempSp3Line.mid(5,13).toDouble();/*X*/
                    epochSp3Data.GLOSP3_data[PRN-1].SP3_y = 1000*tempSp3Line.mid(19,13).toDouble();/*Y*/
                    epochSp3Data.GLOSP3_data[PRN-1].SP3_z = 1000*tempSp3Line.mid(33,13).toDouble();/*Z*/
                    epochSp3Data.GLOSP3_data[PRN-1].SP3_clk = 1000*tempSp3Line.mid(47,13).toDouble()*M_C;/*sp3 Clk*/
                }
                /*BDS system*/
                else if (tempSatType == 'C'&&isInSystem(tempSatType))
                {
                    epochSp3Data.BDSSP3_data[PRN-1].SP3_x = 1000*tempSp3Line.mid(5,13).toDouble();/*X*/
                    epochSp3Data.BDSSP3_data[PRN-1].SP3_y = 1000*tempSp3Line.mid(19,13).toDouble();/*Y*/
                    epochSp3Data.BDSSP3_data[PRN-1].SP3_z = 1000*tempSp3Line.mid(33,13).toDouble();/*Z*/
                    epochSp3Data.BDSSP3_data[PRN-1].SP3_clk = 1000*tempSp3Line.mid(47,13).toDouble()*M_C;/*sp3 Clk*/
                }
                /*Galileo system*/
                else if (tempSatType == 'E'&&isInSystem(tempSatType))
                {
                    epochSp3Data.GLESP3_data[PRN-1].SP3_x = 1000*tempSp3Line.mid(5,13).toDouble();/*X*/
                    epochSp3Data.GLESP3_data[PRN-1].SP3_y = 1000*tempSp3Line.mid(19,13).toDouble();/*Y*/
                    epochSp3Data.GLESP3_data[PRN-1].SP3_z = 1000*tempSp3Line.mid(33,13).toDouble();/*Z*/
                    epochSp3Data.GLESP3_data[PRN-1].SP3_clk = 1000*tempSp3Line.mid(47,13).toDouble()*M_C;/*sp3 Clk*/
                }
                tempSp3Line = sp3file.readLine();/*Read a row of coordinate data*/
            }
            if (!m_allEpochSp3Data.empty() && m_allEpochSp3Data.back().GPSTime == epochSp3Data.GPSTime)
                m_allEpochSp3Data.back() = epochSp3Data;/*Replace equal GPSTime data*/
            else
                m_allEpochSp3Data.append(epochSp3Data);/*Save a file data*/
        }/*End of reading file*/
        sp3file.close();
    }/*Read multiple files at the end*/
    isReadAllSp3Data = true;
}

/*lagrangeSp3Fact point coordinates ；pGPST:
 * lagrangeSp3Fact points within GPS week seconds;
 * GPST satellite launch time within weeks*/
void rinex_sp3::get8Point(int PRN,char SatType,double *pX,double *pY,double *pZ,int *pGPST,double GPST ,double *pClk)
{/*Get the nearest lagrangeSp3Fact points*/
    int lengthEpoch = m_allEpochSp3Data.length();
    /*Discover the location of GPST throughout the epoch*/
    int GPSTflag = m_lastGPSTime_Sp3Flag;
    int numPoint = lagrangeSp3Fact / 2;/*Take numPoint points before and after*/
    /*More than 0.3s indicates that the epoch interval of the observation epoch is
     * definitely above 1 s (reducing the multiple Query position of the same epoch)*/
    if (qAbs(m_lastCmpGPS_Sp3Time - GPST) > 0.3)
    {
        if (ACCELERATE)	m_lastCmpGPS_Sp3Time = GPST;
        for (int i = m_lastGPSTime_Sp3Flag;i < lengthEpoch;i++)
        {
            SP3Data epochSp3Data = m_allEpochSp3Data.at(i);
            if (epochSp3Data.GPSTime >= GPST)
                break;
            else
                GPSTflag++;
        }
    }
    if (ACCELERATE) m_lastGPSTime_Sp3Flag = GPSTflag;/*Save the latest location*/

/*Take numPoint points before and after, consider the boundary problem*/
    if ((GPSTflag >= numPoint - 1) && (GPSTflag <= lengthEpoch - numPoint - 1))
    {/*In front of the middle position four contain itself, the last four do not contain itself*/
        for (int i = 0;i < lagrangeSp3Fact;i++)
        {
            SP3Data epochSp3Data = m_allEpochSp3Data.at(GPSTflag - numPoint + 1 + i);
            /*Determine which system data belongs to*/
            switch(SatType)
            {
            case 'G':
                pX[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'R':
                pX[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'C':
                pX[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'E':
                pX[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            default:
                pX[i] = 0;
                pY[i] = 0;
                pZ[i] = 0;
                pClk[i] = 0;
                pGPST[i] = 0;
            }
        }
    }
    else if(GPSTflag < numPoint - 1)
    {/*At the start position boundary*/
        for (int i = 0;i < lagrangeSp3Fact;i++)
        {
            SP3Data epochSp3Data = m_allEpochSp3Data.at(i);
            /*Determine which system data belongs to*/
            switch(SatType)
            {
            case 'G':
                pX[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'R':
                pX[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'C':
                pX[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'E':
                pX[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            default:
                pX[i] = 0;
                pY[i] = 0;
                pZ[i] = 0;
                pClk[i] = 0;
                pGPST[i] = 0;
            }
        }
    }
    else if(GPSTflag > lengthEpoch - numPoint - 1)
    {/*End position boundary*/
        for (int i = 0;i < lagrangeSp3Fact;i++)
        {
            SP3Data epochSp3Data = m_allEpochSp3Data.at(lengthEpoch - (lagrangeSp3Fact-i));
            /*Determine which system data belongs to*/
            switch(SatType)
            {
            case 'G':
                pX[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GPSSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'R':
                pX[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GLOSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'C':
                pX[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.BDSSP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            case 'E':
                pX[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_x;
                pY[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_y;
                pZ[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_z;
                pClk[i] = epochSp3Data.GLESP3_data[PRN-1].SP3_clk;
                pGPST[i] = epochSp3Data.GPSTime;
                break;
            default:
                pX[i] = 0;
                pY[i] = 0;
                pZ[i] = 0;
                pClk[i] = 0;
                pGPST[i] = 0;
            }
        }
    }
}

/*Lagrangian method*/
void rinex_sp3::lagrangeMethod(int PRN,char SatType,double GPST,double *pXYZ,double *pdXYZ, double *pSp3Clk,double *Orbit_Var)
{
    /*Lagrangian interpolation, here and after the selection - a total of 8 points for interpolation.
     * GPST-satellite launch time within seconds of the week*/
    for (int i = 0;i < 3;i++)
    {/*Initialization, security*/
        pXYZ[i] = 0;
        pdXYZ[i] = 0;
    }
    if(pSp3Clk) *pSp3Clk = 0;
    /*Determine whether to set up a legal system*/
    if (!isInSystem(SatType))
        return ;
    /*Determine whether to read data*/
    if (!isReadAllSp3Data)
    {
        readFileData2Vec(m_SP3FileNames);
    }
    double pX[lagrangeSp3Fact]={0},pY[lagrangeSp3Fact] = {0},pZ[lagrangeSp3Fact] = {0}, pClk[lagrangeSp3Fact] = {0};
    int pGPST[lagrangeSp3Fact] = {0};
    get8Point(PRN,SatType,pX,pY,pZ,pGPST,GPST,pClk);/*Obtain the coordinates of the last 8 points
                                                      *of the PRN satellite launch time*/
    /*Check if the data is missing (the data in the orbit interpolation
     * of the precise ephemeris cannot be missing)*/
    for (int i = 0;i <lagrangeSp3Fact;i++)
        if (!(pX[i]&&pY[i]&&pZ[i])) return ;
    double sumX = 0,sumY = 0,sumZ = 0, sumClk = 0;/*Interpolated coordinates XYZ*/
    double sumDX[2] = {0},sumDY[2] = {0},sumDZ[2] = {0};/*Satellite coordinates of +-0.5s before and after interpolation
                                                         *are used to simultaneously determine the speed*/
    double lk = 1,ldk[2] = {1,1};/*timediff(nav->peph[k].time,time)*/
    double t_diff[lagrangeSp3Fact] = {0};
    for (int k = 0; k < lagrangeSp3Fact;k++)
    {
        t_diff[k] = pGPST[k] - GPST;
        for (int n = 0;n < lagrangeSp3Fact;n++)
        {
            if (k == n) continue;
            lk = lk*(GPST - pGPST[n])/(pGPST[k] - pGPST[n]);
            ldk[0] = ldk[0]*(GPST - 0.5 - pGPST[n])/(pGPST[k] - pGPST[n]);
            ldk[1] = ldk[1]*(GPST + 0.5 - pGPST[n])/(pGPST[k] - pGPST[n]);
        }
        sumX = sumX + pX[k]*lk;sumDX[0] = sumDX[0] + pX[k]*ldk[0];sumDX[1] = sumDX[1] + pX[k]*ldk[1];
        sumY = sumY + pY[k]*lk;sumDY[0] = sumDY[0] + pY[k]*ldk[0];sumDY[1] = sumDY[1] + pY[k]*ldk[1];
        sumZ = sumZ + pZ[k]*lk;sumDZ[0] = sumDZ[0] + pZ[k]*ldk[0];sumDZ[1] = sumDZ[1] + pZ[k]*ldk[1];
        sumClk = sumClk + pClk[k]*lk;
        lk = 1;ldk[0] = 1;ldk[1] = 1;
    }
    pXYZ[0] = sumX;pXYZ[1] = sumY;pXYZ[2] = sumZ;
    if(pSp3Clk) *pSp3Clk = sumClk;
    pdXYZ[0] = sumDX[1] - sumDX[0];pdXYZ[1] = sumDY[1] - sumDY[0];pdXYZ[2] = sumDZ[1] - sumDZ[0];
    /* extrapolation error for orbit */
    double SP3_std = 0.0;
    SP3_std = 5e-7;
    if      (t_diff[0]>0.0) SP3_std+=5e-7*(t_diff[0]*t_diff[0])/2.0;
    else if (t_diff[lagrangeSp3Fact -1]<0.0) SP3_std+=5e-7*(t_diff[lagrangeSp3Fact -1]*t_diff[lagrangeSp3Fact -1])/2.0;
    *Orbit_Var=(SP3_std*SP3_std);
}
/*Get precise ephemeris coordinates and speed*/
void rinex_sp3::getPrcisePoint(int PRN, char SatType, double GPST, double *pXYZ, double *pdXYZ, double *pSp3Clk,double *pOrb_Var)
{
    readFileData2Vec(m_SP3FileNames);
    /*If you are on the first day of GPS week, the GPST time needs to add 604800, the reason for reading the data.*/
    if (!IsSigalSp3File&&m_WeekSp3Order > 0)
    {
        if (GPST < 24*3600)  GPST += 604800;
    }
    lagrangeMethod(PRN,SatType,GPST,pXYZ,pdXYZ, pSp3Clk,pOrb_Var);
}

/*Release all data*/
void rinex_sp3::releaseSP3AllData()
{
    m_allEpochSp3Data.clear();
}
