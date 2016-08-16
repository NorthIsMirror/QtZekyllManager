#ifndef GITOPERATIONTRACKER_H
#define GITOPERATIONTRACKER_H


class GitOperationTracker
{
public:
    GitOperationTracker();

    int updateFetchProgress( double progress );

    void setPullDialog( void * pullDialog ) { pulldialog_ = pullDialog; }
private:
    void *pulldialog_;
};

#endif // GITOPERATIONTRACKER_H
