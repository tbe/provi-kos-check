#ifndef PKOSTYPES_H
#define PKOSTYPES_H
#include <QtCore/QString>

class ProvidenceKOSLookup;
namespace pkos {
class KOSListEntryCompact;

typedef struct {
    QString checkname;

    int pilot;
    int pilot_id;
    QString pilot_name;
    bool pilot_cached;

    int corp;
    int corp_id;
    bool corp_npc;
    QString corp_name;
    bool corp_cached;


    int alliance;
    int alliance_id;
    QString alliance_name;
    bool alliance_cached;

    int history_idx;
    QString unit_icon;

    KOSListEntryCompact* listEntry;
} kos_state_t;

typedef struct {
    QString name;
    QString icon;
    int kos;
    QString kosby;
    bool hotdropper;
    qint32 id;
    int unit_type; // 1=p, 2=c, 3=a
    ProvidenceKOSLookup *updateHandler;
} list_entry_data_t;
};

#define INIT_KOS_STATE(state,unit) state.checkname = unit; state.pilot = -2; state.corp = -2; state.alliance = -2; state.history_idx = 0; \
                                   state.pilot_id = 0; state.corp_id = 0; state.corp_npc = false; state.alliance_id = 0; \
                                   state.pilot_cached = false; state.corp_cached = false; state.alliance_cached = false;
#endif//PKOSTYPES_H
