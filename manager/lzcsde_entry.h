/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
