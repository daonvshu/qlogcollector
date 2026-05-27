#include "tracescope.h"

#include <qthreadstorage.h>
#include <qthread.h>
#include <qbytearray.h>
#include <qdatetime.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qfileinfo.h>

QLOGCOLLECTOR_BEGIN_NAMESPACE

struct TraceFrame {
    QString fileName;
    int line = 0;
    QString className;
    QString functionName;
    QString functionSignature;
    qint64 enterMsSinceEpoch = 0;
    qint64 elapsedMsFromRoot = 0;
};

struct TraceContext {
    QString traceId;
    qint64 rootStartMsSinceEpoch = 0;
    QList<TraceFrame> stack;
};

static QThreadStorage<TraceContext*> g_traceContextStorage;

static QString trimmedFunctionName(const QString& functionSignature) {
    auto p = functionSignature.indexOf('(');
    auto left = (p > 0) ? functionSignature.left(p).trimmed() : functionSignature.trimmed();
    auto pieces = left.split(' ');
    return pieces.isEmpty() ? left : pieces.last();
}

static QString resolveClassName(const QString& functionName) {
    auto pos = functionName.lastIndexOf("::");
    if (pos < 0) {
        return {};
    }
    return functionName.left(pos);
}

static QString resolveSimpleFunctionName(const QString& functionName) {
    auto pos = functionName.lastIndexOf("::");
    if (pos < 0) {
        return functionName;
    }
    return functionName.mid(pos + 2);
}

static TraceContext* getOrCreateContext() {
    if (!g_traceContextStorage.hasLocalData()) {
        g_traceContextStorage.setLocalData(new TraceContext);
    }
    return g_traceContextStorage.localData();
}

static qint64 currentMs() {
    return QDateTime::currentMSecsSinceEpoch();
}

static QString createTraceId() {
    return QString::number(currentMs()) + "_" + QString::number((quintptr)QThread::currentThreadId(), 16);
}

static QJsonObject toJson(const TraceContext& context, const QString& traceIdOverride = QString()) {
    QJsonObject root;
    root.insert("trace_id", traceIdOverride.isEmpty() ? context.traceId : traceIdOverride);
    root.insert("root_start_ms", context.rootStartMsSinceEpoch);
    root.insert("depth", context.stack.size());

    QJsonArray frames;
    QStringList chain;
    for (const auto& frame : context.stack) {
        QJsonObject obj;
        obj.insert("file", frame.fileName);
        obj.insert("line", frame.line);
        obj.insert("class", frame.className);
        obj.insert("func", frame.functionName);
        obj.insert("method", frame.functionSignature);
        obj.insert("enter_ms", frame.enterMsSinceEpoch);
        obj.insert("elapsed_ms_from_root", frame.elapsedMsFromRoot);
        frames.append(obj);
    }
    for (int i = context.stack.size() - 1; i >= 0; --i) {
        const auto& frame = context.stack.at(i);
        chain.append(QString("%1 (%2:%3)")
            .arg(frame.functionName)
            .arg(frame.fileName)
            .arg(frame.line));
    }
    root.insert("frames", frames);
    root.insert("call_chain", QJsonArray::fromStringList(chain));
    root.insert("total_elapsed_ms", context.stack.isEmpty() ? 0 : context.stack.last().elapsedMsFromRoot);
    return root;
}

static TraceContext fromJson(const QJsonObject& root) {
    TraceContext context;
    context.traceId = root.value("trace_id").toString();
    context.rootStartMsSinceEpoch = (qint64)root.value("root_start_ms").toDouble();

    auto frames = root.value("frames").toArray();
    for (const auto& frameValue : frames) {
        auto obj = frameValue.toObject();
        TraceFrame frame;
        frame.fileName = obj.value("file").toString();
        frame.line = obj.value("line").toInt();
        frame.className = obj.value("class").toString();
        frame.functionName = obj.value("func").toString();
        frame.functionSignature = obj.value("method").toString();
        frame.enterMsSinceEpoch = (qint64)obj.value("enter_ms").toDouble();
        frame.elapsedMsFromRoot = (qint64)obj.value("elapsed_ms_from_root").toDouble();
        context.stack.append(frame);
    }
    return context;
}

TraceScope::TraceScope(const QString& fileName, int line, const QString& functionSignature) {
    if (functionSignature.isEmpty()) {
        return;
    }
    pushTraceFrame(fileName, line, functionSignature);
    active = true;
}

TraceScope::~TraceScope() {
    if (!active) {
        return;
    }
    popTraceFunction();
}

void pushTraceFrame(const QString& fileName, int line, const QString& functionSignature) {
    if (functionSignature.isEmpty()) {
        return;
    }

    auto* context = getOrCreateContext();
    auto nowMs = currentMs();
    if (context->traceId.isEmpty()) {
        context->traceId = createTraceId();
    }
    if (context->rootStartMsSinceEpoch <= 0) {
        context->rootStartMsSinceEpoch = nowMs;
    }

    TraceFrame frame;
    frame.fileName = QFileInfo(fileName).fileName();
    frame.line = line;
    frame.functionSignature = functionSignature;
    auto normalizedFunction = trimmedFunctionName(functionSignature);
    frame.className = resolveClassName(normalizedFunction);
    frame.functionName = resolveSimpleFunctionName(normalizedFunction);
    frame.enterMsSinceEpoch = nowMs;
    frame.elapsedMsFromRoot = nowMs - context->rootStartMsSinceEpoch;
    context->stack.append(frame);
}

void popTraceFunction() {
    if (!g_traceContextStorage.hasLocalData()) {
        return;
    }
    auto* context = g_traceContextStorage.localData();
    if (!context->stack.isEmpty()) {
        context->stack.removeLast();
    }
    if (context->stack.isEmpty()) {
        context->traceId.clear();
        context->rootStartMsSinceEpoch = 0;
    }
}

QString exportTraceContext() {
    if (!g_traceContextStorage.hasLocalData()) {
        return {};
    }
    auto* context = g_traceContextStorage.localData();
    if (context->stack.isEmpty()) {
        return {};
    }
    return QString::fromUtf8(QJsonDocument(toJson(*context)).toJson(QJsonDocument::Compact));
}

void importTraceContext(const QString& context) {
    auto* local = getOrCreateContext();
    local->stack.clear();
    local->traceId.clear();
    local->rootStartMsSinceEpoch = 0;

    if (context.isEmpty()) {
        return;
    }

    auto doc = QJsonDocument::fromJson(context.toUtf8());
    if (!doc.isObject()) {
        return;
    }
    *local = fromJson(doc.object());
    if (local->traceId.isEmpty()) {
        local->traceId = createTraceId();
    }
    if (local->rootStartMsSinceEpoch <= 0) {
        local->rootStartMsSinceEpoch = currentMs();
    }
}

void clearTraceContext() {
    if (!g_traceContextStorage.hasLocalData()) {
        return;
    }
    auto* context = g_traceContextStorage.localData();
    context->stack.clear();
    context->traceId.clear();
    context->rootStartMsSinceEpoch = 0;
}

QString currentTraceId() {
    if (!g_traceContextStorage.hasLocalData()) {
        return {};
    }
    return g_traceContextStorage.localData()->traceId;
}

QString currentTraceContextBase64(const QString& traceIdOverride) {
    auto raw = exportTraceContext();
    if (raw.isEmpty()) {
        return {};
    }
    auto* context = g_traceContextStorage.hasLocalData() ? g_traceContextStorage.localData() : nullptr;
    if (context == nullptr) {
        return {};
    }
    auto withTraceId = QString::fromUtf8(QJsonDocument(toJson(*context, traceIdOverride)).toJson(QJsonDocument::Compact));
    return QString::fromLatin1(withTraceId.toUtf8().toBase64());
}

QLOGCOLLECTOR_END_NAMESPACE
