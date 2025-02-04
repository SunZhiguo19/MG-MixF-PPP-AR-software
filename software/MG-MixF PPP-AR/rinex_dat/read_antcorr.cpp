#include "read_antcorr.h"

read_antcorr::read_antcorr()
{

}


void read_antcorr::setAntFileName(QString AntFileName,QString AnTypeName,double ObvJD)
{
    initANTVar();
    if (!AntFileName.isEmpty())
        m_AntFileName = AntFileName;
    m_AntType = AnTypeName;
    m_ObeservTime = ObvJD;
    ant_wind_up = true;
}

/*Initialize variables*/
void read_antcorr::initANTVar()
{
    m_AntFileName = "igs14.atx";
    isReadANTAllData = false;
    isReadAntHead = false;
    isReadSatAntData = false;
    isReadRecvData = false;
    m_ObeservTime = 0;
    m_tempAntLine = " ";
    m_SatData.clear();
    for (int i = 0;i < 3;i++)
    {
        m_sunpos[i] = 0;
        m_moonpos[i] = 0;
    }
    m_gmst = 0;
    m_sunSecFlag = -1;
}

/*Set Julian time*/
void read_antcorr::setObsJD(QString AnTypeName,double ObsJD)
{
    m_AntType = AnTypeName;
    m_ObeservTime = ObsJD;
}


/*open a file*/
bool read_antcorr::openFiles(QString AntFileName)
{
    if(!AntFileName.isEmpty())
    {
        m_ReadAntFileClass.setFileName(AntFileName);
        if (!m_ReadAntFileClass.open(QFile::ReadOnly))/*If the file fails to open...*/
        {
            m_ReadAntFileClass.setFileName("igs14_2101.atx");/*Open to current directory*/
            if (!m_ReadAntFileClass.open(QFile::ReadOnly))
            {
                isReadAntHead = true;
                isReadRecvData = true;
                isReadSatAntData = true;
                isReadANTAllData = true;
                QString infor = "Can not find antmod.atx.";
                disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
                text_disp->data_text.append(infor);
                text_disp->data_lenth++;
                return false;
            }
            else
            {
                return true;
            }
        }
        else
            return true;
    }
    else
    {
        isReadAntHead = true;
        isReadRecvData = true;
        isReadSatAntData = true;
        isReadANTAllData = true;
        QString infor = "AntFileName is None";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
        return false;
    }

    return false;
}

/*Read header file*/
bool read_antcorr::readFileAntHead()
{
    if (isReadAntHead)
        return true;
    m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the first line*/
    QString flagHeadEnd = "HEADER";
    QString endHeadStr = m_tempAntLine.mid(60,20).trimmed();
    while (!endHeadStr.contains(flagHeadEnd,Qt::CaseInsensitive))
    {

        /*Read the next line*/
        m_tempAntLine = m_ReadAntFileClass.readLine();
        endHeadStr = m_tempAntLine.mid(60,20).trimmed();
    }
    m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next line into the data area*/
    isReadAntHead = true;
    return true;
}

/*Read satellite + receiver antenna data*/
bool read_antcorr::getAntAllData()
{
    if (m_AntFileName.isEmpty())
        isReadANTAllData = true;
    if (isReadANTAllData) return true;
    openFiles(m_AntFileName);
    readSatliData();
    readRecvData();
    isReadANTAllData = true;
    m_ReadAntFileClass.close();
    return true;
}

/*Read receiver antenna data*/
bool read_antcorr::readRecvData()
{
    if (!isReadAntHead) readFileAntHead();
    if (isReadRecvData) return true;
    isReadRecvData = true;/*Read receiver antenna data only once (no more search than search)*/
    long int currPos = m_ReadAntFileClass.pos();/*Save current pointer position*/
    /*Search antenna type*/
    while(!m_tempAntLine.mid(0,20).contains(m_AntType))
    {
        m_tempAntLine = m_ReadAntFileClass.readLine();
        if (m_ReadAntFileClass.atEnd())
        {/*Prevents inadvertent loops at the end of the read file, and flags false to indicate that the antenna was not found*/
            m_RecvData.isFrqData = false;
            return false;
        }
    }
    /*Read receiver antenna data*/
    while (!m_tempAntLine.mid(60).contains("END OF ANTENNA"))
    {
        if (m_tempAntLine.mid(60).contains("TYPE / SERIAL NO"))
        {
            m_RecvData.StrAntType = m_tempAntLine.mid(0,20);
            m_RecvData.SatliCNN = m_tempAntLine.mid(20,20);
            m_tempAntLine = m_ReadAntFileClass.readLine();

        }
        else if (m_tempAntLine.mid(60).contains("DAZI"))
        {
            m_RecvData.DAZI = m_tempAntLine.mid(0,8).toDouble();
            m_tempAntLine = m_ReadAntFileClass.readLine();
        }
        else if (m_tempAntLine.mid(60).contains("ZEN1 / ZEN2 / DZEN"))
        {
            m_RecvData.ZEN_12N[0] = m_tempAntLine.mid(2,6).toDouble();
            m_RecvData.ZEN_12N[1] = m_tempAntLine.mid(8,6).toDouble();
            m_RecvData.ZEN_12N[2] = m_tempAntLine.mid(14,6).toDouble();
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
        else if (m_tempAntLine.mid(60).contains("FREQUENCIES"))/*# OF FREQUENCIES*/
        {
            m_RecvData.NumFrqu = m_tempAntLine.mid(0,6).toDouble();
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
        else if (m_tempAntLine.mid(60).contains("VALID FROM"))/*VALID FROM*/
        {
            int Year = m_tempAntLine.mid(0,6).toInt();
            int Month = m_tempAntLine.mid(6,6).toInt();
            int Day = m_tempAntLine.mid(12,6).toInt();
            int Hour = m_tempAntLine.mid(18,6).toInt();
            int Minuts = m_tempAntLine.mid(24,6).toInt();
            int Seconds = m_tempAntLine.mid(30,13).toDouble();
            m_RecvData.ValidJD = computeJD(Year,Month,Day,Hour,Minuts,Seconds);
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
        else if (m_tempAntLine.mid(60).contains("VALID UNTIL"))/*VALID UNTIL*/
        {
            int Year = m_tempAntLine.mid(0,6).toInt();
            int Month = m_tempAntLine.mid(6,6).toInt();
            int Day = m_tempAntLine.mid(12,6).toInt();
            int Hour = m_tempAntLine.mid(18,6).toInt();
            int Minuts = m_tempAntLine.mid(24,6).toInt();
            int Seconds = m_tempAntLine.mid(30,13).toDouble();
            m_RecvData.EndJD = computeJD(Year,Month,Day,Hour,Minuts,Seconds);
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
        else if (m_tempAntLine.mid(60).contains("START OF FREQUENCY"))/*START OF FREQUENCY*/
        {/*Read receiver PCO and PCV*/
            FrqData tempFrqData;
            tempFrqData.PCO[0] = 0;tempFrqData.PCO[1] = 0;tempFrqData.PCO[2] = 0;
            tempFrqData.FrqFlag = m_tempAntLine.mid(3,3);
            m_tempAntLine = m_ReadAntFileClass.readLine();
            if (m_tempAntLine.mid(60).contains("NORTH / EAST / UP"))
            {
                tempFrqData.PCO[0] = m_tempAntLine.mid(0,10).toDouble();
                tempFrqData.PCO[1] = m_tempAntLine.mid(10,10).toDouble();
                tempFrqData.PCO[2] = m_tempAntLine.mid(20,10).toDouble();
            }
            /*Read NOZAI data*/
            m_tempAntLine = m_ReadAntFileClass.readLine();
            int Row = 0,Column = 0;
            if (m_RecvData.DAZI !=  0)
                Row = (int)(360/m_RecvData.DAZI) + 1;
            Column = (int)((m_RecvData.ZEN_12N[1] - m_RecvData.ZEN_12N[0])/m_RecvData.ZEN_12N[2]) + 1;
            tempFrqData.Row = Row;
            tempFrqData.Column = Column;
            if (m_tempAntLine.mid(0,8).contains("NOAZI"))
                for (int i = 0;i < Column;i++)
                    tempFrqData.PCVNoAZI.append(m_tempAntLine.mid(8+i*8,8).toDouble());
            /*Row does not equal 0 to read ZAI data*/
            if (tempFrqData.Row != 0)
            {
                for (int i = 0;i < tempFrqData.Row;i++)
                {
                    m_tempAntLine = m_ReadAntFileClass.readLine();/*Read a row of data*/
                    for (int j = 0;j < tempFrqData.Column;j++)
                    {
                        tempFrqData.PCVAZI.append(m_tempAntLine.mid(8+j*8,8).toDouble());
                    }
                }
            }
            /*PCO*/
            m_RecvData.PCOPCV.append(tempFrqData);
            m_RecvData.isFrqData = true;
            m_RecvData.IsSat = false;
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
        else if (m_tempAntLine.mid(60).contains("END OF FREQUENCY"))/*END OF FREQUENCY*/
        {
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
        else
        {
            m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
        }
    }

    m_ReadAntFileClass.seek(currPos);/*Set as the starting search coordinate*/
    return true;
}

/*Read satellite system antenna data*/
bool read_antcorr::readSatliData()
{
    if (!isReadAntHead) readFileAntHead();
    if(isReadSatAntData) return false;
    long int currPos = m_ReadAntFileClass.pos();/*Save current pointer position*/
    /*Find to start reading data*/
    while(!m_tempAntLine.mid(60).contains("START OF ANTENNA"))
        m_tempAntLine = m_ReadAntFileClass.readLine();
    /*Read satellite antenna data*/
    char tempSatType = '0';
    while (!m_ReadAntFileClass.atEnd())
    {
        AntDataType tempSatData;
        tempSatData.IsSat = true;
        tempSatData.ValidJD = 0;
        tempSatData.EndJD = m_ObeservTime+1;
        /*Read a satellite antenna data*/
        while (!m_tempAntLine.mid(60).contains("END OF ANTENNA"))
        {
            if (m_tempAntLine.mid(60).contains("START OF ANTENNA"))/*START OF ANTENNA*/
            {
                m_tempAntLine = m_ReadAntFileClass.readLine();
            }
            else if (m_tempAntLine.mid(60).contains("TYPE / SERIAL NO"))/*TYPE / SERIAL NO*/
            {
                tempSatData.StrAntType = m_tempAntLine.mid(0,20);
                tempSatData.SatliCNN = m_tempAntLine.mid(20,20).trimmed();/*Satellite system PRN*/
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/

            }
            else if (m_tempAntLine.mid(60).contains("DAZI"))
            {
                tempSatData.DAZI = m_tempAntLine.mid(0,8).toDouble();
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else if (m_tempAntLine.mid(60).contains("ZEN1 / ZEN2 / DZEN"))
            {
                tempSatData.ZEN_12N[0] = m_tempAntLine.mid(2,6).toDouble();
                tempSatData.ZEN_12N[1] = m_tempAntLine.mid(8,6).toDouble();
                tempSatData.ZEN_12N[2] = m_tempAntLine.mid(14,6).toDouble();
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else if (m_tempAntLine.mid(60).contains("FREQUENCIES"))/*# OF FREQUENCIES*/
            {
                tempSatData.NumFrqu = m_tempAntLine.mid(0,6).toInt();
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else if (m_tempAntLine.mid(60).contains("VALID FROM"))/*VALID FROM*/
            {
                int Year = m_tempAntLine.mid(0,6).toInt();
                int Month = m_tempAntLine.mid(6,6).toInt();
                int Day = m_tempAntLine.mid(12,6).toInt();
                int Hour = m_tempAntLine.mid(18,6).toInt();
                int Minuts = m_tempAntLine.mid(24,6).toInt();
                int Seconds = m_tempAntLine.mid(30,13).toDouble();
                tempSatData.ValidJD = computeJD(Year,Month,Day,Hour,Minuts,Seconds);
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else if (m_tempAntLine.mid(60).contains("VALID UNTIL"))/*VALID UNTIL*/
            {
                int Year = m_tempAntLine.mid(0,6).toInt();
                int Month = m_tempAntLine.mid(6,6).toInt();
                int Day = m_tempAntLine.mid(12,6).toInt();
                int Hour = m_tempAntLine.mid(18,6).toInt();
                int Minuts = m_tempAntLine.mid(24,6).toInt();
                int Seconds = m_tempAntLine.mid(30,13).toDouble();
                tempSatData.EndJD = computeJD(Year,Month,Day,Hour,Minuts,Seconds);
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else if (m_tempAntLine.mid(60).contains("START OF FREQUENCY"))/*START OF FREQUENCY*/
            {/*Read the satellite's PCO and PCV*/
                FrqData tempFrqData;
                tempFrqData.PCO[0] = 0;tempFrqData.PCO[1] = 0;tempFrqData.PCO[2] = 0;
                tempFrqData.FrqFlag = m_tempAntLine.mid(3,3);
                m_tempAntLine = m_ReadAntFileClass.readLine();
                if (m_tempAntLine.mid(60).contains("NORTH / EAST / UP"))
                {
                    tempFrqData.PCO[0] = m_tempAntLine.mid(0,10).toDouble();
                    tempFrqData.PCO[1] = m_tempAntLine.mid(10,10).toDouble();
                    tempFrqData.PCO[2] = m_tempAntLine.mid(20,10).toDouble();
                }
                /*Read NOZAI data*/
                m_tempAntLine = m_ReadAntFileClass.readLine();
                int Row = 0,Column = 0;
                if (tempSatData.DAZI !=  0)
                    Row = (int)(360/tempSatData.DAZI) + 1;
                Column = (int)((tempSatData.ZEN_12N[1] - tempSatData.ZEN_12N[0])/tempSatData.ZEN_12N[2]) + 1;
                tempFrqData.Row = Row;
                tempFrqData.Column = Column;
                if (m_tempAntLine.mid(0,8).contains("NOAZI"))
                    for (int i = 0;i < Column;i++)
                        tempFrqData.PCVNoAZI.append(m_tempAntLine.mid(8+i*8,8).toDouble());
                /*Row does not equal 0 to read ZAI data*/
                if (tempFrqData.Row != 0)
                {
                    for (int i = 0;i < tempFrqData.Row;i++)
                    {
                        m_tempAntLine = m_ReadAntFileClass.readLine();/*Read a row of data*/
                        for (int j = 0;j < tempFrqData.Column;j++)
                        {
                            tempFrqData.PCVAZI.append(m_tempAntLine.mid(8+j*8,8).toDouble());
                        }
                    }
                }
                tempSatData.PCOPCV.append(tempFrqData);
                tempSatData.isFrqData = true;
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else if (m_tempAntLine.mid(60).contains("END OF FREQUENCY"))/*END OF FREQUENCY*/
            {
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }
            else
            {
                m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
            }/*If elseif structure*/
            if (m_ReadAntFileClass.atEnd())	break;
        }
        /*Determine if it is valid data*/
        tempSatType = *(tempSatData.SatliCNN.mid(0,1).toLatin1().data());
        if (tempSatData.ValidJD < m_ObeservTime&&tempSatData.EndJD > m_ObeservTime&&isInSystem(tempSatType))
        {
            m_SatData.append(tempSatData);
        }
        /*Not jumping out of the four-system satellite antenna area*/
        if(tempSatType!='G'&&tempSatType!='R'&&tempSatType!='C'&&tempSatType!='E')
            break;
        m_tempAntLine = m_ReadAntFileClass.readLine();/*Read the next row of data*/
    }
    m_ReadAntFileClass.seek(currPos);
    isReadSatAntData = true;/*Read satellite antenna only once (no more searches than search)*/
    return true;
}

/*Bilinear interpolation calculation PCV*/
bool read_antcorr::getPCV(const AntDataType &tempAntData, char SatType, double *PCV12, double Ztop, double AZI, QVector<QString> FrqFlag)
{
    PCV12[0] = 0; PCV12[1] = 0;PCV12[2] = 0;PCV12[3] = 0;PCV12[4] = 0;
    QVector< double > XZen;/*Azimuth angle is equivalent to interpolation X coordinate*/
    QVector< double > XAzi;/*The height angle is equivalent to the interpolated X coordinate*/
    int flagZ = 0,flagA = 0;/*Interpolation left (smaller) nearest neighbor*/
    for (double i = tempAntData.ZEN_12N[0];i <= tempAntData.ZEN_12N[1];i+=tempAntData.ZEN_12N[2])
    {
        XZen.append(i*MM_PI/180);
        if (i*MM_PI/180 < Ztop)
            flagZ++;
    }
    flagZ--;
    if(flagZ < 0) flagZ = 0;/*Azimuth crossing setting 0*/
    if (tempAntData.DAZI !=0)
    {/*PCV value table in the form of a matrix*/
        for (int i = 0;i <= 360;i+=tempAntData.DAZI)
        {
            XAzi.append(i*MM_PI/180);
            if (i*MM_PI/180 < AZI)
                flagA++;
        }
        flagA--;
        if(flagA < 0) flagA = 0;/*Azimuth crossing setting 0，For Geileo*/
    }
    /*Calculate PCV12 (calculate the receiver antenna correction needs,
     * search for the corresponding frequency according to the satellite frequency,
     * search for the first two corrections*/
    FrqData PCOPCV1,PCOPCV2,PCOPCV3,PCOPCV4,PCOPCV5;
    QString FrqStr1 = "C1C", FrqStr2 = "C2C", FrqStr3 = "C3C", FrqStr4 = "C4C", FrqStr5 = "C5C";
    if(FrqFlag.length() > 0) FrqStr1 = FrqFlag.at(0);
    if(FrqFlag.length() > 2) FrqStr2 = FrqFlag.at(2);
    if(FrqFlag.length() > 4) FrqStr3 = FrqFlag.at(4);
    if(FrqFlag.length() > 6) FrqStr4 = FrqFlag.at(6);
    if(FrqFlag.length() > 8) FrqStr5 = FrqFlag.at(8);

    QString destFlag1 = QString(SatType) + "0" + FrqStr1.mid(1,1),
            destFlag2 = QString(SatType) + "0" + FrqStr2.mid(1,1),
            destFlag3 = QString(SatType) + "0" + FrqStr3.mid(1,1),
            destFlag4 = QString(SatType) + "0" + FrqStr4.mid(1,1),
            destFlag5 = QString(SatType) + "0" + FrqStr5.mid(1,1);
    bool isFind1 = false,isFind2 = false,isFind3 = false,isFind4 = false,isFind5 = false;
    for (int i = 0;i < tempAntData.PCOPCV.length();i++)
    {/*This only applies to GPS L1 and L2, as well as GLONASS G1 and G2*/
        QString LFlag = tempAntData.PCOPCV.at(i).FrqFlag.trimmed();

        if (LFlag == destFlag1)
        {
            PCOPCV1 = tempAntData.PCOPCV.at(i);
            isFind1 = true;
        }
        else if (LFlag == destFlag2)
        {
            PCOPCV2 = tempAntData.PCOPCV.at(i);
            isFind2 = true;
        }
        else if (LFlag == destFlag3)
        {
            PCOPCV3 = tempAntData.PCOPCV.at(i);
            isFind3 = true;
        }
        else if (LFlag == destFlag4)
        {
            PCOPCV4 = tempAntData.PCOPCV.at(i);
            isFind4 = true;
        }
        else if (LFlag == destFlag5)
        {
            PCOPCV5 = tempAntData.PCOPCV.at(i);
            isFind5 = true;
        }
    }
    /*Can't find GPS correction, for example, without Geileo and BDS, use GPS correction*/
    if(!isFind1 && tempAntData.PCOPCV.length() > 0) PCOPCV1 = tempAntData.PCOPCV.at(0);
    if(!isFind2 && tempAntData.PCOPCV.length() > 1) PCOPCV2 = tempAntData.PCOPCV.at(1);
    if(!isFind3 && tempAntData.PCOPCV.length() > 2) PCOPCV3 = tempAntData.PCOPCV.at(2);
    if(!isFind4 && tempAntData.PCOPCV.length() > 3) PCOPCV4 = tempAntData.PCOPCV.at(3);
    if(!isFind5 && tempAntData.PCOPCV.length() > 4) PCOPCV5 = tempAntData.PCOPCV.at(4);

    /*Calculate PCV1*/
    int PCOPCV1_len = PCOPCV1.PCVAZI.length(),
        PCOPCV2_len = PCOPCV2.PCVAZI.length(),
        PCOPCV3_len = PCOPCV3.PCVAZI.length(),
        PCOPCV4_len = PCOPCV4.PCVAZI.length(),
        PCOPCV5_len = PCOPCV5.PCVAZI.length();
    if(tempAntData.IsSat)
    {/*The PCV calculation of the satellite is not currently in accordance with the azimuth.
        *Only the Geileo provided by the ESA unit has an azimuth, which is not necessarily accurate,
        *and the satellite antenna azimuth is difficult to determine.*/
        PCOPCV1_len = 0;
        PCOPCV2_len = 0;
        PCOPCV3_len = 0;
        PCOPCV4_len = 0;
        PCOPCV5_len = 0;
    }
    if (PCOPCV1_len == 0)
    {/*Single linear interpolation */
        if(PCOPCV1.PCVNoAZI.length() == 0) return false;

        if (flagZ < XZen.length() - 1)
        {
            double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
            double y1 = PCOPCV1.PCVNoAZI.at(flagZ),y2 = PCOPCV1.PCVNoAZI.at(flagZ+1);
            PCV12[0] = y1+(Ztop - x1)*(y2-y1)/(x2 -x1);
        }
        else
            PCV12[0] = PCOPCV1.PCVNoAZI.at(flagZ);
    }
    else
    {/*Bilinear interpolation*/
        if (flagZ >= XZen.length() - 1)		flagZ--;/*Judging the out of bounds, that is
                                                     *, the height angle is greater than 90 degrees, generally not*/
        if (flagA >= XAzi.length() - 1)		flagA--;/*Judging the out of bounds, that is,
                                                     *the azimuth is greater than 360 degrees, generally not*/
        double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
        double y1 = XAzi.at(flagA),y2 = XAzi.at(flagA+1);
        int RowNum = 0,ColumnNum = 0;/*Calculate the number of rows and columns in the matrix*/
        if (tempAntData.DAZI !=  0)
            RowNum = (int)(360/m_RecvData.DAZI) + 1;
        ColumnNum = (int)((tempAntData.ZEN_12N[1] - tempAntData.ZEN_12N[0])/tempAntData.ZEN_12N[2]) + 1;
        double z11 = PCOPCV1.PCVAZI.at(ColumnNum*flagA+flagZ),z12 = PCOPCV1.PCVAZI.at(ColumnNum*flagA+flagZ+1),
            z21 = PCOPCV1.PCVAZI.at(ColumnNum*(flagA+1)+flagZ),z22 = PCOPCV1.PCVAZI.at(ColumnNum*(flagA+1)+flagZ+1);

        double z1_2 = z11 + (Ztop - x1)*(z12 - z11)/(x2 - x1),
               z2_2 = z21 + (Ztop - x1)*(z22 - z21)/(x2 - x1);

        PCV12[0] = z1_2 + (AZI - y1)*(z2_2 - z1_2)/(y2 - y1);
    }
    /*Calculate PCV2*/
    if (PCOPCV2_len == 0)
    {/*Single linear interpolation*/
        if(PCOPCV2.PCVNoAZI.length() == 0) return false;
        if (flagZ < XZen.length() - 1)
        {
            double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
            double y1 = PCOPCV2.PCVNoAZI.at(flagZ),y2 = PCOPCV2.PCVNoAZI.at(flagZ+1);
            PCV12[1] = y1+(Ztop - x1)*(y2-y1)/(x2 -x1);
        }
        else
            PCV12[1] = PCOPCV2.PCVNoAZI.at(flagZ);

    }
    else
    {/*Bilinear interpolation*/
        if (flagZ >= XZen.length() - 1)		flagZ--;/*Judging the out of bounds, that is,
                                                     *the height angle is greater than 90 degrees, generally not*/
        if (flagA >= XAzi.length() - 1)		flagA--;/*Judging the out of bounds, that is,
                                                     *the azimuth is greater than 360 degrees, generally not*/
        double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
        double y1 = XAzi.at(flagA),y2 = XAzi.at(flagA+1);
        int RowNum = 0,ColumnNum = 0;/*Calculate the number of rows and columns in the matrix*/
        if (tempAntData.DAZI !=  0)
            RowNum = (int)(360/m_RecvData.DAZI) + 1;
        ColumnNum = (int)((tempAntData.ZEN_12N[1] - tempAntData.ZEN_12N[0])/tempAntData.ZEN_12N[2]) + 1;
        double z11 = PCOPCV2.PCVAZI.at(ColumnNum*flagA+flagZ),z12 = PCOPCV2.PCVAZI.at(ColumnNum*flagA+flagZ+1),
            z21 = PCOPCV2.PCVAZI.at(ColumnNum*(flagA+1)+flagZ),z22 = PCOPCV2.PCVAZI.at(ColumnNum*(flagA+1)+flagZ+1);
        double z1_2 = z11 + (Ztop - x1)*(z12 - z11)/(x2 - x1),
            z2_2 = z21 + (Ztop - x1)*(z22 - z21)/(x2 - x1);
        PCV12[1] = z1_2 + (AZI - y1)*(z2_2 - z1_2)/(y2 - y1);

    }
    /*Calculate PCV3*/
    if (PCOPCV3_len == 0)
    {/*Single linear interpolation*/
        if(PCOPCV3.PCVNoAZI.length() == 0) return false;
        if (flagZ < XZen.length() - 1)
        {
            double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
            double y1 = PCOPCV3.PCVNoAZI.at(flagZ),y2 = PCOPCV3.PCVNoAZI.at(flagZ+1);
            PCV12[2] = y1+(Ztop - x1)*(y2-y1)/(x2 -x1);
        }
        else
            PCV12[2] = PCOPCV3.PCVNoAZI.at(flagZ);

    }
    else
    {/*Bilinear interpolation*/
        if (flagZ >= XZen.length() - 1)		flagZ--;/*Judging the out of bounds, that is,
                                                     *the height angle is greater than 90 degrees, generally not*/
        if (flagA >= XAzi.length() - 1)		flagA--;/*Judging the out of bounds, that is,
                                                     *the azimuth is greater than 360 degrees, generally not*/
        double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
        double y1 = XAzi.at(flagA),y2 = XAzi.at(flagA+1);
        int RowNum = 0,ColumnNum = 0;/*Calculate the number of rows and columns in the matrix*/
        if (tempAntData.DAZI !=  0)
            RowNum = (int)(360/m_RecvData.DAZI) + 1;
        ColumnNum = (int)((tempAntData.ZEN_12N[1] - tempAntData.ZEN_12N[0])/tempAntData.ZEN_12N[2]) + 1;
        double z11 = PCOPCV3.PCVAZI.at(ColumnNum*flagA+flagZ),z12 = PCOPCV3.PCVAZI.at(ColumnNum*flagA+flagZ+1),
            z21 = PCOPCV3.PCVAZI.at(ColumnNum*(flagA+1)+flagZ),z22 = PCOPCV3.PCVAZI.at(ColumnNum*(flagA+1)+flagZ+1);
        double z1_2 = z11 + (Ztop - x1)*(z12 - z11)/(x2 - x1),
            z2_2 = z21 + (Ztop - x1)*(z22 - z21)/(x2 - x1);
        PCV12[2] = z1_2 + (AZI - y1)*(z2_2 - z1_2)/(y2 - y1);

    }
    /*Calculate PCV4*/
    if (PCOPCV4_len == 0)
    {/*Single linear interpolation*/
        if(PCOPCV4.PCVNoAZI.length() == 0) return false;
        if (flagZ < XZen.length() - 1)
        {
            double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
            double y1 = PCOPCV4.PCVNoAZI.at(flagZ),y2 = PCOPCV4.PCVNoAZI.at(flagZ+1);
            PCV12[3] = y1+(Ztop - x1)*(y2-y1)/(x2 -x1);
        }
        else
            PCV12[3] = PCOPCV4.PCVNoAZI.at(flagZ);

    }
    else
    {/*Bilinear interpolation*/
        if (flagZ >= XZen.length() - 1)		flagZ--;
        if (flagA >= XAzi.length() - 1)		flagA--;
        double x1 = XZen.at(flagZ),x2 = XZen.at(flagZ+1);
        double y1 = XAzi.at(flagA),y2 = XAzi.at(flagA+1);
        int RowNum = 0,ColumnNum = 0;/*Calculate the number of rows and columns in the matrix*/
        if (tempAntData.DAZI !=  0)
            RowNum = (int)(360/m_RecvData.DAZI) + 1;
        ColumnNum = (int)((tempAntData.ZEN_12N[1] - tempAntData.ZEN_12N[0])/tempAntData.ZEN_12N[2]) + 1;
        double z11 = PCOPCV4.PCVAZI.at(ColumnNum*flagA+flagZ),z12 = PCOPCV4.PCVAZI.at(ColumnNum*flagA+flagZ+1),
            z21 = PCOPCV4.PCVAZI.at(ColumnNum*(flagA+1)+flagZ),z22 = PCOPCV4.PCVAZI.at(ColumnNum*(flagA+1)+flagZ+1);
        double z1_2 = z11 + (Ztop - x1)*(z12 - z11)/(x2 - x1),
            z2_2 = z21 + (Ztop - x1)*(z22 - z21)/(x2 - x1);
        PCV12[3] = z1_2 + (AZI - y1)*(z2_2 - z1_2)/(y2 - y1);

    }

    return true;
}

/*Calculate the correction of the receiver antenna and correct the PCO and PCV to the direction of the satellite signal*/
bool read_antcorr::getRecvL12(double E, double A, char SatType,double *LxOffset, QVector<QString> FrqFlag)
{
    for(int i=0;i<5;i++)
        LxOffset[i] = 0;
    if (!isInSystem(SatType))	return false;
    if (!m_RecvData.isFrqData) return false;
    if (!isReadRecvData) readRecvData();
    FrqData PCOPCV1,PCOPCV2,PCOPCV3,PCOPCV4,PCOPCV5;
    QString FrqStr1 = "C1C", FrqStr2 = "C2C", FrqStr3 = "C3C", FrqStr4 = "C4C", FrqStr5 = "C5C";
    if(FrqFlag.length() > 0) FrqStr1 = FrqFlag.at(0);
    if(FrqFlag.length() > 2) FrqStr2 = FrqFlag.at(2);
    if(FrqFlag.length() > 4) FrqStr3 = FrqFlag.at(4);
    if(FrqFlag.length() > 6) FrqStr4 = FrqFlag.at(6);
    if(FrqFlag.length() > 8) FrqStr5 = FrqFlag.at(8);


    QString destFlag1 = QString(SatType) + "0" + FrqStr1.mid(1,1),
            destFlag2 = QString(SatType) + "0" + FrqStr2.mid(1,1),
            destFlag3 = QString(SatType) + "0" + FrqStr3.mid(1,1),
            destFlag4 = QString(SatType) + "0" + FrqStr4.mid(1,1),
            destFlag5 = QString(SatType) + "0" + FrqStr5.mid(1,1);
    bool IsFind1 = false,IsFind2 = false,IsFind3 = false,IsFind4 = false,IsFind5 = false;
    for (int i = 0;i < m_RecvData.PCOPCV.length();i++)
    {/*This applies only to L1 and L2 of GPS and G1 and G2 of GLONASS. But calculate the BDS and Galieo satellite antennas!!!!!!!!!!!!!
        There is no 01 and 02, but the satellite's PCO and PCV are the same in any frequency band. It is not perfect.*/
        QString LFlag = m_RecvData.PCOPCV.at(i).FrqFlag.trimmed();

        if (LFlag == destFlag1)
        {
            PCOPCV1 = m_RecvData.PCOPCV.at(i);
            IsFind1 = true;
        }
        else if (LFlag == destFlag2)
        {
            PCOPCV2 = m_RecvData.PCOPCV.at(i);
            IsFind2 = true;
        }
        else if (LFlag == destFlag3)
        {
            PCOPCV3 = m_RecvData.PCOPCV.at(i);
            IsFind3 = true;
        }
        else if (LFlag == destFlag4)
        {
            PCOPCV4 = m_RecvData.PCOPCV.at(i);
            IsFind4 = true;
        }
        else if (LFlag == destFlag5)
        {
            PCOPCV5 = m_RecvData.PCOPCV.at(i);
            IsFind5 = true;
        }
    }
    /*Can't find GPS correction, for example, without Geileo and BDS, use GPS correction*/
    if(!IsFind1 && m_RecvData.PCOPCV.length() > 0) PCOPCV1 = m_RecvData.PCOPCV.at(0);
    if(!IsFind2 && m_RecvData.PCOPCV.length() > 1) PCOPCV2 = m_RecvData.PCOPCV.at(1);
    if(!IsFind3 && m_RecvData.PCOPCV.length() > 2) PCOPCV3 = m_RecvData.PCOPCV.at(2);
    if(!IsFind4 && m_RecvData.PCOPCV.length() > 3) PCOPCV4 = m_RecvData.PCOPCV.at(3);
    if(!IsFind5 && m_RecvData.PCOPCV.length() > 4) PCOPCV5 = m_RecvData.PCOPCV.at(4);

    /*Get PCO // NORTH / EAST / UP*/
    double *PCO1 = PCOPCV1.PCO,*PCO2 = PCOPCV2.PCO,*PCO3 = PCOPCV3.PCO,*PCO4 = PCOPCV4.PCO,*PCO5 = PCOPCV5.PCO;
    /*Calculate PCV below*/
    double PCV12[5]={0};
    double Ztop = MM_PI/2 - E;/*Altitude angle converted to zenith angle*/
    getPCV(m_RecvData,SatType,PCV12,Ztop,A, FrqFlag);
    /*Calculate the offset of L1 L2 in the incident direction of the satellite signal mm*/
    LxOffset[0] = -PCV12[0] + PCO1[0]*qCos(E)*qCos(A)+PCO1[1]*qCos(E)*qSin(A)+PCO1[2]*qSin(E);
    LxOffset[1] = -PCV12[1] + PCO2[0]*qCos(E)*qCos(A)+PCO2[1]*qCos(E)*qSin(A)+PCO2[2]*qSin(E);
    LxOffset[2] = -PCV12[2] + PCO3[0]*qCos(E)*qCos(A)+PCO3[1]*qCos(E)*qSin(A)+PCO3[2]*qSin(E);
    LxOffset[3] = -PCV12[3] + PCO4[0]*qCos(E)*qCos(A)+PCO4[1]*qCos(E)*qSin(A)+PCO4[2]*qSin(E);
    LxOffset[4] = -PCV12[4] + PCO5[0]*qCos(E)*qCos(A)+PCO5[1]*qCos(E)*qSin(A)+PCO5[2]*qSin(E);
    LxOffset[0] = LxOffset[0]/1000;
    LxOffset[1] = LxOffset[1]/1000;
    LxOffset[2] = LxOffset[2]/1000;
    LxOffset[3] = LxOffset[3]/1000;
    LxOffset[4] = LxOffset[4]/1000;/*Return meter*/
    return true;
}

/*Calculate the correction of the receiver antenna,
 * correct the PCO and PCV to the satellite signal direction,
 * and search for the first two frequencies according to QVector<QString> FrqFlag.
 * The L12Offset dimension must be greater than 2.*/
bool read_antcorr::getSatOffSet(double *timeV,int PRN,char SatType,double *StaPos,double *RecPos,
                              double *L12Offset, QVector<QString> FrqFlag)
{
    L12Offset[0] = 0; L12Offset[1] = 0;L12Offset[2] = 0; L12Offset[3] = 0;L12Offset[4] = 0;
    if (!isReadSatAntData) return false;/*Determine if the data has been read*/
    if (!isInSystem(SatType))	return false;	/*Determine if it is within the set system*/
    /*Two differences greater than 0.8s are
    considered to replace the epoch to recalculate the sun coordinates*/
    if (qAbs(m_sunSecFlag - (timeV[3]*3600+timeV[4]*60+timeV[5])) > 0.3)
    {/*Calculate the sun coordinates once per epoch, without repeating a lot of calculations*/
        getSunMoonPos(timeV[0],timeV[1],timeV[2],timeV[3],timeV[4],timeV[5],m_sunpos,m_moonpos,&m_gmst);/*An epoch only needs to be calculated once,
                                                                                                    no need to calculate multiple times*/
        m_sunSecFlag = timeV[3]*3600+timeV[4]*60+timeV[5];
    }
    /*Calculate the coordinate system of the star-solid system in the inertial system.
     * The unit vector indicates the ex, ey, and ez axis.*/
    double ex[3]={0},ey[3]={0},ez[3]={0};
    double lenStaPos = qSqrt(dot(StaPos,StaPos,3));/*Calculate the length of the satellite vector*/
    /*Computing satellite and solar cross product*/
    double vectSunSat[3] = {0},lenSunSta = 0.0;
    cross3(StaPos,m_sunpos,vectSunSat);
    lenSunSta = qSqrt(dot(vectSunSat,vectSunSat,3));
    /*Calculate the X-axis length vector*/
    double vectEZ[3] = {0},lenEZ = 0;
    cross3(StaPos,vectSunSat,vectEZ);
    lenEZ = qSqrt(dot(vectEZ,vectEZ,3));
    for (int i = 0;i < 3;i++)
    {
        ez[i] = -StaPos[i]/lenStaPos;
        ey[i] = -vectSunSat[i]/lenSunSta;
        ex[i] = -vectEZ[i]/lenEZ;
    }
    /*Obtain the satellite's PCO and PCV (need to determine the type of satellite)*/
    AntDataType m_SatTemp;
    int tempPRN = 0;
    char tempSatType = '0';
    bool Isfind = false;
    for (int i = 0;i < m_SatData.length();i++)
    {/*Traversing the satellite antenna data required for the query*/
        m_SatTemp = m_SatData.at(i);
        tempPRN = m_SatTemp.SatliCNN.mid(1,2).toInt();
        tempSatType = *(m_SatTemp.SatliCNN.mid(0,1).toLatin1().data());
        if (tempPRN == PRN&&tempSatType == SatType)
        {
            Isfind = true;
            break;
        }
    }
    /*The satellite antenna data was not found*/
    if (!Isfind)	return false;
    /*find PCO data add by xiaogongwei 2019.04.12*/
    double PCOL1ecef[3] = {0},PCOL1[3] = {0},
           PCOL2ecef[3] = {0},PCOL2[3] = {0},
           PCOL3ecef[3] = {0},PCOL3[3] = {0},
           PCOL4ecef[3] = {0},PCOL4[3] = {0},
           PCOL5ecef[3] = {0},PCOL5[3] = {0};
    QString FrqStr1 = "C1C", FrqStr2 = "C2C", FrqStr3 = "C3C", FrqStr4 = "C4C", FrqStr5 = "C5C";
    if(FrqFlag.length() > 0) FrqStr1 = FrqFlag.at(0);
    if(FrqFlag.length() > 2) FrqStr2 = FrqFlag.at(2);
    if(FrqFlag.length() > 4) FrqStr3 = FrqFlag.at(4);
    if(FrqFlag.length() > 6) FrqStr4 = FrqFlag.at(6);
    if(FrqFlag.length() > 8) FrqStr5 = FrqFlag.at(8);
    findPCO(PCOL1, m_SatTemp.PCOPCV,  SatType,  FrqStr1);
    findPCO(PCOL2, m_SatTemp.PCOPCV,  SatType,  FrqStr2);
    findPCO(PCOL3, m_SatTemp.PCOPCV,  SatType,  FrqStr3);
    findPCO(PCOL4, m_SatTemp.PCOPCV,  SatType,  FrqStr4);
    findPCO(PCOL5, m_SatTemp.PCOPCV,  SatType,  FrqStr5);
    /*PCO conversion to ground fixed coordinate system ecef*/
    for (int i = 0;i < 3;i++)
    {
        PCOL1ecef[i] = ex[i]*PCOL1[0] + ey[i]*PCOL1[1] + ez[i]*PCOL1[2];
        PCOL2ecef[i] = ex[i]*PCOL2[0] + ey[i]*PCOL2[1] + ez[i]*PCOL2[2];
        PCOL3ecef[i] = ex[i]*PCOL3[0] + ey[i]*PCOL3[1] + ez[i]*PCOL3[2];
        PCOL4ecef[i] = ex[i]*PCOL4[0] + ey[i]*PCOL4[1] + ez[i]*PCOL4[2];
        PCOL5ecef[i] = ex[i]*PCOL5[0] + ey[i]*PCOL5[1] + ez[i]*PCOL5[2];
    }
    /*Calculate the approximate coordinate vector of the satellite and receiver*/
    double sat2recV[3] = {RecPos[0] - StaPos[0],RecPos[1] - StaPos[1],RecPos[2] - StaPos[2]};
    double lenSat2RecV = qSqrt(dot(sat2recV,sat2recV,3));
    double sat2recVE[3] = {sat2recV[0]/lenSat2RecV,sat2recV[1]/lenSat2RecV,sat2recV[2]/lenSat2RecV};
    double AntHigPCOL1 = dot(PCOL1ecef,sat2recVE,3),
           AntHigPCOL2 = dot(PCOL2ecef,sat2recVE,3),
           AntHigPCOL3 = dot(PCOL3ecef,sat2recVE,3),
           AntHigPCOL4 = dot(PCOL4ecef,sat2recVE,3),
           AntHigPCOL5 = dot(PCOL5ecef,sat2recVE,3);

    /*Calculate PCV below*/
    double PCVAngle = qAcos(dot(ez,sat2recVE,3));
    if (PCVAngle < 0|| PCVAngle > 0.244346095) //0.244346095 is 14 degree
    {
        PCVAngle = MM_PI - PCVAngle;
    }
    double PCV12[5]={0};
    getPCV(m_SatTemp,SatType,PCV12,PCVAngle,0, FrqFlag);/* 0 is degree of satlite Azimuth*/
    if(ant_wind_up)
    {
        L12Offset[0] = (AntHigPCOL1 - PCV12[0])/1000;/*Return unit（m）*/
        L12Offset[1] = (AntHigPCOL2 - PCV12[1])/1000;/*Return unit（m）*/
        L12Offset[2] = (AntHigPCOL3 - PCV12[2])/1000;/*Return unit（m）*/
        L12Offset[3] = (AntHigPCOL4 - PCV12[3])/1000;/*Return unit（m）*/
        L12Offset[4] = (AntHigPCOL5 - PCV12[4])/1000;/*Return unit（m）*/
        L12Offset[5] = (AntHigPCOL1)/1000;/*Return unit（m）*/
        L12Offset[6] = (AntHigPCOL2)/1000;/*Return unit（m）*/
        L12Offset[7] = (AntHigPCOL3)/1000;/*Return unit（m）*/
        L12Offset[8] = (AntHigPCOL4)/1000;/*Return unit（m）*/
        L12Offset[9] = (AntHigPCOL5)/1000;/*Return unit（m）*/
    }
    else
    {
//        L12Offset[0] = 0;/*Return unit（m）*/
//        L12Offset[1] = 0;/*Return unit（m）*/
//        L12Offset[2] = 0;/*Return unit（m）*/
//        L12Offset[3] = 0;/*Return unit（m）*/
//        L12Offset[4] = 0;/*Return unit（m）*/

        L12Offset[0] = ( - PCV12[0])/1000;/*Return unit（m）*/
        L12Offset[1] = ( - PCV12[1])/1000;/*Return unit（m）*/
        L12Offset[2] = ( - PCV12[2])/1000;/*Return unit（m）*/
        L12Offset[3] = ( - PCV12[3])/1000;/*Return unit（m）*/
        L12Offset[4] = ( - PCV12[4])/1000;/*Return unit（m）*/
    }
    return true;
}
/*Pco dimension must be greater than 3*/
void read_antcorr::findPCO(double *pco, QVector<FrqData> &PCOPCV, char SatType, QString frqStr)
{
    memset(pco, 0, 3*sizeof(double));
    QString frqFlag = QString(SatType) + "0" + frqStr.mid(1,1);
    for(int i = 0; i < PCOPCV.length();i++)
    {
        if(PCOPCV.at(i).FrqFlag == frqFlag)
        {
            pco[0] = PCOPCV.at(i).PCO[0];
            pco[1] = PCOPCV.at(i).PCO[1];
            pco[2] = PCOPCV.at(i).PCO[2];
        }
    }
}
/*Calculate the correction of the receiver antenna,
 * correct the PCO and PCV to the satellite signal direction,
 * and search for the first two frequencies according to QVector<QString> FrqFlag.
 * The L12Offset dimension must be greater than 2.*/
bool read_antcorr::getSatOffSet_quater(double *timeV,int PRN,char SatType,double *StaPos,double *RecPos,
                              double *L12Offset, QVector<QString> FrqFlag,MatrixXd quater)
{
    L12Offset[0] = 0; L12Offset[1] = 0;L12Offset[2] = 0; L12Offset[3] = 0;L12Offset[4] = 0;
    if (!isReadSatAntData) return false;/*Determine if the data has been read*/
    if (!isInSystem(SatType))	return false;	/*Determine if it is within the set system*/
    /*Two differences greater than 0.8s are
    considered to replace the epoch to recalculate the sun coordinates*/
    if (qAbs(m_sunSecFlag - (timeV[3]*3600+timeV[4]*60+timeV[5])) > 0.3)
    {/*Calculate the sun coordinates once per epoch, without repeating a lot of calculations*/
        getSunMoonPos(timeV[0],timeV[1],timeV[2],timeV[3],timeV[4],timeV[5],m_sunpos,m_moonpos,&m_gmst);/*An epoch only needs to be calculated once,
                                                                                                    no need to calculate multiple times*/
        m_sunSecFlag = timeV[3]*3600+timeV[4]*60+timeV[5];
    }
    /*Calculate the coordinate system of the star-solid system in the inertial system.
     * The unit vector indicates the ex, ey, and ez axis.*/
    double ex[3]={0},ey[3]={0},ez[3]={0};
    double lenStaPos = qSqrt(dot(StaPos,StaPos,3));/*Calculate the length of the satellite vector*/
    /*Computing satellite and solar cross product*/
    double vectSunSat[3] = {0},lenSunSta = 0.0;
    cross3(StaPos,m_sunpos,vectSunSat);
    lenSunSta = qSqrt(dot(vectSunSat,vectSunSat,3));
    /*Calculate the X-axis length vector*/
    double vectEZ[3] = {0},lenEZ = 0;
    cross3(StaPos,vectSunSat,vectEZ);
    lenEZ = qSqrt(dot(vectEZ,vectEZ,3));
    for (int i = 0;i < 3;i++)
    {
        ez[i] = quater(2,i);
        ey[i] = quater(1,i);
        ex[i] = quater(0,i);
    }
    /*Obtain the satellite's PCO and PCV (need to determine the type of satellite)*/
    AntDataType m_SatTemp;
    int tempPRN = 0;
    char tempSatType = '0';
    bool Isfind = false;
    for (int i = 0;i < m_SatData.length();i++)
    {/*Traversing the satellite antenna data required for the query*/
        m_SatTemp = m_SatData.at(i);
        tempPRN = m_SatTemp.SatliCNN.mid(1,2).toInt();
        tempSatType = *(m_SatTemp.SatliCNN.mid(0,1).toLatin1().data());
        if (tempPRN == PRN&&tempSatType == SatType)
        {
            Isfind = true;
            break;
        }
    }
    /*The satellite antenna data was not found*/
    if (!Isfind)	return false;
    /*find PCO data add by xiaogongwei 2019.04.12*/
    double PCOL1ecef[3] = {0},PCOL1[3] = {0},
           PCOL2ecef[3] = {0},PCOL2[3] = {0},
           PCOL3ecef[3] = {0},PCOL3[3] = {0},
           PCOL4ecef[3] = {0},PCOL4[3] = {0},
           PCOL5ecef[3] = {0},PCOL5[3] = {0};
    QString FrqStr1 = "C1C", FrqStr2 = "C2C", FrqStr3 = "C3C", FrqStr4 = "C4C", FrqStr5 = "C5C";
    if(FrqFlag.length() > 0) FrqStr1 = FrqFlag.at(0);
    if(FrqFlag.length() > 2) FrqStr2 = FrqFlag.at(2);
    if(FrqFlag.length() > 4) FrqStr3 = FrqFlag.at(4);
    if(FrqFlag.length() > 6) FrqStr4 = FrqFlag.at(6);
    if(FrqFlag.length() > 8) FrqStr5 = FrqFlag.at(8);
    findPCO(PCOL1, m_SatTemp.PCOPCV,  SatType,  FrqStr1);
    findPCO(PCOL2, m_SatTemp.PCOPCV,  SatType,  FrqStr2);
    findPCO(PCOL3, m_SatTemp.PCOPCV,  SatType,  FrqStr3);
    findPCO(PCOL4, m_SatTemp.PCOPCV,  SatType,  FrqStr4);
    findPCO(PCOL5, m_SatTemp.PCOPCV,  SatType,  FrqStr5);
    /*PCO conversion to ground fixed coordinate system ecef*/
    for (int i = 0;i < 3;i++)
    {
        PCOL1ecef[i] = ex[i]*PCOL1[0] + ey[i]*PCOL1[1] + ez[i]*PCOL1[2];
        PCOL2ecef[i] = ex[i]*PCOL2[0] + ey[i]*PCOL2[1] + ez[i]*PCOL2[2];
        PCOL3ecef[i] = ex[i]*PCOL3[0] + ey[i]*PCOL3[1] + ez[i]*PCOL3[2];
        PCOL4ecef[i] = ex[i]*PCOL4[0] + ey[i]*PCOL4[1] + ez[i]*PCOL4[2];
        PCOL5ecef[i] = ex[i]*PCOL5[0] + ey[i]*PCOL5[1] + ez[i]*PCOL5[2];
    }
    /*Calculate the approximate coordinate vector of the satellite and receiver*/
    double sat2recV[3] = {RecPos[0] - StaPos[0],RecPos[1] - StaPos[1],RecPos[2] - StaPos[2]};
    double lenSat2RecV = qSqrt(dot(sat2recV,sat2recV,3));
    double sat2recVE[3] = {sat2recV[0]/lenSat2RecV,sat2recV[1]/lenSat2RecV,sat2recV[2]/lenSat2RecV};
    double AntHigPCOL1 = dot(PCOL1ecef,sat2recVE,3),
           AntHigPCOL2 = dot(PCOL2ecef,sat2recVE,3),
           AntHigPCOL3 = dot(PCOL3ecef,sat2recVE,3),
           AntHigPCOL4 = dot(PCOL4ecef,sat2recVE,3),
           AntHigPCOL5 = dot(PCOL5ecef,sat2recVE,3);

    /*Calculate PCV below*/
    double acosvalue = dot(ez,sat2recVE,3);
    if(acosvalue>1)
        acosvalue = 1;
    double PCVAngle = qAcos(acosvalue);
    if (PCVAngle < 0|| PCVAngle > 0.244346095) //0.244346095 is 14 degree
    {
        PCVAngle = MM_PI - PCVAngle;
    }
    double PCV12[5]={0};
    getPCV(m_SatTemp,SatType,PCV12,PCVAngle,0, FrqFlag);/* 0 is degree of satlite Azimuth*/
    if(ant_wind_up)
    {
        L12Offset[0] = (AntHigPCOL1 - PCV12[0])/1000;/*Return unit（m）*/
        L12Offset[1] = (AntHigPCOL2 - PCV12[1])/1000;/*Return unit（m）*/
        L12Offset[2] = (AntHigPCOL3 - PCV12[2])/1000;/*Return unit（m）*/
        L12Offset[3] = (AntHigPCOL4 - PCV12[3])/1000;/*Return unit（m）*/
        L12Offset[4] = (AntHigPCOL5 - PCV12[4])/1000;/*Return unit（m）*/
        L12Offset[5] = (AntHigPCOL1)/1000;/*Return unit（m）*/
        L12Offset[6] = (AntHigPCOL2)/1000;/*Return unit（m）*/
        L12Offset[7] = (AntHigPCOL3)/1000;/*Return unit（m）*/
        L12Offset[8] = (AntHigPCOL4)/1000;/*Return unit（m）*/
        L12Offset[9] = (AntHigPCOL5)/1000;/*Return unit（m）*/
    }
    else
    {
//        L12Offset[0] = 0;/*Return unit（m）*/
//        L12Offset[1] = 0;/*Return unit（m）*/
//        L12Offset[2] = 0;/*Return unit（m）*/
//        L12Offset[3] = 0;/*Return unit（m）*/
//        L12Offset[4] = 0;/*Return unit（m）*/

        L12Offset[0] = ( - PCV12[0])/1000;/*Return unit（m）*/
        L12Offset[1] = ( - PCV12[1])/1000;/*Return unit（m）*/
        L12Offset[2] = ( - PCV12[2])/1000;/*Return unit（m）*/
        L12Offset[3] = ( - PCV12[3])/1000;/*Return unit（m）*/
        L12Offset[4] = ( - PCV12[4])/1000;/*Return unit（m）*/
    }
    if(L12Offset[7]==0)
    {
        L12Offset[7]=L12Offset[2]=L12Offset[6];
    }
    return true;
}
/*Calculate the correction of the receiver antenna,
 * correct the PCO and PCV to the satellite signal direction,
 * and search for the first two frequencies according to QVector<QString> FrqFlag.
 * The L12Offset dimension must be greater than 2.*/
bool read_antcorr::getSatOffSet_RTyaw(double *timeV,int PRN,char SatType,double *StaPos,double *RecPos,
                              double *sat_Ve,double *L12Offset, QVector<QString> FrqFlag,double yaw)
{
    L12Offset[0] = 0; L12Offset[1] = 0;L12Offset[2] = 0; L12Offset[3] = 0;L12Offset[4] = 0;
    if (!isReadSatAntData) return false;/*Determine if the data has been read*/
    if (!isInSystem(SatType))	return false;	/*Determine if it is within the set system*/
    /*Two differences greater than 0.8s are
    considered to replace the epoch to recalculate the sun coordinates*/
    if (qAbs(m_sunSecFlag - (timeV[3]*3600+timeV[4]*60+timeV[5])) > 0.3)
    {/*Calculate the sun coordinates once per epoch, without repeating a lot of calculations*/
        getSunMoonPos(timeV[0],timeV[1],timeV[2],timeV[3],timeV[4],timeV[5],m_sunpos,m_moonpos,&m_gmst);/*An epoch only needs to be calculated once,
                                                                                                    no need to calculate multiple times*/
        m_sunSecFlag = timeV[3]*3600+timeV[4]*60+timeV[5];
    }
    /*Calculate the coordinate system of the star-solid system in the inertial system.
     * The unit vector indicates the ex, ey, and ez axis.*/
    double ex[3]={0},ey[3]={0},ez[3]={0},satV[3];
    double lenStaPos = qSqrt(dot(StaPos,StaPos,3));/*Calculate the length of the satellite vector*/
    /*Computing satellite and solar cross product*/
    double vectSunSat[3] = {0},lenSunSta = 0.0;
    cross3(StaPos,m_sunpos,vectSunSat);
    lenSunSta = qSqrt(dot(vectSunSat,vectSunSat,3));
    /*Calculate the X-axis length vector*/
    double vectEZ[3] = {0},lenEZ = 0,n[3]={0},Ex[3]={0},En[3]={0},Es[3]={0};
    cross3(StaPos,vectSunSat,vectEZ);
    satV[0]=sat_Ve[0]-OMGE*StaPos[1];
    satV[1]=sat_Ve[1]+OMGE*StaPos[0];
    satV[2]=sat_Ve[2];
    cross3(StaPos,satV,n);
    if (!normv3(StaPos,Es)||!normv3(n,En)) return false;
    lenEZ = qSqrt(dot(vectEZ,vectEZ,3));
    /* satellite fixed x,y-vector */
    cross3(En,Es,Ex);
    double cosy,siny;
    cosy=cos(yaw);
    siny=sin(yaw);
    for (int i=0;i<3;i++) {
        ex[i]=-siny*En[i]+cosy*Ex[i];
        ey[i]=-cosy*En[i]-siny*Ex[i];
        ez[i] = -StaPos[i]/lenStaPos;
    }
    /*Obtain the satellite's PCO and PCV (need to determine the type of satellite)*/
    AntDataType m_SatTemp;
    int tempPRN = 0;
    char tempSatType = '0';
    bool Isfind = false;
    for (int i = 0;i < m_SatData.length();i++)
    {/*Traversing the satellite antenna data required for the query*/
        m_SatTemp = m_SatData.at(i);
        tempPRN = m_SatTemp.SatliCNN.mid(1,2).toInt();
        tempSatType = *(m_SatTemp.SatliCNN.mid(0,1).toLatin1().data());
        if (tempPRN == PRN&&tempSatType == SatType)
        {
            Isfind = true;
            break;
        }
    }
    /*The satellite antenna data was not found*/
    if (!Isfind)	return false;
    /*find PCO data add by xiaogongwei 2019.04.12*/
    double PCOL1ecef[3] = {0},PCOL1[3] = {0},
           PCOL2ecef[3] = {0},PCOL2[3] = {0},
           PCOL3ecef[3] = {0},PCOL3[3] = {0},
           PCOL4ecef[3] = {0},PCOL4[3] = {0},
           PCOL5ecef[3] = {0},PCOL5[3] = {0};
    QString FrqStr1 = "C1C", FrqStr2 = "C2C", FrqStr3 = "C3C", FrqStr4 = "C4C", FrqStr5 = "C5C";
    if(FrqFlag.length() > 0) FrqStr1 = FrqFlag.at(0);
    if(FrqFlag.length() > 2) FrqStr2 = FrqFlag.at(2);
    if(FrqFlag.length() > 4) FrqStr3 = FrqFlag.at(4);
    if(FrqFlag.length() > 6) FrqStr4 = FrqFlag.at(6);
    if(FrqFlag.length() > 8) FrqStr5 = FrqFlag.at(8);
    findPCO(PCOL1, m_SatTemp.PCOPCV,  SatType,  FrqStr1);
    findPCO(PCOL2, m_SatTemp.PCOPCV,  SatType,  FrqStr2);
    findPCO(PCOL3, m_SatTemp.PCOPCV,  SatType,  FrqStr3);
    findPCO(PCOL4, m_SatTemp.PCOPCV,  SatType,  FrqStr4);
    findPCO(PCOL5, m_SatTemp.PCOPCV,  SatType,  FrqStr5);
    /*PCO conversion to ground fixed coordinate system ecef*/
    for (int i = 0;i < 3;i++)
    {
        PCOL1ecef[i] = ex[i]*PCOL1[0] + ey[i]*PCOL1[1] + ez[i]*PCOL1[2];
        PCOL2ecef[i] = ex[i]*PCOL2[0] + ey[i]*PCOL2[1] + ez[i]*PCOL2[2];
        PCOL3ecef[i] = ex[i]*PCOL3[0] + ey[i]*PCOL3[1] + ez[i]*PCOL3[2];
        PCOL4ecef[i] = ex[i]*PCOL4[0] + ey[i]*PCOL4[1] + ez[i]*PCOL4[2];
        PCOL5ecef[i] = ex[i]*PCOL5[0] + ey[i]*PCOL5[1] + ez[i]*PCOL5[2];
    }
    /*Calculate the approximate coordinate vector of the satellite and receiver*/
    double sat2recV[3] = {RecPos[0] - StaPos[0],RecPos[1] - StaPos[1],RecPos[2] - StaPos[2]};
    double lenSat2RecV = qSqrt(dot(sat2recV,sat2recV,3));
    double sat2recVE[3] = {sat2recV[0]/lenSat2RecV,sat2recV[1]/lenSat2RecV,sat2recV[2]/lenSat2RecV};
    double AntHigPCOL1 = dot(PCOL1ecef,sat2recVE,3),
           AntHigPCOL2 = dot(PCOL2ecef,sat2recVE,3),
           AntHigPCOL3 = dot(PCOL3ecef,sat2recVE,3),
           AntHigPCOL4 = dot(PCOL4ecef,sat2recVE,3),
           AntHigPCOL5 = dot(PCOL5ecef,sat2recVE,3);

    /*Calculate PCV below*/
    double PCVAngle = qAcos(dot(ez,sat2recVE,3));
    if (PCVAngle < 0|| PCVAngle > 0.244346095) //0.244346095 is 14 degree
    {
        PCVAngle = MM_PI - PCVAngle;
    }
    double PCV12[5]={0};
    getPCV(m_SatTemp,SatType,PCV12,PCVAngle,0, FrqFlag);/* 0 is degree of satlite Azimuth*/
    if(ant_wind_up)
    {
        L12Offset[0] = (AntHigPCOL1 - PCV12[0])/1000;/*Return unit（m）*/
        L12Offset[1] = (AntHigPCOL2 - PCV12[1])/1000;/*Return unit（m）*/
        L12Offset[2] = (AntHigPCOL3 - PCV12[2])/1000;/*Return unit（m）*/
        L12Offset[3] = (AntHigPCOL4 - PCV12[3])/1000;/*Return unit（m）*/
        L12Offset[4] = (AntHigPCOL5 - PCV12[4])/1000;/*Return unit（m）*/
    }
    else
    {
//        L12Offset[0] = 0;/*Return unit（m）*/
//        L12Offset[1] = 0;/*Return unit（m）*/
//        L12Offset[2] = 0;/*Return unit（m）*/
//        L12Offset[3] = 0;/*Return unit（m）*/
//        L12Offset[4] = 0;/*Return unit（m）*/

        L12Offset[0] = ( - PCV12[0])/1000;/*Return unit（m）*/
        L12Offset[1] = ( - PCV12[1])/1000;/*Return unit（m）*/
        L12Offset[2] = ( - PCV12[2])/1000;/*Return unit（m）*/
        L12Offset[3] = ( - PCV12[3])/1000;/*Return unit（m）*/
        L12Offset[4] = ( - PCV12[4])/1000;/*Return unit（m）*/
    }
    return true;
}
