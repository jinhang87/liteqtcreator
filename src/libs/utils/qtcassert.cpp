

#include "qtcassert.h"

#include <QByteArray>

namespace Utils {

void writeAssertLocation(const char *msg)
{
    static bool goBoom = !qgetenv("QTC_FATAL_ASSERTS").isEmpty();
    if (goBoom)
        qFatal("SOFT ASSERT made fatal: %s", msg);
    else
        qDebug("SOFT ASSERT: %s", msg);
}

} // namespace Utils
