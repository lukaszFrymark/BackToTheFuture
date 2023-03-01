#include "progressticks.h"

ProgressTicks::ProgressTicks()
{
}

void ProgressTicks::tick()
{
    qInstallMessageHandler(ProgressTicks::customHandler);
    qInfo().nospace() << "\b" << bar[inc];
    inc = (inc + 1) % 4;
    qInstallMessageHandler(0);
}

void ProgressTicks::customHandler(QtMsgType, const QMessageLogContext&, const QString& msg)
{
    fprintf(stderr, msg.toLatin1().data());
    fflush(stderr);
}
