#include "lzcsde.h"

LZCSDE::LZCSDE()
{
    dummy_entry_.setId(-1);
}

bool LZCSDE::insertFromListing( int id, const QString & listing ) {
    QRegExp rx1("([a-z0-9][a-z0-9][a-z0-9])\\.([A-Z])--(.*) >>(.*)<<");
    QRegExp rx2("([a-z0-9][a-z0-9][a-z0-9]) >>(.*)<<");
    QRegExp rx3("([a-z0-9][a-z0-9][a-z0-9])\\.([A-Z])--(.*)");
    rx1.setCaseSensitivity(Qt::CaseSensitive);
    rx2.setCaseSensitivity(Qt::CaseSensitive);
    rx3.setCaseSensitivity(Qt::CaseSensitive);
    if (rx1.indexIn( listing ) != -1) {
        LZCSDE_Entry entry;
        entry.setId( id );
        entry.setListing( listing );
        entry.setZekyll( rx1.cap(1) );
        entry.setChecked( false );
        entry.setSection( rx1.cap(2) );
        entry.setDescription( rx1.cap(3) );
        entry.setError( rx1.cap(4) );

        entries_.append( entry );
        return true;
    } else if (rx2.indexIn( listing ) != -1) {
        LZCSDE_Entry entry;
        entry.setId( id );
        entry.setListing( listing );
        entry.setZekyll( rx2.cap(1) );
        entry.setChecked( false );
        entry.setSection( "" );
        entry.setDescription( "" );
        entry.setError( rx2.cap(2) );

        entries_.append( entry );
        return true;
    } else if (rx3.indexIn( listing ) != -1) {
        LZCSDE_Entry entry;
        entry.setId( id );
        entry.setListing( listing );
        entry.setZekyll( rx3.cap(1) );
        entry.setChecked( false );
        entry.setSection( rx3.cap(2) );
        entry.setDescription( rx3.cap(3) );
        entry.setError( "" );

        entries_.append( entry );
        return true;
    } else {
        return false;
    }
}

const LZCSDE_Entry & LZCSDE::getId( int id ) {
    foreach( const LZCSDE_Entry & entry, entries_ ) {
        if( id == entry.id() ) {
            return entry;
        }
    }
    return dummy_entry_;
}

const LZCSDE_Entry & LZCSDE::getId( const QString & id ) {
    bool ok = false;
    int intid = id.toInt(&ok);
    if( ok ) {
        return getId( intid );
    } else {
        return dummy_entry_;
    }
}
