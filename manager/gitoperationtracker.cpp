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
