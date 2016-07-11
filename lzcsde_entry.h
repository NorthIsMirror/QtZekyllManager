#ifndef LZCSDE_ENTRY_H
#define LZCSDE_ENTRY_H

#include <QString>

class LZCSDE_Entry
{
    int id_;
    QString listing_;
    QString zekyll_;
    bool checked_;
    QString section_;
    QString description_;
    QString error_;

public:
    LZCSDE_Entry();
    int id() const { return id_; }
    const QString &listing() const { return listing_; }
    const QString &zekyll() const { return zekyll_; }
    bool checked() const { return checked_; }
    const QString &section() const { return section_; }
    const QString &description() const { return description_; }
    const QString &error() const { return error_; }

    void setId( int id ) { id_ = id; }
    void setListing( const QString & listing ) { listing_ = listing; }
    void setZekyll( const QString & zekyll ) { zekyll_ = zekyll; }
    void setChecked( bool checked ) { checked_ = checked; }
    void setSection( const QString & section ) { section_ = section; }
    void setDescription( const QString & description ) { description_ = description; }
    void setError( const QString & error ) { error_ = error; }
};

#endif // LZCSDE_ENTRY_H
