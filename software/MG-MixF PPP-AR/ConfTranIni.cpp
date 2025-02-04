#include "ConfTranIni.h"

ConfTranIni::ConfTranIni(QString Inifilename)
{
    mp_QSettings = new QSettings(Inifilename, QSettings::IniFormat);
    mp_QSettings->setIniCodec("UTF-8");
    m_Inifilename = Inifilename;
}

ConfTranIni::~ConfTranIni()
{
    delete mp_QSettings;
}

QString ConfTranIni::getValue(QString keypath)
{
    return mp_QSettings->value(keypath).toString();
}

bool ConfTranIni::writeJson2Ini(QString Inifilename, QJsonArray jsonArry)
{
    QSettings mySett(Inifilename, QSettings::IniFormat);
    mySett.setIniCodec("UTF-8");
    foreach (QJsonValue tempArry, jsonArry) {
        QJsonObject tempJsonObj = tempArry.toObject();
        QStringList tempKeys = tempJsonObj.keys();
        foreach (QString tempKey, tempKeys) {
            QJsonValue myJsonValue = tempJsonObj.value(tempKey);
            if(myJsonValue.isString()){
                // only parase string
                QString tempValue = myJsonValue.toString();
                // Comments
                if(tempKey.contains("comments", Qt::CaseInsensitive)){
                    mySett.beginGroup("Comments");
                    mySett.setValue(tempKey, tempValue);
                    mySett.endGroup();
                }
                else{
                    // config
                    mySett.beginGroup("MG-MixF PPP-AR");
                    mySett.setValue(tempKey, tempValue);
                    mySett.endGroup();
                }
            }
        }
    }
    return true;
}

bool ConfTranIni::writeJson2Ini(QString Inifilename, QString jsonText)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8());
    QJsonArray jsonArry = jsonDoc.array();
    writeJson2Ini(Inifilename, jsonArry);
    return true;
}

bool ConfTranIni::writeJson2IniS(QString Inifilename, QJsonArray jsonArry)
{
    QSettings mySett(Inifilename, QSettings::IniFormat);
    mySett.setIniCodec("UTF-8");
    foreach (QJsonValue tempArry, jsonArry) {
        QJsonObject tempJsonObj = tempArry.toObject();
        QStringList tempKeys = tempJsonObj.keys();
        foreach (QString tempKey, tempKeys) {
            QJsonValue myJsonValue = tempJsonObj.value(tempKey);
            if(myJsonValue.isString()){
                // only parase string
                QString tempValue = myJsonValue.toString();
                // Comments
                if(tempKey.contains("comments", Qt::CaseInsensitive)){
                    mySett.beginGroup("Comments");
                    mySett.setValue(tempKey, tempValue);
                    mySett.endGroup();
                }
                else{
                    // config
                    mySett.beginGroup("MG-MixF Config");
                    mySett.setValue(tempKey, tempValue);
                    mySett.endGroup();
                }
            }
        }
    }
    return true;
}

bool ConfTranIni::writeJson2IniS(QString Inifilename, QString jsonText)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8());
    QJsonArray jsonArry = jsonDoc.array();
    writeJson2IniS(Inifilename, jsonArry);
    return true;
}

bool ConfTranIni::writeJson2File(QString jsonfilename, QJsonArray jsonArry)
{
    QJsonDocument jsonDoc(jsonArry);
    QByteArray ba = jsonDoc.toJson();
    QFile file(jsonfilename);
    if(!file.open(QIODevice::WriteOnly)) return false;
    file.write(ba);
    file.close();
    return true;
}
