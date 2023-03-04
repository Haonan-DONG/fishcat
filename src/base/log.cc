#include "base/log.h"

void InitialGoogleLog(char **argv)
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_colorlogtostderr = true;
    FLAGS_logtostderr = 1;
}