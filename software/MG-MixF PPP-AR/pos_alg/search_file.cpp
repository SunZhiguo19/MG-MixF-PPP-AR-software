#include "search_file.h"

search_file::search_file()
{

}
search_file::~search_file()
{

}
/*Search for files with the same suffix*/
QStringList search_file::searchFilterFile(QString floder_path, QStringList filers)
{
    QDir floder_Dir(floder_path);
    QStringList all_file_path;
    floder_Dir.setFilter(QDir::Files | QDir::NoSymLinks);
    floder_Dir.setNameFilters(filers);
    QFileInfoList file_infos = floder_Dir.entryInfoList();
    for(int i = 0;i < file_infos.length();i++)
    {
        QFileInfo file_info = file_infos.at(i);
        if(file_info.fileName() != "." || file_info.fileName() != "..")
            all_file_path.append(file_info.absoluteFilePath());
    }
    return all_file_path;
}

/*Run the specified directory file*/
bool search_file::sear_file(int exec_flag,QStringList OFileNamesList,file_dat *dat_fil)
{

    /*find files*/
    QStringList tempFilters,Sp3FileNamesList, ClkFileNamesList, ErpFileNamesList,NavFileNameList,
            AtxFileNamesList, BlqFileNamesList, GrdFileNamesList, FCBsFileNamesList, DCBFileNamesList,OBXFileNamesList,BIAFileNamesList;
    QString  erpFile = "", blqFile = "", atxFile = "", grdFile = "",Navfile="",Obxfile="",Biafile="";
    QString dat_path = dat_fil->dat_path;
    QStringList SsrFileNameList;
    QString clkfil,biafle;
    switch (exec_flag) {
    case POS_PP_PPP_AR_2:
        tempFilters.clear();// find FCBs files
        tempFilters.append("*.fcb");
        tempFilters.append("*.FCB");
        FCBsFileNamesList = searchFilterFile(dat_path, tempFilters);
    case POS_PP_PPP_AR_1:
    case POS_PP_PPP:
        tempFilters.clear();
        tempFilters.append("*.sp3");// find sp3 files
        tempFilters.append("*.SP3");
        tempFilters.append("*.eph");
        Sp3FileNamesList = searchFilterFile(dat_path, tempFilters);
        tempFilters.clear();
        tempFilters.append("*.clk");// find clk files
        tempFilters.append("*.CLK");
        ClkFileNamesList = searchFilterFile(dat_path, tempFilters);
        if(ClkFileNamesList.isEmpty())// if not find clk try clk_*
        {
            tempFilters.clear();
            tempFilters.append("*.clk_*");
            ClkFileNamesList = searchFilterFile(dat_path, tempFilters);
        }
        if(!ClkFileNamesList.isEmpty()&&POS_PP_PPP_AR_1==exec_flag)
        {
            clkfil = ClkFileNamesList.at(0);
            if(clkfil.contains("GBM",Qt::CaseInsensitive))
            {
                dat_fil->int_clk_ar_flag = 1;
                // find obx files
                tempFilters.clear();
                tempFilters.append("*.obx");
                tempFilters.append("*.OBX");
                OBXFileNamesList = searchFilterFile(dat_path, tempFilters);
                // find bia files
                tempFilters.clear();
                tempFilters.append("*.bia");
                tempFilters.append("*.BIA");
                BIAFileNamesList = searchFilterFile(dat_path, tempFilters);
            }
        }
        // find dcbs files
        tempFilters.clear();
        tempFilters.append("*.DCB");
        DCBFileNamesList = searchFilterFile(dat_path, tempFilters);
        // find erp files
        tempFilters.clear();
        tempFilters.append("*.erp");
        tempFilters.append("*.ERP");
        ErpFileNamesList = searchFilterFile(dat_path, tempFilters);
        break;
    case POS_Real_PPP_AR:
    case POS_UDUC_PPP_AR:
        tempFilters.clear();
        tempFilters.append("*.sp3");// find sp3 files
        tempFilters.append("*.SP3");
        tempFilters.append("*.eph");
        Sp3FileNamesList = searchFilterFile(dat_path, tempFilters);
        tempFilters.clear();
        tempFilters.append("*.clk");// find clk files
        tempFilters.append("*.CLK");
        ClkFileNamesList = searchFilterFile(dat_path, tempFilters);
        if(ClkFileNamesList.isEmpty())// if not find clk try clk_*
        {
            tempFilters.clear();
            tempFilters.append("*.clk_*");
            ClkFileNamesList = searchFilterFile(dat_path, tempFilters);
        }

        // find erp files
        tempFilters.clear();
        tempFilters.append("*.erp");
        tempFilters.append("*.ERP");
        ErpFileNamesList = searchFilterFile(dat_path, tempFilters);
        // find obx files
        tempFilters.clear();
        tempFilters.append("*.obx");
        tempFilters.append("*.OBX");
        OBXFileNamesList = searchFilterFile(dat_path, tempFilters);
        // find bia files
        tempFilters.clear();
        tempFilters.append("*.bia");
        tempFilters.append("*.BIA");
        BIAFileNamesList = searchFilterFile(dat_path, tempFilters);
        break;
    default:
        break;
    }
    // find Atx files
    tempFilters.clear();
    tempFilters.append("*.atx");
    QStringList run_floder;
    run_floder = searchFilterFile(dat_path, tempFilters);
    AtxFileNamesList.append(run_floder);
    run_floder.clear();
    // find blq files
    tempFilters.clear();
    tempFilters.append("*.blq");
    run_floder = searchFilterFile(dat_path, tempFilters);
    BlqFileNamesList.append(run_floder);
    run_floder.clear();
    // find grd files
    tempFilters.clear();
    tempFilters.append("*.grd");
    run_floder = searchFilterFile(dat_path, tempFilters);
    GrdFileNamesList.append(run_floder);
    run_floder.clear();

    if(!ErpFileNamesList.isEmpty()) erpFile = ErpFileNamesList.at(0);
    if(!AtxFileNamesList.isEmpty()) atxFile = AtxFileNamesList.at(0);
    if(!BlqFileNamesList.isEmpty()) blqFile = BlqFileNamesList.at(0);
    if(!GrdFileNamesList.isEmpty()) grdFile = GrdFileNamesList.at(0);
    if(!NavFileNameList.isEmpty())  Navfile = NavFileNameList.at(0);
    if(!OBXFileNamesList.isEmpty())  Obxfile = OBXFileNamesList.at(0);
    if(!BIAFileNamesList.isEmpty())  Biafile = BIAFileNamesList.at(0);
    QString infor;
    switch (exec_flag) {
    case POS_PP_PPP_AR_2:
        if(FCBsFileNamesList.isEmpty())
        {
            infor = "search_file::Cant not find FCB file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
            return false;
        }
    case POS_PP_PPP_AR_1:
        if(OBXFileNamesList.isEmpty()&&dat_fil->int_clk_ar_flag==1)
        {
            dat_fil->OBX_flie_flag = 1;
            infor = "search_file::Cant not find OBX file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
        }
    case POS_PP_PPP:
        if(Sp3FileNamesList.isEmpty())
        {
            infor = "search_file::Cant not find Sp3 file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
            return false;
        }
        if(ClkFileNamesList.isEmpty())
        {
            infor = "search_file::Cant not find clk file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
            return false;
        }
        clkfil = ClkFileNamesList.at(0);
        if(clkfil.contains("grg",Qt::CaseInsensitive))
                dat_fil->WLUPD_corr = 1;
        if(DCBFileNamesList.isEmpty()&&dat_fil->int_clk_ar_flag!=1)
        {
            infor = "search_file::Cant not find DCB file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
        }
        break;
    case POS_UDUC_PPP_AR:
    case POS_Real_PPP_AR:
        if(Sp3FileNamesList.isEmpty())
        {
            infor = "search_file::Cant not find Sp3 file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
            return false;
        }
        if(ClkFileNamesList.isEmpty())
        {
            infor = "search_file::Cant not find clk file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
            return false;
        }
        if(OBXFileNamesList.isEmpty())
        {
            dat_fil->OBX_flie_flag = 1;
            infor = "search_file::Cant not find OBX file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
        }
        if(BIAFileNamesList.isEmpty())
        {
            infor = "search_file::Cant not find bia file.";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            text_disp->data_text.append(infor);
            text_disp->data_lenth++;
        }
        biafle = BIAFileNamesList.at(0);
        if(biafle.contains("com",Qt::CaseInsensitive)||biafle.contains("COD",Qt::CaseInsensitive))
            dat_fil->bia_file_flag = 1;
        break;
    default:
        break;
    }
    dat_fil->SsrFileNameList=SsrFileNameList;
    // save data to QPPPModel
    initDatFile(exec_flag,OFileNamesList, Sp3FileNamesList, ClkFileNamesList, FCBsFileNamesList,DCBFileNamesList,erpFile, blqFile, atxFile, grdFile,NavFileNameList,Biafile,Obxfile,dat_fil);
    return true;
}
//Constructor
void search_file::initDatFile(int exec_flag,QStringList OFileName,QStringList Sp3FileNames,QStringList ClkFileNames,QStringList FCBsFileNames ,QStringList DCBFileNames ,
                              QString ErpFileName,QString BlqFileName,QString AtxFileName,QString GrdFileName,QStringList NavFileNameList,QString BiafileName,QString ObxfileName,file_dat *dat_fil)
{
//Set up multi-system data
    QString dat_path = dat_fil->out_dat_path;
    QDir tempDir(dat_path);
    QString Positioning_mode,Comb_mode,fre_mode,fre_Num,filter_M;

    setObsFileName(OFileName);
    int obsTime[5] = {0};
    double Seconds = 0,ObsJD = 0;
    /*Obtain the approximate coordinates of the O file*/
    double receiver_pos[3];

    getApproXYZ(receiver_pos);
    for(int i=0;i<3;i++)  dat_fil->receiver_pos[i] = receiver_pos[i];
    /*Get the initial observation time*/
    getFistObsTime(obsTime,Seconds);
    ObsJD = computeJD(obsTime[0],obsTime[1],obsTime[2],obsTime[3],obsTime[4],Seconds);
    /*Set the antenna effective time*/
    int nav_n=0;QString NavfileName;
    switch (exec_flag) {
    case POS_PP_PPP_AR_2:
        setFcbsFileNames(FCBsFileNames);//set FCBs file path szg
        getFCBAllData();
    case POS_PP_PPP_AR_1:
    case POS_PP_PPP:
        Positioning_mode = "PP_";
        setSP3FileNames(Sp3FileNames);
        setClkFileNames(ClkFileNames);
        if(dat_fil->int_clk_ar_flag!=1)
            setDCBFileNames(DCBFileNames);//set DCB file path szg
        else
        {
            setBIAFileNames(BiafileName);
            setOBXFileNames(ObxfileName);
        }
        setStationName(getMakerName());//Setting the tide requires a station name
        break;
    case POS_Real_PPP_AR:
    case POS_UDUC_PPP_AR:
        Positioning_mode = "OSBs_";
        setSP3FileNames(Sp3FileNames);
        setClkFileNames(ClkFileNames);
        setBIAFileNames(BiafileName);
        setOBXFileNames(ObxfileName);
        break;
    default:
        break;
    }
//    setTropFileNames(GrdFileName,"GMF", m_TropDelay);// Default tropospheric model projection function GMF
    setTideFileName(BlqFileName,ErpFileName);// for OCEAN and Erp tide
    readRepFile(ErpFileName);//  for compute sun and moon position
    getAlltidalData();
    setAntFileName(AtxFileName);
    setObsJD(getAntType(),ObsJD);
    getAntAllData();
    QString Output_system = dat_fil->Output_system;
    QString Out_mod = dat_fil->Out_mod;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd_hh_mm_ss_zzz");
    obs_pravide *pravide_obs = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_pravide_obs();
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();      //2022/12/15 lfh
    switch (opt_corr->Combination_mode) {
    case Comb_IF_mode:
        Comb_mode = "_IF_";
        break;
    case Comb_UD_mode:
        Comb_mode = "_UC_";
        break;
    default:
        break;
    }
    if(opt_corr->frequency_model==mixed_frequency)
        fre_mode = "_Mixed";
    else if(opt_corr->frequency_model==fixed_frequency)
        fre_mode = "_Fixed";
    if(opt_corr->fre_model==double_fre)
        fre_Num = "_Double";
    else if(opt_corr->fre_model==triple_fre)
        fre_Num = "_Triple";
    else if(opt_corr->fre_model==four_fre)
        fre_Num = "_Four";
    else if(opt_corr->fre_model==five_fre)
        fre_Num = "_Five";
    if(opt_corr->filter_model==PPP_KINEMATIC_SRIF||opt_corr->filter_model==PPP_STATIC_SRIF)
        filter_M = "SRIF";
    else if(opt_corr->filter_model==PPP_KINEMATIC_KALM||opt_corr->filter_model==PPP_STATIC_KALM)
        filter_M = "Kalman";


    QString floder_name = Positioning_mode +  pravide_obs->The_same_ant+ fre_mode + Comb_mode +filter_M+fre_Num +"_PPP_" + Output_system + "_" + Out_mod + "_" + current_date + PATHSEG; //+ "_1e8" + "_1e8_new"
    QString RTwriteFloder = tempDir.absoluteFilePath(floder_name);
    setSaveFloder(RTwriteFloder);
}

//Constructor
void search_file::multiple_stations(QVector< QStringList > &multi_sta_OFileName,QString dat_path)
{
//Set up multi-obser data
    /*find files*/
    QStringList tempFilters,OFileNamesList;
    // find obs files
    tempFilters.clear();
    tempFilters.append("*.*o");
    tempFilters.append("*.*O");
    OFileNamesList = searchFilterFile(dat_path, tempFilters);
    QString OfileName = "";
    if (!OFileNamesList.isEmpty())
    {
        OfileName = OFileNamesList.at(0);
    }
    else
    {
        QString infor = "search_file:: Cant not find Obsvertion file.";
        disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
        text_disp->data_text.append(infor);
        text_disp->data_lenth++;
        return ;
    }
    int lenth_obs = OFileNamesList.length(),station_N=0;
    int obs_stations_flag[lenth_obs]={0};
    QStringList obs_stations_filename[lenth_obs];
    for(int i=0;i<lenth_obs;i++)
    {
        if(obs_stations_flag[i]==1) continue;
        QString OBSfilename = OFileNamesList.at(i);
        QString The_station_name="XXX";
        QStringList obs_data_name = OBSfilename.split("/");
        if(obs_data_name.length()==0)   break ;
        OBSfilename = obs_data_name.at(obs_data_name.length()-1);
        obs_data_name.clear();
        if(OBSfilename.length()>8)
            The_station_name = OBSfilename.mid(0,4).trimmed();
        for(int j=i;j<lenth_obs;j++)
        {
            OBSfilename = OFileNamesList.at(j);
            obs_data_name = OBSfilename.split("/");
            OBSfilename = obs_data_name.at(obs_data_name.length()-1);
            if(OBSfilename.contains(The_station_name,Qt::CaseInsensitive))
            {
                obs_stations_filename[station_N].append(OFileNamesList.at(j));
                obs_stations_flag[j]=1;
            }
            obs_data_name.clear();
        }
        station_N++;
    }
    for(int i=0;i<station_N;i++)
    {
        QStringList obs_station = obs_stations_filename[i];
        int obsTim[obs_station.length()];
        for(int j=0;j<obs_station.length();j++)
        {
            QString OBSfilename = obs_station.at(j);
            QStringList obs_data_name = OBSfilename.split("/");
            OBSfilename = obs_data_name.at(obs_data_name.length()-1);
            if(OBSfilename.length()>28)
                obsTim[j] = OBSfilename.mid(16,7).toInt();
        }
        QString Obs_file_Str[obs_station.length()];
        for(int ii=0;ii<obs_station.length();ii++)  Obs_file_Str[ii] = obs_station.at(ii);
        for(int ii=0;ii<obs_station.length();ii++)
        {
            for(int j=0;j<obs_station.length()-ii-1;j++)
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
        int mult_len = obs_station.length();
        obs_station.clear();
        for(int ii=0;ii<mult_len;ii++)
            obs_station.append(Obs_file_Str[ii]);
        multi_sta_OFileName.append(obs_station);
    }
}
void search_file::getFileListDfs(const QString& path, QPair<QStringList, QStringList>& list, const QString& deep)
{
//    for(const QString& file : QDir(path + deep).entryList(QDir::Files)){
//        list.first.append(deep + file);
//    }
    for(const QString& dir : QDir(path + deep).entryList(QDir::Dirs | QDir::NoDotAndDotDot)){
        list.second.append(deep + dir);
//        getFileListDfs(path, list, deep + dir + '/');
    }
}
QPair<QStringList, QStringList> search_file::getFileList(const QString& path)
{
    QPair<QStringList, QStringList> list;
    int pos = path.lastIndexOf('/');
    const QString& m_deep = path.mid(pos);
    list.second.append(m_deep);
    getFileListDfs(path.left(pos), list, m_deep);
    return list;
}
