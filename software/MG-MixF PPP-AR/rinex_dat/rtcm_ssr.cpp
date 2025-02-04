#include "rtcm_ssr.h"

rtcm_ssr::rtcm_ssr()
{

}


/*Read epochNum epoch data
 * (premature to the bottom of the file may be less than epochNum)*/
void rtcm_ssr::getMultSsrData(QVector< VeSSRData >&multEpochData,int epochNum)
{
    if (epochNum < 1) return ;
    int i = 0;
    while(!m_readSsrFileClass.atEnd())
    {
        if (i++ >= epochNum) break;
        VeSSRData SSRepochData;
        /*Get the SSR closest to the current observation time*/
        getSSRData(&SSRepochData);
        /*Store SSR data into array*/
        if(timediff(SSRepochData.ClGPSTime,SSRepochData.OrGPSTime)==0)
            multEpochData.append(SSRepochData);
    }
}
/*Matches the current observation time SSR*/
void rtcm_ssr::getObstime(GNSSephData *SSRData,int *ssrReadL,QVector< VeSSRData > multEpochData,gtime_t Otime)
{
    /*Obtain the latest SSR of observation time*/
    int multSsrLen = multEpochData.length();
    bool flag_ssr=false;
    for(int i=*ssrReadL;i<multSsrLen;i++)
    {
        VeSSRData oneSSRData = multEpochData.at(i);
        /*Judge clock error correction time*/
        if(timediff(oneSSRData.ClGPSTime,Otime) > 0&&timediff(oneSSRData.OrGPSTime,Otime) > 0)
        {
            /*Storage of SSR data longer than observation time*/
            SSRData->GnssSsrData[1].ClGPSTime = oneSSRData.ClGPSTime;
            SSRData->GnssSsrData[1].ClGPSWeek = oneSSRData.ClGPSWeek;
            SSRData->GnssSsrData[1].OrGPSTime = oneSSRData.OrGPSTime;
            SSRData->GnssSsrData[1].OrGPSWeek = oneSSRData.OrGPSWeek;
            SSRData->GnssSsrData[1].CODEGPSTime = oneSSRData.CODEGPSTime;
            SSRData->GnssSsrData[1].CODEGPSWeek = oneSSRData.CODEGPSWeek;
            SSRData->GnssSsrData[1].PHESAGPSTime = oneSSRData.PHESAGPSTime;
            SSRData->GnssSsrData[1].PHESAGPSWeek = oneSSRData.PHESAGPSWeek;
            for(int i=0;i<6;i++)/*Data update rate*/
            {
                SSRData->GnssSsrData[1].upSsrtime[i] = oneSSRData.upSsrtime[i];
            }
            /*Get the entire stream*/
            getupdatessr(&SSRData->GnssSsrData[1],oneSSRData);
            flag_ssr = true;
            break;
        }
        else
        {
            /*Latest SSR less than observation time*/
            SSRData->GnssSsrData[0].ClGPSTime = oneSSRData.ClGPSTime;
            SSRData->GnssSsrData[0].ClGPSWeek = oneSSRData.ClGPSWeek;
            SSRData->GnssSsrData[0].OrGPSTime = oneSSRData.OrGPSTime;
            SSRData->GnssSsrData[0].OrGPSWeek = oneSSRData.OrGPSWeek;
            SSRData->GnssSsrData[0].CODEGPSTime = oneSSRData.CODEGPSTime;
            SSRData->GnssSsrData[0].CODEGPSWeek = oneSSRData.CODEGPSWeek;
            SSRData->GnssSsrData[0].PHESAGPSTime = oneSSRData.PHESAGPSTime;
            SSRData->GnssSsrData[0].PHESAGPSWeek = oneSSRData.PHESAGPSWeek;
            for(int i=0;i<6;i++)/*Data update rate*/
            {
                SSRData->GnssSsrData[0].upSsrtime[i] = oneSSRData.upSsrtime[i];
            }
            /*Get the entire stream*/
            getupdatessr(&SSRData->GnssSsrData[0],oneSSRData);
            /*Flag bit in vector for storing SSR data*/
            *ssrReadL = i;
        }


    }
    if(!flag_ssr&&*ssrReadL!=multSsrLen-1)
        *ssrReadL = multSsrLen-1;

}
/*Matches the current observation time SSR*/
void rtcm_ssr::getupdatessr(VeSSRData *updateSSRData,VeSSRData oneSSRData)
{
    /*Ephemeris correction data of GPS system*/
    for(int i=0;i<MAXPRNGPS;i++)
    {
        /*Determine whether to update*/
        if(oneSSRData.VectSSRdataGPS[i].updatessr == 2||updateSSRData->VectSSRdataGPS[i].updatetim>4)
        {
            updateSSRData->VectSSRdataGPS[i] = oneSSRData.VectSSRdataGPS[i];/**/
            if(updateSSRData->VectSSRdataGPS[i].updatetim>=5)/*Judge the interruption of individual satellite update of SSR products*/
                updateSSRData->VectSSRdataGPS[i].updatetim=0;
        }
        else
        {/**/
            updateSSRData->VectSSRdataGPS[i].updatetim = updateSSRData->VectSSRdataGPS[i].updatetim + 1;
        }
    }
    /*Ephemeris correction data of BDS system*/
    for(int i=0;i<MAXPRNBDS;i++)
    {
        /*Determine whether to update*/
        if(oneSSRData.VectSSRdataBDS[i].updatessr == 2||updateSSRData->VectSSRdataBDS[i].updatetim>4)
        {
            updateSSRData->VectSSRdataBDS[i] = oneSSRData.VectSSRdataBDS[i];/*Judge the interruption of individual satellite update of SSR products*/
            if(updateSSRData->VectSSRdataBDS[i].updatetim>=5)/**/
                updateSSRData->VectSSRdataBDS[i].updatetim=0;
        }
        else
        {/**/
            updateSSRData->VectSSRdataBDS[i].updatetim = updateSSRData->VectSSRdataBDS[i].updatetim + 1;
        }
    }
    /*Ephemeris correction data of GLO system*/
    for(int i=0;i<MAXPRNGLO;i++)
    {
        /*Determine whether to update*/
        if(oneSSRData.VectSSRdataGLO[i].updatessr == 2||updateSSRData->VectSSRdataGLO[i].updatetim>4)
        {
            updateSSRData->VectSSRdataGLO[i] = oneSSRData.VectSSRdataGLO[i];/*Judge the interruption of individual satellite update of SSR products*/
            if(updateSSRData->VectSSRdataGLO[i].updatetim>=5)/**/
                updateSSRData->VectSSRdataGLO[i].updatetim=0;
        }
        else
        {/**/
            updateSSRData->VectSSRdataGLO[i].updatetim = updateSSRData->VectSSRdataGLO[i].updatetim + 1;
        }
    }
    /*Ephemeris correction data of GLE system*/
    for(int i=0;i<MAXPRNGLE;i++)
    {
        /*Determine whether to update*/
        if(oneSSRData.VectSSRdataGLA[i].updatessr == 2||updateSSRData->VectSSRdataGLA[i].updatetim>4)
        {
            updateSSRData->VectSSRdataGLA[i] = oneSSRData.VectSSRdataGLA[i];/*Judge the interruption of individual satellite update of SSR products*/
            if(updateSSRData->VectSSRdataGLA[i].updatetim>=5)/**/
                updateSSRData->VectSSRdataGLA[i].updatetim=0;
        }
        else
        {/**/
            updateSSRData->VectSSRdataGLA[i].updatetim = updateSSRData->VectSSRdataGLA[i].updatetim + 1;
        }
    }


}
/*Get SSR data*/
void rtcm_ssr::getSSRData(VeSSRData *SSRepochData)
{

    while (!m_readSsrFileClass.atEnd())
    {
        /*Loop read find orbit data*/
        while(!tempSSRLine.contains("> ORBIT",Qt::CaseSensitive)&&!m_readSsrFileClass.atEnd())
        {
            tempSSRLine = m_readSsrFileClass.readLine();
        }
        if (tempSSRLine.contains("ORBIT",Qt::CaseSensitive))
        {
            int numOrbit=0;
            /*Read track correction time*/
            ReadORBIT(tempSSRLine,&numOrbit,SSRepochData);
            for(int i=0;i<numOrbit;i++)
            {
                tempSSRLine = m_readSsrFileClass.readLine();/*Read next line*/
                /*Read the orbit correction of all current SSR satellites*/
                readORbitData(tempSSRLine,SSRepochData);
            }
        }
        /*Loop read find header data*/
        while(!tempSSRLine.contains(">",Qt::CaseSensitive)&&!m_readSsrFileClass.atEnd())
        {
            tempSSRLine = m_readSsrFileClass.readLine();
        }
        /*Find clock error correction*/
        if(tempSSRLine.contains("CLOCK",Qt::CaseSensitive))
        {
            int numClock=0;
            /*Read clock error correction time*/
            ReadCLOCK(tempSSRLine,&numClock,SSRepochData);
            for(int i=0;i<numClock;i++)
            {
                tempSSRLine = m_readSsrFileClass.readLine();/*Read next line*/
                /*Read all satellite clock error correction data*/
                readCLOCKData(tempSSRLine,SSRepochData);
            }
        }
        /*Loop read find header data*/
        while(!tempSSRLine.contains(">",Qt::CaseSensitive)&&!m_readSsrFileClass.atEnd())
        {
            tempSSRLine = m_readSsrFileClass.readLine();/*Read next line*/
        }
       /*Find CODE_BIAS*/
        if(tempSSRLine.contains("CODE_BIAS",Qt::CaseSensitive))
        {
            int numCode=0;
            /*Read CODE_BIAS time*/
            ReadCODE_BIAS(tempSSRLine,&numCode,SSRepochData);
            for(int i=0;i<numCode;i++)
            {
                tempSSRLine = m_readSsrFileClass.readLine();/*Read next line*/
                /*Read all satellite CODE_BIAS data*/
                readCODE_BIASData(tempSSRLine,SSRepochData);
            }
        }
        /*Loop read find header data*/
        while(!tempSSRLine.contains(">",Qt::CaseSensitive)&&!m_readSsrFileClass.atEnd())
        {
            tempSSRLine = m_readSsrFileClass.readLine();
        }
        /*Find PHASE_BIAS*/
        if(tempSSRLine.contains("PHASE_BIAS",Qt::CaseSensitive))
        {
            int numPhase=0;
            /*Read PHASE_BIAS time*/
            ReadPHASE_BIAS(tempSSRLine,&numPhase,SSRepochData);
            for(int i=0;i<numPhase+1;i++)
            {
                tempSSRLine = m_readSsrFileClass.readLine();/*Read next line*/
                if( i == 0 ) continue;
                /*Read all satellite PHASE_BIAS data*/
                readPHASE_BIASData(tempSSRLine,SSRepochData);
            }
        }
        break;
    }
}

void rtcm_ssr::setSsrFileNames(QString SsrFileNames)
{
    initSsrVar();
    if (SsrFileNames.trimmed().isEmpty())
    {
        QString erroInfo = "File Name is Empty!(Readssr::Readssr(QString SSRfileName))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }
    m_SsrFileName = SsrFileNames;
    /*set file name is not close*/
    /*open a file*/
    m_readSsrFileClass.setFileName(m_SsrFileName);
    if (!m_readSsrFileClass.open(QFile::ReadOnly))
    {
        QString erroInfo = "Open file bad!(Readssr::Readssr(QString SsrFileNames))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }
    m_IsCloseFile = false;

}
void rtcm_ssr::closeSsrFile(QString SsrFileNames)
{

    if (SsrFileNames.trimmed().isEmpty())
    {
        QString erroInfo = "File Name is Empty!(Readssr::Readssr(QString SSRfileName))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }
    m_SsrFileName = SsrFileNames;
    /*set file name is not close*/
    /*open a file*/
    m_readSsrFileClass.setFileName(m_SsrFileName);
    m_readSsrFileClass.close();

}


void rtcm_ssr::initSsrVar()
{
    isReadHead = false;
    isReadAllData = false;
    IsSigalFile = false;
    m_SsrFileName = "";
    tempSSRLine = "";
    m_SsrFileNames.clear();
    m_IsCloseFile = true;
}



/* convert calendar day/time to time -------------------------------------------
* convert calendar day/time to gtime_t struct
* args   : double *ep       I   day/time {year,month,day,hour,min,sec}
* return : gtime_t struct
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
gtime_t rtcm_ssr::ssr2time(const double *ep)
{
    const int doy[]={1,32,60,91,121,152,182,213,244,274,305,335};
    gtime_t time;
    int days,sec,year=(int)ep[0],mon=(int)ep[1],day=(int)ep[2];

    if (year<1970||2099<year||mon<1||12<mon) return time;

    /* leap year if year%4==0 in 1901-2099 */
    days=(year-1970)*365+(year-1969)/4+doy[mon-1]+day-2+(year%4==0&&mon>=3?1:0);
    sec=(int)floor(ep[5]);
    time.time=(time_t)days*86400+(int)ep[3]*3600+(int)ep[4]*60+sec;
    time.sec=ep[5]-sec;
    return time;
}
/*Obtain the current SSR track correction time from the file szg debug 0514*/
int rtcm_ssr::ReadORBIT(QString tempLine,int *num,VeSSRData *SsrEphochData)
{
    int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0,ssrupT;
    double Seconds = 0;
    if (tempLine.contains("ORBIT",Qt::CaseSensitive))
    {
        QStringList list_line = tempLine.split(' ');
        Qlist_to_dat(list_line,num,&Year,&Month,&Day,&Hours,&Minutes,&Seconds,&ssrupT);
        SsrEphochData->upSsrtime[0] = ssruptime[ssrupT];
        double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
        SsrEphochData->OrGPSTime = ssr2time(GPS_Time);
        SsrEphochData->OrGPSWeek = Week;
        return 1;
    }
    return 0;
}
/*All satellite orbit correction values*/
void rtcm_ssr::readORbitData(QString tempLine,VeSSRData *SsrEphochData)
{
    int PRN = tempLine.mid(1,2).toInt();/*Satellite PRN*/

    if (tempLine.mid(0,1) == "G")
    {
        /*GPS system*/
        SsrEphochData->VectSSRdataGPS[PRN-1].updatessr ++;
        SsrEphochData->VectSSRdataGPS[PRN-1].PRN = PRN;
        SsrEphochData->VectSSRdataGPS[PRN-1].orb_t0 = SsrEphochData->OrGPSTime;
        SsrEphochData->VectSSRdataGPS[PRN-1].iodeOr  = tempLine.mid(10,5).toInt();  /*1，IODE*/
        SsrEphochData->VectSSRdataGPS[PRN-1].r0      = tempLine.mid(15,11).toDouble();  /*2，r0*/
        SsrEphochData->VectSSRdataGPS[PRN-1].a0      = tempLine.mid(26,11).toDouble();  /*3，a0*/
        SsrEphochData->VectSSRdataGPS[PRN-1].c0      = tempLine.mid(37,11).toDouble();  /*4，c0*/
        SsrEphochData->VectSSRdataGPS[PRN-1].dr0     = tempLine.mid(51,11).toDouble();  /*5，dr0*/
        SsrEphochData->VectSSRdataGPS[PRN-1].da0     = tempLine.mid(62,11).toDouble();  /*6，da0*/
        SsrEphochData->VectSSRdataGPS[PRN-1].dc0     = tempLine.mid(73,11).toDouble();  /*7，dc0*/

    }
    else if(tempLine.mid(0,1) == "R")
    {
        /*GLO system*/
        SsrEphochData->VectSSRdataGLO[PRN-1].updatessr ++;
        SsrEphochData->VectSSRdataGLO[PRN-1].PRN = PRN;
        SsrEphochData->VectSSRdataGLO[PRN-1].orb_t0 = SsrEphochData->OrGPSTime;
        SsrEphochData->VectSSRdataGLO[PRN-1].iodeOr  = tempLine.mid(10,5).toInt();  /*1，IODE*/
        SsrEphochData->VectSSRdataGLO[PRN-1].r0      = tempLine.mid(15,11).toDouble();  /*2，r0*/
        SsrEphochData->VectSSRdataGLO[PRN-1].a0      = tempLine.mid(26,11).toDouble();  /*3，a0*/
        SsrEphochData->VectSSRdataGLO[PRN-1].c0      = tempLine.mid(37,11).toDouble();  /*4，c0*/
        SsrEphochData->VectSSRdataGLO[PRN-1].dr0     = tempLine.mid(51,11).toDouble();  /*5，dr0*/
        SsrEphochData->VectSSRdataGLO[PRN-1].da0     = tempLine.mid(62,11).toDouble();  /*6，da0*/
        SsrEphochData->VectSSRdataGLO[PRN-1].dc0     = tempLine.mid(73,11).toDouble();  /*7，dc0*/

    }
    else if(tempLine.mid(0,1) == "E")
    {
        /*GLE system*/
        SsrEphochData->VectSSRdataGLA[PRN-1].updatessr ++;
        SsrEphochData->VectSSRdataGLA[PRN-1].PRN = PRN;
        SsrEphochData->VectSSRdataGLA[PRN-1].orb_t0 = SsrEphochData->OrGPSTime;
        SsrEphochData->VectSSRdataGLA[PRN-1].iodeOr  = tempLine.mid(10,5).toInt();  /*1，IODE*/
        SsrEphochData->VectSSRdataGLA[PRN-1].r0      = tempLine.mid(15,11).toDouble();  /*2，r0*/
        SsrEphochData->VectSSRdataGLA[PRN-1].a0      = tempLine.mid(26,11).toDouble();  /*3，a0*/
        SsrEphochData->VectSSRdataGLA[PRN-1].c0      = tempLine.mid(37,11).toDouble();  /*4，c0*/
        SsrEphochData->VectSSRdataGLA[PRN-1].dr0     = tempLine.mid(51,11).toDouble();  /*5，dr0*/
        SsrEphochData->VectSSRdataGLA[PRN-1].da0     = tempLine.mid(62,11).toDouble();  /*6，da0*/
        SsrEphochData->VectSSRdataGLA[PRN-1].dc0     = tempLine.mid(73,11).toDouble();  /*7，dc0*/
    }
    else if(tempLine.mid(0,1) == "C")
    {
        /*BDS system*/
        SsrEphochData->VectSSRdataBDS[PRN-1].updatessr ++;
        SsrEphochData->VectSSRdataBDS[PRN-1].PRN = PRN;
        SsrEphochData->VectSSRdataBDS[PRN-1].orb_t0 = SsrEphochData->OrGPSTime;
        SsrEphochData->VectSSRdataBDS[PRN-1].iodeOr  = tempLine.mid(10,5).toInt();  //1，IODE
        SsrEphochData->VectSSRdataBDS[PRN-1].r0      = tempLine.mid(15,11).toDouble();  //2，r0
        SsrEphochData->VectSSRdataBDS[PRN-1].a0      = tempLine.mid(26,11).toDouble();  //3，a0
        SsrEphochData->VectSSRdataBDS[PRN-1].c0      = tempLine.mid(37,11).toDouble();  //4，c0
        SsrEphochData->VectSSRdataBDS[PRN-1].dr0     = tempLine.mid(51,11).toDouble();  //5，dr0
        SsrEphochData->VectSSRdataBDS[PRN-1].da0     = tempLine.mid(62,11).toDouble();  //6，da0
        SsrEphochData->VectSSRdataBDS[PRN-1].dc0     = tempLine.mid(73,11).toDouble();  //7，dc0
    }
    /*Enter the first epoch*/

}
/*Current SSR clock error correction time*/
int rtcm_ssr::ReadCLOCK(QString tempLine,int *num,VeSSRData *SsrEphochData)
{
    int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0,ssrupT;
    double Seconds = 0;
    if (tempLine.contains("CLOCK",Qt::CaseSensitive))
    {
        QStringList list_line = tempLine.split(' ');
        Qlist_to_dat(list_line,num,&Year,&Month,&Day,&Hours,&Minutes,&Seconds,&ssrupT);
        SsrEphochData->upSsrtime[1] = ssruptime[ssrupT];
        double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
        SsrEphochData->ClGPSTime = ssr2time(GPS_Time);
        SsrEphochData->ClGPSWeek = Week;
        return 2;
    }
    return 0;

}
/*Read satellite clock error correction data*/
void rtcm_ssr::readCLOCKData(QString tempLine,VeSSRData *SsrEphochData)
{

    int PRN = tempLine.mid(1,2).toInt();/*satellite PRN*/
    /*GPS system*/
    if (tempLine.mid(0,1) == "G")
    {
        SsrEphochData->VectSSRdataGPS[PRN-1].updatessr++;
        SsrEphochData->VectSSRdataGPS[PRN-1].clk_t0 = SsrEphochData->ClGPSTime;
        SsrEphochData->VectSSRdataGPS[PRN-1].iodeCl  = tempLine.mid(10,5).toInt();  /*IODE*/
        SsrEphochData->VectSSRdataGPS[PRN-1].C0      = tempLine.mid(15,11).toDouble();  /*Clock error correction first order quantity 1，C0*/
        SsrEphochData->VectSSRdataGPS[PRN-1].C1      = tempLine.mid(26,11).toDouble();  /*2，C1*/
        SsrEphochData->VectSSRdataGPS[PRN-1].C2      = tempLine.mid(37,11).toDouble();  /*3，C2*/
    }
    else if(tempLine.mid(0,1) == "R")/*GLONASS system*/
    {
        SsrEphochData->VectSSRdataGLO[PRN-1].updatessr++;
        SsrEphochData->VectSSRdataGLO[PRN-1].clk_t0 = SsrEphochData->ClGPSTime;
        SsrEphochData->VectSSRdataGLO[PRN-1].iodeCl  = tempLine.mid(10,5).toInt();  /*IODE*/
        SsrEphochData->VectSSRdataGLO[PRN-1].C0      = tempLine.mid(15,11).toDouble();  /*Clock error correction first order quantity 1，C0*/
        SsrEphochData->VectSSRdataGLO[PRN-1].C1      = tempLine.mid(26,11).toDouble();  /*2，C1*/
        SsrEphochData->VectSSRdataGLO[PRN-1].C2      = tempLine.mid(37,11).toDouble();  /*3，C2*/
    }
    else if(tempLine.mid(0,1) == "E")/*Galileo system*/
    {
        SsrEphochData->VectSSRdataGLA[PRN-1].updatessr++;
        SsrEphochData->VectSSRdataGLA[PRN-1].clk_t0 = SsrEphochData->ClGPSTime;
        SsrEphochData->VectSSRdataGLA[PRN-1].iodeCl  = tempLine.mid(10,5).toInt();  /*IODE*/
        SsrEphochData->VectSSRdataGLA[PRN-1].C0      = tempLine.mid(15,11).toDouble();  /*Clock error correction first order quantity 1，C0*/
        SsrEphochData->VectSSRdataGLA[PRN-1].C1      = tempLine.mid(26,11).toDouble();  /*2，C1*/
        SsrEphochData->VectSSRdataGLA[PRN-1].C2      = tempLine.mid(37,11).toDouble();  /*3，C2*/
    }
    else if(tempLine.mid(0,1) == "C")/*beidou system*/
    {
        SsrEphochData->VectSSRdataBDS[PRN-1].updatessr++;
        SsrEphochData->VectSSRdataBDS[PRN-1].clk_t0 = SsrEphochData->ClGPSTime;
        SsrEphochData->VectSSRdataBDS[PRN-1].iodeCl  = tempLine.mid(10,5).toInt();  /*IODE*/
        SsrEphochData->VectSSRdataBDS[PRN-1].C0      = tempLine.mid(15,11).toDouble();  /*Clock error correction first order quantity 1，C0*/
        SsrEphochData->VectSSRdataBDS[PRN-1].C1      = tempLine.mid(26,11).toDouble();  /*2，C1*/
        SsrEphochData->VectSSRdataBDS[PRN-1].C2      = tempLine.mid(37,11).toDouble();  /*3，C2*/
    }

}
/*Read SSR code bias correction*/
int rtcm_ssr::ReadCODE_BIAS(QString tempLine,int *num,VeSSRData *SsrEphochData)
{
    int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0,ssrupT;
    double Seconds = 0;
    /*Deviation correction time of all satellite codes*/
    if (tempLine.contains("CODE_BIAS",Qt::CaseSensitive))
    {
        QStringList list_line = tempLine.split(' ');
        Qlist_to_dat(list_line,num,&Year,&Month,&Day,&Hours,&Minutes,&Seconds,&ssrupT);
        SsrEphochData->upSsrtime[2] = ssruptime[ssrupT];
        double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
        SsrEphochData->CODEGPSTime = ssr2time(GPS_Time);
        SsrEphochData->CODEGPSWeek = Week;
        return 3;
    }
    return 0;


}
/*SSR code bias data*/
void rtcm_ssr::readCODE_BIASData(QString tempLine,VeSSRData *SsrEphochData)
{
    int PRN = tempLine.mid(1,2).toInt();
    int num_code=0;
    if (tempLine.mid(0,1) == "G")
    {/*Enter the first epoch*/
        SsrEphochData->VectSSRdataGPS[PRN-1].cod_t0 = SsrEphochData->ClGPSTime;
        num_code = tempLine.mid(6,2).toInt();
        for(int i=0;i<num_code;i++)
        {
            if(tempLine.mid(11+i*16,2)=="1C")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE1C =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="1P")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE1P =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="1W")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE1W =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="2L")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE2L =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="2S")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE2S =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="2W")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE2W =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="2X")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE2X =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="5Q")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE5Q =tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="5X")
                SsrEphochData->VectSSRdataGPS[PRN-1].CODE5X =tempLine.mid(13+i*16,11).toDouble();

        }
     }
    /*Glonass System*/
    if (tempLine.mid(0,1) == "R")
    {/*Enter the first epoch*/
        num_code = tempLine.mid(6,2).toInt();
        SsrEphochData->VectSSRdataGLO[PRN-1].cod_t0 = SsrEphochData->ClGPSTime;
        for(int i=0;i<num_code;i++)
        {
            if(tempLine.mid(11+i*16,2)=="1C")
                SsrEphochData->VectSSRdataGLO[PRN-1].CODE1C = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="1P")
                SsrEphochData->VectSSRdataGLO[PRN-1].CODE1P = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="2C")
                SsrEphochData->VectSSRdataGLO[PRN-1].CODE2C = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="2P")
                SsrEphochData->VectSSRdataGLO[PRN-1].CODE2P = tempLine.mid(13+i*16,11).toDouble();
        }
     }
    /*BDS System*/
    if (tempLine.mid(0,1) == "C")
    {/*Enter the first epoch*/
        num_code = tempLine.mid(6,2).toInt();
        SsrEphochData->VectSSRdataBDS[PRN-1].cod_t0 = SsrEphochData->ClGPSTime;
        for(int i=0;i<num_code;i++)
        {
            if(tempLine.mid(11+i*16,2)=="2I")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE2I = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="6I")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE6I = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="7I")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE7I = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="1X")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE1X = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="1P")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE1P = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="5P")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE5P = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="5X")
                SsrEphochData->VectSSRdataBDS[PRN-1].CODE5X = tempLine.mid(13+i*16,11).toDouble();
        }
     }
    /*Gallieo System*/
    if (tempLine.mid(0,1) == "E")
    {/*Enter the first epoch*/
        num_code = tempLine.mid(6,2).toInt();
        SsrEphochData->VectSSRdataGLA[PRN-1].cod_t0 = SsrEphochData->ClGPSTime;
        for(int i=0;i<num_code;i++)
        {
            if(tempLine.mid(11+i*16,2)=="1C")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE1C = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="5Q")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE5Q = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="6C")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE6C = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="7Q")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE7Q = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="5X")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE5X = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="6X")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE6X = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="7X")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE7X = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="8X")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE8X = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="8Q")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE8Q = tempLine.mid(13+i*16,11).toDouble();
            else if(tempLine.mid(11+i*16,2)=="1X")
                SsrEphochData->VectSSRdataGLA[PRN-1].CODE1X = tempLine.mid(13+i*16,11).toDouble();
        }
    }
}
/*Read phase bias time*/
int rtcm_ssr::ReadPHASE_BIAS(QString tempLine,int *num,VeSSRData *SsrEphochData)
{
    int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0,ssrupT;
    double Seconds = 0;
    if (tempLine.contains("PHASE_BIAS",Qt::CaseSensitive))
    {
        QStringList list_line = tempLine.split(' ');
        Qlist_to_dat(list_line,num,&Year,&Month,&Day,&Hours,&Minutes,&Seconds,&ssrupT);
        SsrEphochData->upSsrtime[3] = ssruptime[ssrupT];
        double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
        SsrEphochData->PHESAGPSTime = ssr2time(GPS_Time);
        SsrEphochData->PHESAGPSWeek = Week;
        return 4;
    }
    return 0;

}
/*Read phase bias data*/
void rtcm_ssr::readPHASE_BIASData(QString tempLine,VeSSRData *SsrEphochData)
{
    int PRN = tempLine.mid(1,2).toInt();/*satellite PRN*/
    int num_phase =0;
    if (tempLine.mid(0,1) == "G")
    {/*Enter the first epoch*/
        num_phase = tempLine.mid(32,2).toInt();
        SsrEphochData->VectSSRdataGPS[PRN-1].phe_t0 = SsrEphochData->PHESAGPSTime;
        SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_ClER = tempLine.mid(3,13).toDouble();  /*1，Clock error*/
        SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_MEd  = tempLine.mid(16,13).toDouble();  /*2，Medium error*/
        for(int i=0;i<num_phase;i++)
        {
            if(tempLine.mid(37+i*28,2)=="1C")
            {
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_1C   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_1C1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_1C2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_1C3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="2W")
            {
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_2W   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_2W1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_2W2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_2W3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="5I")
            {
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_5I   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_5I1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_5I2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGPS[PRN-1].PHASE_5I3  = tempLine.mid(58+i*28,4).toInt();
            }
        }
     }
    /*Glonass System*/
    if (tempLine.mid(0,1) == "R")
    {/*Enter the first epoch*/
        SsrEphochData->VectSSRdataGLO[PRN-1].phe_t0 = SsrEphochData->PHESAGPSTime;
        SsrEphochData->VectSSRdataGLO[PRN-1].PHASE_ClER = tempLine.mid(3,13).toDouble();  /*1，Clock error*/
        SsrEphochData->VectSSRdataGLO[PRN-1].PHASE_MEd = tempLine.mid(16,13).toDouble();  /*2，Medium error*/
    }
    /*BDS System*/
    if (tempLine.mid(0,1) == "C")
    {/*Enter the first epoch*/
        SsrEphochData->VectSSRdataBDS[PRN-1].phe_t0 = SsrEphochData->PHESAGPSTime;
        SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_ClER = tempLine.mid(3,13).toDouble();  /*1，Clock error*/
        SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_MEd  = tempLine.mid(16,13).toDouble();  /*2，Medium error*/
        num_phase = tempLine.mid(32,2).toInt();
        for(int i=0;i<num_phase;i++)
        {
            if(tempLine.mid(37+i*28,2)=="2I")
            {
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2I   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2I1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2I2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2I3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="6I")
            {
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_6I   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_6I1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_6I2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_6I3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="7I")
            {
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_7I   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_7I1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_7I2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_7I3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="2W")
            {
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2W   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2W1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2W2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataBDS[PRN-1].PHASE_2W3  = tempLine.mid(58+i*28,4).toInt();
            }

        }
    }
    /*Galieo System*/
    if (tempLine.mid(0,1) == "E")
    {/*Enter the first epoch*/

        SsrEphochData->VectSSRdataGLA[PRN-1].phe_t0 = SsrEphochData->PHESAGPSTime;
        SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_ClER = tempLine.mid(3,13).toDouble();  /*1，Clock error*/
        SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_MEd  = tempLine.mid(16,13).toDouble();  /*2，Medium error*/
        num_phase = tempLine.mid(32,2).toInt();
        for(int i=0;i<num_phase;i++)
        {
            if(tempLine.mid(37+i*28,2)=="1C")
            {
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_1C   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_1C1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_1C2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_1C3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="5Q")
            {
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_5Q   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_5Q1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_5Q2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_5Q3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="7Q")
            {
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_7Q   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_7Q1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_7Q2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_7Q3  = tempLine.mid(58+i*28,4).toInt();
            }
            else if(tempLine.mid(37+i*28,2)=="6C")
            {
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_6C   = tempLine.mid(39+i*28,11).toDouble();/*1C*/
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_6C1  = tempLine.mid(50+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_6C2  = tempLine.mid(54+i*28,4).toInt();
                SsrEphochData->VectSSRdataGLA[PRN-1].PHASE_6C3  = tempLine.mid(58+i*28,4).toInt();
            }
        }

    }
}

void rtcm_ssr::Qlist_to_dat(QStringList list_line,int *num,int *Year,int *Month,int *Day,int *Hours,
                           int *Minutes,double *Seconds,int *ssrupT)
{
    for(int i=0;i<list_line.length();i++)
    {
        QString tem_list = list_line.at(i);
        switch (i) {
        case 2:
            *Year = tem_list.toInt();
            break;
        case 3:
            *Month = tem_list.toInt();
            break;
        case 4:
            *Day = tem_list.toInt();
            break;
        case 5:
            *Hours = tem_list.toInt();
            break;
        case 6:
            *Minutes = tem_list.toInt();
            break;
        case 7:
            *Seconds = tem_list.toDouble();
            break;
        case 8:
            *ssrupT  = tem_list.toInt();
            break;
        case 9:
            *num = tem_list.toInt();
            break;
        case 10:
            break;
        default:
            break;
        }
    }
}
/*Determine if the end of the file is reached (end)*/
bool rtcm_ssr::isSSrEnd()
{
    return m_readSsrFileClass.atEnd();
}
/**/
void rtcm_ssr::closeFile()
{
    if(!m_IsCloseFile) m_readSsrFileClass.close();
}
