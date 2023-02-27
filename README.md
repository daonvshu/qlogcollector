### 日志打印格式化
格式化后的打印输出为以下格式：
```
时间          线程名/id   日志分类       打印类型（I/D/W/E） 日志内容         文件相对路径
12:30:21.374  main       <no-category>  D->                test print log   (./testlog.cpp)
```

### 使用styleconfig配置日志打印配置
```cpp
logcollector::styleConfig
    //设置打印日志和颜色代码的处理方式
    //TARGET_STANDARD_OUTPUT: 使用std::cout打印日志，显示不同颜色时使用linux颜色代码实现（默认值）
    //TARGET_WIN32_CONSOLE_APP: 使用std::cout打印日志，使用SetConsoleTextAttribute设置控制台字符颜色
    //TARGET_WIN32_DEBUG_CONSOLE: 使用OutputDebugString(unicode)打印日志，该模式下对应vs控制台，无法配置颜色
    .target(ConsoleOutputTarget::TARGET_STANDARD_OUTPUT)
    //声明当前程序为控制台应用程序
    .consoleApp()
    //声明当前程序为窗口应用程序
    .windowApp()
    //表明当前调试代码的idea为clion，参数runWithPty对应注册表中的开关run.processes.with.pty
    .ide_clion(false)
    //表明当前调试代码的idea为Visual Studio
    .ide_vs()
    //表明当前调试代码的idea为VSCode
    .ide_vscode()
    //表明当前调试代码的idea为QtCreator（使用等宽字体以对齐格式化后的日志）
    .ide_qtcreator()
    //禁用高亮样式
    .disableLighterStyle()
    //禁用下划线样式
    .disableUnderlineStyle()
    //设置自动换行日志内容的宽度（日志中出现中文会导致日志无法对齐）
    .wordWrap(90)
    //使用本地控制台编码（中文乱码时尝试调用该函数）
    .systemCodePage()
    //文件路径仅使用文件名而不是相对路径
    .simpleCodeLine()
;
```
确定编码的ide情况下，仅配置ide相关参数即可：
```cpp
#ifdef Q_OS_WIN
    logcollector::styleConfig
        .windowApp()
        .ide_clion(false)
        .wordWrap(120)
    ;
#elif defined Q_OS_LINUX
    logcollector::styleConfig
        .wordWrap(120)
        .simpleCodeLine()
    ;
#endif
```

初始化并注册日志：
```cpp
logcollector::QLogCollector::instance().publishService().registerLog();
```
`registerLog`函数内部会调用`qInstallMessageHandler`拦截qDebug输出，如果要主动拦截qDebug，在回调中调用`collectorMessageHandle`
```cpp
void myCustomMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    static QMutex mutex;
    mutex.lock();
    //process your log handler ...
    logcollector::QLogCollector::instance().collectorMessageHandle(type, context, msg);
    mutex.unlock();
}

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);
    
    logcollector::styleConfig
        .windowApp()
        .ide_clion(false)
        .wordWrap(120)
    ;
    logcollector::QLogCollector::init();
    qInstallMessageHandler(myCustomMessageHandler);
    
    //init your application ...
    
    return a.exec();
}
```

### 使用带有颜色的日志内容
在日志中添加颜色等样式，包含`"styledstring.h"`头文件，调用`styled`函数进行格式化：
```cpp
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

### CMakeLists相关的配置
```cmake
#...

#添加qlogcollector模块
add_subdirectory(3rdparty/qlogcollector/src)

#...

#静态链接到qlogcollector
target_link_libraries(${PROJECT_NAME}
    #...
    qlogcollector::server
)
```
