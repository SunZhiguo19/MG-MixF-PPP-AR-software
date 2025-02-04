#ifndef SEARCH_FILE_H
#define SEARCH_FILE_H

#include "pos_algorith.h"
#include "rinex_dat/read_antcorr.h"
#include "rinex_dat/read_dcb.h"
#include "rinex_dat/rinex_clk.h"
#include "rinex_dat/rinex_obs.h"
#include "rinex_dat/rinex_sp3.h"
#include "rinex_dat/sgg_fcb.h"
#include "tidal_corr.h"
#include "filter_alg/srif_algorithm.h"
//#include "inf_write/QRTWrite2File.h"
class search_file:public tidal_corr,public srif_algorithm
{
public:
    search_file();
    ~search_file();
    QStringList searchFilterFile(QString floder_path, QStringList filers);
    bool sear_file(int exec_flag, QStringList OFileNamesList, file_dat *dat_fil);
    void initDatFile(int exec_flag, QStringList OFileName, QStringList Sp3FileNames, QStringList ClkFileNames, QStringList FCBsFileNames , QStringList DCBFileNames ,
                     QString ErpFileName, QString BlqFileName, QString AtxFileName, QString GrdFileName, QStringList NavFileNameList, QString BiafileName, QString ObxfileName, file_dat *dat_fil);
    void multiple_stations(QVector< QStringList > &multi_sta_OFileName,QString dat_path);
    void getFileListDfs(const QString& path, QPair<QStringList, QStringList>& list, const QString& deep);
    QPair<QStringList, QStringList> getFileList(const QString& path);

public:


private:


};

#endif // SEARCH_FILE_H
