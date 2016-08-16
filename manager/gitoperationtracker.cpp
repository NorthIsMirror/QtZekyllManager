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
    return 0;
}

int GitOperationTracker::checkoutNotify(git_checkout_notify_t why, const char *path)
{
    switch( why ) {
    case GIT_CHECKOUT_NOTIFY_NONE:
        break;
    case GIT_CHECKOUT_NOTIFY_CONFLICT:
        break;
    case GIT_CHECKOUT_NOTIFY_DIRTY:
        break;
    case GIT_CHECKOUT_NOTIFY_UPDATED:
        break;
    case GIT_CHECKOUT_NOTIFY_UNTRACKED:
        break;
    case GIT_CHECKOUT_NOTIFY_IGNORED:
        break;
    default:
        break;
    }

    qDebug() << why << path;

    return 0;
}
