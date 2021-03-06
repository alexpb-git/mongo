/**
 *    Copyright (C) 2018-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kControl
#define MONGO_LOGV2_DEFAULT_COMPONENT mongo::logv2::LogComponent::kControl


#include "mongo/platform/basic.h"

#include "mongo/logv2/log.h"
#include "mongo/util/log.h"

#include "mongo/config.h"
#include "mongo/db/server_options.h"
#include "mongo/logger/console_appender.h"
#include "mongo/logger/message_event_utf8_encoder.h"
#include "mongo/logger/ramlog.h"
#include "mongo/logger/rotatable_file_manager.h"
#include "mongo/logv2/log_domain.h"
#include "mongo/logv2/log_domain_global.h"
#include "mongo/logv2/log_manager.h"
#include "mongo/util/assert_util.h"
#include "mongo/util/concurrency/thread_name.h"
#include "mongo/util/stacktrace.h"
#include "mongo/util/time_support.h"

// TODO: Win32 unicode console writing (in logger/console_appender?).
// TODO: Extra log context appending, and re-enable log_user_*.js
// TODO: Eliminate cout/cerr.

namespace mongo {

bool logV2Enabled() {
    return true;
}

bool logV2IsJson(logv2::LogFormat format) {
    return format == logv2::LogFormat::kJson || format == logv2::LogFormat::kDefault;
}

static logger::ExtraLogContextFn _appendExtraLogContext;

Status logger::registerExtraLogContextFn(logger::ExtraLogContextFn contextFn) {
    if (!contextFn)
        return Status(ErrorCodes::BadValue, "Cannot register a NULL log context function.");
    if (_appendExtraLogContext) {
        return Status(ErrorCodes::AlreadyInitialized,
                      "Cannot call registerExtraLogContextFn multiple times.");
    }
    _appendExtraLogContext = contextFn;
    return Status::OK();
}

Tee* const startupWarningsLog = RamLog::get("startupWarnings");  // intentionally leaked

}  // namespace mongo
