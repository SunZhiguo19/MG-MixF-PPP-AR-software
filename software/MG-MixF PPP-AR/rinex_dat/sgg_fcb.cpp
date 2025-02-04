#include "sgg_fcb.h"

sgg_fcb::sgg_fcb()
{

}


/**Set Fcbs file name and Function variable initialization**********************************/
void sgg_fcb::setFcbsFileNames(QStringList FCBsFileNames)
{
    initFCBVar();
    if (FCBsFileNames.length() > 0)
    {
        m_FCBsFileNames =FCBsFileNames;
    }
    else
    {
        isReadFcbAllData = true;
        return ;
    }
}
/****FCB file variable initialization**************************************/
void sgg_fcb::initFCBVar()
{
    isReadHead = false;
    isReadFcbAllData = false;
    m_FCBsFileNames.clear();
    m_FCBsFileName = "";
    m_allEpochFcbData.clear();
}

/**Read all data from multiple files*/
void sgg_fcb::readFileData2Vec(QStringList FCBsFileNames)
{
    if (FCBsFileNames.length() == 0)	isReadFcbAllData = true;
    if (isReadFcbAllData)  return;
    m_allEpochFcbData.clear();
    /*First read the header file to read the file from small to large according to the time*/
    int Year =0,Month = 0,Day = 0,Hours = 0,Minutes = 0,Week = 0;
    int PRN = 0;
    double Seconds = 0;
/*Read all files by time*/
    for (int i = 0;i < FCBsFileNames.length();i++)
    {
        QString FCBsFileName = FCBsFileNames.at(i);
/*open a file*/
        QFile fcbfile(FCBsFileName);
        if (!fcbfile.open(QFile::ReadOnly))
        {
            QString erroInfo = "Open " + FCBsFileName + "faild!(readFileData2Vec)";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
        }
/*Read header file data*/

        tempLine = fcbfile.readLine();/*Read the first line*/
        QString endHeadStr = tempLine.mid(0,1).trimmed();
        while (!endHeadStr.contains("*",Qt::CaseInsensitive))
        {
            /*Do header file data read here skip header file*/
            tempLine = fcbfile.readLine();/*Read the next line*/
            endHeadStr = tempLine.mid(0,1).trimmed();
        }

        while (!fcbfile.atEnd())
        {
            if(fcbfile.atEnd()) break;
            /*Do header file data read here skip header file*/
            bool readfcbFlag = true;
            FCBData epochData;
            if(tempLine.mid(0,1) == "*")
            {
                Year = tempLine.mid(2,4).toInt();
                Month = tempLine.mid(7,2).toInt();
                Day = tempLine.mid(10,2).toInt();
                Hours = tempLine.mid(13,2).toInt();
                Minutes = tempLine.mid(16,2).toInt();
                Seconds = tempLine.mid(20,9).toDouble();
                double fcbTime[6] = {Year,Month,Day,Hours,Minutes,Seconds};
                epochData.GPSTime =epoch2time(fcbTime);
                epochData.GPSWeek = Week;
                tempLine = fcbfile.readLine();/*Read a row of coordinate data*/
                readfcbFlag = false;
             }
            while (tempLine.mid(0,2) == "WL"||tempLine.mid(60,13) == "END OF HEADER")
            {
                bool isReadLineData = false;
                QString sateliteSys = tempLine.mid(4,1).trimmed();
                if (sateliteSys=="G")
                {
                    PRN = tempLine.mid(5,2).toInt();
                    epochData.GPSDataWl[PRN-1].WlFlag = tempLine.mid(5,2).toInt();
                    epochData.GPSDataWl[PRN-1].WlFCB = tempLine.mid(14,6).toDouble();/*GPSwl*/
                    epochData.GPSDataWl[PRN-1].WlFCBvar = tempLine.mid(25,5).toDouble();/*GPSwloffset*/
                    isReadLineData = true;
                }
                else if (sateliteSys=="R")
                {
                    PRN = tempLine.mid(5,2).toInt();
                    epochData.GLODataWl[PRN-1].WlFlag = tempLine.mid(5,2).toInt();
                    epochData.GLODataWl[PRN-1].WlFCB = tempLine.mid(14,6).toDouble();/*GPSwl*/
                    epochData.GLODataWl[PRN-1].WlFCBvar = tempLine.mid(25,5).toDouble();/*GPSwloffset*/
                    isReadLineData = true;
                }
                else if (sateliteSys=="C")
                {
                    PRN = tempLine.mid(5,2).toInt();
                    epochData.BDSDataWl[PRN-1].WlFlag = tempLine.mid(5,2).toInt();
                    epochData.BDSDataWl[PRN-1].WlFCB = tempLine.mid(14,6).toDouble();/*GPSwl*/
                    epochData.BDSDataWl[PRN-1].WlFCBvar = tempLine.mid(25,5).toDouble();/*GPSwloffset*/
                    isReadLineData = true;
                }
                else if (sateliteSys=="E")
                {
                    PRN = tempLine.mid(5,2).toInt();
                    epochData.GLEDataWl[PRN-1].WlFlag = tempLine.mid(5,2).toInt();
                    epochData.GLEDataWl[PRN-1].WlFCB = tempLine.mid(14,6).toDouble();/*GPSwl*/
                    epochData.GLEDataWl[PRN-1].WlFCBvar = tempLine.mid(25,5).toDouble();/*GPSwloffset*/
                    isReadLineData = true;
                }
                tempLine = fcbfile.readLine();/*Read a row of coordinate data*/
            }
            while (tempLine.mid(0,1) == "P")
            {
                 bool isReadLineData = false;
                if (tempLine.mid(1,1) == "G")
                {
                    PRN = tempLine.mid(2,2).toInt();
                    epochData.GPSDataNl[PRN-1].NlFCB = tempLine.mid(23,7).toDouble();/*GPSwl*/
                    epochData.GPSDataNl[PRN-1].NlFCBvar = tempLine.mid(54,6).toDouble();/*GPSwl*/
                    isReadLineData = true;
                }
                else if (tempLine.mid(1,1) == "R")
                {
                    PRN = tempLine.mid(2,2).toInt();
                    epochData.GLODataNl[PRN-1].NlFCB = tempLine.mid(23,7).toDouble();/*GPSwl*/
                    epochData.GLODataNl[PRN-1].NlFCBvar = tempLine.mid(54,6).toDouble();/*GPSwl*/
                    isReadLineData = true;
                }
                else if (tempLine.mid(1,1) == "C")
                {
                    PRN = tempLine.mid(2,2).toInt();
                    epochData.BDSDataNl[PRN-1].NlFCB = tempLine.mid(23,7).toDouble();/*GPSwl*/
                    epochData.BDSDataNl[PRN-1].NlFCBvar = tempLine.mid(54,6).toDouble();/*GPSwl*/
                    isReadLineData = true;
                }
                else if (tempLine.mid(1,1) == "E")
                {
                    PRN = tempLine.mid(2,2).toInt();
                    epochData.GLEDataNl[PRN-1].NlFCB = tempLine.mid(23,7).toDouble();/*GPSwl*/
                    epochData.GLEDataNl[PRN-1].NlFCBvar = tempLine.mid(54,6).toDouble();/*GPSwl*/
                    isReadLineData = true;
                }
                tempLine = fcbfile.readLine();/*Read a row of coordinate data*/
            }
            if(!readfcbFlag)
                m_allEpochFcbData.append(epochData);
            else if(!fcbfile.atEnd())
                tempLine = fcbfile.readLine();

        }
        fcbfile.close();
    }
    isReadFcbAllData = true;
}
/*****Return data*********/
QVector< FCBData > sgg_fcb::getFCBAllData()
{
    readFileData2Vec(m_FCBsFileNames);
    return m_allEpochFcbData;
}
/*Release all data**/
void sgg_fcb::releasefcbAllData()
{
    m_allEpochFcbData.clear();
}

/*Search for wide lane and narrow lane FCB*/
void sgg_fcb::SearchFCBsFile(int PRN,char SatType,double *Fcbswl,double *Fcbsnl,double *NL_fcbs_std,gtime_t GPSOTime)
{

    *Fcbswl = 0;  *Fcbsnl = 0;
    int lengthEpoch = m_allEpochFcbData.length();
    FCBData oneFCBsDate = m_allEpochFcbData.at(0);
    switch (SatType)
    {
    case 'G':
        *Fcbswl = oneFCBsDate.GPSDataWl[PRN - 1].WlFCB;
        break;
    case 'R':
        *Fcbswl = oneFCBsDate.GLODataWl[PRN - 1].WlFCB;
        break;
    case 'C':
        *Fcbswl = oneFCBsDate.BDSDataWl[PRN - 1].WlFCB;
        break;
    case 'E':
        *Fcbswl = oneFCBsDate.GLEDataWl[PRN - 1].WlFCB;
        break;
    default:
        *Fcbswl = 0;
    }
    for (int i = 1;i < lengthEpoch;i++)
    {
        FCBData epochFcbsData = m_allEpochFcbData.at(i);
        if(timediff(epochFcbsData.GPSTime,GPSOTime) < 0.1&&fabs(timediff(epochFcbsData.GPSTime,GPSOTime))<900.1)
        {
            switch (SatType)
            {
            case 'G':
                *Fcbsnl = epochFcbsData.GPSDataNl[PRN-1].NlFCB;
                *NL_fcbs_std = epochFcbsData.GPSDataNl[PRN-1].NlFCBvar;
                break;
            case 'R':
                *Fcbsnl = epochFcbsData.GLODataNl[PRN-1].NlFCB;
                *NL_fcbs_std = epochFcbsData.GLODataNl[PRN-1].NlFCBvar;
                break;
            case 'C':
                *Fcbsnl = epochFcbsData.BDSDataNl[PRN-1].NlFCB;
                *NL_fcbs_std = epochFcbsData.BDSDataNl[PRN-1].NlFCBvar;
                break;
            case 'E':
                *Fcbsnl = epochFcbsData.GLEDataNl[PRN-1].NlFCB;
                *NL_fcbs_std = epochFcbsData.GLEDataNl[PRN-1].NlFCBvar;
                break;
            default:
                *Fcbsnl = 0;
                *NL_fcbs_std = 0;
            }
        }
    }
}

/*Get FCB data*******************/
void sgg_fcb::getStaliteFCBs(int PRN,char SatType,double *Fcbswl,double *Fcbsnl,double *NL_fcbs_std,gtime_t GPSOTime)
{
    readFileData2Vec(m_FCBsFileNames);
    SearchFCBsFile(PRN,SatType,Fcbswl,Fcbsnl,NL_fcbs_std,GPSOTime);
}
