#include "read_cnt_bia.h"

read_cnt_bia::read_cnt_bia()
{

}

void read_cnt_bia::setBIAFileNames(QString biaFileNames)
{
    initbiaVar();
    if(biaFileNames.contains("bia",Qt::CaseInsensitive))
        biafiles = biaFileNames;
    else
    {
        isReadbiaAllData = true;
        return ;
    }
    ref_Second = 0;
    code_ref_Second = 0;
    if(biaFileNames.contains("WUM0MGXRAP",Qt::CaseInsensitive))
        file_type = true;
    /*open a file*/
    m_readBIAFileClass.setFileName(biafiles);
    if (!m_readBIAFileClass.open(QFile::ReadOnly))
    {
        QString infor = "Open file bad!(read_cnt_bia::setBIAFileNames(QString biaFileNames))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
    }
}

void read_cnt_bia::initbiaVar()
{
    isReadbiaAllData = false;
    IsSigalbiaFile = false;
    file_type = false;
    dat_pos = 0;
    ref_flag = 1;
    getbiaFlag = 0;
    biadat_flag = false;
    tempbiaLine = "";
    biafiles = "";
    m_allEpochbiaData.clear();
    BIAData Biadat_init;
    Biadat_ref = Biadat_init;
    InitbiaDatStruct();
    BiaData = Biadat_ref;
}
void read_cnt_bia::InitbiaDatStruct()
{
    if (isInSystem('G'))
    {
        Biadat_ref.GPSbiaData.resize(MAXPRNGPS);
    }
    if (isInSystem('R'))
    {
        Biadat_ref.GLObiaData.resize(MAXPRNGLO);
    }
    if (isInSystem('C'))
    {
        Biadat_ref.BDSbiaData.resize(MAXPRNBDS);
    }
    if (isInSystem('E'))
    {
        Biadat_ref.GLEbiaData.resize(MAXPRNGLE);
    }
}
void read_cnt_bia::getbia_Dat(int obs_day,double Seconds_day,char SatType,int PRN,
                              double *CODE_BIA,double *PHASE_BIA,QVector< QString > wantObserType)
{
    if(!isReadbiaAllData)
    {
        int epochNum=1000;
        readbiaFilePartData(epochNum);
        isReadbiaAllData = true;
    }
    double diff_sec = 0;
    for(int i=dat_pos;i<m_allEpochbiaData.length();i++)
    {
        BIAData test_bia = m_allEpochbiaData.at(i);
        if(obs_day != test_bia.day_Y)
        {
            diff_sec = Seconds_day - test_bia.day_Seconds;
            break ;
        }
        if(Seconds_day - test_bia.day_Seconds>=0)
        {
            BiaData = m_allEpochbiaData.at(i);
            dat_pos = i;
            continue;
        }
        else if(Seconds_day - test_bia.day_Seconds<0)
            break;
    }

    if(dat_pos == m_allEpochbiaData.length() - 1&&Seconds_day - BiaData.day_Seconds>360&&m_allEpochbiaData.length()>1&&!m_readBIAFileClass.atEnd())
    {
        dat_pos = 0;
        isReadbiaAllData = false;
    }
    if(diff_sec>0&&diff_sec<86340)
        return;

    switch (SatType) {
    case 'G':
        for(int k=0;k<wantObserType.length();k=k+2)
        {
            if(wantObserType[k].contains("1C",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C1C;
            else if(wantObserType[k].contains("1P",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C1P;
            else if(wantObserType[k].contains("1W",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C1W;
            if(wantObserType[k+1].contains("L1",Qt::CaseInsensitive))
            {
                PHASE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].L1C;
            }
            if(wantObserType[k].contains("2L",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C2L;
            else if(wantObserType[k].contains("2S",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C2S;
            else if(wantObserType[k].contains("2W",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C2W;
            else if(wantObserType[k].contains("2X",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C2X;
            else if(wantObserType[k].contains("2P",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C2W;
            if(wantObserType[k+1].contains("L2",Qt::CaseInsensitive))
            {
                PHASE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].L2W;
            }
            if(wantObserType[k].contains("5Q",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C5Q;
            else if(wantObserType[k].contains("5X",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].C5X;
            if(wantObserType[k+1].contains("L5",Qt::CaseInsensitive))
            {
                PHASE_BIA[(int)k/2] = BiaData.GPSbiaData[PRN-1].L5I;
            }
        }
        CODE_BIA[4] = BiaData.GPSbiaData[PRN-1].C1W;
        break;
    case 'R':
        for(int k=0;k<wantObserType.size();k=k+2)
        {
            if(wantObserType[k].contains("1C",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLObiaData[PRN-1].C1C;
            }
            else if(wantObserType[k].contains("1P",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLObiaData[PRN-1].C1P;
            }
            else if(wantObserType[k].contains("2C",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLObiaData[PRN-1].C2C;
            }
            else if(wantObserType[k].contains("2P",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLObiaData[PRN-1].C2P;
            }
        }
        break;
    case 'E':
        for(int k=0;k<wantObserType.size();k=k+2)
        {
            if(wantObserType[k].contains("1C",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C1C;
                if(BiaData.GLEbiaData[PRN-1].L1C==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1X;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1C;
//                if(BiaData.GLEbiaData[PRN-1].C1C!=0)
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C1C;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1C;
//                }
//                else
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C1X;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1X;
//                }
            }
            else if(wantObserType[k].contains("5Q",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C5Q;
                if(BiaData.GLEbiaData[PRN-1].L5Q==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5X;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5Q;
//                if(BiaData.GLEbiaData[PRN-1].C5Q!=0)
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C5Q;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5Q;
//                }
//                else
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C5X;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5X;
//                }
            }
            else if(wantObserType[k].contains("6C",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6C;
                if(BiaData.GLEbiaData[PRN-1].L6C==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6X;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6C;
//                PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6C;
//                if(BiaData.GLEbiaData[PRN-1].C6C!=0)
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6C;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6C;
//                }
//                else
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6X;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6X;
//                }
            }
            else if(wantObserType[k].contains("6B",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6X;
                if(BiaData.GLEbiaData[PRN-1].L6X==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6C;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6X;
            }
            else if(wantObserType[k].contains("7Q",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C7Q;
                if(BiaData.GLEbiaData[PRN-1].L7Q==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L7X;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L7Q;
//                PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L7Q;
            }
            else if(wantObserType[k].contains("8Q",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C8Q;
                PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L8Q;
            }
            else if(wantObserType[k].contains("1X",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C1X;

                if(BiaData.GLEbiaData[PRN-1].L1X==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1C;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1X;
//                if(BiaData.GLEbiaData[PRN-1].C1X==0)
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C1C;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1C;
//                }
//                else
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C1X;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L1X;
//                }
            }
            else if(wantObserType[k].contains("5X",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C5X;
//                PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5X;
                if(BiaData.GLEbiaData[PRN-1].L5X==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5Q;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5X;
//                if(BiaData.GLEbiaData[PRN-1].C5X==0)
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C5Q;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5Q;
//                }
//                else
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C5X;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L5X;
//                }
            }
            else if(wantObserType[k].contains("6X",Qt::CaseInsensitive)||wantObserType[k].contains("6B",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6X;
                if(BiaData.GLEbiaData[PRN-1].L6X==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6C;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6X;
//                if(BiaData.GLEbiaData[PRN-1].C6X==0)
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6C;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6C;
//                }
//                else
//                {
//                    CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C6X;
//                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L6X;
//                }
            }
            else if(wantObserType[k].contains("7X",Qt::CaseInsensitive))
            {
                if(BiaData.GLEbiaData[PRN-1].L7X==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L7Q;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L7X;
//                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C7X;
//                PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L7X;
            }
            else if(wantObserType[k].contains("8X",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].C8X;
                if(BiaData.GLEbiaData[PRN-1].L8X==0)
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L8Q;
                else
                    PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L8X;
//                PHASE_BIA[(int)k/2] = BiaData.GLEbiaData[PRN-1].L8X;
            }
        }
        break;
    case 'C':
        for(int k=0;k<wantObserType.size();k=k+2)
        {
            if(wantObserType[k].contains("2I",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C2I;
            else if(wantObserType[k].contains("1P",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C1P;
//                if(BiaData.BDSbiaData[PRN-1].C1P==0)
//                  CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C1X;
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L1X;
            }
            else if(wantObserType[k].contains("1X",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C1X;
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L1X;
            }
            else if(wantObserType[k].contains("1I",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C2I;
            if(wantObserType[k+1].contains("2I",Qt::CaseInsensitive)
                    ||wantObserType[k+1].contains("1I",Qt::CaseInsensitive))
            {
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L2I;
            }
            if(wantObserType[k].contains("7I",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C7I;
            else if(wantObserType[k].contains("6I",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C6I;
            else if(wantObserType[k].contains("5P",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C5P;
//                if(BiaData.BDSbiaData[PRN-1].C5P==0)
//                    CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C5X;
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L5X;
            }
            else if(wantObserType[k].contains("5X",Qt::CaseInsensitive))
            {
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C5X;
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L5X;
            }
            else if(wantObserType[k].contains("8X",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C8X;
            else if(wantObserType[k].contains("7Z",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C7Z;
            else if(wantObserType[k].contains("7D",Qt::CaseInsensitive))
                CODE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].C7D;

            if(wantObserType[k+1].contains("7I",Qt::CaseInsensitive)||wantObserType[k+1].contains("7Z",Qt::CaseInsensitive)
                    ||wantObserType[k+1].contains("7D",Qt::CaseInsensitive))
            {
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L7I;
            }
            else if(wantObserType[k+1].contains("6I",Qt::CaseInsensitive))
            {
                PHASE_BIA[(int)k/2] = BiaData.BDSbiaData[PRN-1].L6I;
            }
        }
        break;
    default:
        break;
    }
}
void read_cnt_bia::closeBIAFile()
{
    m_readBIAFileClass.close();
}
/*Read part data from multiple files*/
void read_cnt_bia::readbiaFilePartData(int epochNum)
{
    if (isReadbiaAllData)
        return;
    m_allEpochbiaData.clear();
    int i = 0;
    while(!m_readBIAFileClass.atEnd())
    {
        if(i!=0&&file_type)
            break;
        if (i++ >= epochNum) break;
        BIAData BiaData = Biadat_ref;
        getbiaFlag = 0;

        /*Get the bia closest to the current observation time*/
        getBIAData(&BiaData);
        if(code_ref_Second==86400)
        {
            Biadat_ref=BiaData;
            code_ref_Second = 86390;
        }
        if(getbiaFlag == 0) break;
        /*Store bia data into array*/
        m_allEpochbiaData.append(BiaData);
    }
    if(file_type)
        getBIAData_WHU(BiaData);
}
void read_cnt_bia::getBIAData(BIAData *BiaData)
{
    while(!m_readBIAFileClass.atEnd())
    {
        while(!tempbiaLine.contains(" OSB  ",Qt::CaseInsensitive)
              &&!m_readBIAFileClass.atEnd())
        {
            tempbiaLine = m_readBIAFileClass.readLine();
            ref_flag = 0;
        }
        int day_Y=-1,day_Seconds=-1;
        ref_Second = tempbiaLine.mid(44,5).toInt();
        day_Y = tempbiaLine.mid(40,3).toInt();
        if(code_ref_Second==0)
        {
            code_ref_Second = tempbiaLine.mid(59,5).toInt();
        }
        day_Seconds = tempbiaLine.mid(44,5).toInt();
        while(((day_Seconds==ref_Second&&ref_flag == 0&&day_Y!=0)||file_type)&&!m_readBIAFileClass.atEnd())
        {
            if(tempbiaLine.mid(15,4) != "    ")
            {
                tempbiaLine = m_readBIAFileClass.readLine();
                continue;
            }
            getbiaFlag = 1;
            int PRN = tempbiaLine.mid(12,2).toInt();
            BiaData->day_Y = day_Y;
            BiaData->day_Seconds=day_Seconds;
            if (tempbiaLine.mid(11,1) == "G"&&isInSystem('G'))
            {
                /*GPS system*/
                if(tempbiaLine.mid(25,3)=="C1C")
                    BiaData->GPSbiaData[PRN-1].C1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1P")
                    BiaData->GPSbiaData[PRN-1].C1P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1W")
                    BiaData->GPSbiaData[PRN-1].C1W = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2S")
                    BiaData->GPSbiaData[PRN-1].C2S = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2L")
                    BiaData->GPSbiaData[PRN-1].C2L = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2X")
                    BiaData->GPSbiaData[PRN-1].C2X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2W")
                    BiaData->GPSbiaData[PRN-1].C2W = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5Q")
                    BiaData->GPSbiaData[PRN-1].C5Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5X")
                    BiaData->GPSbiaData[PRN-1].C5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1C")
                    BiaData->GPSbiaData[PRN-1].L1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L2W")
                    BiaData->GPSbiaData[PRN-1].L2W = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5I"||tempbiaLine.mid(25,3)=="L5Q")
                    BiaData->GPSbiaData[PRN-1].L5I = tempbiaLine.mid(69,22).toDouble();
            }
            else if(tempbiaLine.mid(11,1) == "R"&&isInSystem('R'))
            {
                /*GLO system*/
                if(tempbiaLine.mid(25,3)=="C1C")
                    BiaData->GLObiaData[PRN-1].C1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1P")
                    BiaData->GLObiaData[PRN-1].C1P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2C")
                    BiaData->GLObiaData[PRN-1].C2C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2P")
                    BiaData->GLObiaData[PRN-1].C2P = tempbiaLine.mid(69,22).toDouble();
            }
            else if(tempbiaLine.mid(11,1) == "E"&&isInSystem('E'))
            {
                /*GLE system*/
                if(tempbiaLine.mid(25,3)=="C1C")
                    BiaData->GLEbiaData[PRN-1].C1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5Q")
                    BiaData->GLEbiaData[PRN-1].C5Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7Q")
                    BiaData->GLEbiaData[PRN-1].C7Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C6C")
                    BiaData->GLEbiaData[PRN-1].C6C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C8Q")
                    BiaData->GLEbiaData[PRN-1].C8Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1C")
                    BiaData->GLEbiaData[PRN-1].L1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5Q")
                    BiaData->GLEbiaData[PRN-1].L5Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7Q")
                    BiaData->GLEbiaData[PRN-1].L7Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L6C")
                    BiaData->GLEbiaData[PRN-1].L6C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L8Q")
                    BiaData->GLEbiaData[PRN-1].L8Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1X")
                    BiaData->GLEbiaData[PRN-1].C1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5X")
                    BiaData->GLEbiaData[PRN-1].C5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C6X")
                    BiaData->GLEbiaData[PRN-1].C6X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7X")
                    BiaData->GLEbiaData[PRN-1].C7X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C8X")
                    BiaData->GLEbiaData[PRN-1].C8X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1X")
                    BiaData->GLEbiaData[PRN-1].L1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5X")
                    BiaData->GLEbiaData[PRN-1].L5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L6X")
                    BiaData->GLEbiaData[PRN-1].L6X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7X")
                    BiaData->GLEbiaData[PRN-1].L7X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L8X")
                    BiaData->GLEbiaData[PRN-1].L8X = tempbiaLine.mid(69,22).toDouble();
            }
            else if(tempbiaLine.mid(11,1) == "C"&&isInSystem('C'))
            {
                /*BDS system*/
                if(tempbiaLine.mid(25,3)=="C1P")
                    BiaData->BDSbiaData[PRN-1].C1P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1X")
                    BiaData->BDSbiaData[PRN-1].C1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5P")
                    BiaData->BDSbiaData[PRN-1].C5P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5X")
                    BiaData->BDSbiaData[PRN-1].C5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C8X")
                    BiaData->BDSbiaData[PRN-1].C8X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7Z")
                    BiaData->BDSbiaData[PRN-1].C7Z = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7D")
                    BiaData->BDSbiaData[PRN-1].C7D = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7I")
                    BiaData->BDSbiaData[PRN-1].C7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C6I")
                    BiaData->BDSbiaData[PRN-1].C6I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2I")
                    BiaData->BDSbiaData[PRN-1].C2I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7I")
                    BiaData->BDSbiaData[PRN-1].L7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7D")
                    BiaData->BDSbiaData[PRN-1].L7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7Z")
                    BiaData->BDSbiaData[PRN-1].L7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L2I")
                    BiaData->BDSbiaData[PRN-1].L2I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L6I")
                    BiaData->BDSbiaData[PRN-1].L6I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1X")
                    BiaData->BDSbiaData[PRN-1].L1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5X")
                    BiaData->BDSbiaData[PRN-1].L5X = tempbiaLine.mid(69,22).toDouble();
             }
            tempbiaLine = m_readBIAFileClass.readLine();
            if(tempbiaLine.contains("-BIAS/SOLUTION",Qt::CaseInsensitive))
                return;
            day_Y = tempbiaLine.mid(40,3).toInt();
            day_Seconds = tempbiaLine.mid(44,5).toInt();
        }
        return;
    }
}

void read_cnt_bia::getBIAData_WHU(BIAData &BiaData)
{
    closeBIAFile();
    if (!m_readBIAFileClass.open(QFile::ReadOnly))
    {
        QString infor = "Open file bad!(read_cnt_bia::setBIAFileNames(QString biaFileNames))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
    }
    tempbiaLine = m_readBIAFileClass.readLine();
    while(!m_readBIAFileClass.atEnd())
    {
        while(!tempbiaLine.contains(" OSB  ",Qt::CaseInsensitive)
              &&!m_readBIAFileClass.atEnd())
        {
            tempbiaLine = m_readBIAFileClass.readLine();
            ref_flag = 0;
        }
        int day_Y=-1,day_Seconds=-1;
        ref_Second = tempbiaLine.mid(44,5).toInt();//有效期开始时间秒
        day_Y = tempbiaLine.mid(40,3).toInt();
        code_ref_Second = tempbiaLine.mid(59,5).toInt();//有效期结束时间秒
        day_Seconds = tempbiaLine.mid(44,5).toInt();
        int m_pos = (int)(day_Seconds/900);
        if(m_pos<m_allEpochbiaData.length())
            BiaData = m_allEpochbiaData.at(m_pos);
        else if(m_pos==m_allEpochbiaData.length()&&m_pos!=0)
            BiaData = m_allEpochbiaData.at(0);

        while((day_Seconds==ref_Second&&ref_flag == 0&&day_Y!=0&&!m_readBIAFileClass.atEnd()))
        {
            if(tempbiaLine.mid(15,4) != "    ")
            {
                tempbiaLine = m_readBIAFileClass.readLine();
                continue;
            }
            getbiaFlag = 1;
            int PRN = tempbiaLine.mid(12,2).toInt();
            BiaData.day_Y = day_Y;
            BiaData.day_Seconds=day_Seconds;
            if (tempbiaLine.mid(11,1) == "G"&&isInSystem('G'))
            {
                /*GPS system*/
                if(tempbiaLine.mid(25,3)=="C1C")
                    BiaData.GPSbiaData[PRN-1].C1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1P")
                    BiaData.GPSbiaData[PRN-1].C1P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1W")
                    BiaData.GPSbiaData[PRN-1].C1W = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2S")
                    BiaData.GPSbiaData[PRN-1].C2S = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2L")
                    BiaData.GPSbiaData[PRN-1].C2L = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2X")
                    BiaData.GPSbiaData[PRN-1].C2X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2W")
                    BiaData.GPSbiaData[PRN-1].C2W = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5Q")
                    BiaData.GPSbiaData[PRN-1].C5Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5X")
                    BiaData.GPSbiaData[PRN-1].C5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1C")
                    BiaData.GPSbiaData[PRN-1].L1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L2W")
                    BiaData.GPSbiaData[PRN-1].L2W = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5I"||tempbiaLine.mid(25,3)=="L5Q")
                    BiaData.GPSbiaData[PRN-1].L5I = tempbiaLine.mid(69,22).toDouble();
            }
            else if(tempbiaLine.mid(11,1) == "R"&&isInSystem('R'))
            {
                /*GLO system*/
                if(tempbiaLine.mid(25,3)=="C1C")
                    BiaData.GLObiaData[PRN-1].C1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1P")
                    BiaData.GLObiaData[PRN-1].C1P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2C")
                    BiaData.GLObiaData[PRN-1].C2C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2P")
                    BiaData.GLObiaData[PRN-1].C2P = tempbiaLine.mid(69,22).toDouble();
            }
            else if(tempbiaLine.mid(11,1) == "E"&&isInSystem('E'))
            {
                /*GLE system*/
                if(tempbiaLine.mid(25,3)=="C1C")
                    BiaData.GLEbiaData[PRN-1].C1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5Q")
                    BiaData.GLEbiaData[PRN-1].C5Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7Q")
                    BiaData.GLEbiaData[PRN-1].C7Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C6C")
                    BiaData.GLEbiaData[PRN-1].C6C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C8Q")
                    BiaData.GLEbiaData[PRN-1].C8Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1C")
                    BiaData.GLEbiaData[PRN-1].L1C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5Q")
                    BiaData.GLEbiaData[PRN-1].L5Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7Q")
                    BiaData.GLEbiaData[PRN-1].L7Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L6C")
                    BiaData.GLEbiaData[PRN-1].L6C = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L8Q")
                    BiaData.GLEbiaData[PRN-1].L8Q = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1X")
                    BiaData.GLEbiaData[PRN-1].C1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5X")
                    BiaData.GLEbiaData[PRN-1].C5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C6X")
                    BiaData.GLEbiaData[PRN-1].C6X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7X")
                    BiaData.GLEbiaData[PRN-1].C7X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C8X")
                    BiaData.GLEbiaData[PRN-1].C8X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1X")
                    BiaData.GLEbiaData[PRN-1].L1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5X")
                    BiaData.GLEbiaData[PRN-1].L5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L6X")
                    BiaData.GLEbiaData[PRN-1].L6X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7X")
                    BiaData.GLEbiaData[PRN-1].L7X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L8X")
                    BiaData.GLEbiaData[PRN-1].L8X = tempbiaLine.mid(69,22).toDouble();
            }
            else if(tempbiaLine.mid(11,1) == "C"&&isInSystem('C'))
            {
                /*BDS system*/
                if(tempbiaLine.mid(25,3)=="C1P")
                    BiaData.BDSbiaData[PRN-1].C1P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C1X")
                    BiaData.BDSbiaData[PRN-1].C1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5P")
                    BiaData.BDSbiaData[PRN-1].C5P = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C5X")
                    BiaData.BDSbiaData[PRN-1].C5X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C8X")
                    BiaData.BDSbiaData[PRN-1].C8X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7Z")
                    BiaData.BDSbiaData[PRN-1].C7Z = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7D")
                    BiaData.BDSbiaData[PRN-1].C7D = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C7I")
                    BiaData.BDSbiaData[PRN-1].C7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C6I")
                    BiaData.BDSbiaData[PRN-1].C6I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="C2I")
                    BiaData.BDSbiaData[PRN-1].C2I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7I")
                    BiaData.BDSbiaData[PRN-1].L7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7D")
                    BiaData.BDSbiaData[PRN-1].L7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L7Z")
                    BiaData.BDSbiaData[PRN-1].L7I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L2I")
                    BiaData.BDSbiaData[PRN-1].L2I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L6I")
                    BiaData.BDSbiaData[PRN-1].L6I = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L1X")
                    BiaData.BDSbiaData[PRN-1].L1X = tempbiaLine.mid(69,22).toDouble();
                else if(tempbiaLine.mid(25,3)=="L5X")
                    BiaData.BDSbiaData[PRN-1].L5X = tempbiaLine.mid(69,22).toDouble();
             }
            tempbiaLine = m_readBIAFileClass.readLine();
            if(tempbiaLine.contains("-BIAS/SOLUTION",Qt::CaseInsensitive))
                return;
            day_Y = tempbiaLine.mid(40,3).toInt();
            day_Seconds = tempbiaLine.mid(44,5).toInt();
        }
        if(m_pos<m_allEpochbiaData.length())
            m_allEpochbiaData.replace(m_pos,BiaData);
        else if(m_pos==m_allEpochbiaData.length()&&m_pos!=0)
             m_allEpochbiaData.append(BiaData);
    }
}


/*Release all data*/
void read_cnt_bia::releaseBiaAllData()
{
    m_allEpochbiaData.clear();
}
