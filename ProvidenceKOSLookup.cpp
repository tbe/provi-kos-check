#include "ProvidenceKOSLookup.h"

#include "ui/mainwindow.h"
#include "ui/koslistentrycompact.h"
#include "lib/logwatcher.h"
#include "lib/cvakosapi.h"
#include "lib/eveapi.h"
#include "lib/network.h"
#include "lib/audio.h"
#include "lib/cachedb.h"
#include "lib/hotdropperdb.h"


#include <QtCore/QCoreApplication>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtGui/QDesktopServices>

#include <phonon/AudioOutput>
#include <phonon/Effect>
#include <phonon/BackendCapabilities>
#include <phonon/MediaObject>


#define ADD_KOS_PARAMS(name,icon,kos,kosby) Q_ARG(const QString,name),Q_ARG(const QString, icon),Q_ARG(const int,kos), Q_ARG(const QString,kosby)

ProvidenceKOSLookup::ProvidenceKOSLookup()
{
    settings        = new QSettings(this);
    audio           = pkos::Audio::getInstance();
    windowNet       = new pkos::Network(this);

    mainwindow      = new pkos::MainWindow(windowNet);
    connect(mainwindow,SIGNAL(reloadLogfiles()),this,SLOT(findLogfiles()));


    backgroundNet   = new pkos::Network(this);
    eapi            = new pkos::EveAPI(this,backgroundNet);
    capi            = new pkos::CvaKosAPI(this,backgroundNet);
    watcher         = new pkos::logWatcher(this);

    settings        = new QSettings(this);

    cachedb         = new pkos::CacheDB(this);
    hddb            = new pkos::HotDropperDB(this);

    noSound = false;

    this->findLogfiles();

    connect(watcher,SIGNAL(newCheck(QString,QStringList)),this,SLOT(newCheck(QString,QStringList)));
    connect(mainwindow,SIGNAL(disableSound(bool)),this,SLOT(disableSound(bool)));

    mainwindow->show();
}

ProvidenceKOSLookup::~ProvidenceKOSLookup()
{
    mainwindow->hide();
    delete mainwindow;
}

bool ProvidenceKOSLookup::findLogfiles()
{
    QString logpath;
#ifdef Q_WS_WIN
    logpath = QFileInfo(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).absoluteFilePath();
#else
#ifdef Q_WS_X11
    QSettings settings;
    if ( settings.contains("wine/docpath") )
        logpath = settings.value("wine/docpath").toString();
    else
        logpath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#else
#error Unsupported plattform
#endif
#endif
    logpath.append("/EVE/logs/Chatlogs/");

    qDebug() << "scanning for logfiles in " << logpath;

    QDir logdir(logpath);
    QDate today(QDate::currentDate ());


    QString datestring;
    QStringList filter;
    QFileInfoList results;
    QRegExp logre("^(.*)_\\d+_\\d+\\.txt$");

    datestring = today.toString("yyyyMMdd");
    filter << "*_" + datestring + "_*.txt";

    datestring = today.addDays(-1).toString("yyyyMMdd");
    filter << "*_" + datestring + "_*.txt";

    qDebug() << "using filter patterns: " << filter;

    logdir.setNameFilters(filter);
    logdir.setSorting(QDir::Time);

    results = logdir.entryInfoList();

    watcher->reset();
    seen_prefix.clear();
    Q_FOREACH(QFileInfo finfo,results) {
        int idx = logre.indexIn(finfo.fileName());
        if ( seen_prefix.indexOf(logre.cap(1)) >= 0 )
            continue;
        seen_prefix.append(logre.cap(1));
        watcher->addLog(finfo.absoluteFilePath());
        qDebug() << "added file:" << finfo.absoluteFilePath();
    }
    if (seen_prefix.size() == 0)
        return false;

    return true;
}

void ProvidenceKOSLookup::newCheck(const QString& pilot, const QStringList& units)
{

    QMetaObject::invokeMethod(mainwindow,"addSeperator",Q_ARG(const QString,pilot));
    notifyDone = hdNotifyDone = false;

    QList<pkos::kos_state_t> states;
    Q_FOREACH(QString u,units) {
        pkos::kos_state_t state;
        INIT_KOS_STATE(state,u);
        state.listEntry = mainwindow->addKosEntry(u);
        states.push_back(state);
    }
    Q_FOREACH(pkos::kos_state_t s,states) {
        this->checkKOS(s);
    }
}

void ProvidenceKOSLookup::disableSound(bool disable)
{
    noSound = disable;
}

void ProvidenceKOSLookup::quit()
{
    QCoreApplication::instance()->exit();
}


#ifndef QT_NO_DEBUG_OUTPUT
QDebug operator<<(QDebug dbg, const pkos::kos_state_t &s) {
    dbg.nospace() << "{ checkname = " << s.checkname << ";"
                  <<  " pilot = " << s.pilot << "; pilot_id = " << s.pilot_id << "; pilot_name = " << s.pilot_name << "; pilot_cached = " << s.pilot_cached << ";"
                  <<  " corp  = " << s.corp  << "; corp_id = " << s.corp_id << "; corp_name = " << s.corp_name << "; corp_npc = " << s.corp_npc << "; corp_cached = " << s.corp_cached << ";"
                  <<  " alliance = " << s.alliance << "; alliance_id = " << s.alliance_id << "; alliance_name = " << s.alliance_name << "; alliance_cached = " << s.alliance_cached << ";"
                  <<  " unit_icon ="  << s.unit_icon << "; };";
    return dbg.space();
}
#endif

void ProvidenceKOSLookup::checkKOS(pkos::kos_state_t state)
{
    qDebug() << "starting KOS check for unit" << state.checkname;
    checkUnit(state);
    qDebug() << "  current state:" << state;
    pkos::eveapi::characterInfo_t* charinfo = 0;
    qDebug() << "  looking for last non-npc corp";
    if ( state.corp_npc == true && state.pilot < 1 ) {
        int     first_corp_state  = state.corp;
        qint32  first_corp_id     = state.corp_id;
        QString first_corp_name   = state.corp_name;
        while ( state.corp_npc == true && state.corp < 1 ) {
            // if the current corp is npc, look for the next non-npc corp
            ++state.history_idx;
            if ( ! charinfo ) {
                charinfo = new pkos::eveapi::characterInfo_t;
                *charinfo = eapi->getCharacterInfo(state.pilot_id);
            }
            if ( state.history_idx >= charinfo->employmentHistory.size())
                break;

            pkos::eveapi::corpSheet_t corpinfo = this->eapi->getCorpSheet(charinfo->employmentHistory[state.history_idx].corporationID);
            qDebug() << "    recheking with old corp" << corpinfo.name;
            state.corp_name = corpinfo.name;
            state.corp_id   = corpinfo.id;
            checkCorp(state);
            qDebug() << "      new state:" << state;
        }
        if ( (state.corp_npc == true && state.corp_npc != 1) || state.corp == -2 ) {
            state.corp        = first_corp_state;
            state.corp_id     = first_corp_id;
            state.corp_name   = first_corp_name;
            state.history_idx = 0;
        }
    }

    getUnitImage(state);

    pkos::list_entry_data_t entry_data;
    if ( state.pilot > -2 ) {
        entry_data.unit_type = 1;
        entry_data.id = state.pilot_id;
    }
    else if ( state.corp > -2 ) {
        entry_data.unit_type = 2;
        entry_data.id = state.corp_id;
    }
    else if ( state.alliance > -2 ) {
        entry_data.unit_type = 3;
        entry_data.id = state.alliance_id;
    }
    else
        entry_data.unit_type = 0;

    entry_data.name = state.checkname;
    entry_data.icon = state.unit_icon;
    entry_data.hotdropper = false;
    entry_data.kos = false;

    entry_data.updateHandler = this;
    if ( state.pilot < 0 && state.corp == -2 && state.alliance == -2 ) {
        entry_data.kos = -1;
        state.listEntry->setData(entry_data);
        mainwindow->scrollToEntry(state.listEntry);
        return;
    }

    if ( state.pilot > 0 ) {
        entry_data.kos = true;
        entry_data.kosby = QString("Pilot (") + state.pilot_name + ")" ;
    }
    if ( state.history_idx > 0 ) {
        if (state.corp > 0 || state.alliance > 0 ) {
            entry_data.kos = true;
            entry_data.kosby = QString("Last Corp (") + state.corp_name + ")";
        }
    } else {
        if (state.corp > 0 ) {
            entry_data.kos = true;
            entry_data.kosby = QString("Corp (" +state.corp_name + ")");
        }
        if (state.alliance > 0 ) {
            entry_data.kos = true;
            entry_data.kosby = QString("Alliance (") + state.alliance_name + ")";
        }
    }

    if ( entry_data.kos && state.pilot >= 0 ) {
        qDebug() << "  checking hotdropper state";
        entry_data.hotdropper = hddb->isHotdropper(state.pilot_id);
    }

    if ( entry_data.kos && ( !notifyDone || !hdNotifyDone ) ) {
        if ( !noSound )
            if ( entry_data.hotdropper ) {
                audio->play(settings->value("audio/hotdropper",":/hotdropper.wav").toString());
                notifyDone = hdNotifyDone = true;
            }
            else if ( !notifyDone )
                audio->play(settings->value("audio/file",":/alarm.wav").toString());
        notifyDone = true;
    }
    state.listEntry->setData(entry_data);
    mainwindow->scrollToEntry(state.listEntry);
}

void ProvidenceKOSLookup::updateHD(qint32 id, const QString& name, bool hd)
{
    hddb->insert(id,name,hd);
}


void ProvidenceKOSLookup::checkUnit(pkos::kos_state_t& state)
{
    qDebug() << "  lookup cache for unit" << state.checkname;
    pkos::CacheDB::cacheEntry_t *entry = 0;
    if ( cachedb->isOpen() )
        entry = cachedb->getByName(state.checkname);
    if ( entry != 0 ) {
        qDebug() << "    hit";
        switch (entry->type) {
        case pkos::CacheDB::pilot:
            qDebug() << "      unit type: pilot";
            state.pilot         = entry->kos;
            state.pilot_name    = entry->name;
            state.pilot_id      = entry->eveID;
            state.pilot_cached  = true;
            if ( entry->parentID > 0 ) {
                state.corp_id   = entry->parentID;
                state.corp_name = entry->parentName;
                checkCorp(state);
            }
            break;
        case pkos::CacheDB::corp:
            qDebug() << "      unit type: corp";
            if ( entry->kos == -1 ) {
                state.corp = 0;
                state.corp_npc = true;
            } else {
                state.corp      = entry->kos;
                state.corp_npc = false;
            }
            state.corp_name    = entry->name;
            state.corp_id      = entry->eveID;
            state.corp_cached  = true;
            if ( entry->parentID > 0 ) {
                state.alliance_id   = entry->parentID;
                state.alliance_name = entry->parentName;
                checkAlliance(state);
            }
            break;
        case pkos::CacheDB::alliance:
            qDebug() << "      unit type: corp";
            state.alliance         = entry->kos;
            state.alliance_name    = entry->name;
            state.alliance_id      = entry->eveID;
            state.alliance_cached  = true;
            break;
        default:
            break;
        }
        delete entry;
    } else {
        qDebug() << "    miss";
        capi->checkUnits(state);
        if ( state.pilot == -2 && state.corp == -2 && state.alliance == -2 )
            searchEveUnit(state);
        updateCache(state);
    }
}

void ProvidenceKOSLookup::checkCorp(pkos::kos_state_t& state)
{
    qDebug() << "  lookup cache for corp" << state.corp_name;
    pkos::CacheDB::cacheEntry_t *entry = 0;
    if ( cachedb->isOpen() )
        entry = cachedb->getByName(state.corp_name);
    if ( entry != 0 ) {
        qDebug() << "    hit";
        if ( entry->kos == -1 ) {
            state.corp = 0;
            state.corp_npc = true;
        } else {
            state.corp      = entry->kos;
            state.corp_npc = false;
        }
        state.corp_id   = entry->eveID;
        state.corp_cached = true;
        if ( entry->parentID > 0 ) {
            state.alliance_id     = entry->parentID;
            state.alliance_name   = entry->parentName;
            checkAlliance(state);
        }
        delete entry;
    } else {
        qDebug() << "    miss";
        state.corp_cached = false;
        capi->recheckUnit(state,1);
        // TODO - above fuckup
        if ( state.corp == -2 ) {
            qDebug() << "    corp not found on CVA Database";
            searcheEveCorp(state);
            if ( state.alliance_id != 0 )
                checkAlliance(state);
        }
        updateCache(state,1);
    }
}

void ProvidenceKOSLookup::checkAlliance(pkos::kos_state_t& state)
{
    qDebug() << "  lookup cache for alliance" << state.alliance_name;
    pkos::CacheDB::cacheEntry_t *entry = 0;
    if ( cachedb->isOpen() )
        entry = cachedb->getByName(state.alliance_name);
    if ( entry != 0 ) {
        qDebug() << "    hit";
        state.alliance      = entry->kos;
        state.alliance_id   = entry->eveID;
        state.alliance_cached = true;
        delete entry;
    } else {
        qDebug() << "    miss";
        capi->recheckUnit(state,2);
        updateCache(state,2);
    }
}


void ProvidenceKOSLookup::searchEveUnit(pkos::kos_state_t& state)
{
    qDebug() << "  searching unit in EVE Database";
    qint32 unitID = eapi->getCharacterID(state.checkname);
    qDebug() << "  got unitid" << unitID;
    if ( unitID <= 0 )
        return; // leave here if we don't find the unit in the eve api

    // check if we could get a character info sheet
    pkos::eveapi::characterInfo_t charinfo = eapi->getCharacterInfo(unitID);
    if ( charinfo.id == unitID ) {
        // we got a pilot here!
        qDebug() << "    found character info, must be a pilot";
        state.pilot_id  = unitID;
        state.pilot_name = state.checkname;
        state.pilot     = -1; // pilot exists, but is not in the KOS database
        state.corp_id   = charinfo.corporationID;
        state.corp_name = charinfo.corporation;
        checkCorp(state);
        return; // nothing more to do here
    }
    qDebug() << "    character info not found, could be a corp";
    pkos::eveapi::corpSheet_t corpinfo = this->eapi->getCorpSheet(unitID);
    if ( corpinfo.id == unitID && corpinfo.allianceID != 0 ) {
        // we found a corp, we recheck with the alliance name
        qDebug() << "      found a matching corp, rechecking with current alliance:" << corpinfo.allianceName;
        state.corp_id         = unitID;
        state.corp_name       = state.checkname;
        state.alliance_id     = corpinfo.allianceID;
        state.alliance_name   = corpinfo.allianceName;
        checkAlliance(state);
        return; // done here
    }
    qDebug() << "    didn't found a matching corpsheet. We are alliance. Check is useless";
    state.alliance_id   = unitID;
    state.alliance_name = state.checkname;
}

void ProvidenceKOSLookup::searcheEveCorp(pkos::kos_state_t& state)
{
    qDebug() << "    searching corp in EVE Database";
    if ( state.corp_id == 0 ) {
        // oh, we don't have an corpID right now, lets try to find it
        qint32 unitID = eapi->getCharacterID(state.checkname);
        if ( unitID <= 0 ) {
            // no luck
            qDebug() << "      could not find a matching corp in the EVE database";
            return;
        }
        state.corp_id = unitID;
    }
    pkos::eveapi::corpSheet_t corpinfo = this->eapi->getCorpSheet(state.corp_id);
    if ( corpinfo.id == 0 ) {
        qDebug() << "      could not find a matching corp in the EVE database";
        return;
    }
    if ( corpinfo.id >= 1000002 && corpinfo.id <= 1000182 ) {
        qDebug() << "      the corp has an id in the NPC range";
        state.corp      = 0;
        state.corp_npc  = true;
        state.corp_name = corpinfo.name;
        state.corp_id   = corpinfo.id;
    }
    else if ( corpinfo.allianceID == 0 ) {
        qDebug() << "      the corp has no alliance, setting not listed";
        state.corp      = -2;
        state.corp_name = corpinfo.name;
        state.corp_id   = corpinfo.id;
        state.corp_npc  = false;
    }
    else {
        qDebug() << "    corp found, setting alliance:" << corpinfo.allianceName;
        state.alliance_id   = corpinfo.allianceID;
        state.alliance_name = corpinfo.allianceName;
        state.corp_npc      = false;
    }
}

void ProvidenceKOSLookup::updateCache(pkos::kos_state_t& state, int start)
{
    if (!cachedb->isOpen()) {
        qWarning() << "database not open, could not update cache!";
        return;
    }
    switch(start) {
    case 0:
        if ( state.pilot > -2 && !state.pilot_cached ) {
            qDebug() << "    updating cache for pilot" << state.pilot_id << state.pilot_name;
            cachedb->insert(state.pilot_id,state.pilot_name,pkos::CacheDB::pilot,state.pilot,state.corp_id,state.corp_name);
            state.pilot_cached = true;
        }
    case 1:
        if ( state.corp > -2 && !state.corp_cached) {
            qDebug() << "    updating cache for corp" << state.corp_id << state.corp_name;
            int corp_state = ( state.corp_npc && state.corp == 0 ) ? -1 : state.corp;
            cachedb->insert(state.corp_id,state.corp_name,pkos::CacheDB::corp,corp_state,state.alliance_id,state.alliance_name);
            state.corp_cached = true;
        }
    case 2:
        if ( state.alliance > -2 && !state.alliance_cached) {
            qDebug() << "    updating cache for alliance" << state.alliance_id << state.alliance_name;
            cachedb->insert(state.alliance_id,state.alliance_name,pkos::CacheDB::alliance,state.alliance);
            state.alliance_cached = true;
        }
    default:
        break;
    }
}

void ProvidenceKOSLookup::getUnitImage(pkos::kos_state_t& state)
{
    qint32  unitID = 0;
    QString path;
    QString extension = "png";
    if (state.pilot_id != 0 ) {
        unitID = state.pilot_id;
        path = "Character";
        extension = "jpg";
    }
    else if ( state.corp_id != 0 ) {
        unitID = state.corp_id;
        path = "Corporation";
    }
    else if ( state.alliance_id != 0 ) {
        unitID = state.alliance_id;
        path = "Alliance";
    }
    if ( unitID != 0 )
        state.unit_icon = QString("http://image.eveonline.com/%1/%2_32.%3").arg(path).arg(unitID).arg(extension);
    else
        state.unit_icon = QString("http://image.eveonline.com/Alliance/1_32.png");
}


#include "ProvidenceKOSLookup.moc"
