#include "read_orbex.h"

read_orbex::read_orbex()
{

}

void read_orbex::setOBXFileNames(QString obxFileNames)
{
    initobxVar();
    if(obxFileNames.contains("obx",Qt::CaseInsensitive))
        obxfiles =obxFileNames;
    else
    {
        isReadobxAllData = true;
        return ;
    }
    /*open a file*/
    m_readOBXFileClass.setFileName(obxfiles);
    if (!m_readOBXFileClass.open(QFile::ReadOnly))
    {
        QString erroInfo = "Open file bad!(read_orbex::setOBXFileNames(QString obxFileNames))";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
    }

}

void read_orbex::initobxVar()
{
    isReadobxAllData = false;
    IsSigalobxFile = false;
    obxfiles = "";
    obx_dat_pos = 0;
    getobx_flag = 0;
    m_allEpochobxData.clear();
    tempobxLine = "";
    OBXData Obxdat_init;
    Obxdat_ref = Obxdat_init;
    InitobxDatStruct();
    ObxData = Obxdat_ref;
//    m_obxFileNames = " ";

}
void read_orbex::InitobxDatStruct()
{
    if (isInSystem('G'))
    {
        Obxdat_ref.GPSobxData.resize(MAXPRNGPS);
    }
    if (isInSystem('R'))
    {
        Obxdat_ref.GLOobxData.resize(MAXPRNGLO);
    }
    if (isInSystem('C'))
    {
        Obxdat_ref.BDSobxData.resize(MAXPRNBDS);
    }
    if (isInSystem('E'))
    {
        Obxdat_ref.GLEobxData.resize(MAXPRNGLE);
    }
}

void read_orbex::closeOBXFile()
{
    m_readOBXFileClass.close();
}
/*Read part data from multiple files*/
void read_orbex::readobxFilePartData(int epochNum)
{
    if (isReadobxAllData)
        return;
    m_allEpochobxData.clear();
    int i = 0;
    while(!m_readOBXFileClass.atEnd())
    {
        if (i++ >= epochNum) break;
        OBXData ObxData = Obxdat_ref;
        getobx_flag = 0;
        /*Get the obx closest to the current observation time*/
        getOBXData(&ObxData);
        if(getobx_flag==0)  continue;
        /*Store SSR data into array*/
        m_allEpochobxData.append(ObxData);
    }
}
void read_orbex::getATT_Dat(gtime_t obs_T,char SatType,int PRN,MatrixXd &quater)
{
    if(!isReadobxAllData)
    {
        int epochNum=1000;
        readobxFilePartData(epochNum);
        isReadobxAllData = true;
    }
    double q0_scalar = 0,q1_x= 0,q2_y= 0,q3_z= 0;
    for(int i=obx_dat_pos;i<m_allEpochobxData.length();i++)
    {
        OBXData test_obx = m_allEpochobxData.at(i);
        if(timediff(test_obx.GPSTime,obs_T)<=0)
        {
            ObxData = m_allEpochobxData.at(i);
            obx_dat_pos = i;
            continue;
        }
        else
            break;
    }
    if(obx_dat_pos == m_allEpochobxData.length() - 1&&fabs(timediff(ObxData.GPSTime,obs_T))>120)
    {
        obx_dat_pos = 0;
        isReadobxAllData = false;
    }
    switch (SatType) {
    case 'G':
        q0_scalar=ObxData.GPSobxData[PRN-1].q0_scalar;
        q1_x=ObxData.GPSobxData[PRN-1].q1_x;
        q2_y=ObxData.GPSobxData[PRN-1].q2_y;
        q3_z=ObxData.GPSobxData[PRN-1].q3_z;
        break;
    case 'C':
        q0_scalar=ObxData.BDSobxData[PRN-1].q0_scalar;
        q1_x=ObxData.BDSobxData[PRN-1].q1_x;
        q2_y=ObxData.BDSobxData[PRN-1].q2_y;
        q3_z=ObxData.BDSobxData[PRN-1].q3_z;
        break;
    case 'E':
        q0_scalar=ObxData.GLEobxData[PRN-1].q0_scalar;
        q1_x=ObxData.GLEobxData[PRN-1].q1_x;
        q2_y=ObxData.GLEobxData[PRN-1].q2_y;
        q3_z=ObxData.GLEobxData[PRN-1].q3_z;
        break;
    case 'R':
        q0_scalar=ObxData.GLOobxData[PRN-1].q0_scalar;
        q1_x=ObxData.GLOobxData[PRN-1].q1_x;
        q2_y=ObxData.GLOobxData[PRN-1].q2_y;
        q3_z=ObxData.GLOobxData[PRN-1].q3_z;
        break;
    default:
        break;
    }
    double mor = q0_scalar*q0_scalar+q1_x*q1_x+q2_y*q2_y+q3_z*q3_z;
    if(fabs(mor-1)<1)
    {
        quater(0,0) = q0_scalar*q0_scalar+q1_x*q1_x-q2_y*q2_y-q3_z*q3_z;
        quater(0,1) = 2*(q1_x*q2_y-q0_scalar*q3_z);
        quater(0,2) = 2*(q1_x*q3_z+q0_scalar*q2_y);

        quater(1,0) = 2*(q1_x*q2_y+q0_scalar*q3_z);
        quater(1,1) = q0_scalar*q0_scalar-q1_x*q1_x+q2_y*q2_y-q3_z*q3_z;
        quater(1,2) = 2*(q2_y*q3_z-q0_scalar*q1_x);

        quater(2,0) = 2*(q1_x*q3_z-q0_scalar*q2_y);
        quater(2,1) = 2*(q2_y*q3_z+q0_scalar*q1_x);
        quater(2,2) = q0_scalar*q0_scalar-q1_x*q1_x-q2_y*q2_y+q3_z*q3_z;
    }
}

void read_orbex::getOBXData(OBXData *ObxData)
{
    while(!m_readOBXFileClass.atEnd())
    {
        while(!tempobxLine.contains("##",Qt::CaseInsensitive)&&!tempobxLine.contains("while",Qt::CaseInsensitive)&&!m_readOBXFileClass.atEnd())
        {
            tempobxLine = m_readOBXFileClass.readLine();
        }
        if(tempobxLine.contains("##",Qt::CaseInsensitive))
        {
            getobx_flag= 1;
            int SatNum = tempobxLine.mid(35,5).toInt();
            int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
            double Seconds=0;
            /*init temp_gps_time*/
            Year = tempobxLine.mid(3,4).toInt();
            Month = tempobxLine.mid(8,2).toInt();
            Day = tempobxLine.mid(11,2).toInt();
            Hours = tempobxLine.mid(14,2).toInt();
            Minutes = tempobxLine.mid(17,2).toInt();
            Seconds = tempobxLine.mid(20,9).toDouble();
            double GPS_Time[6] =  {Year,Month,Day,Hours,Minutes,Seconds};/*Observation time to GPS time*/
            ObxData->GPSTime = epoch2time(GPS_Time);
            for(int i=0;i<SatNum;i++)
            {
                tempobxLine = m_readOBXFileClass.readLine();
                QStringList temstr_list = tempobxLine.split(" ");
                temstr_list.removeAll(QString(""));
                int PRN = tempobxLine.mid(6,2).toInt();
                if (tempobxLine.mid(5,1) == "G"&&isInSystem('G'))
                {
                    /*GPS system*/
                    ObxData->GPSobxData[PRN-1].q0_scalar = temstr_list[3].toDouble();;
                    ObxData->GPSobxData[PRN-1].q1_x = temstr_list[4].toDouble();;
                    ObxData->GPSobxData[PRN-1].q2_y = temstr_list[5].toDouble();;
                    ObxData->GPSobxData[PRN-1].q3_z = temstr_list[6].toDouble();;
                }
                else if(tempobxLine.mid(5,1) == "R"&&isInSystem('R'))
                {
                    /*GLO system*/
                    ObxData->GLOobxData[PRN-1].q0_scalar = temstr_list[3].toDouble();;
                    ObxData->GLOobxData[PRN-1].q1_x = temstr_list[4].toDouble();;
                    ObxData->GLOobxData[PRN-1].q2_y = temstr_list[5].toDouble();;
                    ObxData->GLOobxData[PRN-1].q3_z = temstr_list[6].toDouble();;

                }
                else if(tempobxLine.mid(5,1) == "E"&&isInSystem('E'))
                {
                    /*GLE system*/
                    ObxData->GLEobxData[PRN-1].q0_scalar = temstr_list[3].toDouble();;
                    ObxData->GLEobxData[PRN-1].q1_x = temstr_list[4].toDouble();;
                    ObxData->GLEobxData[PRN-1].q2_y = temstr_list[5].toDouble();;
                    ObxData->GLEobxData[PRN-1].q3_z = temstr_list[6].toDouble();;
                }
                else if(tempobxLine.mid(5,1) == "C"&&isInSystem('C'))
                {
                    /*BDS system*/
                    ObxData->BDSobxData[PRN-1].q0_scalar = temstr_list[3].toDouble();;
                    ObxData->BDSobxData[PRN-1].q1_x = temstr_list[4].toDouble();;
                    ObxData->BDSobxData[PRN-1].q2_y = temstr_list[5].toDouble();;
                    ObxData->BDSobxData[PRN-1].q3_z = temstr_list[6].toDouble();;
                }
            }
            break;
        }
    }
}


/*Release all data*/
void read_orbex::releaseOBXAllData()
{
    m_allEpochobxData.clear();
}
