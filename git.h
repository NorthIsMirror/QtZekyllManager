#ifndef GIT_H
#define GIT_H

#include <QObject>

class git : public QObject
{
    Q_OBJECT
public:
    explicit git(QObject *parent = 0);

signals:

public slots:
};

#endif // GIT_H