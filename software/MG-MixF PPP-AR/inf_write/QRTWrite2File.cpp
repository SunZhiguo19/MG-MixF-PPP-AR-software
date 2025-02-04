#include "QRTWrite2File.h"


QRTWrite2File::QRTWrite2File(void)
{
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    RT_writefile->m_save_filenum = 0;
    RT_writefile->m_ZTDW_Clock_head = false;
    RT_writefile->m_AZ_EL_head = false;
}


QRTWrite2File::~QRTWrite2File(void)
{
    closeSaveFile();
}

bool QRTWrite2File::setSaveFloder(QString fload_path)
{
    if(!isDirExist(fload_path))
    {
        QString infor = "can not construct floder: " + fload_path;
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
    }
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    closeSaveFile();
    RT_writefile->m_save_filenum = 0;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();
    QVector<QString> tempallSaveFiles;
    if(opt_corr->Az_El_coff)    tempallSaveFiles<<"Az_El_coff.txt";
    if(opt_corr->position)    tempallSaveFiles << "position.txt";
    if(opt_corr->ZTDW_Clock)    tempallSaveFiles << "ZTDW_Clock.txt";
    if(opt_corr->dop_satN_AR)    tempallSaveFiles << "dop_satN_AR.txt";
    if(opt_corr->bad_satellites)    tempallSaveFiles << "bad_satellites.txt";
    if(opt_corr->Satellite_info)    tempallSaveFiles << "Satellite_info.txt";
    if(opt_corr->ambiguity_inf)    tempallSaveFiles << "Ambiguity_information.txt";

    if(trans_para->for_and_backward_filter&&opt_corr->position)    tempallSaveFiles << "back_position.txt";
    if(trans_para->for_and_backward_filter&&opt_corr->Az_El_coff)    tempallSaveFiles << "back_Az_El_coff.txt";
    if(trans_para->for_and_backward_filter&&opt_corr->ZTDW_Clock)    tempallSaveFiles << "back_ZTDW_Clock.txt";
    if(trans_para->for_and_backward_filter&&opt_corr->dop_satN_AR)    tempallSaveFiles << "back_dop_satN_AR.txt";
    if(trans_para->for_and_backward_filter&&opt_corr->bad_satellites)    tempallSaveFiles << "back_bad_satellites.txt";
    if(trans_para->for_and_backward_filter&&opt_corr->Satellite_info)    tempallSaveFiles << "back_Satellite_info.txt";
    if(trans_para->for_and_backward_filter&&opt_corr->ambiguity_inf)    tempallSaveFiles << "back_Ambiguity_information.txt";
//    tempallSaveFiles << "qdebug.txt";
    QString ambupd_file;
    if(pravide_obs->year_acc_day<10)
        ambupd_file = pravide_obs->The_same_ant + "_ambupd_" + QString::number(pravide_obs->year_) +"00" + QString::number(pravide_obs->year_acc_day);
    else if(pravide_obs->year_acc_day<100)
        ambupd_file = pravide_obs->The_same_ant + "_ambupd_" + QString::number(pravide_obs->year_) +"0" + QString::number(pravide_obs->year_acc_day);
    else
        ambupd_file = pravide_obs->The_same_ant + "_ambupd_" + QString::number(pravide_obs->year_) + QString::number(pravide_obs->year_acc_day);

    pravide_obs->ambupd_file = ambupd_file;
//    tempallSaveFiles << ambupd_file;
    QDir pathname(fload_path);
    for(int i = 0; i < tempallSaveFiles.length();i++)
    {
        QString tempfileName = tempallSaveFiles.at(i);
        QString fileName = pathname.absoluteFilePath(tempfileName);
        RT_writefile->m_QFileVct[i].setFileName(fileName);
//        QString fileName = fload_path + tempfileName;

//        QFile saveFile(fileName);//Saved file
        if (!RT_writefile->m_QFileVct[i].open(QFile::WriteOnly|QFile::Text))
        {
            QString erroInfo = "Open " + fileName + " File Error!";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(erroInfo);
            text_disp->data_lenth++;
            return false;
        }
        RT_writefile->m_QTextStreamVct[i].setDevice(&RT_writefile->m_QFileVct[i]);
        RT_writefile->m_QFileNames[i] = fileName;
        RT_writefile->m_save_filenum++;
    }

}

void QRTWrite2File::closeSaveFile()
{
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    for(int i = 0;i < RT_writefile->m_save_filenum;i++)
    {
        if(RT_writefile->m_QFileVct[i].isOpen())
        {
            RT_writefile->m_QTextStreamVct[i].flush();
            RT_writefile->m_QFileVct[i].flush();
            RT_writefile->m_QFileVct[i].close();
            RT_writefile->m_QFileNames[i] = "";
        }
    }
}

bool QRTWrite2File::writeRecivePosKML(QString fload_path, QString tempfileName)
{
    if (allReciverPos.length() == 0)//If no data is read
        return false;
    if(!isDirExist(fload_path))
    {
        QString infor = "can not construct floder: " + fload_path;
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
    }
    QString fileName = fload_path + tempfileName;
    QFile saveFile(fileName);//Saved file
    if (!saveFile.open(QFile::WriteOnly|QFile::Text))
    {
        QString erroInfo = "Open " + fileName + " File Error!";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
        return false;
    }
    QTextStream saveFileOut(&saveFile);
    // write head information
    saveFileOut << "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>" << endl;
    saveFileOut << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << endl;
    saveFileOut << "<Document>" << endl;
    saveFileOut << "<Style id=\"yellowLineGreenPoly\">" << endl;
    saveFileOut << "<LineStyle>" << endl;
    saveFileOut << "<color>ff0400ff</color>" << endl;
    saveFileOut << "<width>1.4</width>" << endl;
    saveFileOut << "</LineStyle>" << endl;
    saveFileOut << "<PolyStyle>" << endl;
    saveFileOut << "<color>7f00ff00</color>" << endl;
    saveFileOut << "</PolyStyle>" << endl;
    saveFileOut << "</Style>" << endl;
    saveFileOut << "<Placemark>" << endl;
    saveFileOut << "<name>" << endl;
    saveFileOut << "      david_kml" << endl;
    saveFileOut << "</name>" << endl;
    saveFileOut << "<description>" << endl;
    saveFileOut << "      this is create by david_xiao.Email:xiaogongwei10@163.com" << endl;
    saveFileOut << "</description>" << endl;
    saveFileOut << "<styleUrl>#yellowLineGreenPoly</styleUrl>" << endl;
    saveFileOut << "<LineString>" << endl;
    saveFileOut << "<coordinates>" << endl;

    int lenRecivePos = allReciverPos.length();
    double temp_PI = 3.1415926535897932385;
    for (int i = 0;i <lenRecivePos;i++ )
    {
        RecivePos oneRecivePos = allReciverPos.at(i);
        if(oneRecivePos.dX == 0) continue;
        double pXYZ[3] = {oneRecivePos.dX, oneRecivePos.dY, oneRecivePos.dZ},
                pBLH[3] = {0};
        WriteXYZ2BLH(pXYZ, pBLH);
        saveFileOut << QString::number( pBLH[1]*180 / temp_PI, 'f', 12 ) << ","
                << QString::number( pBLH[0]*180 / temp_PI, 'f', 12 ) << ","
                << QString::number( pBLH[2], 'f', 12 )
                << endl;
    }
    saveFileOut << "</coordinates>" << endl;
    saveFileOut << "</LineString>" << endl;
    saveFileOut << "</Placemark>" << endl;
    saveFileOut << "</Document>" << endl;
    saveFileOut << "</kml>" << endl;
    saveFileOut.flush();
    saveFile.close();
    return true;
}

//szg write debug inf
bool QRTWrite2File::writeQdebug2Txt(QString qdebug_log)
{

    int _flag;
    _flag = findFilenameFlag("qdebug.txt");
    if(_flag != -1) {
        RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
        QTextStream &saveFileOut_dubug = RT_writefile->m_QTextStreamVct[_flag];
        saveFileOut_dubug.setFieldWidth(10);
        saveFileOut_dubug << qdebug_log;
        saveFileOut_dubug<<endl;
        saveFileOut_dubug.flush();
    }

    return true;
}
///The calculation result is written to txt
bool QRTWrite2File::writeRecivePos2Txt(const RecivePos oneRecivePos, MatrixXd *Qmat)
{
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("position.txt");
    else
        _flag = findFilenameFlag("back_position.txt");


    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];

    saveFileOut.setFieldWidth(10);
    saveFileOut<<oneRecivePos.UTCtime.epochNum;
    saveFileOut.setFieldWidth(2);
    saveFileOut<<": ";
    //Output year, month, day, hour, minute, second
    saveFileOut.setFieldWidth(4);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Year);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Month);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Day);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Hours);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Minutes);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(10);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Seconds,'f',7);

    //Calculate dE dN dU
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.totolEpochStalitNum);
    // cout SPP Pos
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.spp_pos[0],'f',4);
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.spp_pos[1],'f',4);
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.spp_pos[2],'f',4);
    saveFileOut.setFieldWidth(2);

    // cout PPP pos
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.dX,'f',4);
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.dY,'f',4);
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.dZ,'f',4);
    saveFileOut.setFieldWidth(2);

    saveFileOut<<"  ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(trans_para->mode_solution);


    if(Qmat)
    {
        // write pos zigama = sqrt(Q(i,i)), i=1,2,3
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
        saveFileOut.setFieldWidth(15);
        saveFileOut<<QString::number(sqrt(abs((*Qmat)(0,0))),'f',8);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
        saveFileOut.setFieldWidth(15);
        saveFileOut<<QString::number(sqrt(abs((*Qmat)(1,1))),'f',8);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
        saveFileOut.setFieldWidth(15);
        saveFileOut<<QString::number(sqrt(abs((*Qmat)(2,2))),'f',8);
        saveFileOut.setFieldWidth(2);
    }

    saveFileOut<<endl;
    saveFileOut.flush();

    return true;
}

//Various error corrections are written to the .ppp file
bool QRTWrite2File::writePPP2Txt(const QVector < SatlitData > epochSatlite,int out_Comb_mode)
{
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("Satellite_info.txt");
    else
        _flag = findFilenameFlag("back_Satellite_info.txt");
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];

    int StallitNumbers = epochSatlite.length();
    if(StallitNumbers == 0) return false;
    //Output the first epoch, and start tag">"
    saveFileOut.setFieldWidth(0);
    saveFileOut << ">epoch_num:";
    saveFileOut << epochSatlite.at(0).UTCTime.epochNum;
    saveFileOut << endl;
    //Output header information
    saveFileOut<<"Satellite Number:";
    saveFileOut.setFieldWidth(3);
    saveFileOut<<StallitNumbers;
    saveFileOut<<",(yyyy-mm-dd-hh-mm-ss):";
    saveFileOut.setFieldWidth(4);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Year);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Month);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Day);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Hours);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Minutes);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(10);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Seconds, 'f', 7);
    saveFileOut.setFieldWidth(6);
    saveFileOut<<",ZHD:";
    // write ZHD and Epoch Flag
    SatlitData oneStallit_t = epochSatlite.at(0);
    saveFileOut.setFieldWidth(7);
    saveFileOut<<QString::number(oneStallit_t.UTCTime.TropZHD,'f',4);
    saveFileOut.setFieldWidth(12);
    saveFileOut<<", BadFlag: ";
    saveFileOut.setFieldWidth(3);
    saveFileOut<<QString::number(oneStallit_t.EpochFlag);
    saveFileOut<<endl;
    //Output model correction information
    for (int j = 0;j < StallitNumbers;j++)
    {
        SatlitData oneStallit= epochSatlite.at(j);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<oneStallit.SatType;
        saveFileOut.setFieldWidth(2);
        if (oneStallit.PRN < 10)
            saveFileOut<<"0"+QString::number(oneStallit.PRN);
        else
            saveFileOut<<oneStallit.PRN;

        saveFileOut.setFieldWidth(2);
        saveFileOut<<": ";

        if(out_Comb_mode == Comb_UD_mode)
        {
            // ion
            saveFileOut.setFieldWidth(14);
            saveFileOut<<QString::number(oneStallit.ionL1,'f',4);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
        }
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.X,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.Y,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.Z,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.StaClock,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.EA[0],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.EA[1],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.SatTrop,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.StaTropMap,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.Relativty,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.Sagnac,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.TideEffect,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.AntHeight,'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.SatLxOffset[0],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.SatLxOffset[1],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.LxOffset[0],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.LxOffset[1],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(14);
        saveFileOut<<QString::number(oneStallit.AntWindup,'f',4);
        //QString::number(oneStallit.pos[2],'f',4);

        saveFileOut<<endl;//Output coordinates of a satellite in the epoch
    }
    saveFileOut.flush();
    return true;
}
//Various error corrections are written to the .ppp file
bool QRTWrite2File::writeAmbiguity_information(const QVector < SatlitData > epochSatlite,int out_Comb_mode,VectorXd X,MatrixXd *Xq_mat)
{
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("Ambiguity_information.txt");
    else
        _flag = findFilenameFlag("back_Ambiguity_information.txt");
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];
    int StallitNumbers = epochSatlite.length();
    if(StallitNumbers == 0) return false;
    //Output the first epoch, and start tag">"
    saveFileOut.setFieldWidth(0);
    saveFileOut << ">epoch_num:";
    saveFileOut << epochSatlite.at(0).UTCTime.epochNum;
    saveFileOut << endl;
    //Output header information
    saveFileOut<<"Satellite Number:";
    saveFileOut.setFieldWidth(1);
    saveFileOut<<StallitNumbers;
    saveFileOut<<",";
    saveFileOut.setFieldWidth(4);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Year);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Month);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Day);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Hours);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Minutes);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(5);
    saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Seconds, 'f', 2);
    saveFileOut.setFieldWidth(22);
    saveFileOut<<",reference satellite:";
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    for(int j=0;j<StallitNumbers;j++)
    {
        SatlitData oneStallit_t = epochSatlite.at(j);
        if(oneStallit_t.maxEA_PRN!=oneStallit_t.PRN)
            continue;
        saveFileOut.setFieldWidth(1);
        saveFileOut<<oneStallit_t.SatType;
        saveFileOut.setFieldWidth(2);
        if (oneStallit_t.PRN < 10)
            saveFileOut<<"0"+QString::number(oneStallit_t.PRN);
        else
            saveFileOut<<oneStallit_t.PRN;
        saveFileOut.setFieldWidth(1);
        saveFileOut<<" ";
    }
    // write ZHD and Epoch Flag
    saveFileOut.setFieldWidth(1);
    saveFileOut<<",";
    saveFileOut.setFieldWidth(7);
    saveFileOut<<"fixed:";
    saveFileOut.setFieldWidth(4);
    saveFileOut << QString::number(trans_para->fixed_sat);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<",";
    saveFileOut.setFieldWidth(7);
    saveFileOut<<"ratio:";
    saveFileOut.setFieldWidth(6);
    saveFileOut<<QString::number(trans_para->lambda_radio,'f',1);
    saveFileOut<<endl;
    int frequency = 0;
    //Output model correction information
    for (int j = 0;j < StallitNumbers;j++)
    {
        SatlitData oneStallit= epochSatlite.at(j);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<oneStallit.SatType;
        saveFileOut.setFieldWidth(2);
        if (oneStallit.PRN < 10)
            saveFileOut<<"0"+QString::number(oneStallit.PRN);
        else
            saveFileOut<<oneStallit.PRN;

        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        saveFileOut.setFieldWidth(4);
        saveFileOut<<QString::number(oneStallit.EA[0],'f',2);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<", ";
        if(out_Comb_mode == Comb_IF_mode)
        {
            // cycle slip
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"CS,";
            saveFileOut.setFieldWidth(2);
            saveFileOut<<QString::number(oneStallit.cycly_slipF);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            // Smooth arc length
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"WL,";
            saveFileOut.setFieldWidth(5);
            saveFileOut<<QString::number(oneStallit.satMW_wlSmoN);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD Wl ambiguity  float
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_Wl_float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD Wl ambiguity fixed
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_Wl_Int);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            // par select ranking
            saveFileOut.setFieldWidth(4);
            saveFileOut<<"PAR,";
            saveFileOut.setFieldWidth(2);
            saveFileOut<<QString::number(oneStallit.par_sat_rank);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //IF combination ambiguity
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"IF,";
            saveFileOut.setFieldWidth(8);
            saveFileOut<<QString::number(X(j+trans_para->const_parm),'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD NL ambiguity  float
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"NL,";
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_Nl_Float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD NL ambiguity fixed
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_Nl_fix);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            for (int k = 0;k < StallitNumbers;k++)
            {
                // fangcha
                saveFileOut.setFieldWidth(14);
                saveFileOut<<QString::number((*Xq_mat)(j+trans_para->const_parm,k+trans_para->const_parm),'f',6);
                saveFileOut.setFieldWidth(2);
                saveFileOut<<", ";
            }
        }
        else if(out_Comb_mode == Comb_UD_mode)
        {
            // cycle slip
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"CS,";
            saveFileOut.setFieldWidth(2);
            saveFileOut<<QString::number(oneStallit.cycly_slipF);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";

            saveFileOut.setFieldWidth(4);
            saveFileOut<<"Ion,";
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(X(j+trans_para->const_parm),'f',3);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";

            // Smooth arc length
            saveFileOut.setFieldWidth(4);
            saveFileOut<<"EW5,";
            saveFileOut.setFieldWidth(5);
            saveFileOut<<QString::number(oneStallit.satMW_ewl25SmoN);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_EWl25_float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_EWl25_Int);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            // Smooth arc length
            saveFileOut.setFieldWidth(4);
            saveFileOut<<"EW4,";
            saveFileOut.setFieldWidth(5);
            saveFileOut<<QString::number(oneStallit.satMW_ewl24SmoN);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_EWl24_float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_EWl24_Int);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            // Smooth arc length
            saveFileOut.setFieldWidth(4);
            saveFileOut<<"EW3,";
            saveFileOut.setFieldWidth(5);
            saveFileOut<<QString::number(oneStallit.satMW_ewl23SmoN);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD Wl ambiguity  float
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_EWl23_float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_EWl23_Int);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            // Smooth arc length
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"WL,";
            saveFileOut.setFieldWidth(5);
            saveFileOut<<QString::number(oneStallit.satMW_wlSmoN);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD Wl ambiguity  float
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_Wl_float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD Wl ambiguity fixed
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_Wl_Int);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            // par select ranking
            saveFileOut.setFieldWidth(2);
            saveFileOut<<QString::number(oneStallit.par_sat_rank);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";

            saveFileOut.setFieldWidth(3);
            saveFileOut<<"NL,";
            //SD NL ambiguity  float
            saveFileOut.setFieldWidth(6);
            saveFileOut<<QString::number(oneStallit.SD_Nl_Float,'f',2);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            //SD NL ambiguity fixed
            saveFileOut.setFieldWidth(4);
            saveFileOut<<QString::number(oneStallit.SD_Nl_fix);
            saveFileOut.setFieldWidth(2);
            saveFileOut<<", ";
            saveFileOut.setFieldWidth(3);
            saveFileOut<<"QR,";
//            for (int k = 0;k < StallitNumbers;k++)
//            {
//                // fangcha
//                saveFileOut.setFieldWidth(14);
//                saveFileOut<<QString::number((*Xq_mat)(StallitNumbers+trans_para->const_parm+frequency,StallitNumbers+trans_para->const_parm+k*oneStallit.ObsLenfre),'f',6);
//                saveFileOut.setFieldWidth(2);
//                saveFileOut<<", ";
//            }
//            frequency = frequency + oneStallit.ObsLenfre;
        }

        saveFileOut<<endl;//Output coordinates of a satellite in the epoch
    }
    saveFileOut.flush();
    return true;
}

//Outputs the satellite altitude and azimuth for positioning to the .txt file
bool QRTWrite2File::writeAzEL2Txt(const QVector < SatlitData > epochSatlite)
{
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("Az_El_coff.txt");
    else
        _flag = findFilenameFlag("back_Az_El_coff.txt");
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];

    if(!RT_writefile->m_AZ_EL_head)
    {
        // write head information
        QString head_string[5] = {"epoch","Observation time", "satellite", "AZ(deg)", "EL(deg)"};
        saveFileOut.setFieldWidth(10);
        saveFileOut << head_string[0]+"|";
        saveFileOut.setFieldWidth(26);
        saveFileOut << head_string[1]+"|";
        saveFileOut.setFieldWidth(10);
        saveFileOut << head_string[2]+"|";
        saveFileOut.setFieldWidth(8);
        saveFileOut << head_string[3]+"|";
        saveFileOut.setFieldWidth(8);
        saveFileOut << head_string[4]+"|";
        saveFileOut << endl;
        RT_writefile->m_AZ_EL_head = true;
    }
    for(int i=0;i<epochSatlite.length();i++)
    {
        saveFileOut.setFieldWidth(10);
        saveFileOut<<epochSatlite.at(0).UTCTime.epochNum;
        saveFileOut.setFieldWidth(2);
        saveFileOut<<": ";
        //Output year, month, day, hour, minute, second
        saveFileOut.setFieldWidth(4);
        saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Year);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<"-";
        saveFileOut.setFieldWidth(2);
        if (epochSatlite.at(0).UTCTime.Month < 10)
            saveFileOut<<"0"+QString::number(epochSatlite.at(0).UTCTime.Month);
        else
            saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Month);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<"-";
        saveFileOut.setFieldWidth(2);
        if (epochSatlite.at(0).UTCTime.Day < 10)
            saveFileOut<<"0"+QString::number(epochSatlite.at(0).UTCTime.Day);
        else
            saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Day);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<" ";
        saveFileOut.setFieldWidth(2);
        if (epochSatlite.at(0).UTCTime.Hours < 10)
            saveFileOut<<"0"+QString::number(epochSatlite.at(0).UTCTime.Hours);
        else
            saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Hours);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<":";
        saveFileOut.setFieldWidth(2);
        if (epochSatlite.at(0).UTCTime.Minutes < 10)
            saveFileOut<<"0"+QString::number(epochSatlite.at(0).UTCTime.Minutes);
        else
            saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Minutes);
        saveFileOut.setFieldWidth(1);
        saveFileOut<<":";
        saveFileOut.setFieldWidth(5);
        if (epochSatlite.at(0).UTCTime.Seconds < 10)
            saveFileOut<<"0"+QString::number(epochSatlite.at(0).UTCTime.Seconds,'f',2);
        else
            saveFileOut<<QString::number(epochSatlite.at(0).UTCTime.Seconds,'f',2);
        SatlitData oneStallit= epochSatlite.at(i);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
        saveFileOut.setFieldWidth(1);
        saveFileOut<<oneStallit.SatType;
        saveFileOut.setFieldWidth(2);
        if (oneStallit.PRN < 10)
            saveFileOut<<"0"+QString::number(oneStallit.PRN);
        else
            saveFileOut<<oneStallit.PRN;
        saveFileOut.setFieldWidth(8);
        saveFileOut<<QString::number(oneStallit.EA[1],'f',2);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
        saveFileOut.setFieldWidth(8);
        saveFileOut<<QString::number(oneStallit.EA[0],'f',2);
        saveFileOut<<endl;
    }
    saveFileOut.flush();
    return true;
}
//Write the zenith wet delay and clock difference to txt, the first column wet delay second column clock difference
bool QRTWrite2File::writeClockZTDW2Txt(const ClockData epochZTDWClock,QString write_sys_str)
{
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("ZTDW_Clock.txt");
    else
        _flag = findFilenameFlag("back_ZTDW_Clock.txt");
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];

    if(!RT_writefile->m_ZTDW_Clock_head)
    {
        // write head information
        QString head_string[10] = {"epoch", "Observation time", "ZTD(m)", "diff_clk_G(m)",
                                 "diff_clk_C2(m)","diff_clk_C3(m)", "diff_clk_R(m)", "diff_clk_E(m)"};
        QString  base_sys = "G";
        if(!write_sys_str.isEmpty()) base_sys = write_sys_str.at(0);
        switch (base_sys[0].toLatin1()) {
        case 'G':
            head_string[3] = "base_clk_G(m)";
            break;
        case 'C':
            head_string[4] = "base_clk_C(m)";
            break;
        case 'R':
            head_string[6] = "base_clk_R(m)";
            break;
        case 'E':
            head_string[7] = "base_clk_E(m)";
            break;
        default:
            break;
        }
        for(int i = 0;i < 7;i++)
        {
            saveFileOut.setFieldWidth(12);
            if(i == 1) saveFileOut.setFieldWidth(26);
            saveFileOut << head_string[i]+"|";
        }
        saveFileOut << endl;
        RT_writefile->m_ZTDW_Clock_head = true;
    }

    saveFileOut.setFieldWidth(10);
    saveFileOut<<epochZTDWClock.UTCTime.epochNum;
    saveFileOut.setFieldWidth(2);
    saveFileOut<<": ";
    //Output year, month, day, hour, minute, second
    saveFileOut.setFieldWidth(4);
    saveFileOut<<QString::number(epochZTDWClock.UTCTime.Year);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochZTDWClock.UTCTime.Month);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochZTDWClock.UTCTime.Day);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochZTDWClock.UTCTime.Hours);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(epochZTDWClock.UTCTime.Minutes);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(10);
    saveFileOut<<QString::number(epochZTDWClock.UTCTime.Seconds,'f',7);
    //Output X system receiver clock error, Xi system relative X system receiver clock offset
    saveFileOut.setFieldWidth(10);
    saveFileOut<<QString::number(epochZTDWClock.ZTD_W,'f',4);
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";
    for(int k = 0; k < 5;k++)
    {
        saveFileOut.setFieldWidth(10);
        saveFileOut<<QString::number(epochZTDWClock.clockData[k],'f',4);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
    }
    saveFileOut<<endl;
    saveFileOut.flush();
    return true;
}

//All satellites write txt files named "G32.txt", "C02.txt", "R08.txt", etc., the first column of ambiguity
bool QRTWrite2File::writeAmbiguity2Txt(QString fload_path)
{
    if(!isDirExist(fload_path))
    {
        QString infor = "can not construct floder: " + fload_path;
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
    }

    //Find the name of the satellite that observes all systems
    int lenSatAbm = allAmbiguity.length();
    QVector< char > store_SatType;
    QVector< int > store_SatPRN;
    for (int i = 0;i < lenSatAbm;i++)
    {
        Ambiguity oneSatAbm = allAmbiguity.at(i);
        char tSatType = '0';
        int tSatPRN = -1;
        bool isFind = false;
        for (int j = 0;j < store_SatPRN.length();j++)
        {
            tSatPRN = store_SatPRN.at(j);
            tSatType = store_SatType.at(j);
            if (tSatPRN == oneSatAbm.PRN && tSatType == oneSatAbm.SatType)
            {
                isFind = true;
                break;
            }
        }
        if (!isFind)
        {
            store_SatType.append(oneSatAbm.SatType);
            store_SatPRN.append(oneSatAbm.PRN);
        }
    }
    //Write all satellite names to the file
    for (int i = 0;i < store_SatPRN.length();i++)
    {
        char tSatType = '0';
        int tSatPRN = -1;
        tSatType = store_SatType.at(i);
        tSatPRN = store_SatPRN.at(i);
        WriteAmbPRN(fload_path, tSatPRN,tSatType);
    }
    return true;
}

//The ambiguity of writing to the PRN satellite
bool QRTWrite2File::WriteAmbPRN(QString temp_floder,int PRN,char SatType)
{
    if (allAmbiguity.length() == 0)//If no data is read
        return false;
    QString fileName,strPRN;
    strPRN = QString::number(PRN);
    if(PRN < 10)
        strPRN = QString::number(0) + strPRN;
    fileName = QString(SatType) + strPRN + QString(".txt");
    fileName = temp_floder + fileName; //Path of Current exec file
    QFile saveFile(fileName);//Saved file
    if (!saveFile.open(QFile::WriteOnly|QFile::Text))
    {
        QString erroInfo = "Open " + fileName + " File Error!";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
        return false;
    }

    QTextStream saveFileOut(&saveFile);

    int lenAbm = allAmbiguity.length();
    for (int i = 0;i <lenAbm;i++ )
    {
        Ambiguity oneSatAmb = allAmbiguity.at(i);
        if (oneSatAmb.PRN != PRN || oneSatAmb.SatType != SatType)
        {
            continue;
        }
        saveFileOut.setFieldWidth(10);
        saveFileOut<<(oneSatAmb.UTCTime.epochNum);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<": ";
        //Output GPS week seconds
        saveFileOut.setFieldWidth(16);
        double GPSSecond = WriteYMD2GPSTime(oneSatAmb.UTCTime.Year,oneSatAmb.UTCTime.Month,oneSatAmb.UTCTime.Day,oneSatAmb.UTCTime.Hours,oneSatAmb.UTCTime.Minutes,oneSatAmb.UTCTime.Seconds);
        saveFileOut<<QString::number(GPSSecond,'f',6);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";

        //Write ambiguity
        saveFileOut.setFieldWidth(10);
        saveFileOut<<QString::number(1);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";
        saveFileOut.setFieldWidth(16);
        if (oneSatAmb.isIntAmb)
            saveFileOut<<QString::number(qRound(oneSatAmb.Amb));
        else
            saveFileOut<<QString::number(oneSatAmb.Amb,'f',4);

        saveFileOut.setFieldWidth(2);
        saveFileOut<<"  ";

        saveFileOut<<endl;
    }
    saveFileOut.flush();
    saveFile.close();

    return true;
}


//Write the satellite stored in the variable allAmbiguity to the file
bool QRTWrite2File::WriteEpochPRN(QString fload_path, QString tempfileName)
{
    if (allAmbiguity.length() == 0)//If no data is read
        return false;
    if(!isDirExist(fload_path))
    {
        QString infor = "can not construct floder: " + fload_path;
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
    }
    QString fileName = fload_path + tempfileName;

    QFile saveFile(fileName);//Saved file
    if (!saveFile.open(QFile::WriteOnly|QFile::Text))
    {
        QString erroInfo = "Open " + fileName + " File Error!";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(erroInfo);
        text_disp->data_lenth++;
        return false;
    }
    QTextStream saveFileOut(&saveFile);
    //Write data only
    int lengAllSat = allAmbiguity.length();
    QString tQstrPRN = "";
    for (int i = 0; i < lengAllSat;i++)
    {
        Ambiguity oneSatAmb = allAmbiguity.at(i);
        //Write epoch
        saveFileOut.setFieldWidth(10);
        saveFileOut<<(oneSatAmb.UTCTime.epochNum);
        saveFileOut.setFieldWidth(2);
        saveFileOut<<": ";
        //Write satellite type and number, such as G24, C01
        saveFileOut.setFieldWidth(1);
        saveFileOut<<QString(oneSatAmb.SatType);
        saveFileOut.setFieldWidth(2);
        if (oneSatAmb.PRN < 10)
            tQstrPRN = QString::number(0) + QString::number(oneSatAmb.PRN);
        else
            tQstrPRN = QString::number(oneSatAmb.PRN);
        saveFileOut<<tQstrPRN;
        saveFileOut<<endl;
    }
    saveFileOut.flush();
    saveFile.close();

    return true;
}

bool QRTWrite2File::writeBadSatliteData(const SatlitData oneSatAmb)
{

    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("bad_satellites.txt");
    else
        _flag = findFilenameFlag("back_bad_satellites.txt");
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];
    //Write data only
    QString tQstrPRN = "";
    //Write epoch
    saveFileOut.setFieldAlignment(QTextStream::AlignRight);
    saveFileOut.setFieldWidth(10);
    saveFileOut<<(oneSatAmb.UTCTime.epochNum);
    saveFileOut.setFieldWidth(2);
    saveFileOut<<": ";
    //Write satellite type and number, such as G24, C01
    saveFileOut.setFieldWidth(1);
    saveFileOut<<QString(oneSatAmb.SatType);
    saveFileOut.setFieldWidth(2);
    if (oneSatAmb.PRN < 10)
        tQstrPRN = QString::number(0) + QString::number(oneSatAmb.PRN);
    else
        tQstrPRN = QString::number(oneSatAmb.PRN);
    saveFileOut<<tQstrPRN;
    saveFileOut.setFieldWidth(2);
    saveFileOut<<"  ";

    //Output year, month, day, hour, minute, second
    saveFileOut.setFieldWidth(4);
    saveFileOut<<QString::number(oneSatAmb.UTCTime.Year);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneSatAmb.UTCTime.Month);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneSatAmb.UTCTime.Day);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneSatAmb.UTCTime.Hours);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneSatAmb.UTCTime.Minutes);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(10);
    saveFileOut<<QString::number(oneSatAmb.UTCTime.Seconds,'f',7);

    saveFileOut.setFieldWidth(18);
    saveFileOut<<"Eliminate info: ";
    for(int n = 0;n < oneSatAmb.badMsg.length();n++)
    {
        saveFileOut.setFieldAlignment(QTextStream::AlignLeft);
        saveFileOut.setFieldWidth(132);
        saveFileOut<<oneSatAmb.badMsg.at(n);
        saveFileOut.setFieldWidth(3);
        saveFileOut<< " | ";
    }
    saveFileOut.setFieldWidth(1);
    saveFileOut<<endl;
    saveFileOut.flush();

    return true;
}

bool QRTWrite2File::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }
    else
    {
       bool ok = dir.mkpath(fullPath);//Create a multi-level directory
       return ok;
    }
}

int QRTWrite2File::findFilenameFlag(QString Filename)
{
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    for(int i = 0; i < MAX_FILE_WRITE;i++)
    {
        if(RT_writefile->m_QFileNames[i].contains(Filename))
            return i;
    }

    return -1;
}
bool QRTWrite2File::write_dops(const RecivePos oneRecivePos)
{

    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int _flag;
    if(!trans_para->backward_filter)
        _flag = findFilenameFlag("dop_satN_AR.txt");
    else
        _flag = findFilenameFlag("back_dop_satN_AR.txt");
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];
    saveFileOut.setFieldWidth(10);
    saveFileOut<<oneRecivePos.UTCtime.epochNum;
    saveFileOut.setFieldWidth(2);
    saveFileOut<<": ";
    //Output year, month, day, hour, minute, second
    saveFileOut.setFieldWidth(4);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Year);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Month);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<"-";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Day);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<" ";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Hours);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(2);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Minutes);
    saveFileOut.setFieldWidth(1);
    saveFileOut<<":";
    saveFileOut.setFieldWidth(10);
    saveFileOut<<QString::number(oneRecivePos.UTCtime.Seconds,'f',7);

    //
    saveFileOut.setFieldWidth(15);
    saveFileOut<<QString::number(oneRecivePos.totolEpochStalitNum);
    //dops
    for(int ii=0;ii<4;ii++)
    {
        saveFileOut.setFieldWidth(10);
        saveFileOut << QString::number(oneRecivePos.dops[ii],'f',4);
    }
    saveFileOut.setFieldWidth(10);
    saveFileOut << QString::number(oneRecivePos.radio,'f',2);
    saveFileOut.setFieldWidth(10);
    saveFileOut << QString::number(oneRecivePos.fixed_sat);
    saveFileOut<<endl;
    saveFileOut.flush();

    return true;
}
/*Go to GPS time*/
double QRTWrite2File::WriteYMD2GPSTime(int Year,int Month,int Day,int HoursInt,int Minutes,double Seconds,int *GPSWeek, int *day)
{
    double Hours = HoursInt + ((Minutes * 60) + Seconds)/3600.0;
    /*Get JD*/
    double JD = 0.0;
    if(Month<=2)
        JD = (int)(365.25*(Year-1)) + (int)(30.6001*(Month+12+1)) + Day + Hours/24.0 + 1720981.5;
    else
        JD = (int)(365.25*(Year)) + (int)(30.6001*(Month+1)) + Day + Hours/24.0 + 1720981.5;

    int Week = (int)((JD - 2444244.5) / 7);
    int N =(int)(JD + 1.5)%7;
    if (GPSWeek)	*GPSWeek = Week;
    return (N*24*3600 + HoursInt*3600 + Minutes*60 + Seconds);
}
void QRTWrite2File::WriteXYZ2BLH(double *pXYZ,double *m_pBLH)
{
    double X=pXYZ[0],Y=pXYZ[1],Z=pXYZ[2];
    if (X == 0)
        return;
    if (X > 0&&Y>0)
        m_pBLH[1] = atan(Y/X);
    else if(X < 0&&Y > 0)
        m_pBLH[1] = atan(Y/X) + MM_PI;
    else if (X < 0&&Y < 0)
        m_pBLH[1] = -(MM_PI - atan(Y/X));
    else
        m_pBLH[1] = atan(Y/X);
    //Compute B
    if(X == 0&&Y == 0)
        return;
    double t0 = Z/qSqrt(X*X + Y*Y);
    double P = (elipsePara[2]*elipsePara[4])/qSqrt(X*X + Y*Y);
    double k = 1 + elipsePara[5];
    double eps = 1e-12;
    double ti = t0;
    double ti1 = 0;
    double Bi = qAtan(ti);
    double Bi1 = 0;
    do
    {
        ti1 = t0 + P*ti/qSqrt(k + ti*ti);
        Bi = qAtan(ti);
        Bi1 = qAtan(ti1);
        ti = ti1;
    } while (qAbs(Bi1-Bi) > eps);
    m_pBLH[0] = Bi1;
    double W = qSqrt(1 - elipsePara[4]*qSin(Bi1)*qSin(Bi1));
    double N = elipsePara[0]/W;
    m_pBLH[2] = Z/qSin(Bi1) - N*(1 - elipsePara[4]);
}
//Various error corrections are written to the .ppp file
bool QRTWrite2File::writeambupd(const QVector < SatlitData > epochSatlite)
{
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();
    int _flag = findFilenameFlag(pravide_obs->ambupd_file);
    if(_flag == -1) return false;
    RTwriteFile *RT_writefile = AGNSS_APPData::sGetAPPDataInstance()->mGetRTwriteData();
    QTextStream &saveFileOut = RT_writefile->m_QTextStreamVct[_flag];

    int StallitNumbers = epochSatlite.length();
    if(StallitNumbers == 0) return false;
    //Output header information
    //Output model correction information
    for (int j = 0;j < StallitNumbers;j++)
    {
        SatlitData oneStallit= epochSatlite.at(j);
        saveFileOut.setFieldWidth(8);
        saveFileOut<<QString::number(oneStallit.MJD);
        saveFileOut.setFieldWidth(10);
        saveFileOut<<QString::number(oneStallit.seconds_day,'f',1);
        saveFileOut.setFieldWidth(5);
        saveFileOut<<pravide_obs->The_same_ant;
        saveFileOut.setFieldWidth(2);
        saveFileOut<<oneStallit.SatType;
        saveFileOut.setFieldWidth(2);
        if (oneStallit.PRN < 10)
            saveFileOut<<"0"+QString::number(oneStallit.PRN);
        else
            saveFileOut<<oneStallit.PRN;

        saveFileOut.setFieldWidth(19);
        saveFileOut<<QString::number(oneStallit.Amb,'f',3);
        saveFileOut.setFieldWidth(19);
        saveFileOut<<QString::number(oneStallit.satMW_wlAmbi[0],'f',3);
        saveFileOut.setFieldWidth(10);
        saveFileOut<<QString::number(oneStallit.satMW_wlAmbi[1],'f',3);

        saveFileOut<<endl;//Output coordinates of a satellite in the epoch
    }
    saveFileOut.flush();
    return true;
}
