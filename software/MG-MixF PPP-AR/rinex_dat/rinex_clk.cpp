#include "rinex_clk.h"

rinex_clk::rinex_clk()
{

}

void rinex_clk::setClkFileNames(QStringList ClkFileNames)
{
    initClkVar();
    if(ClkFileNames.length() == 1) IsSigalClkFile = true;
    if (ClkFileNames.length() > 0)
    {
        m_ClkFileNames =ClkFileNames;
    }
    else
    {
        isReadClkAllData = true;
        return ;
    }
}

void rinex_clk::initClkVar()
{
    isReadClkHead = false;
    isReadClkAllData = false;
    IsSigalClkFile = false;
    CLKData epochClk_init;
    epochClkData_ref=epochClk_init;
    InitClkDatStruct();
    m_ClkFileName = "";
    m_ClkFileNames.clear();
    lagrangeFact = 10;
    m_allEpochClkData.clear();
    grg_UPD.clear();

    tempCLkLine ="";
    /*Contains only a total of 32 satellites in the GPS system*/
    m_lastGPSTimeFlag = 0;/*As a starting marker*/
    m_lastCmpGPSTime = 999999999;/*Record the GPS time of the initial calculation*/
    m_WeekClkOrder = 0;
    /*1：Acceleration program (time is from small to large)
     * 0：Do not speed up the program (time chaos)*/
    ACCELERATE_CLK = true;
}
void rinex_clk::InitClkDatStruct()
{
    if (isInSystem('G'))
    {
        epochClkData_ref.GPSClkData.resize(MAXPRNGPS);
        upd_WL.GPS_UPD.resize(MAXPRNGPS);
    }
    if (isInSystem('R'))
    {
        epochClkData_ref.GLOClkData.resize(MAXPRNGLO);
        upd_WL.GLONASS_UPD.resize(MAXPRNGLO);
    }
    if (isInSystem('C'))
    {
        epochClkData_ref.BDSClkData.resize(MAXPRNBDS);
        upd_WL.BDS_UPD.resize(MAXPRNBDS);
    }
    if (isInSystem('E'))
    {
        epochClkData_ref.GLEClkData.resize(MAXPRNGLE);
        upd_WL.GALILEO_UPD.resize(MAXPRNGLE);
    }
}


/*Read all data from multiple files*/
void rinex_clk::readFileData2Vec(QStringList ClkFileNames)
{
    if (ClkFileNames.length() == 0)	isReadClkAllData = true;
    if (isReadClkAllData)
        return;
    m_allEpochClkData.clear();

    /*First read the header file to read the file from
     * small to large according to the time*/
    int minGPSWeek = 999999999;double verson;
    QVector< int > tempGPSWeek,fileFlagSeconds;/*Save the file to observe the actual time*/
    QVector< double > tempGPSSeconds;
    for (int i = 0;i < ClkFileNames.length();i++)
    {
        int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,GPSWeek = 0;
        double Seconds = 0,GPSSeconds = 0;
        QString CLKFileName = ClkFileNames.at(i);
        QFile clkfile(CLKFileName);
        if (!clkfile.open(QFile::ReadOnly))
        {
            QString erroInfo = "Open " + CLKFileName + "faild!(readCLKFileData2Vec)";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
        }
        /*Skip header file*/

        tempCLkLine = clkfile.readLine();
        verson=tempCLkLine.mid(0,10).toDouble();
        do
        {
            tempCLkLine = clkfile.readLine();/*Read the first line*/
            if (clkfile.atEnd())
            {
                QString erroInfo = "Can not read clk file!(readFileData2Vec)";
                disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
                text_disp->data_text.append(erroInfo);
                text_disp->data_lenth++;
                break;
            }
        } while (!tempCLkLine.contains("END OF HEADER",Qt::CaseInsensitive));
        /*Find the time corresponding to the AS line*/
        do
        {
            tempCLkLine = clkfile.readLine();/*Read the first line*/
            if (clkfile.atEnd())
            {
                QString erroInfo = "Can not read clk file!(readFileData2Vec)";
                disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
                text_disp->data_text.append(erroInfo);
                text_disp->data_lenth++;
                break;
            }
        } while (!tempCLkLine.mid(0,2).contains("AS",Qt::CaseInsensitive));
        /*Get Time*/
        if(verson==3.04)
        {
        Year = tempCLkLine.mid(13,4).toInt();
        Month = tempCLkLine.mid(18,2).toInt();
        Day = tempCLkLine.mid(21,2).toInt();
        Hours = tempCLkLine.mid(24,2).toInt();
        Minutes = tempCLkLine.mid(27,2).toInt();
        Seconds = tempCLkLine.mid(30,11).toDouble();
        }
        else
        {
            Year = tempCLkLine.mid(8,4).toInt();
            Month = tempCLkLine.mid(13,2).toInt();
            Day = tempCLkLine.mid(16,2).toInt();
            Hours = tempCLkLine.mid(19,2).toInt();
            Minutes = tempCLkLine.mid(22,2).toInt();
            Seconds = tempCLkLine.mid(25,11).toDouble();
        }
        GPSSeconds = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&GPSWeek);
        if (GPSWeek <= minGPSWeek)
            minGPSWeek = GPSWeek;
        tempGPSWeek.append(GPSWeek);
        tempGPSSeconds.append(GPSSeconds);
        clkfile.close();
    }
    /*Convert to seconds*/
    QVector< int > WeekOrder;/*Save data, mark whether cross-week，Cross: 1 does not span: 0*/
    for (int i = 0;i < tempGPSWeek.length();i++)
    {
        double Seconds = (tempGPSWeek.at(i) - minGPSWeek)*604800 + tempGPSSeconds.at(i);
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
                QString tempFileName = ClkFileNames.at(i);
                ClkFileNames[i] = ClkFileNames.at(j);
                ClkFileNames[j] = tempFileName;
                /*Exchange week sign*/
                int tempWeek = WeekOrder.at(i);
                WeekOrder[i] = WeekOrder.at(j);
                WeekOrder[j] = tempWeek;
            }
        }
    }
    m_WeekClkOrder = 0;
    for (int i = 0;i < WeekOrder.length();i++)
    {
        m_WeekClkOrder += WeekOrder[i];/*Save cross-week mark*/
    }
/*Read all files by time*/

    for (int i = 0;i < ClkFileNames.length();i++)
    {
        QString CLKFileName = ClkFileNames.at(i);
/*open a file*/
        QFile clkfile(CLKFileName);
        if (!clkfile.open(QFile::ReadOnly))
        {
            QString erroInfo = "Open " + CLKFileName + "faild!(readFileData2Vec)";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
        }
        //
        /*First time tempCLkLine AR......*/
        int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
        double Seconds = 0, temp_gps_time = 0;
        /*resd wl fcb*/
        Sat_WL_UPD curr_UPD = upd_WL;
//        tempCLkLine = clkfile.readLine();/*Read the first line*/
        int GPRN = 0 ;
        while (!tempCLkLine.contains("END OF HEADER",Qt::CaseInsensitive))
        {
            tempCLkLine = clkfile.readLine();/*Read the first line*/
            QString GWlStr = tempCLkLine.mid(0,3).trimmed();
            while (GWlStr.contains("WL",Qt::CaseInsensitive))
            {
                /*Do header file data read here skip header file*/
                QString SatTyb = tempCLkLine.mid(3,1).trimmed();
                GPRN   = tempCLkLine.mid(4,2).toInt();
                double time_GPST[6] = {0};
                /*init temp_gps_time*/
                Year = tempCLkLine.mid(6,6).toInt();
                Month = tempCLkLine.mid(13,2).toInt();
                Day = tempCLkLine.mid(16,2).toInt();
                Hours = tempCLkLine.mid(19,2).toInt();
                Minutes = tempCLkLine.mid(22,2).toInt();
                Seconds = tempCLkLine.mid(25,9).toDouble();


                if(SatTyb == "G")
                {
                    if(isInSystem('G'))
                    {
                        curr_UPD.GPS_UPD[GPRN-1].UPD_value = tempCLkLine.mid(40,13).toDouble();
                        curr_UPD.GPS_UPD[GPRN-1].wlupd_T = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds);
                    }
                    tempCLkLine = clkfile.readLine();/*Read the next line*/
                    GWlStr = tempCLkLine.mid(0,3).trimmed();
                }
                else if(SatTyb == "C")
                {
                    if(isInSystem('C'))
                    {
                        curr_UPD.BDS_UPD[GPRN-1].UPD_value = tempCLkLine.mid(40,13).toDouble();
                        curr_UPD.BDS_UPD[GPRN-1].wlupd_T = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds);

                    }
                    tempCLkLine = clkfile.readLine();/*Read the next line*/
                    GWlStr = tempCLkLine.mid(0,3).trimmed();
                }
                else if(SatTyb == "E")
                {
                    if(isInSystem('E'))
                    {
                        curr_UPD.GALILEO_UPD[GPRN-1].UPD_value = tempCLkLine.mid(40,13).toDouble();
                        curr_UPD.GALILEO_UPD[GPRN-1].wlupd_T = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds);
                    }
                    tempCLkLine = clkfile.readLine();/*Read the next line*/
                    GWlStr = tempCLkLine.mid(0,3).trimmed();
                }
                else if(SatTyb == "R")
                {
                    if(isInSystem('R'))
                    {
                        curr_UPD.GLONASS_UPD[GPRN-1].UPD_value = tempCLkLine.mid(40,13).toDouble();
                        curr_UPD.GLONASS_UPD[GPRN-1].wlupd_T = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds);
                    }
                    tempCLkLine = clkfile.readLine();/*Read the next line*/
                    GWlStr = tempCLkLine.mid(0,3).trimmed();
                }
            }

        }
        grg_UPD.append(curr_UPD);
        tempCLkLine = clkfile.readLine();/*Read the next line into the data area*/

/*Read data area*/

        while (!clkfile.atEnd())
        {
            /*Skip the area that is not the beginning of the clock error*/
            while(tempCLkLine.mid(0,3) != "AS ")
            {
                tempCLkLine = clkfile.readLine();
                if(clkfile.atEnd())
                    break;
            }
            if(clkfile.atEnd()) break;
            CLKData epochClkData = epochClkData_ref;

            /*Read an epoch error*/
            /*init temp_gps_time*/
            if(verson==3.04)
            {
                Year = tempCLkLine.mid(13,4).toInt();
                Month = tempCLkLine.mid(18,2).toInt();
                Day = tempCLkLine.mid(21,2).toInt();
                Hours = tempCLkLine.mid(24,2).toInt();
                Minutes = tempCLkLine.mid(27,2).toInt();
                Seconds = tempCLkLine.mid(30,11).toDouble();
            }
            else
            {
                Year = tempCLkLine.mid(8,4).toInt();
                Month = tempCLkLine.mid(13,2).toInt();
                Day = tempCLkLine.mid(16,2).toInt();
                Hours = tempCLkLine.mid(19,2).toInt();
                Minutes = tempCLkLine.mid(22,2).toInt();
                Seconds = tempCLkLine.mid(25,9).toDouble();
            }

            epochClkData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;
            epochClkData.GPSWeek = Week;
            temp_gps_time = epochClkData.GPSTime;

            while (!clkfile.atEnd())
            {/*Enter the first epoch*/
                int PRN = 0;
                char tempSatType = 'G';
                bool isReadLineData = false;
                tempSatType = *(tempCLkLine.mid(3,1).toLatin1().data());
                /*GPS system*/
                if (tempSatType == 'G'&&isInSystem(tempSatType)&&tempCLkLine.mid(0,3) == "AS ")
                {
                    /*Read year, month, day, hour, minute, second*/
                    if(verson==3.04)
                    {
                        Year = tempCLkLine.mid(13,4).toInt();
                        Month = tempCLkLine.mid(18,2).toInt();
                        Day = tempCLkLine.mid(21,2).toInt();
                        Hours = tempCLkLine.mid(24,2).toInt();
                        Minutes = tempCLkLine.mid(27,2).toInt();
                        Seconds = tempCLkLine.mid(30,11).toDouble();
                    }
                    else
                    {
                        Year = tempCLkLine.mid(8,4).toInt();
                        Month = tempCLkLine.mid(13,2).toInt();
                        Day = tempCLkLine.mid(16,2).toInt();
                        Hours = tempCLkLine.mid(19,2).toInt();
                        Minutes = tempCLkLine.mid(22,2).toInt();
                        Seconds = tempCLkLine.mid(25,9).toDouble();
                    }
                    epochClkData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//Add 604800s across the week
                    epochClkData.GPSWeek = Week;
                    temp_gps_time = epochClkData.GPSTime;
                    /*Read satellite PRN, clock error, medium error*/
                    PRN = tempCLkLine.mid(4,2).toInt();/*PRN*/
                    int clk_num;
                    if(verson==3.04)
                    clk_num = tempCLkLine.mid(40,2).toInt();
                    else
                        clk_num = tempCLkLine.mid(35,2).toInt();
                    double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
                    epochClkData.GPSClkData[PRN-1].prec_clkT = epoch2time(GPS_Time);
                    for(int i=0;i<clk_num;i++)
                    {
                        if(i==0)
                        {
                            if(verson==3.04)
                            epochClkData.GPSClkData[PRN-1].prec_clk = tempCLkLine.mid(45,20).toDouble();/*Clock error*/
                        else
                                epochClkData.GPSClkData[PRN-1].prec_clk = tempCLkLine.mid(40,20).toDouble();/*Clock error*/
                        }
                            else if(i==1)
                        {
                            if(verson==3.04)
                            epochClkData.GPSClkData[PRN-1].prec_clkVar = tempCLkLine.mid(65,20).toDouble();/*Medium error*/
                            else
                                epochClkData.GPSClkData[PRN-1].prec_clkVar = tempCLkLine.mid(60,20).toDouble();/*Medium error*/
                        }
                    }
                    isReadLineData = true;
                }
                /*Glonass system*/
                else if (tempSatType == 'R'&&isInSystem(tempSatType)&&tempCLkLine.mid(0,3) == "AS ")
                {
                    if(verson==3.04)
                    {
                        Year = tempCLkLine.mid(13,4).toInt();
                        Month = tempCLkLine.mid(18,2).toInt();
                        Day = tempCLkLine.mid(21,2).toInt();
                        Hours = tempCLkLine.mid(24,2).toInt();
                        Minutes = tempCLkLine.mid(27,2).toInt();
                        Seconds = tempCLkLine.mid(30,9).toDouble();
                    }
                    else
                    {
                        /*Read year, month, day, hour, minute, second*/
                        Year = tempCLkLine.mid(8,4).toInt();
                        Month = tempCLkLine.mid(13,2).toInt();
                        Day = tempCLkLine.mid(16,2).toInt();
                        Hours = tempCLkLine.mid(19,2).toInt();
                        Minutes = tempCLkLine.mid(22,2).toInt();
                        Seconds = tempCLkLine.mid(25,9).toDouble();
                    }
                    epochClkData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//Add 604800s across the week
                    epochClkData.GPSWeek = Week;
                    temp_gps_time = epochClkData.GPSTime;
                    /*Read satellite PRN, clock error, medium error*/
                    PRN = tempCLkLine.mid(4,2).toInt();/*PRN*/
                    int clk_num ;
                    if(verson==3.04)
                           clk_num = tempCLkLine.mid(40,2).toInt();
                    else
                        clk_num = tempCLkLine.mid(35,2).toInt();
                    double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
                    epochClkData.GLOClkData[PRN-1].prec_clkT = epoch2time(GPS_Time);
                    for(int i=0;i<clk_num;i++)
                    {
                        if(i==0)
                        {
                            if(verson==3.04)
                            epochClkData.GLOClkData[PRN-1].prec_clk = tempCLkLine.mid(45,20).toDouble();/*Clock error*/
                        else
                                epochClkData.GLOClkData[PRN-1].prec_clk = tempCLkLine.mid(40,20).toDouble();/*Clock error*/
                        }
                            else if(i==1)
                        {
                            if(verson==3.04)
                            epochClkData.GLOClkData[PRN-1].prec_clkVar = tempCLkLine.mid(65,20).toDouble();/*Medium error*/
                            else
                                epochClkData.GLOClkData[PRN-1].prec_clkVar = tempCLkLine.mid(60,20).toDouble();/*Medium error*/
                        }
                    }
                    isReadLineData = true;
                }
                /*BDS system*/
                else if (tempSatType == 'C'&&isInSystem(tempSatType)&&tempCLkLine.mid(0,3) == "AS ")
                {
                    /*Read year, month, day, hour, minute, second*/
                    if(verson==3.04)
                    {
                        Year = tempCLkLine.mid(13,4).toInt();
                        Month = tempCLkLine.mid(18,2).toInt();
                        Day = tempCLkLine.mid(21,2).toInt();
                        Hours = tempCLkLine.mid(24,2).toInt();
                        Minutes = tempCLkLine.mid(27,2).toInt();
                        Seconds = tempCLkLine.mid(30,9).toDouble();
                    }
                    else
                    {
                        Year = tempCLkLine.mid(8,4).toInt();
                        Month = tempCLkLine.mid(13,2).toInt();
                        Day = tempCLkLine.mid(16,2).toInt();
                        Hours = tempCLkLine.mid(19,2).toInt();
                        Minutes = tempCLkLine.mid(22,2).toInt();
                        Seconds = tempCLkLine.mid(25,9).toDouble();
                    }
                    epochClkData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//Add 604800s across the week
                    epochClkData.GPSWeek = Week;
                    temp_gps_time = epochClkData.GPSTime;
                    /*Read satellite PRN, clock error, medium error*/
                    PRN = tempCLkLine.mid(4,2).toInt();/*PRN*/
                    int clk_num ;
                    if(verson==3.04)
                           clk_num = tempCLkLine.mid(40,2).toInt();
                    else
                        clk_num = tempCLkLine.mid(35,2).toInt();
                    double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
                    epochClkData.BDSClkData[PRN-1].prec_clkT = epoch2time(GPS_Time);
                    for(int i=0;i<clk_num;i++)
                    {
                        if(i==0)
                        {
                            if(verson==3.04)
                            epochClkData.BDSClkData[PRN-1].prec_clk = tempCLkLine.mid(45,20).toDouble();/*Clock error*/
                        else
                                epochClkData.BDSClkData[PRN-1].prec_clk = tempCLkLine.mid(40,20).toDouble();/*Clock error*/
                        }
                            else if(i==1)
                        {
                            if(verson==3.04)
                            epochClkData.BDSClkData[PRN-1].prec_clkVar = tempCLkLine.mid(65,20).toDouble();/*Medium error*/
                            else
                                epochClkData.BDSClkData[PRN-1].prec_clkVar = tempCLkLine.mid(60,20).toDouble();/*Medium error*/
                        }                    }
                    isReadLineData = true;
                }
                /*Galileo system*/
                else if (tempSatType == 'E'&&isInSystem(tempSatType)&&tempCLkLine.mid(0,3) == "AS ")
                {
                    /*Read year, month, day, hour, minute, second*/
                    if(verson==3.04)
                    {
                        Year = tempCLkLine.mid(13,4).toInt();
                        Month = tempCLkLine.mid(18,2).toInt();
                        Day = tempCLkLine.mid(21,2).toInt();
                        Hours = tempCLkLine.mid(24,2).toInt();
                        Minutes = tempCLkLine.mid(27,2).toInt();
                        Seconds = tempCLkLine.mid(30,9).toDouble();
                    }
                    else
                    {
                        Year = tempCLkLine.mid(8,4).toInt();
                        Month = tempCLkLine.mid(13,2).toInt();
                        Day = tempCLkLine.mid(16,2).toInt();
                        Hours = tempCLkLine.mid(19,2).toInt();
                        Minutes = tempCLkLine.mid(22,2).toInt();
                        Seconds = tempCLkLine.mid(25,9).toDouble();
                    }
                    epochClkData.GPSTime = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//Add 604800s across the week
                    epochClkData.GPSWeek = Week;
                    temp_gps_time = epochClkData.GPSTime;
                    /*Read satellite PRN, clock error, medium error*/
                    PRN = tempCLkLine.mid(4,2).toInt();/*PRN*/
                    int clk_num ;
                    if(verson==3.04)
                           clk_num = tempCLkLine.mid(40,2).toInt();
                    else
                        clk_num = tempCLkLine.mid(35,2).toInt();
                    double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
                    epochClkData.GLEClkData[PRN-1].prec_clkT = epoch2time(GPS_Time);
                    for(int i=0;i<clk_num;i++)
                    {
                        if(i==0)
                        {
                            if(verson==3.04)
                                epochClkData.GLEClkData[PRN-1].prec_clk = tempCLkLine.mid(45,20).toDouble();/*Clock error*/
                            else
                                epochClkData.GLEClkData[PRN-1].prec_clk = tempCLkLine.mid(40,19).toDouble();/*Clock error*/
                        }
                        else if(i==1)
                        {
                            if(verson==3.04)
                                epochClkData.GLEClkData[PRN-1].prec_clkVar = tempCLkLine.mid(65,20).toDouble();/*Medium error*/
                            else
                                epochClkData.GLEClkData[PRN-1].prec_clkVar = tempCLkLine.mid(60,20).toDouble();/*Medium error*/
                        }
                    }
                    isReadLineData = true;
                }
                tempCLkLine = clkfile.readLine();/*Read a row of coordinate data*/
                /*use temp_gps_time juge epoch is become  debug 2018.11.15 xiaogongwei*/
                if(tempCLkLine.mid(0,3) == "AS " && temp_gps_time != 0)
                {
                    double cureent_gps_time = 0;
                    if(verson==3.04)
                    {
                    Year = tempCLkLine.mid(13,4).toInt();
                    Month = tempCLkLine.mid(18,2).toInt();
                    Day = tempCLkLine.mid(21,2).toInt();
                    Hours = tempCLkLine.mid(24,2).toInt();
                    Minutes = tempCLkLine.mid(27,2).toInt();
                    Seconds = tempCLkLine.mid(30,9).toDouble();
                    }
                    else
                    {
                    Year = tempCLkLine.mid(8,4).toInt();
                    Month = tempCLkLine.mid(13,2).toInt();
                    Day = tempCLkLine.mid(16,2).toInt();
                    Hours = tempCLkLine.mid(19,2).toInt();
                    Minutes = tempCLkLine.mid(22,2).toInt();
                    Seconds = tempCLkLine.mid(25,9).toDouble();
                    }
                    cureent_gps_time = YMD2GPSTime(Year,Month,Day,Hours,Minutes,Seconds,&Week) + WeekOrder.at(i)*604800;//Add 604800s across the week
                    if(cureent_gps_time != temp_gps_time)
                        break;/*only one jump while 2020.12.07*/
                }
            }
            m_allEpochClkData.append(epochClkData);
        }/*End of reading file data   while (!clkfile.atEnd())*/
        clkfile.close();
    }
    isReadClkAllData = true;
    /*Determine the clock error epoch interval to determine the number of interpolation points*/
    CLKData epoch1 = m_allEpochClkData.at(20);
    CLKData epoch2 = m_allEpochClkData.at(21);
    if (qAbs(epoch1.GPSTime - epoch2.GPSTime) < 60)
        lagrangeFact = 2;
}


/*Release all data*/
void rinex_clk::releaseCLKAllData()
{
    m_allEpochClkData.clear();
}

/*pX,pY,pZ：lagrangeFact point coordinates ；pGPST:lagrangeFact points GPS week seconds;
 * GPST Satellite launch time within seconds of the week*/
void rinex_clk::get8Point(int PRN,char SatType,double *pCLKT,double *pCLK_Var,double *pGPST,double GPST)/*pCLK_Var szg debug 0314*/
{/*Get the nearest lagrangeFact points*/
    /*Check for acceleration*/
    int lengthEpoch = m_allEpochClkData.length();
    /*Discover the location of GPST throughout the epoch*/
    int GPSTflag = m_lastGPSTimeFlag;
    int numPoint = lagrangeFact / 2;/*Take numPoint points before and after*/
    /*More than 0.3s indicates that the epoch interval of the observation epoch is
     * definitely above 1 s (reducing the multiple Query position of the same epoch)*/
    if (qAbs(m_lastCmpGPSTime - GPST) > 0.3)
    {
        if (ACCELERATE_CLK)	m_lastCmpGPSTime = GPST;
        for (int i = m_lastGPSTimeFlag;i < lengthEpoch;i++)
        {
            CLKData epochClkData = m_allEpochClkData.at(i);
            if (epochClkData.GPSTime >= GPST)
                break;
            else
                GPSTflag++;
        }
        /*Two-point interpolation occurs because the satellite
         * propagates for 0.00n seconds, causing the GPSTflag to be 1 more.*/
        if (numPoint == 1) GPSTflag--;
    }



/*Take lagrangeFact points before and after  Consider the boundary problem*/

    if (GPSTflag < 0) GPSTflag = 0;
    if (ACCELERATE_CLK)	m_lastGPSTimeFlag = GPSTflag;/*Save the latest location*/

    if ((GPSTflag >= numPoint - 1) && (GPSTflag <= lengthEpoch - numPoint - 1))
    {/*In front of the middle position four contain itself, the last four do not contain itself*/
        for (int i = 0;i < lagrangeFact;i++)
        {
            CLKData epochClkData = m_allEpochClkData.at(GPSTflag - numPoint + 1 + i);
            /*Determine which system data belongs to*/
            switch (SatType)
            {
            case 'G':
                pCLKT[i] = epochClkData.GPSClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GPSClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'R':
                pCLKT[i] = epochClkData.GLOClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GLOClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'C':
                pCLKT[i] = epochClkData.BDSClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.BDSClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'E':
                pCLKT[i] = epochClkData.GLEClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GLEClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            default:
                pCLKT[i] = 0;
                pCLK_Var[i] = 0;
                pGPST[i] = 0;
            }
        }
    }
    else if(GPSTflag < numPoint - 1)
    {/*At the start position boundary*/
        for (int i = 0;i < lagrangeFact;i++)
        {
            CLKData epochClkData = m_allEpochClkData.at(i);
            /*Determine which system data belongs to*/
            switch (SatType)
            {
            case 'G':
                pCLKT[i] = epochClkData.GPSClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GPSClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'R':
                pCLKT[i] = epochClkData.GLOClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GLOClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'C':
                pCLKT[i] = epochClkData.BDSClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.BDSClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'E':
                pCLKT[i] = epochClkData.GLEClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GLEClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            default:
                pCLKT[i] = 0;
                pCLK_Var[i] = 0;
                pGPST[i] = 0;
            }
        }
    }
    else if(GPSTflag > lengthEpoch - numPoint - 1)
    {/*End position boundary*/
        for (int i = 0;i < lagrangeFact;i++)
        {
            CLKData epochClkData = m_allEpochClkData.at(lengthEpoch - (lagrangeFact-i));
            /*Determine which system data belongs to*/
            switch (SatType)
            {
            case 'G':
                pCLKT[i] = epochClkData.GPSClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GPSClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'R':
                pCLKT[i] = epochClkData.GLOClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GLOClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'C':
                pCLKT[i] = epochClkData.BDSClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.BDSClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            case 'E':
                pCLKT[i] = epochClkData.GLEClkData[PRN-1].prec_clk;
                pCLK_Var[i] = epochClkData.GLEClkData[PRN-1].prec_clkVar;
                pGPST[i] = epochClkData.GPSTime;
                break;
            default:
                pCLKT[i] = 0;
                pCLK_Var[i] = 0;
                pGPST[i] = 0;
            }
        }
    }
}


/*Lagrangian method*/
void rinex_clk::lagrangeMethod(int PRN,char SatType,double GPST,double *pCLKT,double *pCLKTVar,double *G_UPDs,file_dat *dat_fil)
{
    /*Lagrangian interpolation, here before and after the selection - a total
     *  of 8 points for interpolation GPST satellite launch time week seconds*/
    *pCLKT = 0;
    /*Determine whether to set up a legal system*/
    /*Determine whether to read data*/
    if (!isReadClkAllData)
    {
        readFileData2Vec(m_ClkFileNames);
    }
    if(grg_UPD.length()!=0)
    {
        for(int i=0;i<grg_UPD.length();i++)
        {
            if(SatType == 'G'&&grg_UPD[i].GPS_UPD.length()>0)
            {
                if((fabs(grg_UPD[i].GPS_UPD[PRN-1].wlupd_T - GPST)>=0&&fabs(grg_UPD[i].GPS_UPD[PRN-1].wlupd_T - GPST)<=43200&&dat_fil->int_clk_ar_flag!=1)
                        ||(dat_fil->int_clk_ar_flag==1&&(GPST - grg_UPD[i].GPS_UPD[PRN-1].wlupd_T)>-1&&(GPST - grg_UPD[i].GPS_UPD[PRN-1].wlupd_T)<=86400))
                    *G_UPDs = grg_UPD[i].GPS_UPD[PRN-1].UPD_value;
            }
            else if(SatType == 'C'&&grg_UPD[i].BDS_UPD.length()>0)
            {
                if((fabs(grg_UPD[i].BDS_UPD[PRN-1].wlupd_T - GPST)>=0&&fabs(grg_UPD[i].BDS_UPD[PRN-1].wlupd_T - GPST)<=43200&&dat_fil->int_clk_ar_flag!=1)
                        ||(dat_fil->int_clk_ar_flag==1&&(GPST - grg_UPD[i].BDS_UPD[PRN-1].wlupd_T)>-1&&(GPST - grg_UPD[i].BDS_UPD[PRN-1].wlupd_T)<=86400))
                    *G_UPDs = grg_UPD[i].BDS_UPD[PRN-1].UPD_value;
            }
            else if(SatType == 'E'&&grg_UPD[i].GALILEO_UPD.length()>0)
            {
                if((fabs(grg_UPD[i].GALILEO_UPD[PRN-1].wlupd_T - GPST)>=0&&fabs(grg_UPD[i].GALILEO_UPD[PRN-1].wlupd_T - GPST)<=43200&&dat_fil->int_clk_ar_flag!=1)
                        ||(dat_fil->int_clk_ar_flag==1&&(GPST - grg_UPD[i].GALILEO_UPD[PRN-1].wlupd_T)>-1&&(GPST - grg_UPD[i].GALILEO_UPD[PRN-1].wlupd_T)<=86400))
                    *G_UPDs = grg_UPD[i].GALILEO_UPD[PRN-1].UPD_value;
            }
            else if(SatType == 'R'&&grg_UPD[i].GLONASS_UPD.length()>0)
            {
                if((fabs(grg_UPD[i].GLONASS_UPD[PRN-1].wlupd_T - GPST)>=0&&fabs(grg_UPD[i].GLONASS_UPD[PRN-1].wlupd_T - GPST)<=43200&&dat_fil->int_clk_ar_flag!=1)
                        ||(dat_fil->int_clk_ar_flag==1&&(GPST - grg_UPD[i].GLONASS_UPD[PRN-1].wlupd_T)>-1&&(GPST - grg_UPD[i].GLONASS_UPD[PRN-1].wlupd_T)<=86400))
                    *G_UPDs = grg_UPD[i].GLONASS_UPD[PRN-1].UPD_value;
            }
        }
    }

    double m_CLKT[12] = {0};/*Save the acquired interpolation data*/
    double m_pGPST[12] = {0};
    double m_CLKT_Var[12] = {0},t_diff[12] = {0};
    get8Point(PRN,SatType,m_CLKT,m_CLKT_Var,m_pGPST,GPST);/*Obtain the last 8 or 2 o'clock difference of the PRN satellite launch time*/
    /*Verify data integrity (data cannot be missing in precision clock-interpolation)*/
    for (int i = 0;i <lagrangeFact;i++)
        if (!m_CLKT[i]) return ;
    /*Lagrangian interpolation*/
    double sumCLK = 0;/*Interpolated coordinates XYZ*/
    for (int k = 0; k < lagrangeFact;k++)
    {
        double lk = 1;
        for (int n = 0;n < lagrangeFact;n++)
        {
            if (k == n) continue;
            lk *= (GPST - m_pGPST[n])/(m_pGPST[k] - m_pGPST[n]);
        }
        sumCLK += m_CLKT[k]*lk;
    }
    t_diff[0] = m_pGPST[0] - GPST;
    t_diff[1] = m_pGPST[1] - GPST;
    double Var_Std = 0.0;
    int Vari;
    if (t_diff[0]<=0.0) {
        Var_Std=m_CLKT_Var[0]*M_C;
    }
    else if (t_diff[1]>=0.0) {
        Var_Std=m_CLKT_Var[1]*M_C;
    }
    else if (t_diff[0]!=0.0&&t_diff[1]!=0.0) {
        Vari=m_pGPST[0]<-m_pGPST[1]?0:1;
        Var_Std=m_CLKT_Var[0+Vari]*M_C;
    }
    *pCLKTVar =Var_Std*Var_Std;
    *pCLKT = sumCLK*M_C;
}



/*Obtain the satellite clock error at the time of transmission (multi-system)*/
void rinex_clk::getStaliteClk(int PRN,char SatType,double GPST,double *pCLKT,double *pClkVar,double *G_UPDs,file_dat *dat_fil)
{
    readFileData2Vec(m_ClkFileNames);
    /*If you are on the first day of GPS week, the GPST time needs to add 604800,
     * the reason for reading the data.*/
    if (!IsSigalClkFile&&m_WeekClkOrder > 0)
    {
        if (GPST < 24*3600)  GPST += 604800;
    }
    lagrangeMethod(PRN,SatType,GPST,pCLKT,pClkVar,G_UPDs,dat_fil);
}
