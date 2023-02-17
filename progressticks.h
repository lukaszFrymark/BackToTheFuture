#ifndef PROGRESSTICKS_H
#define PROGRESSTICKS_H
#include <QtCore>

class ProgressTicks
{
    public:

    ProgressTicks();
    void tick();
    void static customHandler(QtMsgType, const QMessageLogContext &, const QString &msg);
    int inc = 0;
    char bar[5] = "-\\|/";
};

#endif // PROGRESSTICKS_H
