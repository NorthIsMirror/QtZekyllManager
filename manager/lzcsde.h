#ifndef LZCSDE_H
#define LZCSDE_H

#include "lzcsde_entry.h"
#include <QObject>
#include <QVector>
#include <Qstring>

class LZCSDE
{
    QVector<LZCSDE_Entry> entries_;
    LZCSDE_Entry dummy_entry_;

public:
    LZCSDE();

    bool insertFromListing( int id, const QString & listing );
    void insertFrom( int id, const QString & listing, const QString & zekyll, bool checked,
                     const QString & section, const QString & description, const QString & error )
    {
        LZCSDE_Entry entry;
        entry.setId(id);
        entry.setListing(listing);
        entry.setZekyll(zekyll);
        entry.setChecked(checked);
        entry.setSection(section);
        entry.setDescription(description);
        entry.setError(error);
        entries_.append(entry);
    }

    void insertFrom( int id, const QString & listing, const QString & zekyll, const QString & section, const QString & description ) {
        LZCSDE_Entry entry;
        entry.setId(id);
        entry.setListing(listing);
        entry.setZekyll(zekyll);
        entry.setChecked(true);
        entry.setSection(section);
        entry.setDescription(description);
        entry.setError("");
        entries_.append(entry);
    }

    void appendEntry( const LZCSDE_Entry & entry ) {
        entries_.append( entry );
    }

    void clear() { entries_.clear(); }

    bool removeId( int id );

    bool removeId( const QString & id );

    bool move( int sourceId, int destId );

    QStringList getZekylls();

    LZCSDE_Entry & getId( int id );

    LZCSDE_Entry & getId( const QString & id );

    QVector<LZCSDE_Entry> & entries() { return entries_; }

    int findIdxOfId( int id );

    bool updateSectionOfId( const QString & id, const QString & section );

    bool updateDescriptionOfId( const QString & id, const QString & description );

    unsigned int count() const { return entries_.count(); }
};

#endif // LZCSDE_H
