## QLogCollector
格式化日志输出

### 导入库

```cmake
find_package(QLogCollector 1.x.x REQUIRED COMPONENTS Server)
add_compile_definitions(ROOT_PROJECT_PATH="${CMAKE_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} QLogCollector::Server)
```

### 初始化

```c++
#include <qlogcollector/server/logcollector.h>
#include <qlogcollector/server/outputs/fileoutputtarget.h>
#include <qlogcollector/server/outputs/memoryoutputtarget.h>
#include <qlogcollector/server/outputs/traceroutputtarget.h>

QLOGCOLLECTOR_USE_NAMESPACE

int main(int argc, char* argv[]) { 
    QApplication a(argc, argv);
    //...
    LogCollector::styleConfig
        .wordWrap(120) //限制单行字符宽度（可选）
        .simpleCodeLine() //显示简单文件名，不包含路径（可选）
        .systemCodePage() //使用系统编码（可选）
        .disableNonAscii() //启用非ascii字符打印检查（可选）
        .projectSourceCodeRootPath(ROOT_PROJECT_PATH) //设置源代码工程根路径，设置后将计算文件相对路径用于定位
    ;
    //注册QDebug日志
    LogCollector::registerLog();
    //添加控制台输出目标
    LogCollector::addOutputTarget(OutputTarget::currentConsoleOutput(Ide::clion));
    //添加内存输出目标
    auto memoryOutput = new MemoryOutputTarget;
    LogCollector::addOutputTarget(memoryOutput);
    //添加文件输出目标
    LogCollector::addOutputTarget(new FileOutputTarget(
        FileOutputConfigBuilder()
            .baseFileName("my_log") //设置文件名前缀
            .contentLimitLines(1000) //限制文件内容行数（可选）
            .fileLimitSize(10) //限制文件个数（可选）
            .machineEncodeMode(true) //使用json结构化字符串并base64编码保存到文件（可选）
            .saveDir(QCoreApplication::applicationDirPath()) //设置保存文件目录（可选）
    ));
    //添加trace输出目标（可选）
    //启用后日志末尾会追加trace_id，并将trace详情写入csv文件
    //如果与FileOutputTarget同时使用，TracerOutputTarget参数会跟随FileOutputTarget
    LogCollector::addOutputTarget(new TracerOutputTarget(
        TracerOutputConfigBuilder()
            .baseFileName("my_log") //设置文件名前缀（可选）
            .contentLimitLines(1000) //限制文件内容行数（可选）
            .fileLimitSize(10) //限制文件个数（可选）
            .saveDir(QCoreApplication::applicationDirPath()) //设置保存文件目录（可选）
    ));
    //绑定异常信号
    LogCollector::bindSignalFatal();
}
```

使用自己的`MessageHandler`
```c++
#include <qlogcollector/server/logcollector.h>
#include <qlogcollector/server/outputs/fileoutputtarget.h>
#include <qlogcollector/server/outputs/memoryoutputtarget.h>
#include <qlogcollector/server/outputs/traceroutputtarget.h>

QLOGCOLLECTOR_USE_NAMESPACE

void myCustomMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    //...
    LogCollector::collectorMessageHandle(type, context, msg);
}

int main(int argc, char* argv[]) { 
    QApplication a(argc, argv);
    //...
    LogCollector::styleConfig
        .wordWrap(120) //限制单行字符宽度（可选）
        .simpleCodeLine() //显示简单文件名，不包含路径（可选）
        .systemCodePage() //使用系统编码（可选）
        .disableNonAscii() //启用非ascii字符打印检查（可选）
        .projectSourceCodeRootPath(ROOT_PROJECT_PATH) //设置源代码工程根路径，设置后将计算文件相对路径用于定位
    ;
    //注册QDebug日志
    LogCollector::init();
    //添加控制台输出目标
    LogCollector::addOutputTarget(OutputTarget::currentConsoleOutput(Ide::clion));
    //添加内存输出目标
    auto memoryOutput = new MemoryOutputTarget;
    LogCollector::addOutputTarget(memoryOutput);
    //添加文件输出目标
    LogCollector::addOutputTarget(new FileOutputTarget(
        FileOutputConfigBuilder()
            .baseFileName("my_log") //设置文件名前缀
            .contentLimitLines(1000) //限制文件内容行数（可选）
            .fileLimitSize(10) //限制文件个数（可选）
            .machineEncodeMode(true) //使用json结构化字符串并base64编码保存到文件（可选）
            .saveDir(QCoreApplication::applicationDirPath()) //设置保存文件目录（可选）
    ));
    //添加trace输出目标（可选）
    //如果与FileOutputTarget同时使用，TracerOutputTarget参数会跟随FileOutputTarget
    LogCollector::addOutputTarget(new TracerOutputTarget(
        TracerOutputConfigBuilder()
            .baseFileName("my_log")
            .saveDir(QCoreApplication::applicationDirPath())
    ));
    //绑定异常信号
    LogCollector::bindSignalFatal();
    //自定义消息处理
    qInstallMessageHandler(myCustomMessageHandler);
}
```

### Trace调用链记录

包含`"tracescope.h"`头文件，在函数入口直接使用`QLOG_TRACE_SCOPE`：

```cpp
#include <qlogcollector/server/tracescope.h>

void foo() {
    QLOG_TRACE_SCOPE;
    qDebug() << "log in foo";
}
```

跨线程传递trace上下文：

```cpp
auto ctx = LogCollector::exportTraceContext();
QtConcurrent::run([ctx]{
    LogCollector::importTraceContext(ctx);
    QLOG_TRACE_SCOPE;
    qDebug() << "log in worker thread";
    LogCollector::clearTraceContext(); //结束当前线程链路传递
});
```

说明：
- 仅当添加了`TracerOutputTarget`时，trace信息才会被记录。
- 普通日志末尾只附加`trace_id`，trace详情写入`*_trace_YYYY-MM-DD_N.csv`。
- 当`FileOutputTarget`和`TracerOutputTarget`同时启用时，trace文件会跟随日志文件使用相同分包参数。

### Trace解码工具

项目根目录提供单文件静态页面：`tool/trace_decoder.html`。

使用方式：
- 用Chromium内核浏览器打开`tool/trace_decoder.html`。
- 选择trace文件夹（包含`*_trace_*.csv`）。
- 输入`trace_id`后解码，即可查看原始json和结构化详情。

### 日志打印格式化

使用格式化模板输出日志，例如（默认值）：
```c++
LogCollector::setMessageFormat("[%d{HH:mm:ss.zzz}[fg=g]] [%t] %c %p-> %m (%f)");
```

|符号| 含义            |
|:-:|:--------------|
|%d| 时间格式化         |
|%t| 线程名/id        |
|%c| 日志分类          |
|%p| 打印类型（I/D/W/E） |
|%m| 日志内容          |
|%f| 文件名/路径        |

其中，`[fg=g]`用于定义元素颜色，仅`%d%t%c%f`有效，`fg`为前景色，`bg`为背景色，`bl`为闪烁，`ul`为下划线，对于颜色可用的值有：

| 颜色 | 值  |
|:--:|:---|
| r  | 红色 |
| g  | 绿色 |
| y  | 黄色 |
| b  | 蓝色 |
| p  | 紫色 |
| c  | 青色 |
| w  | 白色 |

格式化后的打印输出如下所示：
```
[10:09:41.766] [main] main.log D-> post a log with category...... level d                 (./test/printtest.cpp:13)
```

### 使用带有颜色的日志内容
在日志中添加颜色等样式，包含`"styledstring.h"`头文件，调用`styled`函数进行格式化：
```cpp
#include <qlogcollector/server/colors/styledstring.h>

qDebug() << "post a" + styled("color string", true, true).r().yb(1) + "log";
qCDebug(mainLog) << "post a" << styled("color string").b(1).cb() << "log";
```

`styled`函数可配置的参数说明：
```cpp
styled("log", true, true) //日志内容，是否闪烁（仅支持的控制台有效），是否使用下划线
  .b(1) //文字颜色blue，使用单词首字母表示颜色，参数1表示高亮（下同）
  .bb(1) //背景颜色blue，函数名第二个字母b表示使用背景（下同）
  .g() //green
  .gb() //背景green
  .c() //cyan
  .cb()
  .r() //red
  .rb()
  .p() //purple
  .pb()
  .y() //yellow
  .yb()
  .w() //white
  .wb()
;
```
