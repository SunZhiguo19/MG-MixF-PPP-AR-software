#include "rinex_obs.h"

rinex_obs::rinex_obs()
{

}

/*Initialize obs variables*/
void rinex_obs::initObsVar()
{
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();

    pravide_obs->m_OfileName = "";
    tempLine = "";
    isReadHead = false;
    m_IsCloseFile = true;
//    mult_OfileName.clear();
    m_wantObsTypeVct.clear();
    m_obsVarNamesVer3.clear();
    m_ObservVarsNames_2.clear();
    m_allObsTypePos.clear();
/*Header information*/
    RinexVersion = 0;
    FileIdType = 0;
    SatelliteSys = 0;
    PGM = "";
    RUNBY = "";
    CreatFileDate = "";
    CommentInfo = "";
    MarkerName = "";
    ObserverNames = "";
    Agency = "";
    ReciverREC = "";
    ReciverType = "";
    ReciverVers = "";
    AntNumber = "";
    AntType = "";
    for (int i = 0; i < 3; i++)
    {
        ApproxXYZ[i] = 0;
        AntHEN[i] = 0;
    }
    FactL12[0] = -1; FactL12[1] = -1;
    m_TypeObservNum_2 = -1;
    IntervalSeconds = -1;
    for (int i = 0; i < 5; i++)
    {
        YMDHM[i] = 0;
    }
    ObsSeconds = 0;
    SateSystemOTime = "";
    matchHead.setPattern(".*[GRSCE][0-9][0-9].*");/*Simple match header file (2.X) (can be optimized)*/
}


/*Set observation file Class initialization*/
void rinex_obs::setObsFileName(QStringList OfileName)
{
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();

    initObsVar();

    pravide_obs->mult_OfileName = OfileName;
    arrange_obs();

    if(!OfileName.isEmpty())
        pravide_obs->m_OfileName = OfileName.at(0);
    Serial_number = 0;
    /*Read file header information and parse*/
    getHeadInf();/*The file is not closed here*/
    baseYear = (int)(YMDHM[0]/100)*100;
    pravide_obs->year_ = YMDHM[0];
    pravide_obs->year_acc_day = YearAccDay(YMDHM[0],YMDHM[1],YMDHM[2]);
}

void rinex_obs::arrange_obs()
{
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();

    QString OBSfilename = pravide_obs->mult_OfileName.at(0);
    QStringList obs_data_name = OBSfilename.split("/");
    if(obs_data_name.length()==0)   return ;
    OBSfilename = obs_data_name.at(obs_data_name.length()-1);
    int obsTim[pravide_obs->mult_OfileName.length()];
    if(OBSfilename.length()>8)
        pravide_obs->The_same_ant = OBSfilename.mid(0,4).trimmed();
    for(int i=0;i<pravide_obs->mult_OfileName.length();i++)
    {
        OBSfilename = pravide_obs->mult_OfileName.at(i);
        obs_data_name = OBSfilename.split("/");
        OBSfilename = obs_data_name.at(obs_data_name.length()-1);
        if(!OBSfilename.contains(pravide_obs->The_same_ant,Qt::CaseInsensitive))
        {
            pravide_obs->mult_OfileName.removeAt(i);
            i=i-1;
        }
        else
        {
            if(OBSfilename.length()>28)
                obsTim[i] = OBSfilename.mid(16,7).toInt();
        }
    }
    obs_data_name.clear();
    QString Obs_file_Str[pravide_obs->mult_OfileName.length()];
    for(int i=0;i<pravide_obs->mult_OfileName.length();i++)  Obs_file_Str[i] = pravide_obs->mult_OfileName.at(i);
    for(int i=0;i<pravide_obs->mult_OfileName.length();i++)
    {
        for(int j=0;j<pravide_obs->mult_OfileName.length()-i-1;j++)
        {
            if(obsTim[j]>obsTim[j+1])
            {
                 int ssrnumS = obsTim[j];
                 obsTim[j] = obsTim[j+1];
                 obsTim[j+1] = ssrnumS;
                 QString Obsfilename = Obs_file_Str[j];
                 Obs_file_Str[j] = Obs_file_Str[j+1];
                 Obs_file_Str[j+1] = Obsfilename;
            }
        }
    }
    int mult_len = pravide_obs->mult_OfileName.length();
    pravide_obs->mult_OfileName.clear();
    for(int i=0;i<mult_len;i++)
        pravide_obs->mult_OfileName.append(Obs_file_Str[i]);
}

/*wantObsTypeVct = {{G, L1C, L1W}, {R, L1C, L2P},...}*/
void rinex_obs::setWangObsType(QVector<QStringList> wantObsTypeVct)
{
    m_wantObsTypeVct = wantObsTypeVct;
}

/*Read the header file to the variable
 * (each version O file header file is similar,The file is not closed here）*/
void rinex_obs::getHeadInf()
{
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();
    if (isReadHead)	return ;
    /*open a file*/
    m_readOFileClass.setFileName(pravide_obs->m_OfileName);
    if (!m_readOFileClass.open(QFile::ReadOnly))
    {
        QString erroInfo = "Open file bad!(rinex_obs(QString OfileName))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }

    m_IsCloseFile = false;
    /*Read header file*/
    while (!m_readOFileClass.atEnd())
    {
        tempLine = m_readOFileClass.readLine();
        if (tempLine.contains("END OF HEADER",Qt::CaseInsensitive))
            break;
        if (tempLine.contains("RINEX VERSION / TYPE",Qt::CaseInsensitive))
        {
            RinexVersion =  tempLine.mid(0,20).trimmed().toDouble();
            FileIdType = tempLine.at(20);
            SatelliteSys = tempLine.at(40);
        }
        else if (tempLine.contains("PGM / RUN BY / DATE",Qt::CaseInsensitive))
        {
            PGM = tempLine.mid(0,20).trimmed();
            RUNBY = tempLine.mid(20,20).trimmed();
            CreatFileDate = tempLine.mid(40,20).trimmed();
        }
        else if (tempLine.contains("COMMENT",Qt::CaseInsensitive))
        {
            CommentInfo+=tempLine.mid(0,60).trimmed() + "\n";
        }
        else if (tempLine.contains("MARKER NAME",Qt::CaseInsensitive))
        {
            MarkerName = tempLine.mid(0,60).trimmed().toUpper();
        }
        else if (tempLine.contains("MARKER NUMBER"))
        {
            MarkerNumber = tempLine.mid(0,60).trimmed();
        }
        else if (tempLine.contains("OBSERVER / AGENCY",Qt::CaseInsensitive))
        {
            ObserverNames = tempLine.mid(0,20).trimmed();
            Agency = tempLine.mid(20,40).trimmed();
        }
        else if (tempLine.contains("REC # / TYPE / VERS"))
        {
            ReciverREC = tempLine.mid(0,20).trimmed();
            ReciverType = tempLine.mid(20,20).trimmed();
            ReciverVers = tempLine.mid(40,20).trimmed();
        }
        else if (tempLine.contains("ANT # / TYPE",Qt::CaseInsensitive))
        {
            AntNumber = tempLine.mid(0,20).trimmed();
            AntType = tempLine.mid(20,20).trimmed();
        }
        else if (tempLine.contains("APPROX POSITION XYZ",Qt::CaseInsensitive))
        {
            QString tempPos = tempLine.mid(0, 59).trimmed();
            tempPos = tempPos.simplified();
            QStringList headList = tempPos.split(" ");
            for(int i = 0;i < 3;i++)
                ApproxXYZ[i] = headList.at(i).toDouble();
        }
        else if (tempLine.contains("ANTENNA: DELTA H/E/N",Qt::CaseInsensitive))
        {
            QString tempHEN = tempLine.mid(0, 59).trimmed();
            tempHEN = tempHEN.simplified();
            QStringList headList = tempHEN.split(" ");
            for(int i = 0;i < 3;i++)
                AntHEN[i] = headList.at(i).toDouble();
        }
        else if (tempLine.contains("WAVELENGTH FACT L1/2",Qt::CaseInsensitive))
        {
            FactL12[0] = tempLine.mid(0,6).trimmed().toInt();
            FactL12[1] = tempLine.mid(6,6).trimmed().toInt();
        }
        else if (tempLine.contains("TYPES OF OBSERV",Qt::CaseInsensitive))
        {
            /*Due to the use of the global variable Static,
             * the function has a big bug. It is forbidden to use global variables.
             * It will not disappear in the main function.*/
            if (RinexVersion >= 3) continue;
            if(m_TypeObservNum_2 < 1)
                m_TypeObservNum_2 = tempLine.left(6).trimmed().toInt();
            if (m_TypeObservNum_2 != 0 )
            {
                for (int i = 0;i < 9;i++)
                {
                    QString tempObserVar = "";
                    tempObserVar = tempLine.left(6*i + 12).right(6).trimmed();
                    if (tempObserVar.isEmpty())
                        break;
                    m_ObservVarsNames_2.append(tempObserVar);
                }
            }
            else
            {
                QString erroInfo = "Lack TYPES OF OBSERV!(QReadOFile::QReadOFile(QString OfileName))";
                disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
                text_disp->data_text.append(erroInfo);
                text_disp->data_lenth++;
            }

        }/* TYPES OF OBSERV if End*/
        else if (tempLine.contains("SYS / # / OBS TYPES",Qt::CaseInsensitive))
        {/*Renix 3.0 added header file*/
            if (RinexVersion < 3)	continue;
            obsVarNamesVer3 tempObsType;
            tempObsType.SatType = tempLine.mid(0, 1);
            tempObsType.obsNum3ver = tempLine.mid(3, 3).trimmed().toInt();
            QString obstypeName = "";
            int flag = 0, read_hang_num = 0, i = 0;
            read_hang_num = (int)(tempObsType.obsNum3ver/13.01);/*tempObsType.obsNum3ver if a multiple of 13 read one less line*/

            for (i = 0; i < tempObsType.obsNum3ver;i++)
            {
                obstypeName = tempLine.mid(6 + flag * 4, 4).trimmed();
                flag++;
                tempObsType.obsNames3ver.append(obstypeName);
                /*Exceeding 13 parsing the next line*/
                if (flag % 13 == 0 && read_hang_num > 0)
                {
                    tempLine = m_readOFileClass.readLine();
                    flag = 0; read_hang_num--;
                }
            }
            m_obsVarNamesVer3.append(tempObsType);/*Save this system data*/
        }
        else if (tempLine.contains("INTERVAL",Qt::CaseInsensitive))
        {
            IntervalSeconds = tempLine.mid(0,10).trimmed().toDouble();
        }
        else if (tempLine.contains("TIME OF FIRST OBS",Qt::CaseInsensitive))
        {
            for (int i = 0;i < 5;i++)
            {
                YMDHM[i] = tempLine.left((i+1)*6).right(6).trimmed().toInt();
            }
            ObsSeconds = tempLine.left(43).right(13).trimmed().toDouble();
            SateSystemOTime = tempLine.left(51).right(8).trimmed();
        }
    }
    isReadHead = true;
    /*process Reniex 3*/
    if(RinexVersion >= 3)
        PriorityRanking();
    /*If the header file does not have obs time to retrieve the observed data*/
    if(0 == YMDHM[0])
    {
        int current_pos = m_readOFileClass.pos();
        tempLine = m_readOFileClass.readLine();
        baseYear = 2000;/*Document 2.0 before 2000 does not support!!!*/
        if(RinexVersion < 3)
        {
            YMDHM[0] = tempLine.mid(0,3).trimmed().toInt() + baseYear;
            YMDHM[1] = tempLine.mid(3,3).trimmed().toInt();
            YMDHM[2] = tempLine.mid(6,3).trimmed().toInt();
            YMDHM[3] = tempLine.mid(9,3).trimmed().toInt();
            YMDHM[4] = tempLine.mid(12,3).trimmed().toInt();
            ObsSeconds = tempLine.mid(15,11).trimmed().toDouble();
        }
        else if(RinexVersion >= 3)
        {
            YMDHM[0] = tempLine.mid(1,5).trimmed().toInt();
            YMDHM[1] = tempLine.mid(6,3).trimmed().toInt();
            YMDHM[2] = tempLine.mid(9,3).trimmed().toInt();
            YMDHM[3] = tempLine.mid(12,3).trimmed().toInt();
            YMDHM[4] = tempLine.mid(15,3).trimmed().toInt();
            ObsSeconds = tempLine.mid(18,11).trimmed().toDouble();
        }
        m_readOFileClass.seek(current_pos);
    }
}

/*Convert characters to data lines, parse them into satellite data, and obtain satellite ranging codes and carrier data*/
bool rinex_obs::getOneSatlitData(QString &dataString,SatlitData &oneSatlite)
{/*Read Rinex 2.X and 3.X observation files can be used*/
    if (dataString.isEmpty())	return false;
    int obserNumbers = 0;
    /*Obtain satellite ranging code and carrier data location by version*/
    obserNumbers = oneSatlite.obserType.length();
    for(int i = 0;i < obserNumbers; i++)
    {
        QString obsType = oneSatlite.obserType.at(i);
        double obsValue = dataString.mid(i*16, 14).trimmed().toDouble();
        oneSatlite.obserValue.append(obsValue);
        int obsSig_Inten = dataString.mid(i*16+15,1).trimmed().toInt();
        oneSatlite.obsSigInten.append(obsSig_Inten);
        if(obsType.contains("L"))
        {
            int obs_LL1_LOCK = dataString.mid(i*16+14,1).trimmed().toInt();
            oneSatlite.obsLL1_LOCK.append(obs_LL1_LOCK);
        }
    }
    for(int i = 0;i < 5; i++)
    {
        oneSatlite.CodeP[i] = 0;
        oneSatlite.PhaseL[i] = 0;
    }
    return true;
}

/*Acquisition frequency*/
void rinex_obs::getFrequency(SatlitData &oneSatlite)
{
    if (RinexVersion >= 3)
        getFrequencyVer3(oneSatlite);
    else if (RinexVersion < 3)
        getFrequencyVer2(oneSatlite);
    return ;
}

/*Get the frequency of L1 and L2 according to the type of satellite PRN*/
void rinex_obs::getFrequencyVer2(SatlitData &oneSatlite)
{
    oneSatlite.Frq[0] = 0; oneSatlite.Frq[1] = 0; oneSatlite.Frq[2] = 0;
    if (oneSatlite.SatType == 'G')
    {/*There is a bug: if the GPS frequency is C2 C1*/
        oneSatlite.Frq[0] = g_GPSFrq[1];
        oneSatlite.Frq[1] = g_GPSFrq[2];
        oneSatlite.Frq[2] = g_GPSFrq[5];
    }
    else if (oneSatlite.SatType == 'R')
    {
        oneSatlite.Frq[0] = M_GLONASSF1(g_GlonassK[oneSatlite.PRN - 1]);
        oneSatlite.Frq[1] = M_GLONASSF2(g_GlonassK[oneSatlite.PRN - 1]);
    }
    else if (oneSatlite.SatType == 'C')
    {
        oneSatlite.Frq[0] = g_BDSFrq[1];
        oneSatlite.Frq[1] = g_BDSFrq[7];
        oneSatlite.Frq[2] = g_BDSFrq[6];
    }
    else if (oneSatlite.SatType == 'E')
    {
        oneSatlite.Frq[0] = g_GalieoFrq[1];
        oneSatlite.Frq[1] = g_GalieoFrq[5];
    }

}

/*Get the frequency of L1 and L2 according to the type of satellite PRN*/
void rinex_obs::getFrequencyVer3(SatlitData &oneSatlite)
{
    oneSatlite.Frq[0] = 0; oneSatlite.Frq[1] = 0; oneSatlite.Frq[2] = 0; oneSatlite.Frq[3] = 0;oneSatlite.Frq[4] = 0;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();

    int frequence_flag[5];
    for(int i=0;i<5;i++)
    {
        if(opt_corr->Combination_mode==Comb_UD_mode)
        {
            if(oneSatlite.SatType=='G')
                frequence_flag[i] = opt_corr->Frequently_GPS[i];
            else if(oneSatlite.SatType=='C')
                frequence_flag[i] = opt_corr->Frequently_BDS[i];
            else if(oneSatlite.SatType=='E')
                frequence_flag[i] = opt_corr->Frequently_GAL[i];
            else if(oneSatlite.SatType=='R')
                frequence_flag[i] = opt_corr->Frequently_GLO[i];
        }
        else
            frequence_flag[i]=1;
    }

    if (oneSatlite.SatType == 'G')
    {
        if(opt_corr->Combination_mode==Comb_UD_mode)
        {
            int LType[3];
            for(int i=0;i<3;i++)
            {
                if(frequence_flag[i]>0)
                    LType[i] = oneSatlite.wantObserType.at(i*2+1).mid(1,1).toInt();
                else
                    LType[i] = -1;
            }
            for(int i=0;i<3;i++)
            {
                if(LType[i] >= 0)
                {
                    oneSatlite.Lambda[i]=M_GPSLamta(LType[i]);
                    oneSatlite.Frq[i] = g_GPSFrq[LType[i]];
                }
            }
        }
        else
        {
            int L1Type = oneSatlite.wantObserType.at(1).mid(1,1).toInt(),
                L2Type = oneSatlite.wantObserType.at(3).mid(1,1).toInt(),
                L3Type = oneSatlite.wantObserType.at(5).mid(1,1).toInt();
            if(L1Type >= 0)
            {
                oneSatlite.Lambda[0]=M_GPSLamta(L1Type);
                oneSatlite.Frq[0] = g_GPSFrq[L1Type];
            }
            if(L2Type >= 0)
            {
                oneSatlite.Lambda[1]=M_GPSLamta(L2Type);
                oneSatlite.Frq[1] = g_GPSFrq[L2Type];
            }
            if(L3Type >= 0)
            {
                oneSatlite.Lambda[2]=M_GPSLamta(L3Type);
                oneSatlite.Frq[2] = g_GPSFrq[L3Type];
            }
        }

    }
    else if (oneSatlite.SatType == 'R')
    {

        oneSatlite.Frq[0] = M_GLONASSF1(g_GlonassK[oneSatlite.PRN - 1]);
        oneSatlite.Frq[1] = M_GLONASSF2(g_GlonassK[oneSatlite.PRN - 1]);
        oneSatlite.Frq[2] = 1.202025e9;
        oneSatlite.Lambda[0] = M_GLOSSLamta1k(g_GlonassK[oneSatlite.PRN - 1]);
        oneSatlite.Lambda[1] = M_GLOSSLamta2k(g_GlonassK[oneSatlite.PRN - 1]);
        oneSatlite.Lambda[2] = M_C/1.202025e9;

    }
    else if (oneSatlite.SatType == 'C')
    {
        if(opt_corr->Combination_mode==Comb_UD_mode)
        {
            int LType[5];
            for(int i=0;i<5;i++)
            {
                if(frequence_flag[i]>0)
                    LType[i] = oneSatlite.wantObserType.at(i*2+1).mid(1,1).toInt();
                else
                    LType[i] = -1;
            }

            for(int i=0;i<5;i++)
            {
                if(LType[i] >= 0)
                {
                    oneSatlite.Lambda[i]=M_BDSLamtak(LType[i]);
                    oneSatlite.Frq[i] = g_BDSFrq[LType[i]];
                }
            }
        }
        else
        {
            oneSatlite.Frq[0] = g_BDSFrq[2];/*B1I*/
            oneSatlite.Frq[1] = g_BDSFrq[6];/*B3I*/
            oneSatlite.Frq[2] = g_BDSFrq[7];/*B2I*/
            oneSatlite.Frq[3] = g_BDSFrq[1];/*B1C*/
            oneSatlite.Frq[4] = g_BDSFrq[5];/*B2A*/
            oneSatlite.Lambda[0] = M_BDSLamtak(2);/*B1I*/
            oneSatlite.Lambda[1] = M_BDSLamtak(6);/*B3I*/
            oneSatlite.Lambda[2] = M_BDSLamtak(7);/*B2I*/
            oneSatlite.Lambda[3] = M_BDSLamtak(1);/*B1C*/
            oneSatlite.Lambda[4] = M_BDSLamtak(5);/*B2A*/
        }

    }
    else if (oneSatlite.SatType == 'E')
    {
        if(opt_corr->Combination_mode==Comb_UD_mode)
        {
            int LType[5];
            for(int i=0;i<5;i++)
            {
                if(frequence_flag[i]>0)
                    LType[i] = oneSatlite.wantObserType.at(i*2+1).mid(1,1).toInt();
                else
                    LType[i] = -1;
            }

            for(int i=0;i<5;i++)
            {
                if(LType[i] >= 0)
                {
                    oneSatlite.Lambda[i]=M_GalieoLamtak(LType[i]);
                    oneSatlite.Frq[i] = g_GalieoFrq[LType[i]];
                }
            }
        }
        else
        {
            int L1Type = oneSatlite.wantObserType.at(1).mid(1,1).toInt(),
                L2Type = oneSatlite.wantObserType.at(3).mid(1,1).toInt(),
                L3Type = oneSatlite.wantObserType.at(5).mid(1,1).toInt(),
                L4Type = oneSatlite.wantObserType.at(7).mid(1,1).toInt(),
                L5Type = oneSatlite.wantObserType.at(9).mid(1,1).toInt();
            if(L1Type >= 0)
            {
                oneSatlite.Frq[0] = g_GalieoFrq[L1Type];
                oneSatlite.Lambda[0] = M_GalieoLamtak(L1Type);
            }
            if(L2Type >= 0)
            {
                oneSatlite.Frq[1] = g_GalieoFrq[L2Type];
                oneSatlite.Lambda[1] = M_GalieoLamtak(L2Type);
            }
            if(L3Type >= 0)
            {
                oneSatlite.Frq[2] = g_GalieoFrq[L3Type];
                oneSatlite.Lambda[2] = M_GalieoLamtak(L3Type);
            }
            if(L4Type >= 0)
            {
                oneSatlite.Frq[3] = g_GalieoFrq[L4Type];
                oneSatlite.Lambda[3] = M_GalieoLamtak(L4Type);
            }
            if(L5Type >= 0)
            {
                oneSatlite.Frq[4] = g_GalieoFrq[L5Type];
                oneSatlite.Lambda[4] = M_GalieoLamtak(L5Type);
            }
        }

    }

}

/*Get the type of observation (version greater than 3)*/
void rinex_obs::getObsType(SatlitData &oneSatlite)
{
    QString satType(oneSatlite.SatType);
    if(RinexVersion >= 3)
    {
        for(int i = 0;i < m_obsVarNamesVer3.length();i++)
        {
            obsVarNamesVer3 tempObsVar= m_obsVarNamesVer3.at(i);
            if(tempObsVar.SatType.contains(satType))
            {
                oneSatlite.obserType = tempObsVar.obsNames3ver;
                break;
            }
        }
    }
}
/*Read Rinec 3.X observation files (read all observations)*/
void rinex_obs::readEpochVer3(QVector< SatlitData > &epochData)
{
    if (!isReadHead) getHeadInf();
    tempLine = m_readOFileClass.readLine();
    /*Enter data area to read*/
    while (!m_readOFileClass.atEnd())
    {/*Read data segment*/
        if (0 != tempLine.mid(0,1).compare(">"))
        {
            tempLine = m_readOFileClass.readLine();
            continue;
        }
        /*Read header data*/
        SatlitData oneSatlit;/*One satellite data*/
        int epochStalitNum = 0;
        oneSatlit.UTCTime.Year = tempLine.mid(1,5).trimmed().toInt();
        oneSatlit.UTCTime.Month = tempLine.mid(6,3).trimmed().toInt();
        oneSatlit.UTCTime.Day = tempLine.mid(9,3).trimmed().toInt();
        oneSatlit.UTCTime.Hours = tempLine.mid(12,3).trimmed().toInt();
        oneSatlit.UTCTime.Minutes = tempLine.mid(15,3).trimmed().toInt();
        oneSatlit.UTCTime.Seconds = tempLine.mid(18,11).trimmed().toDouble();
        oneSatlit.EpochFlag = tempLine.mid(30,2).trimmed().toInt();
        epochStalitNum = tempLine.mid(32,3).trimmed().toInt();/*Number of satellites in this epoch*/
        /*Get the receiver clock deviation unit: s*/
        double rec_off = tempLine.mid(40,16).toDouble();
        /*The current epoch data has an error*/
        if(oneSatlit.UTCTime.Year == 0 || oneSatlit.EpochFlag > 1)
            return ;
        /*Reading satellite data */
        char tempSatType = '0';
        QString dataString = "";
        for (int i = 0;i < epochStalitNum;i++)
        {
            tempLine = m_readOFileClass.readLine();
            tempSatType = *(tempLine.mid(0,1).toLatin1().data());/*Types of*/
            if (isInSystem(tempSatType))
            {
                oneSatlit.SigInten = -1;/*set -1 at 2018.07.30 store signal intensity*/
                oneSatlit.LL1_LOCK = -1;
                oneSatlit.BDS_2flag = false;
                oneSatlit.PRN = tempLine.mid(1,2).toInt();
                if(tempSatType=='C'&&oneSatlit.PRN<=18)
                    oneSatlit.BDS_2flag = true;/*Types of*/
                oneSatlit.SatType = tempSatType;
                oneSatlit.obserType.clear();
                oneSatlit.obserValue.clear();
                oneSatlit.obsLL1_LOCK.clear();
                oneSatlit.obsSigInten.clear();
                oneSatlit.wantObserType.clear();
                getObsType(oneSatlit);/*Get the type of observation*/
                dataString = tempLine.mid(3);
                getOneSatlitData(dataString,oneSatlit);/*Obtain satellite ranging code and carrier data*/
                getWantData_3(oneSatlit);/*Get the data you need*/
                getFrequency(oneSatlit);/*Acquisition frequency*/
                epochData.append(oneSatlit);/*Save a satellite in the epoch*/

            }
        }

           break;
     }
}

/*Read Rixin 2.X observation files (read all observations)*/
void rinex_obs::readEpochVer2(QVector< SatlitData > &epochData)
{
    if (!isReadHead) getHeadInf();
    tempLine = m_readOFileClass.readLine();
    /*Enter data area to read*/
    while (!m_readOFileClass.atEnd())
    {/*Read data segment*/
        if (tempLine.mid(18,1) != ".")
        {
            tempLine = m_readOFileClass.readLine();
            continue;
        }
        /*Read header data*/
        SatlitData oneSatlit;
        int epochStalitNum = 0;
        oneSatlit.UTCTime.Year = tempLine.mid(0,3).trimmed().toInt() + baseYear;
        oneSatlit.UTCTime.Month = tempLine.mid(3,3).trimmed().toInt();
        oneSatlit.UTCTime.Day = tempLine.mid(6,3).trimmed().toInt();
        oneSatlit.UTCTime.Hours = tempLine.mid(9,3).trimmed().toInt();
        oneSatlit.UTCTime.Minutes = tempLine.mid(12,3).trimmed().toInt();
        oneSatlit.UTCTime.Seconds = tempLine.mid(15,11).trimmed().toDouble();
        oneSatlit.EpochFlag = tempLine.mid(26,3).trimmed().toInt();
        epochStalitNum = tempLine.mid(29,3).trimmed().toInt();/*Number of satellites in this epoch*/
        oneSatlit.obserType = m_ObservVarsNames_2;/*All satellites initialize the same type of observation*/
        /*Get the receiver clock deviation unit: s*/
        double rec_off = tempLine.mid(68,12).toDouble();
        /*The current epoch data has an error*/
        if(oneSatlit.UTCTime.Year == 0 || oneSatlit.EpochFlag > 1)
            return ;
        /*Judge the situation (first parse the header file)*/
        int headHang = epochStalitNum/12,headHangLast = epochStalitNum%12,tempPrn = 0;
        QVector< char > saveType;/*Save the parsed header file type*/
        QVector< int > savePRN;/*Save the parsed PRN*/
        QString tempSatName = "";/*The name of a satellite*/
        char tempCh = 'G';
        for(int i = 0;i < headHang;i++)
        {
            for(int j = 0;j < 12;j++)
            {/*Store up to 12 satellites in a row*/
                tempSatName = tempLine.mid(32+j*3,3);
                tempCh = *(tempSatName.mid(0,1).toLatin1().data());
                if(tempCh == 32) tempCh = 'G';
                tempPrn = tempSatName.mid(1,2).trimmed().toInt();
                saveType.append(tempCh);
                savePRN.append(tempPrn);
            }
            tempLine = m_readOFileClass.readLine();
        }
        for(int i = 0;i < headHangLast;i++)
        {/*Read a satellite with less than one line (including the first line is not enough)*/
            tempSatName = tempLine.mid(32+i*3,3);
            tempCh = *(tempSatName.mid(0,1).toLatin1().data());
            if(tempCh == 32) tempCh = 'G';
            tempPrn = tempSatName.mid(1,2).trimmed().toInt();
            saveType.append(tempCh);
            savePRN.append(tempPrn);
        }
        if (headHangLast > 0)
        {/*Read the remaining incoming data rows, different from the integer rows*/
            tempLine = m_readOFileClass.readLine();
        }

        /*Reading satellite data*/
        QString dataString = "";
        /*The number of rows of observation data is equivalent to the ceil (TypeObservNum) function*/
        int dataHang = (m_TypeObservNum_2/5.0 - m_TypeObservNum_2/5 == 0)?(m_TypeObservNum_2/5):(m_TypeObservNum_2/5 + 1);
        for (int i = 0;i < epochStalitNum;i++)
        {
            tempCh = saveType.at(i);
            tempPrn = savePRN.at(i);
            oneSatlit.SatType = tempCh;/*Types of*/
            oneSatlit.PRN = tempPrn;/*PRN*/
            dataString = tempLine;/*Save a row of data*/
            dataString.replace("\n", "");
            while(dataString.length() < 80)
                dataString.append(" ");
            for (int j = 0;j < dataHang - 1;j++)/*Read extra rows of data*/
            {
                QString tempLine80 = m_readOFileClass.readLine();
                tempLine80.replace("\n", "");
                while(tempLine80.length() < 80)
                    tempLine80.append(" ");
                dataString += tempLine80;
            }
            if (isInSystem(tempCh))
            {
                oneSatlit.LL1_LOCK = -1;
                oneSatlit.SigInten = -1;
                oneSatlit.wantObserType.clear();
                oneSatlit.obserValue.clear();
                oneSatlit.obsLL1_LOCK.clear();
                oneSatlit.obsSigInten.clear();
                getOneSatlitData(dataString,oneSatlit);/*Obtain satellite ranging code and carrier data*/
                getWantData_2(oneSatlit);/*Get the data you need*/
                getFrequency(oneSatlit);/*Acquisition frequency*/
                epochData.append(oneSatlit);/*Save a satellite in the epoch*/
            }
            if (i  != epochStalitNum - 1)
            {/*Do not read the last satellite, leave it as the top of this function to read*/
                tempLine = m_readOFileClass.readLine();
            }
        }
        break;
    }
}

/*Use less than 3, get obsType observations(obsType: Rinex2:C1 P1 L1 L2)*/
QString rinex_obs::getObsTypeData(SatlitData &oneSatlit, QString obsType, double &obsValue)
{
    int obsLen = oneSatlit.obserType.length();
    QString satObsType = "";
    obsValue = 0;
    for(int i = 0;i < obsLen;i++)
    {
        satObsType = oneSatlit.obserType.at(i);
        if(0 == satObsType.compare(obsType))
        {
            obsValue = oneSatlit.obserValue.at(i);
            return satObsType;
        }
    }
    return satObsType;
}

void rinex_obs::ProcessCLPos(obsVarNamesVer3 epochSystem)
{
    CLPos ObsTypePos;
    QVector< QString > obsType = epochSystem.obsNames3ver;
    int prioLen = -1;
    /*init data*/
    QString frqenceStr[5] = {"0", "0", "0", "0", "0"};/*store frqence number*/
    QString prioArry[8] = {"0", "0", "0", "0", "0", "0", "0", "0"};
    ObsTypePos.SatType = epochSystem.SatType;
    /*juge satalite system*/
    if(epochSystem.SatType.contains("G", Qt::CaseInsensitive))
    {/*GPS 1 2 5 frqence*/
        frqenceStr[0] = "1"; frqenceStr[1] = "2"; frqenceStr[2] = "5";
        prioLen = 8;
        prioArry[0] = "C"; prioArry[1] = "W"; prioArry[2] = "P";
        prioArry[3] = "I"; prioArry[4] = "L"; prioArry[5] = "S";
        prioArry[6] = "Q"; prioArry[7] = "X";
    }
    else if(epochSystem.SatType.contains("R", Qt::CaseInsensitive))
    {/*GLONASS  C1C C2C*/
        frqenceStr[0] = "1"; frqenceStr[1] = "2"; frqenceStr[2] = "3";
        prioLen = 5;
        prioArry[0] = "P"; prioArry[1] = "C"; prioArry[2] = "Q";
        prioArry[3] = "X"; prioArry[4] = "I";
    }
    else if(epochSystem.SatType.contains("C", Qt::CaseInsensitive))
    {/*BDS 1 7 6 or 2*/
        frqenceStr[0] = "2"; frqenceStr[1] = "6"; frqenceStr[2] = "7"; frqenceStr[3] = "1";frqenceStr[4] = "5";
        prioLen = 5;
        prioArry[0] = "I"; prioArry[1] = "Q"; prioArry[2] = "X";prioArry[3] = "P";prioArry[4] = "D";
    }
    else if(epochSystem.SatType.contains("E", Qt::CaseInsensitive))
    {/*Galileo 1 5 6 7 8*/
        frqenceStr[0] = "1"; frqenceStr[1] = "5";frqenceStr[2] = "6"; frqenceStr[3] = "7";frqenceStr[4] = "8";/*frqenceStr[3] = "8";*/
        prioLen = 6;
        prioArry[0] = "C"; prioArry[1] = "Q";prioArry[2] = "X";prioArry[3] = "B";prioArry[4] = "I";
    }

    /*search System obsType*/
    for(int i = 0; i < prioLen;i++)
    {

        QString C1i = "C" + frqenceStr[0] + prioArry[i], L1i = "L" + frqenceStr[0] + prioArry[i],S1i = "S" + frqenceStr[0] + prioArry[i],
                C2i = "C" + frqenceStr[1] + prioArry[i], L2i = "L" + frqenceStr[1] + prioArry[i],S2i = "S" + frqenceStr[1] + prioArry[i],
                C3i = "C" + frqenceStr[2] + prioArry[i], L3i = "L" + frqenceStr[2] + prioArry[i],S3i = "S" + frqenceStr[2] + prioArry[i],
                C4i = "C" + frqenceStr[3] + prioArry[i], L4i = "L" + frqenceStr[3] + prioArry[i],S4i = "S" + frqenceStr[3] + prioArry[i],
                C5i = "C" + frqenceStr[4] + prioArry[i], L5i = "L" + frqenceStr[4] + prioArry[i],S5i = "S" + frqenceStr[4] + prioArry[i];

          int tempPos1 = -1, tempPos2 = -1,tempPos3 = -1;
          /*frequence 1*/
          /*C1 L1*/
           tempPos1 = obsType.indexOf(C1i);/*find pos of C1P etc*/
           tempPos2 = obsType.indexOf(L1i);/*find pos of L1P etc*/
           tempPos3 = obsType.indexOf(S1i);/*find pos of S1P etc*/
          if(tempPos1 != -1 && tempPos2 != -1)
           {
                 ObsTypePos.C1Type.append(C1i);
                 ObsTypePos.C1Pos.append(tempPos1);
               /*L1*/
                 ObsTypePos.L1Type.append(L1i);
                 ObsTypePos.L1Pos.append(tempPos2);
                 /*Si*/
                 ObsTypePos.S1Type.append(S1i);
                 ObsTypePos.S1Pos.append(tempPos3);
            }
           /*frequence 2*/
           /*C2 L2*/
           tempPos1 = obsType.indexOf(C2i);/*find pos of C2P etc*/
           tempPos2 = obsType.indexOf(L2i);/*find pos of L2P etc*/
           tempPos3 = obsType.indexOf(S2i);/*find pos of S2P etc*/
           if(tempPos1 != -1 && tempPos2 != -1)
           {
                  ObsTypePos.C2Type.append(C2i);
                  ObsTypePos.C2Pos.append(tempPos1);
                  /*L2*/
                  ObsTypePos.L2Type.append(L2i);
                  ObsTypePos.L2Pos.append(tempPos2);
                  /*Si*/
                  ObsTypePos.S2Type.append(S2i);
                  ObsTypePos.S2Pos.append(tempPos3);
            }
            /*frequence 3*/
            /*C3 L3*/
            tempPos1 = obsType.indexOf(C3i);/*find pos of C3P etc*/
            tempPos2 = obsType.indexOf(L3i);/*find pos of L3P etc*/
            tempPos3 = obsType.indexOf(S3i);/*find pos of S3P etc*/
            if(tempPos1 != -1 && tempPos2 != -1)
             {
                   ObsTypePos.C3Type.append(C3i);
                   ObsTypePos.C3Pos.append(tempPos1);
                    /*L2*/
                   ObsTypePos.L3Type.append(L3i);
                   ObsTypePos.L3Pos.append(tempPos2);
                   /*Si*/
                   ObsTypePos.S3Type.append(S3i);
                   ObsTypePos.S3Pos.append(tempPos3);
             }
             /*frequence 4*/
             /*C4 L4*/
             tempPos1 = obsType.indexOf(C4i);/*find pos of C4P etc*/
             tempPos2 = obsType.indexOf(L4i);/*find pos of L4P etc*/
             tempPos3 = obsType.indexOf(S4i);/*find pos of S4P etc*/
             if(tempPos1 != -1 && tempPos2 != -1)
             {
                 ObsTypePos.C4Type.append(C4i);
                 ObsTypePos.C4Pos.append(tempPos1);
                 /*L2*/
                 ObsTypePos.L4Type.append(L4i);
                 ObsTypePos.L4Pos.append(tempPos2);
                 /*Si*/
                 ObsTypePos.S4Type.append(S4i);
                 ObsTypePos.S4Pos.append(tempPos3);
              }
             /*frequence 5*/
             /*C5 L5*/
             tempPos1 = obsType.indexOf(C5i);/*find pos of C5P etc*/
             tempPos2 = obsType.indexOf(L5i);/*find pos of L5P etc*/
             tempPos3 = obsType.indexOf(S5i);/*find pos of S5P etc*/
             if(tempPos1 != -1 && tempPos2 != -1)
             {
                 ObsTypePos.C5Type.append(C5i);
                 ObsTypePos.C5Pos.append(tempPos1);
                 /*L2*/
                 ObsTypePos.L5Type.append(L5i);
                 ObsTypePos.L5Pos.append(tempPos2);
                 /*Si*/
                 ObsTypePos.S5Type.append(S5i);
                 ObsTypePos.S5Pos.append(tempPos3);
              }

    }
    /*Set the PPP dual-frequency observation type*/
    QStringList wantList;
    for(int i = 0; i < m_wantObsTypeVct.length();i++){
        /*find want systeam type*/
        QStringList tempList = m_wantObsTypeVct.at(i);
        QString sysStr;
        if(tempList.length() > 0) sysStr = tempList.at(0);
        if(epochSystem.SatType.contains(sysStr, Qt::CaseInsensitive)){
            wantList = tempList; break;
        }
    }
    if(wantList.length() >= 2)
    {/*set observation type*/
        QString L1Type = wantList.at(1);
        QString C1i = "C" + L1Type.mid(1,2), L1i = "L" + L1Type.mid(1,2);
        int tempPos1 = -1, tempPos2 = -1;
        /*set C1 and L1*/
        tempPos1 = obsType.indexOf(C1i); tempPos2 = obsType.indexOf(L1i);// find pos of C1P L1P etc
        ObsTypePos.C1Type.clear(); ObsTypePos.C1Pos.clear();
        ObsTypePos.L1Type.clear(); ObsTypePos.L1Pos.clear();
        if(tempPos1 != -1){
            ObsTypePos.C1Type.append(C1i);
            ObsTypePos.C1Pos.append(tempPos1);
        }
        if(tempPos2 != -1){
            ObsTypePos.L1Type.append(L1i);
            ObsTypePos.L1Pos.append(tempPos2);
        }
    }
    if(wantList.length() >= 3)
    {
        QString L2Type = wantList.at(2);
        QString C2i = "C" + L2Type.mid(1,2), L2i = "L" + L2Type.mid(1,2);
        int tempPos1 = -1, tempPos2 = -1;
        /*set C2 and L2*/
        tempPos1 = obsType.indexOf(C2i); tempPos2 = obsType.indexOf(L2i);// find pos of C1P L1P etc
        ObsTypePos.C2Type.clear(); ObsTypePos.C2Pos.clear();
        ObsTypePos.L2Type.clear(); ObsTypePos.L2Pos.clear();
        if(tempPos1 != -1){
            ObsTypePos.C2Type.append(C2i);
            ObsTypePos.C2Pos.append(tempPos1);
        }
        if(tempPos2 != -1){
            ObsTypePos.L2Type.append(L2i);
            ObsTypePos.L2Pos.append(tempPos2);
        }
    }
    /*juge some system maybe single frequence*/
    if(isInSystem(*(ObsTypePos.SatType.toLatin1().data())) && (ObsTypePos.C1Pos.length()*ObsTypePos.C2Pos.length() == 0))
    {
        QString warning_str = "Please check >>>SYS / # / OBS TYPES<<<." + ENDLINE
                + "You select system maybe single frequence!"+ ENDLINE + "System ( " + ObsTypePos.SatType
                + " ) maybe single frequence";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(warning_str);
        text_disp->data_lenth++;
    }
    m_allObsTypePos.append(ObsTypePos);
}

/*Sort observations by priority (2018.08.02) Version is greater than or equal to 3*/
/*The observation type is sorted according to P, W, C, X, S priority,
 *  that is, the end of
 * the observation value (for example: C2P, C2W, C2C, C2X, C2S)*/
void rinex_obs::PriorityRanking()
{
    for(int i = 0;i < m_obsVarNamesVer3.length();i++)
    {
        obsVarNamesVer3 epochSystem = m_obsVarNamesVer3.at(i);
        ProcessCLPos(epochSystem);
    }
}

/*Get the data you need (my PPP needs pseudorange and carrier,
 *  please change this function if you need other data)*/
void rinex_obs::getWantData_2(SatlitData &oneSatlit)
{
    /*init 8 QString*/
    for(int i = 0;i < 8;i++)
        oneSatlit.wantObserType.append("");
    /*get obs data store Carrier and Pesudorange*/
    double obsValue = 0;
    /*get Frenquce 1 best*/
    /*C1*/
    getObsTypeData(oneSatlit, "P1", obsValue);
    if(0 != obsValue)
    {
        oneSatlit.CodeP[0] = obsValue;
        oneSatlit.wantObserType[0] = "P1";
    }
    else
    {
        getObsTypeData(oneSatlit, "C1", obsValue);
        oneSatlit.CodeP[0] = obsValue;
        oneSatlit.wantObserType[0] = "C1";
    }
    /*L1*/
    getObsTypeData(oneSatlit, "L1", obsValue);
    oneSatlit.PhaseL[0] = obsValue;
    oneSatlit.wantObserType[1] = "L1";


    /*get Frenquce 2 best*/
    /*C2*/
    getObsTypeData(oneSatlit, "P2", obsValue);
    if(0 != obsValue)
    {
        oneSatlit.CodeP[1] = obsValue;
        oneSatlit.wantObserType[2] = "P2";
    }
    else
    {
        getObsTypeData(oneSatlit, "C2", obsValue);
        oneSatlit.CodeP[1] = obsValue;
        oneSatlit.wantObserType[2] = "C2";
    }
    /*L2*/
    getObsTypeData(oneSatlit, "L2", obsValue);
    oneSatlit.PhaseL[1] = obsValue;
    oneSatlit.wantObserType[3] = "L2";

    /*get Frenquce 3 best*/
    /*C3*/
    getObsTypeData(oneSatlit, "P3", obsValue);
    if(0 != obsValue)
    {
        oneSatlit.CodeP[2] = obsValue;
        oneSatlit.wantObserType[4] = "P3";
    }
    else
    {
        getObsTypeData(oneSatlit, "C3", obsValue);
        oneSatlit.CodeP[2] = obsValue;
        oneSatlit.wantObserType[4] = "C3";
    }
    /*L3*/
    getObsTypeData(oneSatlit, "L3", obsValue);
    oneSatlit.PhaseL[2] = obsValue;
    oneSatlit.wantObserType[5] = "L3";

    /*get Frenquce 4 best*/
    /*C4*/
    getObsTypeData(oneSatlit, "P4", obsValue);
    if(0 != obsValue)
    {
        oneSatlit.CodeP[3] = obsValue;
        oneSatlit.wantObserType[6] = "P4";
    }
    else
    {
        getObsTypeData(oneSatlit, "C4", obsValue);
        oneSatlit.CodeP[3] = obsValue;
        oneSatlit.wantObserType[6] = "C4";
    }
    /*L4*/
    getObsTypeData(oneSatlit, "L4", obsValue);
    oneSatlit.PhaseL[3] = obsValue;
    oneSatlit.wantObserType[7] = "L4";
}

/*Get the data you need (my PPP needs pseudorange and
 * carrier, please change this function if you need other data)*/
/*The main idea: read C2P first, if C2P is 0, read C2W, and then recursively*/
void rinex_obs::getWantData_3(SatlitData &oneSatlit)
{

            corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
            /*all obstype number (C1, C2, C3, C4, L1, L2, L3, L4)*/
            int obsNumber = 10;
            for(int i = 0;i < obsNumber;i++)
                oneSatlit.wantObserType.append("");
            int frequence_flag[5];
            for(int i=0;i<5;i++)
            {
                if(opt_corr->Combination_mode==Comb_UD_mode)
                {
                    if(oneSatlit.SatType=='G')
                        frequence_flag[i] = opt_corr->Frequently_GPS[i];
                    else if(oneSatlit.SatType=='C')
                        frequence_flag[i] = opt_corr->Frequently_BDS[i];
                    else if(oneSatlit.SatType=='E')
                        frequence_flag[i] = opt_corr->Frequently_GAL[i];
                    else if(oneSatlit.SatType=='R')
                        frequence_flag[i] = opt_corr->Frequently_GLO[i];
                }
                else
                {
                    frequence_flag[i]=1;
                }
            }
            /*find prioData*/
            CLPos prioData;
            for(int i = 0;i < m_allObsTypePos.length(); i++)
            {
                char ch_temp1 = *(m_allObsTypePos.at(i).SatType.toLatin1().data()),
                     ch_temp2 = oneSatlit.SatType;
                if(ch_temp2  == ch_temp1)
                {
                    prioData = m_allObsTypePos.at(i);
                    break;
                }
            }
    /*The purpose of using a for loop is to replace the idea between channels of the same frequency
     * (ie, reading C2W data missing, replacing with C2X;
     * L2W missing using L2X instead will produce a weekly jump)
     * be used as "substitute ideas"*/

            double SNR_value=opt_corr->SNR;
            double obsValue = 0,Signal_strength = 0;
            int obs_LL1_LOCK=0;
            for(int i=0;i<oneSatlit.obsLL1_LOCK.length();i++)
            {
                obs_LL1_LOCK = oneSatlit.obsLL1_LOCK.at(i);
                if(obs_LL1_LOCK==1)
                    return;
            }
            int flag = 0;int Sig_flag = 0;int i =0;
            if(opt_corr->Combination_mode==Comb_UD_mode)
            {
                for(int j=0;j<5;j++)
                {
                    /*get Frenquce 1 best*/
                    /*C1*/
                    switch(frequence_flag[j])
                    {
                    case 1:
                        for(i = 0; i < prioData.C1Pos.length();i++)
                        {

                            flag = prioData.C1Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S1Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2] = prioData.C1Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.CodeP[j] = obsValue;

                                break;
                            }
                        }
                        /*L1*/
                        for(i = 0; i < prioData.L1Pos.length();i++)
                        {
                            flag = prioData.L1Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S1Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2+1] = prioData.L1Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.PhaseL[j] = obsValue;

                                break;
                            }
                        }
                        break;
                    case 2:
                        /*C2*/
                        for(i = 0; i < prioData.C2Pos.length();i++)
                        {
                            flag = prioData.C2Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S2Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2] = prioData.C2Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.CodeP[j] = obsValue;


                                break;
                            }
                        }
                        /*L2*/
                        for(i = 0; i < prioData.L2Pos.length();i++)
                        {
                            flag = prioData.L2Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S2Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2+1] = prioData.L2Type.at(i);/*SZG 24-11-30 获取第一频率*/
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.PhaseL[j] = obsValue;

                                break;
                            }
                        }
                        break;
                    case 3:
                        /*get Frenquce 3 best*/
                        obsValue = 0;
                        /*C3*/
                        for(i = 0; i < prioData.C3Pos.length();i++)
                        {
                            flag = prioData.C3Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S3Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2] = prioData.C3Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.CodeP[j] = obsValue;

                                break;
                            }
                        }
                        /*L3*/
                        for(i = 0; i < prioData.L3Pos.length();i++)
                        {
                            flag = prioData.L3Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S3Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2+1] = prioData.L3Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.PhaseL[j] = obsValue;

                                break;
                            }
                        }
                        break;
                    case 4:
                        /*get Frenquce 4 best*/
                        obsValue = 0;
                        /*C4*/
                        for(i = 0; i < prioData.C4Pos.length();i++)
                        {
                            flag = prioData.C4Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S4Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2] = prioData.C4Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.CodeP[j] = obsValue;

                                break;
                            }

                        }
                        /*L4*/
                        for(i = 0; i < prioData.L4Pos.length();i++)
                        {
                            flag = prioData.L4Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S4Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2+1] = prioData.L4Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.PhaseL[j] = obsValue;

                                break;
                            }
                        }
                        break;
                    case 5:
                        /*get Frenquce 5 best*/
                        obsValue = 0;
                        /*C5*/
                        for(i = 0; i < prioData.C5Pos.length();i++)
                        {
                            flag = prioData.C5Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S5Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2] = prioData.C5Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.CodeP[j] = obsValue;

                                break;
                            }
                        }
                        /*L5*/
                        for(i = 0; i < prioData.L5Pos.length();i++)
                        {
                            flag = prioData.L5Pos.at(i);
                            obsValue = oneSatlit.obserValue.at(flag);
                            Sig_flag = prioData.S5Pos.at(i);
                            Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                            oneSatlit.wantObserType[j*2+1] = prioData.L5Type.at(i);
                            if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
                            {
                                oneSatlit.PhaseL[j] = obsValue;

                                break;
                            }
                        }
                        break;
                    }

                }
            }
            else
            {
                /*C1*/
                for(int i = 0; i < prioData.C1Pos.length();i++)
                {

                    int flag = prioData.C1Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S1Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[0]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.CodeP[frequence_flag[0]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[0]-1)*2] = prioData.C1Type.at(i);
                        }
                        else
                        {
                            oneSatlit.CodeP[0] = obsValue;
                            oneSatlit.wantObserType[0] = prioData.C1Type.at(i);
                        }
                        break;
                    }
    //                else
    //                    break;
                }
                /*L1*/
                for(int i = 0; i < prioData.L1Pos.length();i++)
                {
                    int flag = prioData.L1Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S1Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[0]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.PhaseL[frequence_flag[0]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[0]-1)*2+1] = prioData.L1Type.at(i);
                        }
                        else
                        {
                            oneSatlit.PhaseL[0] = obsValue;
                            oneSatlit.wantObserType[1] = prioData.L1Type.at(i);
                        }

                        break;
                    }
    //                else
    //                    break;
                }
                /*get Frenquce 2 best*/
                obsValue = 0;
                /*C2*/
                for(int i = 0; i < prioData.C2Pos.length();i++)
                {
                    int flag = prioData.C2Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S2Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[1]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.CodeP[frequence_flag[1]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[1]-1)*2] = prioData.C2Type.at(i);
                        }
                        else
                        {
                            oneSatlit.CodeP[1] = obsValue;
                            oneSatlit.wantObserType[2] = prioData.C2Type.at(i);
                        }

                        break;
                    }
    //                else
    //                    break;
                }
                /*L2*/
                for(int i = 0; i < prioData.L2Pos.length();i++)
                {
                    int flag = prioData.L2Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S2Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[1]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.PhaseL[frequence_flag[1]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[1]-1)*2+1] = prioData.L2Type.at(i);
                        }
                        else
                        {
                            oneSatlit.PhaseL[1] = obsValue;
                            oneSatlit.wantObserType[3] = prioData.L2Type.at(i);
                        }
                        break;
                    }
    //                else
    //                    break;
                }
                /*get Frenquce 3 best*/
                obsValue = 0;
                /*C3*/
                for(int i = 0; i < prioData.C3Pos.length();i++)
                {
                    int flag = prioData.C3Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S3Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[2]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.CodeP[frequence_flag[2]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[2]-1)*2] = prioData.C3Type.at(i);
                        }
                        else
                        {
                            oneSatlit.CodeP[2] = obsValue;
                            oneSatlit.wantObserType[4] = prioData.C3Type.at(i);
                        }

                        break;
                    }
    //                else
    //                    break;
                }
                /*L3*/
                for(int i = 0; i < prioData.L3Pos.length();i++)
                {
                    int flag = prioData.L3Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S3Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[2]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.PhaseL[frequence_flag[2]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[2]-1)*2+1] = prioData.L3Type.at(i);
                        }
                        else
                        {
                            oneSatlit.PhaseL[2] = obsValue;
                            oneSatlit.wantObserType[5] = prioData.L3Type.at(i);
                        }
                        break;
                    }
    //                else
    //                    break;
                }
                /*get Frenquce 4 best*/
                obsValue = 0;
                /*C4*/
                for(int i = 0; i < prioData.C4Pos.length();i++)
                {
                    int flag = prioData.C4Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S4Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[3]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.CodeP[frequence_flag[3]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[3]-1)*2] = prioData.C4Type.at(i);
                        }
                        else
                        {
                            oneSatlit.CodeP[3] = obsValue;
                            oneSatlit.wantObserType[6] = prioData.C4Type.at(i);
                        }

                        break;
                    }
    //                else
    //                    break;
                }
                /*L4*/
                for(int i = 0; i < prioData.L4Pos.length();i++)
                {
                    int flag = prioData.L4Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S4Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[3]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.PhaseL[frequence_flag[3]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[3]-1)*2+1] = prioData.L4Type.at(i);
                        }
                        else
                        {
                            oneSatlit.PhaseL[3] = obsValue;
                            oneSatlit.wantObserType[7] = prioData.L4Type.at(i);
                        }
                        break;
                    }
    //                else
    //                    break;
                }
                /*get Frenquce 5 best*/
                obsValue = 0;
                /*C5*/
                for(int i = 0; i < prioData.C5Pos.length();i++)
                {
                    int flag = prioData.C5Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S5Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[4]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.CodeP[frequence_flag[4]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[4]-1)*2] = prioData.C5Type.at(i);
                        }
                        else
                        {
                            oneSatlit.CodeP[4] = obsValue;
                            oneSatlit.wantObserType[8] = prioData.C5Type.at(i);
                        }
                        break;
                    }
    //                else
    //                    break;
                }
                /*L5*/
                for(int i = 0; i < prioData.L5Pos.length();i++)
                {
                    int flag = prioData.L5Pos.at(i);
                    obsValue = oneSatlit.obserValue.at(flag);
                    int Sig_flag = prioData.S5Pos.at(i);
                    Signal_strength = oneSatlit.obserValue.at(Sig_flag);
                    if( frequence_flag[4]&&1 &&Signal_strength>SNR_value)
                    {
                        if(opt_corr->Combination_mode==Comb_UD_mode)
                        {
                            oneSatlit.PhaseL[frequence_flag[4]-1] = obsValue;
                            oneSatlit.wantObserType[(frequence_flag[4]-1)*2+1] = prioData.L5Type.at(i);
                        }
                        else
                        {
                            oneSatlit.PhaseL[4] = obsValue;
                            oneSatlit.wantObserType[9] = prioData.L5Type.at(i);
                        }
                        break;
                    }
    //                else
    //                    break;
                }
            }
//            for(int j=0;j<5;j++)
//            {
//                /*get Frenquce 1 best*/
//                /*C1*/
//                switch(frequence_flag[j])
//                {
//                case 1:
//                    for(i = 0; i < prioData.C1Pos.length();i++)
//                    {

//                        flag = prioData.C1Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S1Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.CodeP[j] = obsValue;
//                            oneSatlit.wantObserType[j*2] = prioData.C1Type.at(i);
//                            break;
//                        }
//                    }
//                    /*L1*/
//                    for(i = 0; i < prioData.L1Pos.length();i++)
//                    {
//                        flag = prioData.L1Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S1Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.PhaseL[j] = obsValue;
//                            oneSatlit.wantObserType[j*2+1] = prioData.L1Type.at(i);
//                            break;
//                        }
//                    }
//                    break;
//                case 2:
//                    /*C2*/
//                    for(i = 0; i < prioData.C2Pos.length();i++)
//                    {
//                        flag = prioData.C2Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S2Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.CodeP[j] = obsValue;
//                            oneSatlit.wantObserType[j*2] = prioData.C2Type.at(i);

//                            break;
//                        }
//                    }
//                    /*L2*/
//                    for(i = 0; i < prioData.L2Pos.length();i++)
//                    {
//                        flag = prioData.L2Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S2Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.PhaseL[j] = obsValue;
//                            oneSatlit.wantObserType[j*2+1] = prioData.L2Type.at(i);
//                            break;
//                        }
//                    }
//                    break;
//                case 3:
//                    /*get Frenquce 3 best*/
//                    obsValue = 0;
//                    /*C3*/
//                    for(i = 0; i < prioData.C3Pos.length();i++)
//                    {
//                        flag = prioData.C3Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S3Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.CodeP[j] = obsValue;
//                            oneSatlit.wantObserType[j*2] = prioData.C3Type.at(i);
//                            break;
//                        }
//                    }
//                    /*L3*/
//                    for(i = 0; i < prioData.L3Pos.length();i++)
//                    {
//                        flag = prioData.L3Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S3Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.PhaseL[j] = obsValue;
//                            oneSatlit.wantObserType[j*2+1] = prioData.L3Type.at(i);
//                            break;
//                        }
//                    }
//                    break;
//                case 4:
//                    /*get Frenquce 4 best*/
//                    obsValue = 0;
//                    /*C4*/
//                    for(i = 0; i < prioData.C4Pos.length();i++)
//                    {
//                        flag = prioData.C4Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S4Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.CodeP[j] = obsValue;
//                            oneSatlit.wantObserType[j*2] = prioData.C4Type.at(i);
//                            break;
//                        }

//                    }
//                    /*L4*/
//                    for(i = 0; i < prioData.L4Pos.length();i++)
//                    {
//                        flag = prioData.L4Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S4Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.PhaseL[j] = obsValue;
//                            oneSatlit.wantObserType[j*2+1] = prioData.L4Type.at(i);
//                            break;
//                        }
//                    }
//                    break;
//                case 5:
//                    /*get Frenquce 5 best*/
//                    obsValue = 0;
//                    /*C5*/
//                    for(i = 0; i < prioData.C5Pos.length();i++)
//                    {
//                        flag = prioData.C5Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S5Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.CodeP[j] = obsValue;
//                            oneSatlit.wantObserType[j*2] = prioData.C5Type.at(i);
//                            break;
//                        }
//                    }
//                    /*L5*/
//                    for(i = 0; i < prioData.L5Pos.length();i++)
//                    {
//                        flag = prioData.L5Pos.at(i);
//                        obsValue = oneSatlit.obserValue.at(flag);
//                        Sig_flag = prioData.S5Pos.at(i);
//                        Signal_strength = oneSatlit.obserValue.at(Sig_flag);
//                        if( frequence_flag[j]&&1 &&Signal_strength>SNR_value)
//                        {
//                            oneSatlit.PhaseL[j] = obsValue;
//                            oneSatlit.wantObserType[j*2+1] = prioData.L5Type.at(i);
//                            break;
//                        }
//                    }
//                    break;
//                }

//            }



}
/*Read an epoch (can be expanded by version)*/
void rinex_obs::getEpochData(QVector< SatlitData > &epochData)
{
    if (RinexVersion >= 3)
    {
        readEpochVer3(epochData);
    }
    else if (RinexVersion < 3)
    {
        readEpochVer2(epochData);
    }
    return ;
}

/*Read epochNum epoch data (premature to the bottom of the file may be less than epochNum)*/
void rinex_obs::getMultEpochData(QVector< QVector< SatlitData > >&multEpochData,int epochNum)
{
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();

    if (epochNum < 1) return ;
    int i = 0;
    while(!m_readOFileClass.atEnd())
    {
        if (i++ >= epochNum) break;
        QVector< SatlitData > epochData;
        getEpochData(epochData);
        if(epochData.length() > 0)
            multEpochData.append(epochData);
    }
    while(m_readOFileClass.atEnd()&&Serial_number<pravide_obs->mult_OfileName.length()-1)
    {
        Serial_number++;
        initObsVar();
        pravide_obs->m_OfileName = pravide_obs->mult_OfileName.at(Serial_number);
        /*Read file header information and parse*/
        getHeadInf();/*The file is not closed here*/
        baseYear = (int)(YMDHM[0]/100)*100;
        if (i++ >= epochNum) return ;
        while(!m_readOFileClass.atEnd())
        {
            if (i++ >= epochNum) break;
            QVector< SatlitData > epochData;
            getEpochData(epochData);
            if(epochData.length() > 0)
                multEpochData.append(epochData);
        }
    }
}
/*Determine if the end of the file is reached (end)*/
bool rinex_obs::isObsEnd()
{
    return m_readOFileClass.atEnd();
}
void rinex_obs::closeobsFile()
{
    if(!m_IsCloseFile) m_readOFileClass.close();
}

/*Get header file comment information*/
QString rinex_obs::getComment()
{
    return CommentInfo;
}
/*Get approximate coordinates*/
void rinex_obs::getApproXYZ(double* AppXYZ)
{
    for (int i = 0;i < 3;i++)
        AppXYZ[i] = ApproxXYZ[i];
}

/*Get the HEN correction of the antenna*/
void rinex_obs::getAntHEN(double* m_AntHEM)
{
    for (int i = 0;i < 3;i++)
        m_AntHEM[i] = AntHEN[i];
}
/*Obtain the initial observation epoch time*/
void rinex_obs::getFistObsTime(int* m_YMDHM,double &Seconds)
{
    Seconds = ObsSeconds;
    for (int i = 0;i < 5;i++)
        m_YMDHM[i] = YMDHM[i];
}
/*Get the antenna marker point name*/
QString rinex_obs::getMakerName()
{
    return MarkerName;
}
/*Obtain the observation interval (unit s)*/
double rinex_obs::getInterval()
{
    return IntervalSeconds;
}
/*Get the receiver antenna type*/
QString rinex_obs::getAntType()
{
    return AntType;
}
/*Get receiver type*/
QString rinex_obs::getReciveType()
{
    return ReciverType;
}

