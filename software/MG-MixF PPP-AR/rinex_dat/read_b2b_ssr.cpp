#include "read_b2b_ssr.h"

read_b2b_ssr::read_b2b_ssr()
{

}
void read_b2b_ssr::init_b2b_SsrVar()
{
    isRead_b2b_Head = false;
    isRead_b2b_AllData = false;
    IsSigal_b2b_File = false;
    m_b2b_SsrFileName = "";
    temp_b2b_SSRLine = "";
    m_b2b_SsrFileName.clear();
    m_IsClose_b2b_File = true;
    frist_falg = true;
}
void read_b2b_ssr::set_b2b_Ssr_FileNames(QString SsrFileNames)
{
    init_b2b_SsrVar();
    if (SsrFileNames.trimmed().isEmpty())
    {
        QString erroInfo = "File Name is Empty!(Readssr::Readssr(QString SSRfileName))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }
    m_b2b_SsrFileName = SsrFileNames;
    /*set file name is not close*/
    /*open a file*/
    m_read_b2b_SsrFileClass.setFileName(m_b2b_SsrFileName);
    if (!m_read_b2b_SsrFileClass.open(QFile::ReadOnly))
    {
        QString erroInfo = "Open file bad!(Readssr::Readssr(QString SsrFileNames))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }
    m_IsClose_b2b_File = false;

}
/*Read epochNum epoch data
 * (premature to the bottom of the file may be less than epochNum)*/
void read_b2b_ssr::getMult_b2b_SsrData(QVector< Ve_B2B_CLKData >&multB2B_CLKData,
                                       QVector< Ve_B2B_ORBData >&multB2B_ORBData,
                                       QVector< Ve_B2B_CBSData >&multB2B_CBSData,int epochNum)
{
    if (epochNum < 1) return ;
    int i = 0;
    Ve_B2B_CLKData B2B_CLK_save_Data;
    Ve_B2B_ORBData B2B_ORB_save_Data;
    Ve_B2B_CBSData B2B_CBS_save_Data;
    int b2b_clk_update = 0, b2b_cbs_update = 0, b2b_orb_update = 0;
    while(!m_read_b2b_SsrFileClass.atEnd())
    {
        if (i++ >= epochNum) break;
        Ve_B2B_CLKData B2B_CLKData;
        Ve_B2B_ORBData B2B_ORBData;
        Ve_B2B_CBSData B2B_CBSData;
        int b2b_clk_flag = 0, b2b_cbs_flag = 0, b2b_orb_flag = 0;
        /*Get the SSR closest to the current observation time*/
        getB2BSSRData(&B2B_CLKData,&B2B_ORBData,&B2B_CBSData,&b2b_clk_flag,&b2b_cbs_flag,&b2b_orb_flag);
        if(b2b_clk_update==0)
        {
            if(b2b_clk_flag==1)
            {
                B2B_CLK_save_Data = B2B_CLKData;
                b2b_clk_update = 1;
            }
        }
        else
        {
            if(b2b_clk_flag==1)
            {
                if(timediff(B2B_CLK_save_Data.ClGPSTime,B2B_CLKData.ClGPSTime)!=0)
                {
                    multB2B_CLKData.append(B2B_CLK_save_Data);
                    B2B_CLK_save_Data = B2B_CLKData;
                }
                else
                {
                    for(int j=0;j<MAXPRNGPS;j++)
                    {
                        if(B2B_CLKData.VeB2B_CLKDataGPS[j].PRN!=0)
                            B2B_CLK_save_Data.VeB2B_CLKDataGPS[j] = B2B_CLKData.VeB2B_CLKDataGPS[j];
                    }
                    for(int j=0;j<MAXPRNBDS;j++)
                    {
                        if(B2B_CLKData.VeB2B_CLKDataBDS[j].PRN!=0)
                            B2B_CLK_save_Data.VeB2B_CLKDataBDS[j] = B2B_CLKData.VeB2B_CLKDataBDS[j];
                    }
                }
            }
        }
        if(b2b_orb_update==0)
        {
            if(b2b_orb_flag==1)
            {
                B2B_ORB_save_Data = B2B_ORBData;
                b2b_orb_update = 1;
            }
        }
        else
        {
            if(b2b_orb_flag==1)
            {
                if(timediff(B2B_ORB_save_Data.ORBGPSTime,B2B_ORBData.ORBGPSTime)!=0)
                {
                    multB2B_ORBData.append(B2B_ORB_save_Data);
                    B2B_ORB_save_Data = B2B_ORBData;
                }
                else
                {
                    for(int j=0;j<MAXPRNGPS;j++)
                    {
                        if(B2B_ORBData.VeB2B_ORBDataGPS[j].PRN!=0)
                            B2B_ORB_save_Data.VeB2B_ORBDataGPS[j] = B2B_ORBData.VeB2B_ORBDataGPS[j];
                    }
                    for(int j=0;j<MAXPRNBDS;j++)
                    {
                        if(B2B_ORBData.VeB2B_ORBDataBDS[j].PRN!=0)
                            B2B_ORB_save_Data.VeB2B_ORBDataBDS[j] = B2B_ORBData.VeB2B_ORBDataBDS[j];
                    }
                }
            }

        }
        if(b2b_cbs_update==0)
        {
            if(b2b_cbs_flag==1)
            {
                B2B_CBS_save_Data = B2B_CBSData;
                b2b_cbs_update = 1;
            }
        }
        else
        {
            if(b2b_cbs_flag==1)
            {
                if(timediff(B2B_CBS_save_Data.CBSGPSTime,B2B_CBSData.CBSGPSTime)!=0)
                {
                    multB2B_CBSData.append(B2B_CBS_save_Data);
                    B2B_CBS_save_Data = B2B_CBSData;
                }
                else
                {
                    for(int j=0;j<MAXPRNGPS;j++)
                    {
                        if(B2B_CBSData.VeB2B_CBSDataGPS[j].PRN!=0)
                            B2B_CBS_save_Data.VeB2B_CBSDataGPS[j] = B2B_CBSData.VeB2B_CBSDataGPS[j];
                    }
                    for(int j=0;j<MAXPRNBDS;j++)
                    {
                        if(B2B_CBSData.VeB2B_CBSDataBDS[j].PRN!=0)
                            B2B_CBS_save_Data.VeB2B_CBSDataBDS[j] = B2B_CBSData.VeB2B_CBSDataBDS[j];
                    }
                }
            }

        }

    }
}
/*Get SSR data*/
void read_b2b_ssr::getB2BSSRData(Ve_B2B_CLKData *B2B_CLKData,Ve_B2B_ORBData *B2B_ORBData,Ve_B2B_CBSData *B2B_CBSData,int *b2b_clk_flag,int *b2b_cbs_flag,int *b2b_orb_flag)
{

    gtime_t b2b_clk_t0;gtime_t b2b_cbs_t0;gtime_t b2b_orb_t0;
    gtime_t b2b_clk_t1;gtime_t b2b_cbs_t1;gtime_t b2b_orb_t1;
    bool frist_flag0 = false;
    while (!m_read_b2b_SsrFileClass.atEnd())
    {
        if(frist_flag0||frist_falg)
        {
            temp_b2b_SSRLine = m_read_b2b_SsrFileClass.readLine();
            frist_falg = false;
            frist_flag0 = true;
        }
        else
            frist_flag0 = true;

        /*Loop read find orbit data*/
        if(temp_b2b_SSRLine.contains(">Msg4/CLK",Qt::CaseSensitive))
        {
            int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
            int PRN=0;
            double Seconds = 0;
            Year = temp_b2b_SSRLine.mid(52,4).toInt();
            Month = temp_b2b_SSRLine.mid(57,2).toInt();
            Day = temp_b2b_SSRLine.mid(60,2).toInt();
            Hours = temp_b2b_SSRLine.mid(63,2).toInt();
            Minutes = temp_b2b_SSRLine.mid(66,2).toInt();
            Seconds = temp_b2b_SSRLine.mid(69,5).toDouble();
            PRN = temp_b2b_SSRLine.mid(84,2).toInt();
            double GPS_Time[6];
            GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
            GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
            b2b_clk_t1 = ssr2time(GPS_Time);
            if(*b2b_clk_flag==0)
            {
                *b2b_clk_flag = 1;
                b2b_clk_t0 = ssr2time(GPS_Time);
                B2B_CLKData->ClGPSTime = b2b_clk_t0;
            }
            else if(timediff(b2b_clk_t0,b2b_clk_t1)!=0)
                break;

            if (temp_b2b_SSRLine.mid(83,1) == "G")
            {
                B2B_CLKData->VeB2B_CLKDataGPS[PRN-1].clk_t0 = ssr2time(GPS_Time);
                Year = temp_b2b_SSRLine.mid(11,4).toInt();
                Month = temp_b2b_SSRLine.mid(16,2).toInt();
                Day = temp_b2b_SSRLine.mid(19,2).toInt();
                Hours = temp_b2b_SSRLine.mid(22,2).toInt();
                Minutes = temp_b2b_SSRLine.mid(25,2).toInt();
                Seconds = temp_b2b_SSRLine.mid(28,5).toDouble();
                GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
                GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
                B2B_CLKData->VeB2B_CLKDataGPS[PRN-1].obs_b2b_time = ssr2time(GPS_Time);
                PRN = temp_b2b_SSRLine.mid(84,2).toInt();
                B2B_CLKData->VeB2B_CLKDataGPS[PRN-1].PRN = PRN;
                B2B_CLKData->VeB2B_CLKDataGPS[PRN-1].iodp = temp_b2b_SSRLine.mid(80,2).toInt();
                B2B_CLKData->VeB2B_CLKDataGPS[PRN-1].clk_iodcor = temp_b2b_SSRLine.mid(94,1).toInt();
                B2B_CLKData->VeB2B_CLKDataGPS[PRN-1].C0 = temp_b2b_SSRLine.mid(96,8).toDouble();
            }
            else if (temp_b2b_SSRLine.mid(83,1) == "C")
            {
                B2B_CLKData->VeB2B_CLKDataBDS[PRN-1].clk_t0 = ssr2time(GPS_Time);
                Year = temp_b2b_SSRLine.mid(11,4).toInt();
                Month = temp_b2b_SSRLine.mid(16,2).toInt();
                Day = temp_b2b_SSRLine.mid(19,2).toInt();
                Hours = temp_b2b_SSRLine.mid(22,2).toInt();
                Minutes = temp_b2b_SSRLine.mid(25,2).toInt();
                Seconds = temp_b2b_SSRLine.mid(28,5).toDouble();
                GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
                GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
                B2B_CLKData->VeB2B_CLKDataBDS[PRN-1].PRN = PRN;
                B2B_CLKData->VeB2B_CLKDataBDS[PRN-1].obs_b2b_time = ssr2time(GPS_Time);
                B2B_CLKData->VeB2B_CLKDataBDS[PRN-1].iodp = temp_b2b_SSRLine.mid(80,2).toInt();
                B2B_CLKData->VeB2B_CLKDataBDS[PRN-1].clk_iodcor = temp_b2b_SSRLine.mid(94,1).toInt();
                B2B_CLKData->VeB2B_CLKDataBDS[PRN-1].C0 = temp_b2b_SSRLine.mid(96,8).toDouble();
            }

        }
        if (temp_b2b_SSRLine.contains(">Msg3/CBS",Qt::CaseSensitive))
        {
            int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
            int PRN=0;int CBS_nb=0;
            double Seconds = 0;
            Year = temp_b2b_SSRLine.mid(52,4).toInt();
            Month = temp_b2b_SSRLine.mid(57,2).toInt();
            Day = temp_b2b_SSRLine.mid(60,2).toInt();
            Hours = temp_b2b_SSRLine.mid(63,2).toInt();
            Minutes = temp_b2b_SSRLine.mid(66,2).toInt();
            Seconds = temp_b2b_SSRLine.mid(69,5).toDouble();
            PRN = temp_b2b_SSRLine.mid(76,2).toInt();
            double GPS_Time[6];
            GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
            GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
            b2b_cbs_t1 = ssr2time(GPS_Time);
            if(*b2b_cbs_flag==0)
            {
                *b2b_cbs_flag = 1;
                b2b_cbs_t0 = ssr2time(GPS_Time);
                B2B_CBSData->CBSGPSTime = b2b_cbs_t0;
            }
            else if(timediff(b2b_cbs_t0,b2b_cbs_t1)!=0)
                break;
            if (temp_b2b_SSRLine.mid(75,1) == "G")
            {
                B2B_CBSData->VeB2B_CBSDataGPS[PRN-1].cbs_t0 = ssr2time(GPS_Time);
                Year = temp_b2b_SSRLine.mid(11,4).toInt();
                Month = temp_b2b_SSRLine.mid(16,2).toInt();
                Day = temp_b2b_SSRLine.mid(19,2).toInt();
                Hours = temp_b2b_SSRLine.mid(22,2).toInt();
                Minutes = temp_b2b_SSRLine.mid(25,2).toInt();
                Seconds = temp_b2b_SSRLine.mid(28,5).toDouble();
                GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
                GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
                B2B_CBSData->VeB2B_CBSDataGPS[PRN-1].PRN = PRN;
                B2B_CBSData->VeB2B_CBSDataGPS[PRN-1].obs_b2b_time = ssr2time(GPS_Time);
                CBS_nb = temp_b2b_SSRLine.mid(82,2).toInt();
                B2B_CBSData->VeB2B_CBSDataGPS[PRN-1].CBSType.clear();
                for(int i=0;i<CBS_nb;i++)
                {
                    QString CBS_type = temp_b2b_SSRLine.mid(85+i*13,3);
                    double CBS_value = temp_b2b_SSRLine.mid(88+i*13,9).toDouble();
                    B2B_CBSData->VeB2B_CBSDataGPS[PRN-1].CBSType.append(CBS_type);
                    B2B_CBSData->VeB2B_CBSDataGPS[PRN-1].CBSvalue.append(CBS_value);
                }
            }
            else if (temp_b2b_SSRLine.mid(75,1) == "C")
            {
                B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].cbs_t0 = ssr2time(GPS_Time);
                Year = temp_b2b_SSRLine.mid(11,4).toInt();
                Month = temp_b2b_SSRLine.mid(16,2).toInt();
                Day = temp_b2b_SSRLine.mid(19,2).toInt();
                Hours = temp_b2b_SSRLine.mid(22,2).toInt();
                Minutes = temp_b2b_SSRLine.mid(25,2).toInt();
                Seconds = temp_b2b_SSRLine.mid(28,5).toDouble();
                GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
                GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
                B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].PRN = PRN;
                B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].obs_b2b_time = ssr2time(GPS_Time);
                CBS_nb = temp_b2b_SSRLine.mid(82,2).toInt();
                B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].CBSType.clear();
                B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].CBSvalue.clear();
                for(int i=0;i<CBS_nb;i++)
                {
                    QString CBS_type = temp_b2b_SSRLine.mid(85+i*13,3);
                    double CBS_value = temp_b2b_SSRLine.mid(88+i*13,9).toDouble();
                    B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].CBSType.append(CBS_type);
                    B2B_CBSData->VeB2B_CBSDataBDS[PRN-1].CBSvalue.append(CBS_value);
                }
            }
        }
        if (temp_b2b_SSRLine.contains(">Msg2/ORB",Qt::CaseSensitive))
        {
            int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
            int PRN=0;
            double Seconds = 0;
            Year = temp_b2b_SSRLine.mid(52,4).toInt();
            Month = temp_b2b_SSRLine.mid(57,2).toInt();
            Day = temp_b2b_SSRLine.mid(60,2).toInt();
            Hours = temp_b2b_SSRLine.mid(63,2).toInt();
            Minutes = temp_b2b_SSRLine.mid(66,2).toInt();
            Seconds = temp_b2b_SSRLine.mid(69,5).toDouble();
            PRN = temp_b2b_SSRLine.mid(86,2).toInt();
            double GPS_Time[6];
            GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
            GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
            b2b_orb_t1 = ssr2time(GPS_Time);
            if(*b2b_orb_flag==0)
            {
                *b2b_orb_flag = 1;
                b2b_orb_t0 = ssr2time(GPS_Time);
                B2B_ORBData->ORBGPSTime = b2b_orb_t0;
            }
            else if(timediff(b2b_orb_t0,b2b_orb_t1)!=0)
                break;
            if (temp_b2b_SSRLine.mid(85,1) == "G")
            {
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].orb_t0 = ssr2time(GPS_Time);
                Year = temp_b2b_SSRLine.mid(11,4).toInt();
                Month = temp_b2b_SSRLine.mid(16,2).toInt();
                Day = temp_b2b_SSRLine.mid(19,2).toInt();
                Hours = temp_b2b_SSRLine.mid(22,2).toInt();
                Minutes = temp_b2b_SSRLine.mid(25,2).toInt();
                Seconds = temp_b2b_SSRLine.mid(28,5).toDouble();
                GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
                GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].PRN = PRN;
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].obs_b2b_time = ssr2time(GPS_Time);
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].iodn = temp_b2b_SSRLine.mid(80,4).toInt();
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].orb_iodcor = temp_b2b_SSRLine.mid(96,1).toInt();
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].r0 = temp_b2b_SSRLine.mid(97,9).toDouble();
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].a0 = temp_b2b_SSRLine.mid(106,9).toDouble();
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].c0 = temp_b2b_SSRLine.mid(117,9).toDouble();
                B2B_ORBData->VeB2B_ORBDataGPS[PRN-1].ura_ = temp_b2b_SSRLine.mid(126,9).toDouble();
            }
            else if (temp_b2b_SSRLine.mid(85,1) == "C")
            {
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].orb_t0 = ssr2time(GPS_Time);
                Year = temp_b2b_SSRLine.mid(11,4).toInt();
                Month = temp_b2b_SSRLine.mid(16,2).toInt();
                Day = temp_b2b_SSRLine.mid(19,2).toInt();
                Hours = temp_b2b_SSRLine.mid(22,2).toInt();
                Minutes = temp_b2b_SSRLine.mid(25,2).toInt();
                Seconds = temp_b2b_SSRLine.mid(28,5).toDouble();
                GPS_Time[0] = Year;GPS_Time[1] = Month;GPS_Time[2] = Day;
                GPS_Time[3] = Hours;GPS_Time[4] = Minutes;GPS_Time[5] = Seconds;
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].PRN = PRN;
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].obs_b2b_time = ssr2time(GPS_Time);
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].iodn = temp_b2b_SSRLine.mid(80,4).toInt();
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].orb_iodcor = temp_b2b_SSRLine.mid(96,1).toInt();
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].r0 = temp_b2b_SSRLine.mid(97,9).toDouble();
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].a0 = temp_b2b_SSRLine.mid(106,9).toDouble();
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].c0 = temp_b2b_SSRLine.mid(117,9).toDouble();
                B2B_ORBData->VeB2B_ORBDataBDS[PRN-1].ura_ = temp_b2b_SSRLine.mid(126,9).toDouble();
            }
        }
    }
}
void read_b2b_ssr::getPPP_B2b_time(PPP_B2b_DAT *Obs_B2b_Data,int *ssrRead_CLK_L,
                                   int *ssrRead_ORB_L,int *ssrRead_CBS_L,QVector< Ve_B2B_CLKData > multB2B_CLKData,
                                   QVector< Ve_B2B_ORBData > multB2B_ORBData,QVector< Ve_B2B_CBSData > multB2B_CBSData,gtime_t Otime)
{
    /*Obtain the latest SSR of observation time*/
    int multB2B_CLK_Len = multB2B_CLKData.length();int multB2B_ORB_Len = multB2B_ORBData.length();int multB2B_CBS_Len = multB2B_CBSData.length();
    bool flag_B2B_CLK=false,flag_B2B_ORB=false,flag_B2B_CBS=false;

    for(int i=*ssrRead_CLK_L;i<multB2B_CLK_Len;i++)
    {
        Ve_B2B_CLKData oneB2B_CLKData = multB2B_CLKData.at(i);
        /*Judge clock error correction time*/
        if(timediff(oneB2B_CLKData.ClGPSTime,Otime) > 0)
        {
            /*Storage of SSR data longer than observation time*/
            for(int j = 0;j< MAXPRNGPS;j++)
            {
                Obs_B2b_Data->B2B_CLKData[1].VeB2B_CLKDataGPS[j] = oneB2B_CLKData.VeB2B_CLKDataGPS[j];
            }
            for(int j = 0;j< MAXPRNBDS;j++)
            {
                Obs_B2b_Data->B2B_CLKData[1].VeB2B_CLKDataBDS[j] = oneB2B_CLKData.VeB2B_CLKDataBDS[j];
            }

            flag_B2B_CLK = true;
            break;
        }
        else if(timediff(oneB2B_CLKData.ClGPSTime,Otime) <=0)
        {
            for(int j = 0;j< MAXPRNGPS;j++)
            {
                Obs_B2b_Data->B2B_CLKData[0].VeB2B_CLKDataGPS[j] = oneB2B_CLKData.VeB2B_CLKDataGPS[j];
            }
            for(int j = 0;j< MAXPRNBDS;j++)
            {
                Obs_B2b_Data->B2B_CLKData[0].VeB2B_CLKDataBDS[j] = oneB2B_CLKData.VeB2B_CLKDataBDS[j];
            }
            /*Flag bit in vector for storing SSR data*/
            *ssrRead_CLK_L = i;
        }
    }
    for(int i=*ssrRead_ORB_L;i<multB2B_ORB_Len;i++)
    {
        Ve_B2B_ORBData oneB2B_ORBData = multB2B_ORBData.at(i);
        /*Judge clock error correction time*/
        if(timediff(oneB2B_ORBData.ORBGPSTime,Otime) > 0)
        {
            /*Storage of SSR data longer than observation time*/
            for(int j = 0;j< MAXPRNGPS;j++)
            {
                Obs_B2b_Data->B2B_ORBData[1].VeB2B_ORBDataGPS[j] = oneB2B_ORBData.VeB2B_ORBDataGPS[j];
            }
            for(int j = 0;j< MAXPRNBDS;j++)
            {
                Obs_B2b_Data->B2B_ORBData[1].VeB2B_ORBDataBDS[j] = oneB2B_ORBData.VeB2B_ORBDataBDS[j];
            }

            flag_B2B_ORB = true;
            break;
        }
        else if(timediff(oneB2B_ORBData.ORBGPSTime,Otime) <=0)
        {
            for(int j = 0;j< MAXPRNGPS;j++)
            {
                Obs_B2b_Data->B2B_ORBData[0].VeB2B_ORBDataGPS[j] = oneB2B_ORBData.VeB2B_ORBDataGPS[j];
            }
            for(int j = 0;j< MAXPRNBDS;j++)
            {
                Obs_B2b_Data->B2B_ORBData[0].VeB2B_ORBDataBDS[j] = oneB2B_ORBData.VeB2B_ORBDataBDS[j];
            }
            /*Flag bit in vector for storing SSR data*/
            *ssrRead_ORB_L = i;
        }
    }
    for(int i=*ssrRead_CBS_L;i<multB2B_CBS_Len;i++)
    {
        Ve_B2B_CBSData oneB2B_CBSData = multB2B_CBSData.at(i);
        /*Judge clock error correction time*/
        if(timediff(oneB2B_CBSData.CBSGPSTime,Otime) > 0)
        {
            /*Storage of SSR data longer than observation time*/
            for(int j = 0;j< MAXPRNGPS;j++)
            {
                Obs_B2b_Data->B2B_CBSData[1].VeB2B_CBSDataGPS[j] = oneB2B_CBSData.VeB2B_CBSDataGPS[j];
            }
            for(int j = 0;j< MAXPRNBDS;j++)
            {
                Obs_B2b_Data->B2B_CBSData[1].VeB2B_CBSDataBDS[j] = oneB2B_CBSData.VeB2B_CBSDataBDS[j];
            }

            flag_B2B_CBS = true;
            break;
        }
        else if(timediff(oneB2B_CBSData.CBSGPSTime,Otime) <=0)
        {
            for(int j = 0;j< MAXPRNGPS;j++)
            {
                Obs_B2b_Data->B2B_CBSData[0].VeB2B_CBSDataGPS[j] = oneB2B_CBSData.VeB2B_CBSDataGPS[j];
            }
            for(int j = 0;j< MAXPRNBDS;j++)
            {
                Obs_B2b_Data->B2B_CBSData[0].VeB2B_CBSDataBDS[j] = oneB2B_CBSData.VeB2B_CBSDataBDS[j];
            }
            /*Flag bit in vector for storing SSR data*/
            *ssrRead_CBS_L = i;
        }
    }

}

/*Determine if the end of the file is reached (end)*/
bool read_b2b_ssr::isSSr_B2B_End()
{
    return m_read_b2b_SsrFileClass.atEnd();
}
/**/
void read_b2b_ssr::close_B2B_File()
{
    if(!m_IsClose_b2b_File) m_read_b2b_SsrFileClass.close();
}
