#pragma once

#include <configkey.h>

namespace logcollector {

    struct Message : public JsonDumpInterface {
        ConfigKey<qint64> timePoint{"a"};
        ConfigKey<QString> category{"b"};
        ConfigKey<QString> fileName{"c"};
        ConfigKey<int> codeLine{"d"};
        ConfigKey<QString> threadName{"e"};
        ConfigKey<qint64> threadId{"f"};
        ConfigKey<int> level{"g"};
        ConfigKey<QString> log{"h"};

        QList<JsonReadInterface *> prop() override {
            return { &timePoint, &category, &fileName, &codeLine, &threadName, &threadId, &level, &log };
        }
    };
}