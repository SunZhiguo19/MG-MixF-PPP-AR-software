#include "read_dcb.h"

read_dcb::read_dcb()
{

}

/*Set DCB file name****/
void read_dcb::setDCBFileNames(QStringList DCBFileNames)
{
    initDCBVar();

    if(DCBFileNames.length() == 1) IsSigalFile = true;
    if(DCBFileNames.length() > 0)
    {
        m_DCBFileNames =DCBFileNames;
    }
    else
    {
        isReadAllData = true;
        return ;
    }
}
/**Initialize required variables**/
void read_dcb::initDCBVar()
{
    isReadHead = false;
    isReadAllData = false;
    SatDCBData epochDcbData_init;
    epochDcbData = epochDcbData_init;
    tempLine = "";
    IsSigalFile = false;
    DCBTypeflag=0;
}

/**Read all DCB data from multiple files****/
void read_dcb::readFileData2Vec(QStringList DCBFileNames)
{
    if (DCBFileNames.length() == 0)	isReadAllData = true;
    if (isReadAllData)  return;

    /*First read the header file to read the file from small to large according to the time*/
    int PRN = 0;
/*Read all files by time*/
    for (int i = 0;i < DCBFileNames.length();i++)
    {
        QString DCBFileName = DCBFileNames.at(i);
/*open a file*/
        QFile dcbfile(DCBFileName);
        if (!dcbfile.open(QFile::ReadOnly))
        {
            QString erroInfo = "Open " + DCBFileName + "faild!(readFileData)";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
        }
        if(DCBFileName.contains("P1C1",Qt::CaseInsensitive))
            DCBTypeflag=1;
        else if(DCBFileName.contains("P1P2",Qt::CaseInsensitive))
            DCBTypeflag=2;
        else if(DCBFileName.contains("P2C2",Qt::CaseInsensitive))
            DCBTypeflag=3;
/*Read header file data*/
        QString endHeadStr = tempLine.mid(0,1).trimmed();
        while (!endHeadStr.contains("*",Qt::CaseInsensitive))
        {
            /*Do header file data read here skip header file*/
            tempLine = dcbfile.readLine();/*Read the next line*/
            endHeadStr = tempLine.mid(0,1).trimmed();
        }
        while (!dcbfile.atEnd())
        {
            if(dcbfile.atEnd()) break;
            /*Do header file data read here skip header file*/
            tempLine = dcbfile.readLine();/*Read a row of coordinate data*/
            if(DCBTypeflag==1)
            {
                if(tempLine.mid(0,1) == "G")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GPSdcbData[PRN-1].P1_C1_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GPSdcbData[PRN-1].P1_C1_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "R")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GLOdcbData[PRN-1].P1_C1_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GLOdcbData[PRN-1].P1_C1_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "C")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.BDSdcbData[PRN-1].P1_C1_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.BDSdcbData[PRN-1].P1_C1_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "E")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GLEdcbData[PRN-1].P1_C1_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GLEdcbData[PRN-1].P1_C1_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
            }
            else if(DCBTypeflag==2)
            {
                if(tempLine.mid(0,1) == "G")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GPSdcbData[PRN-1].P1_P2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GPSdcbData[PRN-1].P1_P2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "R")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GLOdcbData[PRN-1].P1_P2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GLOdcbData[PRN-1].P1_P2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "C")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.BDSdcbData[PRN-1].P1_P2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.BDSdcbData[PRN-1].P1_P2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "E")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GLEdcbData[PRN-1].P1_P2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GLEdcbData[PRN-1].P1_P2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
            }
            else if(DCBTypeflag==3)
            {
                if(tempLine.mid(0,1) == "G")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GPSdcbData[PRN-1].P2_C2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GPSdcbData[PRN-1].P2_C2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "R")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GLOdcbData[PRN-1].P2_C2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GLOdcbData[PRN-1].P2_C2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "C")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.BDSdcbData[PRN-1].P2_C2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.BDSdcbData[PRN-1].P2_C2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
                else if(tempLine.mid(0,1) == "E")
                {
                    PRN = tempLine.mid(1,2).toInt();
                    epochDcbData.GLEdcbData[PRN-1].P2_C2_DCB=tempLine.mid(29,6).toDouble();/*P1-C1*/
                    epochDcbData.GLEdcbData[PRN-1].P2_C2_DCBVar=tempLine.mid(42,5).toDouble();/*P1-C1*/
                }
            }
        }
        dcbfile.close();
    }
    isReadAllData = true;
}


/**Obtain DCB data corresponding to satellite*/
void read_dcb::SearchDCBFile(int PRN,char SatType,double *DCBs,double *DCBVar,int DCBtype)
{

    *DCBs = 0;
    switch (SatType)
    {
    case 'G':
        if(DCBtype==1)
        {
            *DCBs = epochDcbData.GPSdcbData[PRN-1].P1_C1_DCB;
            *DCBVar = epochDcbData.GPSdcbData[PRN-1].P1_C1_DCBVar;
        }
        else if(DCBtype==2)
        {
            *DCBs = epochDcbData.GPSdcbData[PRN-1].P1_P2_DCB;
            *DCBVar = epochDcbData.GPSdcbData[PRN-1].P1_P2_DCBVar;
        }
        else if(DCBtype==3)
        {
            *DCBs = epochDcbData.GPSdcbData[PRN-1].P2_C2_DCB;
            *DCBVar = epochDcbData.GPSdcbData[PRN-1].P2_C2_DCBVar;
        }
        break;
    case 'R':
        if(DCBtype==1)
        {
            *DCBs = epochDcbData.GLOdcbData[PRN-1].P1_C1_DCB;
            *DCBVar = epochDcbData.GLOdcbData[PRN-1].P1_C1_DCBVar;
        }
        else if(DCBtype==2)
        {
            *DCBs = epochDcbData.GLOdcbData[PRN-1].P1_P2_DCB;
            *DCBVar = epochDcbData.GLOdcbData[PRN-1].P1_P2_DCBVar;
        }
        else if(DCBtype==3)
        {
            *DCBs = epochDcbData.GLOdcbData[PRN-1].P2_C2_DCB;
            *DCBVar = epochDcbData.GLOdcbData[PRN-1].P2_C2_DCBVar;
        }
        break;
    case 'C':
        if(DCBtype==1)
        {
            *DCBs = epochDcbData.BDSdcbData[PRN-1].P1_C1_DCB;
            *DCBVar = epochDcbData.BDSdcbData[PRN-1].P1_C1_DCBVar;
        }
        else if(DCBtype==2)
        {
            *DCBs = epochDcbData.BDSdcbData[PRN-1].P1_P2_DCB;
            *DCBVar = epochDcbData.BDSdcbData[PRN-1].P1_P2_DCBVar;
        }
        else if(DCBtype==3)
        {
            *DCBs = epochDcbData.BDSdcbData[PRN-1].P2_C2_DCB;
            *DCBVar = epochDcbData.BDSdcbData[PRN-1].P2_C2_DCBVar;
        }
        break;
    case 'E':
        if(DCBtype==1)
        {
            *DCBs = epochDcbData.GLEdcbData[PRN-1].P1_C1_DCB;
            *DCBVar = epochDcbData.GLEdcbData[PRN-1].P1_C1_DCBVar;
        }
        else if(DCBtype==2)
        {
            *DCBs = epochDcbData.GLEdcbData[PRN-1].P1_P2_DCB;
            *DCBVar = epochDcbData.GLEdcbData[PRN-1].P1_P2_DCBVar;
        }
        else if(DCBtype==3)
        {
            *DCBs = epochDcbData.GLEdcbData[PRN-1].P2_C2_DCB;
            *DCBVar = epochDcbData.GLEdcbData[PRN-1].P2_C2_DCBVar;
        }
        break;
    default:
        *DCBs = 0;
        *DCBVar = 0;
        break;
    }

}

/***Acquire satellite DCB data******/
void read_dcb::getStaliteDCB(int PRN,char SatType,double *DCBswl,double *DCBVar,int DCBtype)
{
    readFileData2Vec(m_DCBFileNames);
    SearchDCBFile(PRN,SatType,DCBswl,DCBVar,DCBtype);
}
