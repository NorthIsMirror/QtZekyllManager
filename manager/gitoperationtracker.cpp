#include "gitoperationtracker.h"

#include <QString>
#include <QDebug>

GitOperationTracker::GitOperationTracker()
{

}

int GitOperationTracker::updateFetchProgress( double progress )
{
    qDebug() << "Progress: " << progress;
    return 0;
}
