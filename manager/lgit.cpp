#include "lgit.h"
#ifdef _WIN32
# include <Windows.h>
#else
# include <unistd.h>
#endif

lgit::lgit(QObject *parent) : QObject(parent) , repo_(NULL)
{
    git_libgit2_init();
}

lgit::~lgit()
{
    git_libgit2_shutdown();
}

int lgit::openRepo()
{

}
