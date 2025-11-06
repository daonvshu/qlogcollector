#pragma once

#include <QtCore/QtGlobal>

#define QLOGCOLLECTOR_BEGIN_NAMESPACE namespace QLogCollector {
#define QLOGCOLLECTOR_END_NAMESPACE }
#define QLOGCOLLECTOR_USE_NAMESPACE using namespace QLogCollector;

#ifndef QLOGCOLLECTOR_EXPORT
#  ifdef QLOGCOLLECTOR_STATIC
#    define QLOGCOLLECTOR_EXPORT
#  else
#    ifdef QLOGCOLLECTOR_LIBRARY
#      define QLOGCOLLECTOR_EXPORT Q_DECL_EXPORT
#    else
#      define QLOGCOLLECTOR_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif