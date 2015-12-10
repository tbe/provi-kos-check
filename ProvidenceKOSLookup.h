#ifndef ProvidenceKOSLookup_H
#define ProvidenceKOSLookup_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <phonon/Path>

#include "include/pkostypes.h"

class QSettings;
class QThread;


namespace Phonon {
class AudioOutput;
class MediaObject;
}

namespace pkos {
class MainWindow;
class CacheDB;
class CvaKosAPI;
class EveAPI;
class logWatcher;
class Network;
class Audio;
class HotDropperDB;
}

class ProvidenceKOSLookup : public QObject
{
    Q_OBJECT
public:
    friend class pkos::MainWindow;

    ProvidenceKOSLookup();
    virtual ~ProvidenceKOSLookup();

public slots:
    void updateHD(qint32 id, const QString& name, bool hd);
private slots:
    bool findLogfiles();
    void newCheck(const QString& pilot, const QStringList& units);
    void disableSound(bool disable = false);
    void quit();


private:
    void checkKOS(pkos::kos_state_t state);
    void checkUnit(pkos::kos_state_t& state);
    void checkCorp(pkos::kos_state_t& state);
    void checkAlliance(pkos::kos_state_t& state);

    void searchEveUnit(pkos::kos_state_t& state);
    void searcheEveCorp(pkos::kos_state_t& state);

    void updateCache(pkos::kos_state_t& state, int start = 0);
    void getUnitImage(pkos::kos_state_t& state);

    pkos::MainWindow*   mainwindow;
    pkos::CvaKosAPI*    capi;
    pkos::EveAPI*       eapi;
    pkos::logWatcher*   watcher;
    QList<QString>      seen_prefix;

    QThread*            backgroundThr;

    pkos::Network*      windowNet;
    pkos::Network*      backgroundNet;

    QSettings*          settings;

    pkos::Audio*        audio;
    bool                noSound;
    bool                notifyDone,hdNotifyDone;

    pkos::CacheDB*      cachedb;
    pkos::HotDropperDB* hddb;

};

#endif // ProvidenceKOSLookup_H
