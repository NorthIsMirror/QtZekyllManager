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

#include "gitoperationtracker.h"

#include "pulldialog.h"

#include <QString>
#include <QDebug>

GitOperationTracker::GitOperationTracker()
{

}

int GitOperationTracker::updateFetchProgress( double progress )
{
    PullDialog *pdialog = static_cast< PullDialog * > ( pulldialog_ );
    if( !pdialog ) {
        return 1;
    }

    return 0;
}

int GitOperationTracker::checkoutNotify(git_checkout_notify_t why, const char *path)
{
    PullDialog *pdialog = static_cast< PullDialog * > ( pulldialog_ );
    if( !pdialog ) {
        return 1;
    }

    pdialog->addNotification( why, path );

    return 0;
}
